#include "labels.hpp"

#include <sstream>

#include <libpika/data/database.hpp>
#include <libpika/data/row.hpp>
#include <libpika/data/statement.hpp>

using libpika::bittorrent::_aux::models::Labels;

template<typename T>
static std::string ToString(const T &hash)
{
    std::stringstream ss;
    ss << hash;
    return ss.str();
}

void Labels::Delete(libpika::data::Database& db, const libtorrent::info_hash_t &hash)
{
    auto stmt = db.PrepareStatement("DELETE FROM labels WHERE addtorrentparams_id = \n"
                                    "   (SELECT id FROM addtorrentparams p\n"
                                    "       WHERE (p.info_hash_v1 = $1 AND p.info_hash_v2 IS NULL)\n"
                                    "       OR (p.info_hash_v1 IS NULL AND p.info_hash_v2 = $2)\n"
                                    "       OR (p.info_hash_v1 = $1 AND p.info_hash_v2 = $2));");

    stmt->Bind(1, hash.has_v1()
                  ? std::optional(ToString(hash.v1))
                  : std::nullopt);

    stmt->Bind(2, hash.has_v2()
                  ? std::optional(ToString(hash.v2))
                  : std::nullopt);

    stmt->Step();
}

std::map<std::string, std::string> Labels::Get(
    libpika::data::Database& db,
    const libtorrent::info_hash_t &hash)
{
    std::map<std::string, std::string> labels;

    auto stmt = db.PrepareStatement("SELECT name, value FROM labels l\n"
                                    "JOIN addtorrentparams p ON p.id = l.addtorrentparams_id\n"
                                    "WHERE (p.info_hash_v1 = $1    AND p.info_hash_v2 IS NULL)\n"
                                    "   OR (p.info_hash_v1 IS NULL AND p.info_hash_v2 = $2)\n"
                                    "   OR (p.info_hash_v1 = $1    AND p.info_hash_v2 = $2);");

    stmt->Bind(1, hash.has_v1()
                  ? std::optional(ToString(hash.v1))
                  : std::nullopt);

    stmt->Bind(2, hash.has_v2()
                  ? std::optional(ToString(hash.v2))
                  : std::nullopt);

    do
    {
        auto row = stmt->Step();
        if (row == nullptr) break;

        labels.insert({ row->GetStdString(0), row->GetStdString(1) });
    } while(true);

    return labels;
}

void Labels::Set(
    libpika::data::Database& db,
    const libtorrent::info_hash_t &hash,
    const std::map<std::string, std::string> &labels)
{
    auto stmt = db.PrepareStatement("INSERT INTO labels (name, value, addtorrentparams_id)\n"
                                    "VALUES($1, $2, (SELECT id FROM addtorrentparams p\n"
                                    "WHERE (p.info_hash_v1 = $3 AND p.info_hash_v2 IS NULL)\n"
                                    "OR (p.info_hash_v1 IS NULL AND p.info_hash_v2 = $4)\n"
                                    "OR (p.info_hash_v1 = $3 AND p.info_hash_v2 = $4)));";

    for (const auto& [key, val] : labels)
    {
        stmt->Bind(1, key);
        stmt->Bind(2, val);

        stmt->Bind(3, hash.has_v1()
                      ? std::optional(ToString(hash.v1))
                      : std::nullopt);

        stmt->Bind(4, hash.has_v2()
                      ? std::optional(ToString(hash.v2))
                      : std::nullopt);

        stmt->Step();
        stmt->Reset();
    }
}
