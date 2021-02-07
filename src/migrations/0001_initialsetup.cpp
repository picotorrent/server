#include "0001_initialsetup.hpp"

#include <boost/log/trivial.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/settings_pack.hpp>

#include "../data/models/settingspack.hpp"

using pt::Server::Migrations::InitialSetup;

int InitialSetup::Migrate(sqlite3* db)
{
    BOOST_LOG_TRIVIAL(info) << "Creating settings_pack table";

    int res = sqlite3_exec(
        db,
        "CREATE TABLE settings_pack ("
            "id                                INTEGER PRIMARY KEY,"
            "name                              TEXT    NOT NULL,"
            "description                       TEXT    NOT NULL,"

            // string types
            "user_agent                        TEXT    NOT NULL,"
            "announce_ip                       TEXT    NOT NULL,"
            "handshake_client_version          TEXT    NOT NULL,"
            "peer_fingerprint                  TEXT    NOT NULL,"

            // bool types
            "allow_multiple_connections_per_ip INTEGER NOT NULL CHECK(allow_multiple_connections_per_ip IN (0,1)),"
            "send_redundant_have               INTEGER NOT NULL CHECK(send_redundant_have               IN (0,1)),"
            "use_dht_as_fallback               INTEGER NOT NULL CHECK(use_dht_as_fallback               IN (0,1)),"
            "upnp_ignore_nonrouters            INTEGER NOT NULL CHECK(upnp_ignore_nonrouters            IN (0,1)),"
            "use_parole_mode                   INTEGER NOT NULL CHECK(use_parole_mode                   IN (0,1)),"
            "auto_manage_prefer_seeds          INTEGER NOT NULL CHECK(auto_manage_prefer_seeds          IN (0,1)),"
            "dont_count_slow_torrents          INTEGER NOT NULL CHECK(dont_count_slow_torrents          IN (0,1)),"
            "close_redundant_connections       INTEGER NOT NULL CHECK(close_redundant_connections       IN (0,1)),"
            "prioritize_partial_pieces         INTEGER NOT NULL CHECK(prioritize_partial_pieces         IN (0,1)),"
            "rate_limit_ip_overhead            INTEGER NOT NULL CHECK(rate_limit_ip_overhead            IN (0,1)),"
            "announce_to_all_tiers             INTEGER NOT NULL CHECK(announce_to_all_tiers             IN (0,1)),"
            "announce_to_all_trackers          INTEGER NOT NULL CHECK(announce_to_all_trackers          IN (0,1)),"
            "prefer_udp_trackers               INTEGER NOT NULL CHECK(prefer_udp_trackers               IN (0,1)),"
            "disable_hash_checks               INTEGER NOT NULL CHECK(disable_hash_checks               IN (0,1)),"
            "volatile_read_cache               INTEGER NOT NULL CHECK(volatile_read_cache               IN (0,1)),"
            "no_atime_storage                  INTEGER NOT NULL CHECK(no_atime_storage                  IN (0,1)),"
            "incoming_starts_queued_torrents   INTEGER NOT NULL CHECK(incoming_starts_queued_torrents   IN (0,1)),"
            "report_true_downloaded            INTEGER NOT NULL CHECK(report_true_downloaded            IN (0,1)),"
            "strict_end_game_mode              INTEGER NOT NULL CHECK(strict_end_game_mode              IN (0,1)),"
            "enable_outgoing_utp               INTEGER NOT NULL CHECK(enable_outgoing_utp               IN (0,1)),"
            "enable_incoming_utp               INTEGER NOT NULL CHECK(enable_incoming_utp               IN (0,1)),"
            "enable_outgoing_tcp               INTEGER NOT NULL CHECK(enable_outgoing_tcp               IN (0,1)),"
            "enable_incoming_tcp               INTEGER NOT NULL CHECK(enable_incoming_tcp               IN (0,1)),"
            "no_recheck_incomplete_resume      INTEGER NOT NULL CHECK(no_recheck_incomplete_resume      IN (0,1)),"
            "anonymous_mode                    INTEGER NOT NULL CHECK(anonymous_mode                    IN (0,1)),"
            "report_web_seed_downloads         INTEGER NOT NULL CHECK(report_web_seed_downloads         IN (0,1)),"
            "seeding_outgoing_connections      INTEGER NOT NULL CHECK(seeding_outgoing_connections      IN (0,1)),"
            "no_connect_privileged_ports       INTEGER NOT NULL CHECK(no_connect_privileged_ports       IN (0,1)),"
            "smooth_connects                   INTEGER NOT NULL CHECK(smooth_connects                   IN (0,1)),"
            "always_send_user_agent            INTEGER NOT NULL CHECK(always_send_user_agent            IN (0,1)),"
            "apply_ip_filter_to_trackers       INTEGER NOT NULL CHECK(apply_ip_filter_to_trackers       IN (0,1)),"
            "ban_web_seeds                     INTEGER NOT NULL CHECK(ban_web_seeds                     IN (0,1)),"
            "allow_partial_disk_writes         INTEGER NOT NULL CHECK(allow_partial_disk_writes         IN (0,1)),"
            "support_share_mode                INTEGER NOT NULL CHECK(support_share_mode                IN (0,1)),"
            "report_redundant_bytes            INTEGER NOT NULL CHECK(report_redundant_bytes            IN (0,1)),"
            "listen_system_port_fallback       INTEGER NOT NULL CHECK(listen_system_port_fallback       IN (0,1)),"
            "announce_crypto_support           INTEGER NOT NULL CHECK(announce_crypto_support           IN (0,1)),"
            "enable_upnp                       INTEGER NOT NULL CHECK(enable_upnp                       IN (0,1)),"
            "enable_natpmp                     INTEGER NOT NULL CHECK(enable_natpmp                     IN (0,1)),"
            "enable_lsd                        INTEGER NOT NULL CHECK(enable_lsd                        IN (0,1)),"
            "enable_dht                        INTEGER NOT NULL CHECK(enable_dht                        IN (0,1)),"
            "prefer_rc4                        INTEGER NOT NULL CHECK(prefer_rc4                        IN (0,1)),"
            "auto_sequential                   INTEGER NOT NULL CHECK(auto_sequential                   IN (0,1)),"
            "enable_ip_notifier                INTEGER NOT NULL CHECK(enable_ip_notifier                IN (0,1)),"
            "dht_prefer_verified_node_ids      INTEGER NOT NULL CHECK(dht_prefer_verified_node_ids      IN (0,1)),"
            "dht_restrict_routing_ips          INTEGER NOT NULL CHECK(dht_restrict_routing_ips          IN (0,1)),"
            "dht_restrict_search_ips           INTEGER NOT NULL CHECK(dht_restrict_search_ips           IN (0,1)),"
            "dht_extended_routing_table        INTEGER NOT NULL CHECK(dht_extended_routing_table        IN (0,1)),"
            "dht_aggressive_lookups            INTEGER NOT NULL CHECK(dht_aggressive_lookups            IN (0,1)),"
            "dht_privacy_lookups               INTEGER NOT NULL CHECK(dht_privacy_lookups               IN (0,1)),"
            "dht_enforce_node_id               INTEGER NOT NULL CHECK(dht_enforce_node_id               IN (0,1)),"
            "dht_ignore_dark_internet          INTEGER NOT NULL CHECK(dht_ignore_dark_internet          IN (0,1)),"
            "dht_read_only                     INTEGER NOT NULL CHECK(dht_read_only                     IN (0,1)),"
            "piece_extent_affinity             INTEGER NOT NULL CHECK(piece_extent_affinity             IN (0,1)),"
            "validate_https_trackers           INTEGER NOT NULL CHECK(validate_https_trackers           IN (0,1)),"
            "ssrf_mitigation                   INTEGER NOT NULL CHECK(ssrf_mitigation                   IN (0,1)),"
            "allow_idna                        INTEGER NOT NULL CHECK(allow_idna                        IN (0,1)),"
            "enable_set_file_valid_data        INTEGER NOT NULL CHECK(enable_set_file_valid_data        IN (0,1)),"

            // int types
            "tracker_completion_timeout        INTEGER NOT NULL,"
            "tracker_receive_timeout           INTEGER NOT NULL,"
            "stop_tracker_timeout              INTEGER NOT NULL,"
            "tracker_maximum_response_length   INTEGER NOT NULL,"
            "piece_timeout                     INTEGER NOT NULL,"
            "request_timeout                   INTEGER NOT NULL,"
            "request_queue_time                INTEGER NOT NULL,"
            "max_allowed_in_request_queue      INTEGER NOT NULL,"
            "max_out_request_queue             INTEGER NOT NULL,"
            "whole_pieces_threshold            INTEGER NOT NULL,"
            "peer_timeout                      INTEGER NOT NULL,"
            "urlseed_timeout                   INTEGER NOT NULL,"
            "urlseed_pipeline_size             INTEGER NOT NULL,"
            "urlseed_wait_retry                INTEGER NOT NULL,"
            "file_pool_size                    INTEGER NOT NULL,"
            "max_failcount                     INTEGER NOT NULL,"
            "min_reconnect_time                INTEGER NOT NULL,"
            "peer_connect_timeout              INTEGER NOT NULL,"
            "connection_speed                  INTEGER NOT NULL,"
            "inactivity_timeout                INTEGER NOT NULL,"
            "unchoke_interval                  INTEGER NOT NULL,"
            "optimistic_unchoke_interval       INTEGER NOT NULL,"
            "num_want                          INTEGER NOT NULL,"
            "initial_picker_threshold          INTEGER NOT NULL,"
            "allowed_fast_set_size             INTEGER NOT NULL,"
            "suggest_mode                      INTEGER NOT NULL,"
            "max_queued_disk_bytes             INTEGER NOT NULL,"
            "handshake_timeout                 INTEGER NOT NULL,"
            "send_buffer_low_watermark         INTEGER NOT NULL,"
            "send_buffer_watermark             INTEGER NOT NULL,"
            "send_buffer_watermark_factor      INTEGER NOT NULL,"
            "choking_algorithm                 INTEGER NOT NULL,"
            "seed_choking_algorithm            INTEGER NOT NULL,"
            "disk_io_write_mode                INTEGER NOT NULL,"
            "disk_io_read_mode                 INTEGER NOT NULL,"
            "outgoing_port                     INTEGER NOT NULL,"
            "peer_tos                          INTEGER NOT NULL,"
            "active_downloads                  INTEGER NOT NULL,"
            "active_seeds                      INTEGER NOT NULL,"
            "active_checking                   INTEGER NOT NULL,"
            "active_dht_limit                  INTEGER NOT NULL,"
            "active_tracker_limit              INTEGER NOT NULL,"
            "active_lsd_limit                  INTEGER NOT NULL,"
            "active_limit                      INTEGER NOT NULL,"
            "auto_manage_interval              INTEGER NOT NULL,"
            "seed_time_limit                   INTEGER NOT NULL,"
            "auto_scrape_interval              INTEGER NOT NULL,"
            "auto_scrape_min_interval          INTEGER NOT NULL,"
            "max_peerlist_size                 INTEGER NOT NULL,"
            "max_paused_peerlist_size          INTEGER NOT NULL,"
            "min_announce_interval             INTEGER NOT NULL,"
            "auto_manage_startup               INTEGER NOT NULL,"
            "seeding_piece_quota               INTEGER NOT NULL,"
            "max_rejects                       INTEGER NOT NULL,"
            "recv_socket_buffer_size           INTEGER NOT NULL,"
            "send_socket_buffer_size           INTEGER NOT NULL,"
            "max_peer_recv_buffer_size         INTEGER NOT NULL,"
            "read_cache_line_size              INTEGER NOT NULL,"
            "write_cache_line_size             INTEGER NOT NULL,"
            "optimistic_disk_retry             INTEGER NOT NULL,"
            "max_suggest_pieces                INTEGER NOT NULL,"
            "local_service_announce_interval   INTEGER NOT NULL,"
            "dht_announce_interval             INTEGER NOT NULL,"
            "udp_tracker_token_expiry          INTEGER NOT NULL,"
            "num_optimistic_unchoke_slots      INTEGER NOT NULL,"
            "max_pex_peers                     INTEGER NOT NULL,"
            "tick_interval                     INTEGER NOT NULL,"
            "share_mode_target                 INTEGER NOT NULL,"
            "upload_rate_limit                 INTEGER NOT NULL,"
            "download_rate_limit               INTEGER NOT NULL,"
            "dht_upload_rate_limit             INTEGER NOT NULL,"
            "unchoke_slots_limit               INTEGER NOT NULL,"
            "connections_limit                 INTEGER NOT NULL,"
            "connections_slack                 INTEGER NOT NULL,"
            "utp_target_delay                  INTEGER NOT NULL,"
            "utp_gain_factor                   INTEGER NOT NULL,"
            "utp_min_timeout                   INTEGER NOT NULL,"
            "utp_syn_resends                   INTEGER NOT NULL,"
            "utp_fin_resends                   INTEGER NOT NULL,"
            "utp_num_resends                   INTEGER NOT NULL,"
            "utp_connect_timeout               INTEGER NOT NULL,"
            "utp_loss_multiplier               INTEGER NOT NULL,"
            "mixed_mode_algorithm              INTEGER NOT NULL,"
            "listen_queue_size                 INTEGER NOT NULL,"
            "torrent_connect_boost             INTEGER NOT NULL,"
            "alert_queue_size                  INTEGER NOT NULL,"
            "max_metadata_size                 INTEGER NOT NULL,"
            "hashing_threads                   INTEGER NOT NULL,"
            "checking_mem_usage                INTEGER NOT NULL,"
            "predictive_piece_announce         INTEGER NOT NULL,"
            "aio_threads                       INTEGER NOT NULL,"
            "tracker_backoff                   INTEGER NOT NULL,"
            "share_ratio_limit                 INTEGER NOT NULL,"
            "seed_time_ratio_limit             INTEGER NOT NULL,"
            "peer_turnover                     INTEGER NOT NULL,"
            "peer_turnover_cutoff              INTEGER NOT NULL,"
            "peer_turnover_interval            INTEGER NOT NULL,"
            "connect_seed_every_n_download     INTEGER NOT NULL,"
            "max_http_recv_buffer_size         INTEGER NOT NULL,"
            "max_retry_port_bind               INTEGER NOT NULL,"
            "alert_mask                        INTEGER NOT NULL,"
            "out_enc_policy                    INTEGER NOT NULL,"
            "in_enc_policy                     INTEGER NOT NULL,"
            "allowed_enc_level                 INTEGER NOT NULL,"
            "inactive_down_rate                INTEGER NOT NULL,"
            "inactive_up_rate                  INTEGER NOT NULL,"
            "urlseed_max_request_bytes         INTEGER NOT NULL,"
            "web_seed_name_lookup_retry        INTEGER NOT NULL,"
            "close_file_interval               INTEGER NOT NULL,"
            "utp_cwnd_reduce_timer             INTEGER NOT NULL,"
            "max_web_seed_connections          INTEGER NOT NULL,"
            "resolver_cache_timeout            INTEGER NOT NULL,"
            "send_not_sent_low_watermark       INTEGER NOT NULL,"
            "rate_choker_initial_threshold     INTEGER NOT NULL,"
            "upnp_lease_duration               INTEGER NOT NULL,"
            "max_concurrent_http_announces     INTEGER NOT NULL,"
            "dht_max_peers_reply               INTEGER NOT NULL,"
            "dht_search_branching              INTEGER NOT NULL,"
            "dht_max_fail_count                INTEGER NOT NULL,"
            "dht_max_torrents                  INTEGER NOT NULL,"
            "dht_max_dht_items                 INTEGER NOT NULL,"
            "dht_max_peers                     INTEGER NOT NULL,"
            "dht_max_torrent_search_reply      INTEGER NOT NULL,"
            "dht_block_timeout                 INTEGER NOT NULL,"
            "dht_block_ratelimit               INTEGER NOT NULL,"
            "dht_item_lifetime                 INTEGER NOT NULL,"
            "dht_sample_infohashes_interval    INTEGER NOT NULL,"
            "dht_max_infohashes_sample_count   INTEGER NOT NULL,"
            "max_piece_count                   INTEGER NOT NULL"
        ");",
        nullptr,
        nullptr,
        nullptr);

    if (res != SQLITE_OK) { return res; }

    Data::SettingsPack::Create(db, "Default",               "Default settings",               lt::default_settings());
    Data::SettingsPack::Create(db, "High-performance seed", "High-performance seed settings", lt::high_performance_seed());
    Data::SettingsPack::Create(db, "Min. memory usage",     "Minimal memory usage settings",  lt::min_memory_usage());

    BOOST_LOG_TRIVIAL(info) << "Creating profiles table";

    res = sqlite3_exec(
        db,
        "CREATE TABLE profiles ("
            "id                  INTEGER PRIMARY KEY,"
            "name                TEXT NOT NULL,"
            "description         TEXT NULL,"
            "is_active           INTEGER NOT NULL DEFAULT 0,"
            "settings_pack_id    INTEGER NOT NULL REFERENCES settings_pack(id),"
            "created_at          TEXT NOT NULL DEFAULT (strftime('%s'))"
        ");",
        nullptr,
        nullptr,
        nullptr);

    if (res != SQLITE_OK) { return res; }

    BOOST_LOG_TRIVIAL(info) << "Inserting default profile";

    res = sqlite3_exec(
        db,
        "INSERT INTO profiles (name, description, is_active, settings_pack_id) VALUES ('Default', 'Default PicoTorrent profile', 1, (SELECT id FROM settings_pack WHERE name = 'Default'));",
        nullptr,
        nullptr,
        nullptr);

    if (res != SQLITE_OK) { return res; }

    BOOST_LOG_TRIVIAL(info) << "Creating session_params table";

    res = sqlite3_exec(
        db,
        "CREATE TABLE session_params ("
            "data      BLOB NOT NULL,"
            "timestamp INTEGER NOT NULL DEFAULT (strftime('%s'))"
        ");",
        nullptr,
        nullptr,
        nullptr);

    if (res != SQLITE_OK) { return res; }

    BOOST_LOG_TRIVIAL(info) << "Creating torrents table";

    res = sqlite3_exec(
        db,
        "CREATE TABLE torrents ("
            "info_hash      TEXT PRIMARY KEY,"
            "queue_position INTEGER NOT NULL,"
            "resume_data    BLOB NULL,"
            "torrent_data   BLOB NOT NULL"
        ");",
        nullptr,
        nullptr,
        nullptr);

    if (res != SQLITE_OK) { return res; }

    return SQLITE_OK;
}
