#include "session.hpp"

#include <boost/log/trivial.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/client_data.hpp>
#include <libtorrent/create_torrent.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/read_resume_data.hpp>
#include <libtorrent/write_resume_data.hpp>

#include "data/statement.hpp"
#include "data/sqliteexception.hpp"

namespace lt = libtorrent;
using pt::Server::Data::Statement;
using pt::Server::Data::SQLiteException;
using pt::Server::ISession;
using pt::Server::Session;

static std::string to_str(lt::info_hash_t hash)
{
    std::stringstream ss;
    if (hash.has_v2()) { ss << hash.v2; }
    else { ss << hash.v1; }
    return ss.str();
}

struct add_params
{
    bool muted;
};

Session::Session(boost::asio::io_context &io, sqlite3 *db, int id, std::string name, const libtorrent::session_params &params)
    : m_io(io),
    m_timer(io),
    m_db(db)
{
    m_session = std::make_unique<lt::session>(params);
    m_session->set_alert_notify(
        [this]()
        {
            boost::asio::post(m_io, [this] { ReadAlerts(); });
        });

    int added  =  0;
    int stored = -1;

    Statement::ForEach(
        m_db,
        "SELECT COUNT(*) FROM torrents WHERE session_id = $1",
        [&](Statement::Row const& row)
        {
            stored = row.GetInt32(0);
        },
        [&](sqlite3_stmt* stmt)
        {
            sqlite3_bind_int(stmt, 1, id);
        });

    BOOST_LOG_TRIVIAL(info) << "Loading " << stored << " torrents for session " << name;

    Statement::ForEach(
        m_db,
        "SELECT t.info_hash, t.resume_data, t.torrent_data, mu.uri, mu.save_path FROM torrents t\n"
        "LEFT JOIN magnet_uris mu ON t.info_hash = mu.info_hash\n"
        "WHERE t.session_id = $1\n"
        "ORDER BY queue_position ASC",
        [&added, &session = m_session](Statement::Row const& row)
        {
            bool haveMagnetUri = !row.IsNull(3);
            bool haveTorrentData = !row.IsNull(2);
            bool haveResumeData = !row.IsNull(1);

            // This is the add_torrent_params instance we add to the session.
            lt::add_torrent_params params;

            // We have a magnet uri, and no torrent data. This is expected in cases where PicoTorrent
            // shuts down before the metadata has been received. In that case, add the torrent as a
            // regular magnet link again and begin search.
            if (haveMagnetUri && !haveTorrentData)
            {
                lt::error_code ec;
                lt::parse_magnet_uri(row.GetStdString(3), params, ec);

                if (ec)
                {
                    BOOST_LOG_TRIVIAL(error) << "Failed to parse stored magnet URI: " << ec;
                    return;
                }

                params.save_path = row.GetStdString(4);

                if (params.save_path.empty())
                {
                    BOOST_LOG_TRIVIAL(error) << "Cannot add magnet URI with no save path";
                    return;
                }
            }
                // We have torrent data - ie. we can add this as a normal torrent to the session.
            else if (haveTorrentData)
            {
                lt::error_code ec;

                if (haveResumeData)
                {
                    auto buffer = row.GetBlob(1);
                    params = lt::read_resume_data(
                        lt::span<const char>(buffer.data(), buffer.size()),
                        ec);

                    if (ec)
                    {
                        BOOST_LOG_TRIVIAL(warning) << "Failed to read resume data for torrent: " << ec.message();
                    }
                }

                auto torrentBuffer = row.GetBlob(2);
                lt::bdecode_node node = lt::bdecode(
                    lt::span<const char>(torrentBuffer.data(), torrentBuffer.size()),
                    ec);

                if (ec)
                {
                    BOOST_LOG_TRIVIAL(error) << "Failed to read torrent data: " << ec.message();
                    return;
                }

                params.ti = std::make_shared<lt::torrent_info>(node);
            }

            auto extra = new add_params();
            extra->muted = true;

            params.userdata = lt::client_data_t(extra);

            session->async_add_torrent(params);
            added++;
        },
        [&id](sqlite3_stmt* stmt)
        {
            sqlite3_bind_int(stmt, 1, id);
        });

    BOOST_LOG_TRIVIAL(info) << "Loaded " << added << " (of " << stored << ") torrent(s)";

    boost::system::error_code ec;
    m_timer.expires_from_now(boost::posix_time::seconds(1), ec);
    m_timer.async_wait([this](auto && PH1) { PostUpdates(std::forward<decltype(PH1)>(PH1)); });
}

Session::~Session()
{
    m_session->set_alert_notify([] {});
    m_timer.cancel();

    std::vector<char> stateBuffer = lt::write_session_params_buf(
        m_session->session_state(),
        lt::session::save_dht_state);

    BOOST_LOG_TRIVIAL(info) << "Saving session params (" << stateBuffer.size() << " bytes)";

    try
    {
        Statement::ForEach(
            m_db,
            "INSERT INTO session_params (data, timestamp) VALUES ($1, strftime('%s'));",
            [](auto const&) {},
            [&](sqlite3_stmt* stmt)
            {
                sqlite3_bind_blob(stmt, 1, stateBuffer.data(), static_cast<int>(stateBuffer.size()), SQLITE_TRANSIENT);
            });
    }
    catch (SQLiteException&)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to save session params";
    }

    m_session->pause();

    // Save each torrents resume data
    int num_outstanding_resume = 0;
    int num_paused = 0;
    int num_failed = 0;

    auto temp = m_session->get_torrent_status([](lt::torrent_status const&) { return true; });

    for (lt::torrent_status& st : temp)
    {
        if (!st.handle.is_valid()
            || !st.has_metadata
            || !st.need_save_resume)
        {
            continue;
        }

        st.handle.save_resume_data(
            lt::torrent_handle::flush_disk_cache);

        ++num_outstanding_resume;
    }

    BOOST_LOG_TRIVIAL(info) << "Saving data for " << num_outstanding_resume << " torrent(s)";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(m_db, "UPDATE torrents SET queue_position = $1, resume_data = $2 WHERE info_hash = $3", -1, &stmt, nullptr);

    while (num_outstanding_resume > 0)
    {
        lt::alert const* tmp = m_session->wait_for_alert(lt::seconds(10));
        if (tmp == nullptr) { continue; }

        std::vector<lt::alert*> alerts;
        m_session->pop_alerts(&alerts);

        for (lt::alert* a : alerts)
        {
            auto* tp = lt::alert_cast<lt::torrent_paused_alert>(a);

            if (tp)
            {
                ++num_paused;
                continue;
            }

            if (lt::alert_cast<lt::save_resume_data_failed_alert>(a))
            {
                ++num_failed;
                --num_outstanding_resume;
                continue;
            }

            auto* rd = lt::alert_cast<lt::save_resume_data_alert>(a);
            if (!rd) { continue; }
            --num_outstanding_resume;

            std::vector<char> buffer = lt::write_resume_data_buf(rd->params);
            std::string info_hash = to_str(rd->handle.info_hashes());

            // Store state
            sqlite3_bind_int(stmt, 1, static_cast<int>(rd->handle.status().queue_position));
            sqlite3_bind_blob(stmt, 2, buffer.data(), static_cast<int>(buffer.size()), SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 3, info_hash.c_str(), -1, SQLITE_TRANSIENT);

            if (sqlite3_step(stmt) != SQLITE_DONE)
            {
                BOOST_LOG_TRIVIAL(warning) << "Failed to save resume data for torrent " << info_hash << ": " << sqlite3_errmsg(m_db);
            }

            sqlite3_reset(stmt);
        }
    }

    sqlite3_finalize(stmt);

    BOOST_LOG_TRIVIAL(info) << "Session state saved";
}

lt::info_hash_t Session::AddTorrent(lt::add_torrent_params &params)
{
    params.userdata = lt::client_data_t(new add_params());

    m_session->async_add_torrent(params);

    return params.ti
           ? params.ti->info_hashes()
           : params.info_hashes;
}

void Session::ApplySettings(const lt::settings_pack& settings)
{
    m_session->apply_settings(settings);
}

std::shared_ptr<pt::Server::ITorrentHandle> Session::FindTorrent(const lt::info_hash_t& hash)
{
    auto status = m_torrents.find(hash);

    if (status == m_torrents.end())
    {
        return nullptr;
    }

    return nullptr;

    //return std::make_shared<TorrentHandleActor>(status->second);
}

void Session::ForEachTorrent(std::function<bool(lt::torrent_status const& ts)> const& iteree)
{
    for (auto const& item : m_torrents)
    {
        if (!iteree(item.second)) break;
    }
}


void Session::RemoveTorrent(lt::info_hash_t const& hash, bool remove_files)
{
    lt::remove_flags_t flags = {};

    if (remove_files)
    {
        flags = lt::session::delete_files;
    }

    auto find = m_torrents.find(hash);

    if (find == m_torrents.end())
    {
        BOOST_LOG_TRIVIAL(warning) << "Torrent not found: " << to_str(hash);
        return;
    }

    if (!find->second.handle.is_valid())
    {
        BOOST_LOG_TRIVIAL(warning) << "Torrent handle not valid: " << to_str(hash);
        return;
    }

    m_session->remove_torrent(find->second.handle, flags);
}

void Session::ReadAlerts()
{
    std::vector<lt::alert*> alerts;
    m_session->pop_alerts(&alerts);

    for (auto const& alert : alerts)
    {
        BOOST_LOG_TRIVIAL(trace) << alert->message();

        switch (alert->type())
        {
            case lt::add_torrent_alert::alert_type:
            {
                auto* ata = lt::alert_cast<lt::add_torrent_alert>(alert);

                if (ata->error)
                {
                    BOOST_LOG_TRIVIAL(error) << "Error when adding torrent: " << ata->error;
                    return;
                }

                auto* extra = ata->params.userdata.get<add_params>();

                if (extra == nullptr)
                {
                    BOOST_LOG_TRIVIAL(error) << "No internal add_params for " << to_str(ata->params.info_hashes);
                    m_session->remove_torrent(ata->handle, lt::session::delete_files);
                    continue;
                }

                lt::torrent_status ts = ata->handle.status();
                m_torrents.insert({ ts.info_hashes, ts });

                if (!extra->muted)
                {
                    BOOST_LOG_TRIVIAL(debug) << "Saving torrent " << to_str(ts.info_hashes) << " in database";

                    // Store torrent in database. This will always create an entry in the 'torrents' table
                    // and if it is a magnet uri (ie. no torrent file exists) it will also add an entry to
                    // the 'magnet_uris' table.

                    Statement::ForEach(
                        m_db,
                        "INSERT INTO torrents (info_hash, queue_position, torrent_data) VALUES($1, $2, $3);",
                        [](auto const&) {},
                        [&](sqlite3_stmt* stmt)
                        {
                            sqlite3_bind_text(stmt, 1, to_str(ata->params.info_hashes).c_str(), -1, SQLITE_TRANSIENT);
                            sqlite3_bind_int(stmt,  2, static_cast<int>(ts.queue_position));

                            if (ata->params.ti)
                            {
                                lt::create_torrent ct(*ata->params.ti);
                                std::vector<char> buffer;
                                lt::bencode(std::back_inserter(buffer), ct.generate());

                                sqlite3_bind_blob(stmt, 3, buffer.data(), static_cast<int>(buffer.size()), SQLITE_TRANSIENT);
                            }
                            else
                            {
                                sqlite3_bind_null(stmt, 3);
                            }
                        });

                    if (ata->params.ti)
                    {
                        // If we have a torrent file, we save resume data here
                        // to get the save path stored.

                        ata->handle.save_resume_data();
                    }
                    else
                    {
                        // No torrent file. Store the magnet uri with data in the database.

                        BOOST_LOG_TRIVIAL(debug) << "... also saving magnet uri in database";

                        Statement::ForEach(
                            m_db,
                            "INSERT INTO magnet_uris (info_hash, save_path, uri) VALUES ($1, $2, $3);",
                            [](auto const&) {},
                            [&](sqlite3_stmt* stmt)
                            {
                                auto uri = lt::make_magnet_uri(ata->handle);

                                // Store magnet uri
                                sqlite3_bind_text(stmt, 1, to_str(ata->params.info_hashes).c_str(), -1, SQLITE_TRANSIENT);
                                sqlite3_bind_text(stmt, 2, ata->params.save_path.c_str(),           -1, SQLITE_TRANSIENT);
                                sqlite3_bind_text(stmt, 3, uri.c_str(),                             -1, SQLITE_TRANSIENT);
                            });
                    }

                    BOOST_LOG_TRIVIAL(info) << "Torrent " << to_str(ata->handle.info_hashes()) << " added";
                }

                // TODO: invoke event handlers

                break;
            }
            case lt::listen_failed_alert::alert_type:
            {
                BOOST_LOG_TRIVIAL(error) << alert->message();
                break;
            }
            case lt::metadata_received_alert::alert_type:
            {
                auto* mra = lt::alert_cast<lt::metadata_received_alert>(alert);

                lt::create_torrent ct(*mra->handle.torrent_file());
                std::vector<char> buffer;
                lt::bencode(std::back_inserter(buffer), ct.generate());

                std::string hash = to_str(mra->handle.info_hashes());

                BOOST_LOG_TRIVIAL(debug) << "Saving metadata for torrent " << hash;

                Statement::ForEach(
                    m_db,
                    "UPDATE torrents SET torrent_data = $1 WHERE info_hash = $2",
                    [](auto const&){},
                    [&](sqlite3_stmt* stmt)
                    {
                        sqlite3_bind_blob(stmt, 1, buffer.data(), static_cast<int>(buffer.size()), SQLITE_TRANSIENT);
                        sqlite3_bind_text(stmt, 2, hash.c_str(),  static_cast<int>(hash.size()),   SQLITE_TRANSIENT);
                    });

                Statement::ForEach(
                    m_db,
                    "DELETE FROM magnet_uris WHERE info_hash = $1",
                    [](auto const&){},
                    [&](sqlite3_stmt* stmt)
                    {
                        sqlite3_bind_text(stmt, 1, hash.c_str(), static_cast<int>(hash.size()), SQLITE_TRANSIENT);
                    });

                BOOST_LOG_TRIVIAL(info) << "Metadata saved for torrent " << to_str(mra->handle.info_hashes());

                break;
            }
            case lt::save_resume_data_alert::alert_type:
            {
                auto* srda = lt::alert_cast<lt::save_resume_data_alert>(alert);

                std::vector<char> buffer = lt::write_resume_data_buf(srda->params);
                std::string info_hash = to_str(srda->handle.info_hashes());

                Statement::ForEach(
                    m_db,
                    "UPDATE torrents SET queue_position = $1, resume_data = $2 WHERE info_hash = $3",
                    [](auto&&){},
                    [&](sqlite3_stmt* stmt)
                    {
                        sqlite3_bind_int(stmt, 1, static_cast<int>(srda->handle.status().queue_position));
                        sqlite3_bind_blob(stmt, 2, buffer.data(), static_cast<int>(buffer.size()), SQLITE_TRANSIENT);
                        sqlite3_bind_text(stmt, 3, info_hash.c_str(), -1, SQLITE_TRANSIENT);
                    });

                break;
            }
            case lt::session_stats_alert::alert_type:
            {
                auto* ssa = lt::alert_cast<lt::session_stats_alert>(alert);
                auto counters = ssa->counters();

                // TODO: invoke event handlers

                break;
            }
            case lt::state_update_alert::alert_type:
            {
                auto* sua = lt::alert_cast<lt::state_update_alert>(alert);

                /*json j;
                j["type"] = "torrent.updated";
                j["torrents"] = json::object();

                for (auto const& status : sua->status)
                {
                    m_torrents.at(status.info_hashes) = status;
                    j["torrents"][to_str(status.info_hashes)] = status;
                }

                if (!sua->status.empty())
                {
                    Broadcast(j);
                }*/

                // TODO: invoke event handlers

                break;
            }
            case lt::torrent_removed_alert::alert_type:
            {
                auto* tra = lt::alert_cast<lt::torrent_removed_alert>(alert);
                std::string hash = to_str(tra->info_hashes);

                m_torrents.erase(tra->info_hashes);

                Statement::ForEach(
                    m_db,
                    "DELETE FROM torrents WHERE info_hash = $1",
                    [](auto const&){},
                    [&](sqlite3_stmt* stmt)
                    {
                        sqlite3_bind_text(stmt, 1, hash.c_str(), static_cast<int>(hash.size()), SQLITE_TRANSIENT);
                    });

                /*json j;
                j["type"] = "torrent.removed";
                j["info_hash"] = tra->info_hashes;

                Broadcast(j);*/

                // TODO: invoke event handlers

                BOOST_LOG_TRIVIAL(info) << "Torrent " << hash << " removed";

                break;
            }
        }
    }
}

void Session::PostUpdates(boost::system::error_code ec)
{
    if (ec)
    {
        BOOST_LOG_TRIVIAL(error) << "Error in timer: " << ec;
        return;
    }

    m_session->post_dht_stats();
    m_session->post_session_stats();
    m_session->post_torrent_updates();

    m_timer.expires_from_now(boost::posix_time::seconds(1), ec);
    m_timer.async_wait([this](auto && PH1) { PostUpdates(std::forward<decltype(PH1)>(PH1)); });
}
