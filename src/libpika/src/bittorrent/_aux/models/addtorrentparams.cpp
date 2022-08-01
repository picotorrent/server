#include "addtorrentparams.hpp"

#include <libpika/data/database.hpp>
#include <libpika/data/row.hpp>
#include <libpika/data/statement.hpp>
#include <libtorrent/read_resume_data.hpp>
#include <libtorrent/write_resume_data.hpp>

namespace lt = libtorrent;
using libpika::bittorrent::_aux::models::AddTorrentParams;

template<typename T>
static std::string ToString(const T &hash)
{
    std::stringstream ss;
    ss << hash;
    return ss.str();
}

int AddTorrentParams::Count(libpika::data::Database& db)
{
    return db.PrepareStatement("SELECT COUNT(*) FROM addtorrentparams;")
        ->Step()
        ->GetInt32(0);
}

bool AddTorrentParams::Exists(libpika::data::Database& db, const libtorrent::info_hash_t &hash)
{
    return false;
}

void AddTorrentParams::ForEach(libpika::data::Database& db, const std::function<void(libtorrent::add_torrent_params &)> &cb)
{
    auto stmt = db.PrepareStatement("SELECT resume_data_buf FROM addtorrentparams\n"
                                    "ORDER BY queue_position ASC");

    do
    {
        auto row = stmt->Step();
        if (row == nullptr) break;

        auto buf = row->GetBuffer(0);
        auto params = lt::read_resume_data(buf);
        cb(params);
    } while(true);
}

void AddTorrentParams::Insert(libpika::data::Database& db, const libtorrent::add_torrent_params &params, int queuePosition)
{
    std::vector<char> buf = lt::write_resume_data_buf(params);

    auto stmt = db.PrepareStatement("INSERT INTO addtorrentparams\n"
                                    "   (info_hash_v1, info_hash_v2, queue_position, resume_data_buf)\n"
                                    "VALUES ($1, $2, $3, $4);");

    stmt->Bind(1, params.info_hashes.has_v1()
        ? std::optional(ToString(params.info_hashes.v1))
        : std::nullopt);

    stmt->Bind(2, params.info_hashes.has_v2()
        ? std::optional(ToString(params.info_hashes.v2))
        : std::nullopt);

    stmt->Bind(3, queuePosition);
    stmt->Bind(4, buf);

    stmt->Step();
}

void AddTorrentParams::Remove(libpika::data::Database& db, const libtorrent::info_hash_t &hash)
{
    auto stmt = db.PrepareStatement("DELETE FROM addtorrentparams\n"
                                    "WHERE (info_hash_v1 = $1 AND info_hash_v2 IS NULL)\n"
                                    "   OR (info_hash_v1 IS NULL AND info_hash_v2 = $2)\n"
                                    "   OR (info_hash_v1 = $1 AND info_hash_v2 = $2);");

    stmt->Bind(1, hash.has_v1()
                  ? std::optional(ToString(hash.v1))
                  : std::nullopt);

    stmt->Bind(2, hash.has_v2()
                  ? std::optional(ToString(hash.v2))
                  : std::nullopt);

    stmt->Step();
}

void AddTorrentParams::Update(libpika::data::Database& db, const libtorrent::add_torrent_params &params, int queuePosition)
{
    std::vector<char> buf = lt::write_resume_data_buf(params);

    auto stmt = db.PrepareStatement("UPDATE addtorrentparams SET queue_position = $1, resume_data_buf = $2\n"
                                    "WHERE (info_hash_v1 = $3 AND info_hash_v2 IS NULL)\n"
                                    "   OR (info_hash_v1 IS NULL AND info_hash_v2 = $4)\n"
                                    "   OR (info_hash_v1 = $3 AND info_hash_v2 = $4);");

    stmt->Bind(1, queuePosition);
    stmt->Bind(2, buf);

    stmt->Bind(3, params.info_hashes.has_v1()
                  ? std::optional(ToString(params.info_hashes.v1))
                  : std::nullopt);

    stmt->Bind(4, params.info_hashes.has_v2()
                  ? std::optional(ToString(params.info_hashes.v2))
                  : std::nullopt);

    stmt->Step();
}
