#include "settingspack.hpp"

#include <string>
#include <unordered_set>

#include <boost/log/trivial.hpp>

#include "../datareader.hpp"
#include "../sqliteexception.hpp"

namespace lt = libtorrent;
using pt::Server::Data::SettingsPack;
using pt::Server::Data::SQLiteException;

std::shared_ptr<SettingsPack> SettingsPack::Create(
    sqlite3* db,
    std::string_view const& name,
    std::string_view const& desc,
    libtorrent::settings_pack settings)
{
    sqlite3_stmt* stmt;
    int res = sqlite3_prepare_v2(
        db,
        "INSERT INTO settings_pack ("
            "name,"
            "description,"

            "user_agent,"
            "announce_ip,"
            "handshake_client_version,"
            "peer_fingerprint,"

            "allow_multiple_connections_per_ip,"
            "send_redundant_have,"
            "use_dht_as_fallback,"
            "upnp_ignore_nonrouters,"
            "use_parole_mode,"
            "auto_manage_prefer_seeds,"
            "dont_count_slow_torrents,"
            "close_redundant_connections,"
            "prioritize_partial_pieces,"
            "rate_limit_ip_overhead,"
            "announce_to_all_tiers,"
            "announce_to_all_trackers,"
            "prefer_udp_trackers,"
            "disable_hash_checks,"
            "volatile_read_cache,"
            "no_atime_storage,"
            "incoming_starts_queued_torrents,"
            "report_true_downloaded,"
            "strict_end_game_mode,"
            "enable_outgoing_utp,"
            "enable_incoming_utp,"
            "enable_outgoing_tcp,"
            "enable_incoming_tcp,"
            "no_recheck_incomplete_resume,"
            "anonymous_mode,"
            "report_web_seed_downloads,"
            "seeding_outgoing_connections,"
            "no_connect_privileged_ports,"
            "smooth_connects,"
            "always_send_user_agent,"
            "apply_ip_filter_to_trackers,"
            "ban_web_seeds,"
            "allow_partial_disk_writes,"
            "support_share_mode,"
            "report_redundant_bytes,"
            "listen_system_port_fallback,"
            "announce_crypto_support,"
            "enable_upnp,"
            "enable_natpmp,"
            "enable_lsd,"
            "enable_dht,"
            "prefer_rc4,"
            "auto_sequential,"
            "enable_ip_notifier,"
            "dht_prefer_verified_node_ids,"
            "dht_restrict_routing_ips,"
            "dht_restrict_search_ips,"
            "dht_extended_routing_table,"
            "dht_aggressive_lookups,"
            "dht_privacy_lookups,"
            "dht_enforce_node_id,"
            "dht_ignore_dark_internet,"
            "dht_read_only,"
            "piece_extent_affinity,"
            "validate_https_trackers,"
            "ssrf_mitigation,"
            "allow_idna,"
            "enable_set_file_valid_data,"

            "tracker_completion_timeout,"
            "tracker_receive_timeout,"
            "stop_tracker_timeout,"
            "tracker_maximum_response_length,"
            "piece_timeout,"
            "request_timeout,"
            "request_queue_time,"
            "max_allowed_in_request_queue,"
            "max_out_request_queue,"
            "whole_pieces_threshold,"
            "peer_timeout,"
            "urlseed_timeout,"
            "urlseed_pipeline_size,"
            "urlseed_wait_retry,"
            "file_pool_size,"
            "max_failcount,"
            "min_reconnect_time,"
            "peer_connect_timeout,"
            "connection_speed,"
            "inactivity_timeout,"
            "unchoke_interval,"
            "optimistic_unchoke_interval,"
            "num_want,"
            "initial_picker_threshold,"
            "allowed_fast_set_size,"
            "suggest_mode,"
            "max_queued_disk_bytes,"
            "handshake_timeout,"
            "send_buffer_low_watermark,"
            "send_buffer_watermark,"
            "send_buffer_watermark_factor,"
            "choking_algorithm,"
            "seed_choking_algorithm,"
            "disk_io_write_mode,"
            "disk_io_read_mode,"
            "outgoing_port,"
            "peer_tos,"
            "active_downloads,"
            "active_seeds,"
            "active_checking,"
            "active_dht_limit,"
            "active_tracker_limit,"
            "active_lsd_limit,"
            "active_limit,"
            "auto_manage_interval,"
            "seed_time_limit,"
            "auto_scrape_interval,"
            "auto_scrape_min_interval,"
            "max_peerlist_size,"
            "max_paused_peerlist_size,"
            "min_announce_interval,"
            "auto_manage_startup,"
            "seeding_piece_quota,"
            "max_rejects,"
            "recv_socket_buffer_size,"
            "send_socket_buffer_size,"
            "max_peer_recv_buffer_size,"
            "read_cache_line_size,"
            "write_cache_line_size,"
            "optimistic_disk_retry,"
            "max_suggest_pieces,"
            "local_service_announce_interval,"
            "dht_announce_interval,"
            "udp_tracker_token_expiry,"
            "num_optimistic_unchoke_slots,"
            "max_pex_peers,"
            "tick_interval,"
            "share_mode_target,"
            "upload_rate_limit,"
            "download_rate_limit,"
            "dht_upload_rate_limit,"
            "unchoke_slots_limit,"
            "connections_limit,"
            "connections_slack,"
            "utp_target_delay,"
            "utp_gain_factor,"
            "utp_min_timeout,"
            "utp_syn_resends,"
            "utp_fin_resends,"
            "utp_num_resends,"
            "utp_connect_timeout,"
            "utp_loss_multiplier,"
            "mixed_mode_algorithm,"
            "listen_queue_size,"
            "torrent_connect_boost,"
            "alert_queue_size,"
            "max_metadata_size,"
            "hashing_threads,"
            "checking_mem_usage,"
            "predictive_piece_announce,"
            "aio_threads,"
            "tracker_backoff,"
            "share_ratio_limit,"
            "seed_time_ratio_limit,"
            "peer_turnover,"
            "peer_turnover_cutoff,"
            "peer_turnover_interval,"
            "connect_seed_every_n_download,"
            "max_http_recv_buffer_size,"
            "max_retry_port_bind,"
            // "alert_mask,"
            "out_enc_policy,"
            "in_enc_policy,"
            "allowed_enc_level,"
            "inactive_down_rate,"
            "inactive_up_rate,"
            "urlseed_max_request_bytes,"
            "web_seed_name_lookup_retry,"
            "close_file_interval,"
            "utp_cwnd_reduce_timer,"
            "max_web_seed_connections,"
            "resolver_cache_timeout,"
            "send_not_sent_low_watermark,"
            "rate_choker_initial_threshold,"
            "upnp_lease_duration,"
            "max_concurrent_http_announces,"
            "dht_max_peers_reply,"
            "dht_search_branching,"
            "dht_max_fail_count,"
            "dht_max_torrents,"
            "dht_max_dht_items,"
            "dht_max_peers,"
            "dht_max_torrent_search_reply,"
            "dht_block_timeout,"
            "dht_block_ratelimit,"
            "dht_item_lifetime,"
            "dht_sample_infohashes_interval,"
            "dht_max_infohashes_sample_count,"
            "max_piece_count"
        ") "
        "VALUES ("
            "?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,"
            "?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,"
            "?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,"
            "?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,"
            "?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,"
            "?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,"
            "?,?,?,?,?,?,?,?,?,?,?,?"
        ");",
        -1,
        &stmt,
        nullptr);

    if (res != SQLITE_OK)
    {
        throw SQLiteException();
    }

    sqlite3_bind_text(stmt, 1, name.data(), static_cast<int>(name.size()), SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, desc.data(), static_cast<int>(desc.size()), SQLITE_TRANSIENT);

    sqlite3_bind_text(stmt, 3, settings.get_str(lt::settings_pack::user_agent).c_str(),               -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, settings.get_str(lt::settings_pack::announce_ip).c_str(),              -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, settings.get_str(lt::settings_pack::handshake_client_version).c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, settings.get_str(lt::settings_pack::peer_fingerprint).c_str(),         -1, SQLITE_TRANSIENT);

    sqlite3_bind_int(stmt,  7, settings.get_bool(lt::settings_pack::allow_multiple_connections_per_ip) ? 1 : 0);
    sqlite3_bind_int(stmt,  8, settings.get_bool(lt::settings_pack::send_redundant_have)               ? 1 : 0);
    sqlite3_bind_int(stmt,  9, settings.get_bool(lt::settings_pack::use_dht_as_fallback)               ? 1 : 0);
    sqlite3_bind_int(stmt, 10, settings.get_bool(lt::settings_pack::upnp_ignore_nonrouters)            ? 1 : 0);
    sqlite3_bind_int(stmt, 11, settings.get_bool(lt::settings_pack::use_parole_mode)                   ? 1 : 0);
    sqlite3_bind_int(stmt, 12, settings.get_bool(lt::settings_pack::auto_manage_prefer_seeds)          ? 1 : 0);
    sqlite3_bind_int(stmt, 13, settings.get_bool(lt::settings_pack::dont_count_slow_torrents)          ? 1 : 0);
    sqlite3_bind_int(stmt, 14, settings.get_bool(lt::settings_pack::close_redundant_connections)       ? 1 : 0);
    sqlite3_bind_int(stmt, 15, settings.get_bool(lt::settings_pack::prioritize_partial_pieces)         ? 1 : 0);
    sqlite3_bind_int(stmt, 16, settings.get_bool(lt::settings_pack::rate_limit_ip_overhead)            ? 1 : 0);
    sqlite3_bind_int(stmt, 17, settings.get_bool(lt::settings_pack::announce_to_all_tiers)             ? 1 : 0);
    sqlite3_bind_int(stmt, 18, settings.get_bool(lt::settings_pack::announce_to_all_trackers)          ? 1 : 0);
    sqlite3_bind_int(stmt, 19, settings.get_bool(lt::settings_pack::prefer_udp_trackers)               ? 1 : 0);
    sqlite3_bind_int(stmt, 20, settings.get_bool(lt::settings_pack::disable_hash_checks)               ? 1 : 0);
    sqlite3_bind_int(stmt, 21,                                                                               0);
    sqlite3_bind_int(stmt, 22, settings.get_bool(lt::settings_pack::no_atime_storage)                  ? 1 : 0);
    sqlite3_bind_int(stmt, 23, settings.get_bool(lt::settings_pack::incoming_starts_queued_torrents)   ? 1 : 0);
    sqlite3_bind_int(stmt, 24, settings.get_bool(lt::settings_pack::report_true_downloaded)            ? 1 : 0);
    sqlite3_bind_int(stmt, 25, settings.get_bool(lt::settings_pack::strict_end_game_mode)              ? 1 : 0);
    sqlite3_bind_int(stmt, 26, settings.get_bool(lt::settings_pack::enable_outgoing_utp)               ? 1 : 0);
    sqlite3_bind_int(stmt, 27, settings.get_bool(lt::settings_pack::enable_incoming_utp)               ? 1 : 0);
    sqlite3_bind_int(stmt, 28, settings.get_bool(lt::settings_pack::enable_outgoing_tcp)               ? 1 : 0);
    sqlite3_bind_int(stmt, 29, settings.get_bool(lt::settings_pack::enable_incoming_tcp)               ? 1 : 0);
    sqlite3_bind_int(stmt, 30, settings.get_bool(lt::settings_pack::no_recheck_incomplete_resume)      ? 1 : 0);
    sqlite3_bind_int(stmt, 31, settings.get_bool(lt::settings_pack::anonymous_mode)                    ? 1 : 0);
    sqlite3_bind_int(stmt, 32, settings.get_bool(lt::settings_pack::report_web_seed_downloads)         ? 1 : 0);
    sqlite3_bind_int(stmt, 33, settings.get_bool(lt::settings_pack::seeding_outgoing_connections)      ? 1 : 0);
    sqlite3_bind_int(stmt, 34, settings.get_bool(lt::settings_pack::no_connect_privileged_ports)       ? 1 : 0);
    sqlite3_bind_int(stmt, 35, settings.get_bool(lt::settings_pack::smooth_connects)                   ? 1 : 0);
    sqlite3_bind_int(stmt, 36, settings.get_bool(lt::settings_pack::always_send_user_agent)            ? 1 : 0);
    sqlite3_bind_int(stmt, 37, settings.get_bool(lt::settings_pack::apply_ip_filter_to_trackers)       ? 1 : 0);
    sqlite3_bind_int(stmt, 38, settings.get_bool(lt::settings_pack::ban_web_seeds)                     ? 1 : 0);
    sqlite3_bind_int(stmt, 39,                                                                               0);
    sqlite3_bind_int(stmt, 40, settings.get_bool(lt::settings_pack::support_share_mode)                ? 1 : 0);
    sqlite3_bind_int(stmt, 41, settings.get_bool(lt::settings_pack::report_redundant_bytes)            ? 1 : 0);
    sqlite3_bind_int(stmt, 42, settings.get_bool(lt::settings_pack::listen_system_port_fallback)       ? 1 : 0);
    sqlite3_bind_int(stmt, 43, settings.get_bool(lt::settings_pack::announce_crypto_support)           ? 1 : 0);
    sqlite3_bind_int(stmt, 44, settings.get_bool(lt::settings_pack::enable_upnp)                       ? 1 : 0);
    sqlite3_bind_int(stmt, 45, settings.get_bool(lt::settings_pack::enable_natpmp)                     ? 1 : 0);
    sqlite3_bind_int(stmt, 46, settings.get_bool(lt::settings_pack::enable_lsd)                        ? 1 : 0);
    sqlite3_bind_int(stmt, 47, settings.get_bool(lt::settings_pack::enable_dht)                        ? 1 : 0);
    sqlite3_bind_int(stmt, 48, settings.get_bool(lt::settings_pack::prefer_rc4)                        ? 1 : 0);
    sqlite3_bind_int(stmt, 49, settings.get_bool(lt::settings_pack::auto_sequential)                   ? 1 : 0);
    sqlite3_bind_int(stmt, 50, settings.get_bool(lt::settings_pack::enable_ip_notifier)                ? 1 : 0);
    sqlite3_bind_int(stmt, 51, settings.get_bool(lt::settings_pack::dht_prefer_verified_node_ids)      ? 1 : 0);
    sqlite3_bind_int(stmt, 52, settings.get_bool(lt::settings_pack::dht_restrict_routing_ips)          ? 1 : 0);
    sqlite3_bind_int(stmt, 53, settings.get_bool(lt::settings_pack::dht_restrict_search_ips)           ? 1 : 0);
    sqlite3_bind_int(stmt, 54, settings.get_bool(lt::settings_pack::dht_extended_routing_table)        ? 1 : 0);
    sqlite3_bind_int(stmt, 55, settings.get_bool(lt::settings_pack::dht_aggressive_lookups)            ? 1 : 0);
    sqlite3_bind_int(stmt, 56, settings.get_bool(lt::settings_pack::dht_privacy_lookups)               ? 1 : 0);
    sqlite3_bind_int(stmt, 57, settings.get_bool(lt::settings_pack::dht_enforce_node_id)               ? 1 : 0);
    sqlite3_bind_int(stmt, 58, settings.get_bool(lt::settings_pack::dht_ignore_dark_internet)          ? 1 : 0);
    sqlite3_bind_int(stmt, 59, settings.get_bool(lt::settings_pack::dht_read_only)                     ? 1 : 0);
    sqlite3_bind_int(stmt, 60, settings.get_bool(lt::settings_pack::piece_extent_affinity)             ? 1 : 0);
    sqlite3_bind_int(stmt, 61, settings.get_bool(lt::settings_pack::validate_https_trackers)           ? 1 : 0);
    sqlite3_bind_int(stmt, 62, settings.get_bool(lt::settings_pack::ssrf_mitigation)                   ? 1 : 0);
    sqlite3_bind_int(stmt, 63, settings.get_bool(lt::settings_pack::allow_idna)                        ? 1 : 0);
    sqlite3_bind_int(stmt, 64, settings.get_bool(lt::settings_pack::enable_set_file_valid_data)        ? 1 : 0);

    sqlite3_bind_int(stmt, 65, settings.get_int(lt::settings_pack::tracker_completion_timeout));
    sqlite3_bind_int(stmt, 66, settings.get_int(lt::settings_pack::tracker_receive_timeout));
    sqlite3_bind_int(stmt, 67, settings.get_int(lt::settings_pack::stop_tracker_timeout));
    sqlite3_bind_int(stmt, 68, settings.get_int(lt::settings_pack::tracker_maximum_response_length));
    sqlite3_bind_int(stmt, 69, settings.get_int(lt::settings_pack::piece_timeout));
    sqlite3_bind_int(stmt, 70, settings.get_int(lt::settings_pack::request_timeout));
    sqlite3_bind_int(stmt, 71, settings.get_int(lt::settings_pack::request_queue_time));
    sqlite3_bind_int(stmt, 72, settings.get_int(lt::settings_pack::max_allowed_in_request_queue));
    sqlite3_bind_int(stmt, 73, settings.get_int(lt::settings_pack::max_out_request_queue));
    sqlite3_bind_int(stmt, 74, settings.get_int(lt::settings_pack::whole_pieces_threshold));
    sqlite3_bind_int(stmt, 75, settings.get_int(lt::settings_pack::peer_timeout));
    sqlite3_bind_int(stmt, 76, settings.get_int(lt::settings_pack::urlseed_timeout));
    sqlite3_bind_int(stmt, 77, settings.get_int(lt::settings_pack::urlseed_pipeline_size));
    sqlite3_bind_int(stmt, 78, settings.get_int(lt::settings_pack::urlseed_wait_retry));
    sqlite3_bind_int(stmt, 79, settings.get_int(lt::settings_pack::file_pool_size));
    sqlite3_bind_int(stmt, 80, settings.get_int(lt::settings_pack::max_failcount));
    sqlite3_bind_int(stmt, 81, settings.get_int(lt::settings_pack::min_reconnect_time));
    sqlite3_bind_int(stmt, 82, settings.get_int(lt::settings_pack::peer_connect_timeout));
    sqlite3_bind_int(stmt, 83, settings.get_int(lt::settings_pack::connection_speed));
    sqlite3_bind_int(stmt, 84, settings.get_int(lt::settings_pack::inactivity_timeout));
    sqlite3_bind_int(stmt, 85, settings.get_int(lt::settings_pack::unchoke_interval));
    sqlite3_bind_int(stmt, 86, settings.get_int(lt::settings_pack::optimistic_unchoke_interval));
    sqlite3_bind_int(stmt, 87, settings.get_int(lt::settings_pack::num_want));
    sqlite3_bind_int(stmt, 88, settings.get_int(lt::settings_pack::initial_picker_threshold));
    sqlite3_bind_int(stmt, 89, settings.get_int(lt::settings_pack::allowed_fast_set_size));
    sqlite3_bind_int(stmt, 90, settings.get_int(lt::settings_pack::suggest_mode));
    sqlite3_bind_int(stmt, 91, settings.get_int(lt::settings_pack::max_queued_disk_bytes));
    sqlite3_bind_int(stmt, 92, settings.get_int(lt::settings_pack::handshake_timeout));
    sqlite3_bind_int(stmt, 93, settings.get_int(lt::settings_pack::send_buffer_low_watermark));
    sqlite3_bind_int(stmt, 94, settings.get_int(lt::settings_pack::send_buffer_watermark));
    sqlite3_bind_int(stmt, 95, settings.get_int(lt::settings_pack::send_buffer_watermark_factor));
    sqlite3_bind_int(stmt, 96, settings.get_int(lt::settings_pack::choking_algorithm));
    sqlite3_bind_int(stmt, 97, settings.get_int(lt::settings_pack::seed_choking_algorithm));
    sqlite3_bind_int(stmt, 98, settings.get_int(lt::settings_pack::disk_io_write_mode));
    sqlite3_bind_int(stmt, 99, settings.get_int(lt::settings_pack::disk_io_read_mode));
    sqlite3_bind_int(stmt, 100, settings.get_int(lt::settings_pack::outgoing_port));
    sqlite3_bind_int(stmt, 101, settings.get_int(lt::settings_pack::peer_tos));
    sqlite3_bind_int(stmt, 102, settings.get_int(lt::settings_pack::active_downloads));
    sqlite3_bind_int(stmt, 103, settings.get_int(lt::settings_pack::active_seeds));
    sqlite3_bind_int(stmt, 104, settings.get_int(lt::settings_pack::active_checking));
    sqlite3_bind_int(stmt, 105, settings.get_int(lt::settings_pack::active_dht_limit));
    sqlite3_bind_int(stmt, 106, settings.get_int(lt::settings_pack::active_tracker_limit));
    sqlite3_bind_int(stmt, 107, settings.get_int(lt::settings_pack::active_lsd_limit));
    sqlite3_bind_int(stmt, 108, settings.get_int(lt::settings_pack::active_limit));
    sqlite3_bind_int(stmt, 109, settings.get_int(lt::settings_pack::auto_manage_interval));
    sqlite3_bind_int(stmt, 110, settings.get_int(lt::settings_pack::seed_time_limit));
    sqlite3_bind_int(stmt, 111, settings.get_int(lt::settings_pack::auto_scrape_interval));
    sqlite3_bind_int(stmt, 112, settings.get_int(lt::settings_pack::auto_scrape_min_interval));
    sqlite3_bind_int(stmt, 113, settings.get_int(lt::settings_pack::max_peerlist_size));
    sqlite3_bind_int(stmt, 114, settings.get_int(lt::settings_pack::max_paused_peerlist_size));
    sqlite3_bind_int(stmt, 115, settings.get_int(lt::settings_pack::min_announce_interval));
    sqlite3_bind_int(stmt, 116, settings.get_int(lt::settings_pack::auto_manage_startup));
    sqlite3_bind_int(stmt, 117, settings.get_int(lt::settings_pack::seeding_piece_quota));
    sqlite3_bind_int(stmt, 118, settings.get_int(lt::settings_pack::max_rejects));
    sqlite3_bind_int(stmt, 119, settings.get_int(lt::settings_pack::recv_socket_buffer_size));
    sqlite3_bind_int(stmt, 120, settings.get_int(lt::settings_pack::send_socket_buffer_size));
    sqlite3_bind_int(stmt, 121, settings.get_int(lt::settings_pack::max_peer_recv_buffer_size));
    sqlite3_bind_int(stmt, 122, 0);
    sqlite3_bind_int(stmt, 123, 0);
    sqlite3_bind_int(stmt, 124, settings.get_int(lt::settings_pack::optimistic_disk_retry));
    sqlite3_bind_int(stmt, 125, settings.get_int(lt::settings_pack::max_suggest_pieces));
    sqlite3_bind_int(stmt, 126, settings.get_int(lt::settings_pack::local_service_announce_interval));
    sqlite3_bind_int(stmt, 127, settings.get_int(lt::settings_pack::dht_announce_interval));
    sqlite3_bind_int(stmt, 128, settings.get_int(lt::settings_pack::udp_tracker_token_expiry));
    sqlite3_bind_int(stmt, 129, settings.get_int(lt::settings_pack::num_optimistic_unchoke_slots));
    sqlite3_bind_int(stmt, 130, settings.get_int(lt::settings_pack::max_pex_peers));
    sqlite3_bind_int(stmt, 131, settings.get_int(lt::settings_pack::tick_interval));
    sqlite3_bind_int(stmt, 132, settings.get_int(lt::settings_pack::share_mode_target));
    sqlite3_bind_int(stmt, 133, settings.get_int(lt::settings_pack::upload_rate_limit));
    sqlite3_bind_int(stmt, 134, settings.get_int(lt::settings_pack::download_rate_limit));
    sqlite3_bind_int(stmt, 135, settings.get_int(lt::settings_pack::dht_upload_rate_limit));
    sqlite3_bind_int(stmt, 136, settings.get_int(lt::settings_pack::unchoke_slots_limit));
    sqlite3_bind_int(stmt, 137, settings.get_int(lt::settings_pack::connections_limit));
    sqlite3_bind_int(stmt, 138, settings.get_int(lt::settings_pack::connections_slack));
    sqlite3_bind_int(stmt, 139, settings.get_int(lt::settings_pack::utp_target_delay));
    sqlite3_bind_int(stmt, 140, settings.get_int(lt::settings_pack::utp_gain_factor));
    sqlite3_bind_int(stmt, 141, settings.get_int(lt::settings_pack::utp_min_timeout));
    sqlite3_bind_int(stmt, 142, settings.get_int(lt::settings_pack::utp_syn_resends));
    sqlite3_bind_int(stmt, 143, settings.get_int(lt::settings_pack::utp_fin_resends));
    sqlite3_bind_int(stmt, 144, settings.get_int(lt::settings_pack::utp_num_resends));
    sqlite3_bind_int(stmt, 145, settings.get_int(lt::settings_pack::utp_connect_timeout));
    sqlite3_bind_int(stmt, 146, settings.get_int(lt::settings_pack::utp_loss_multiplier));
    sqlite3_bind_int(stmt, 147, settings.get_int(lt::settings_pack::mixed_mode_algorithm));
    sqlite3_bind_int(stmt, 148, settings.get_int(lt::settings_pack::listen_queue_size));
    sqlite3_bind_int(stmt, 149, settings.get_int(lt::settings_pack::torrent_connect_boost));
    sqlite3_bind_int(stmt, 150, settings.get_int(lt::settings_pack::alert_queue_size));
    sqlite3_bind_int(stmt, 151, settings.get_int(lt::settings_pack::max_metadata_size));
    sqlite3_bind_int(stmt, 152, settings.get_int(lt::settings_pack::hashing_threads));
    sqlite3_bind_int(stmt, 153, settings.get_int(lt::settings_pack::checking_mem_usage));
    sqlite3_bind_int(stmt, 154, settings.get_int(lt::settings_pack::predictive_piece_announce));
    sqlite3_bind_int(stmt, 155, settings.get_int(lt::settings_pack::aio_threads));
    sqlite3_bind_int(stmt, 156, settings.get_int(lt::settings_pack::tracker_backoff));
    sqlite3_bind_int(stmt, 157, settings.get_int(lt::settings_pack::share_ratio_limit));
    sqlite3_bind_int(stmt, 158, settings.get_int(lt::settings_pack::seed_time_ratio_limit));
    sqlite3_bind_int(stmt, 159, settings.get_int(lt::settings_pack::peer_turnover));
    sqlite3_bind_int(stmt, 160, settings.get_int(lt::settings_pack::peer_turnover_cutoff));
    sqlite3_bind_int(stmt, 161, settings.get_int(lt::settings_pack::peer_turnover_interval));
    sqlite3_bind_int(stmt, 162, settings.get_int(lt::settings_pack::connect_seed_every_n_download));
    sqlite3_bind_int(stmt, 163, settings.get_int(lt::settings_pack::max_http_recv_buffer_size));
    sqlite3_bind_int(stmt, 164, settings.get_int(lt::settings_pack::max_retry_port_bind));
    // sqlite3_bind_int(stmt, 165, settings.get_int(lt::settings_pack::alert_mask));
    sqlite3_bind_int(stmt, 165, settings.get_int(lt::settings_pack::out_enc_policy));
    sqlite3_bind_int(stmt, 166, settings.get_int(lt::settings_pack::in_enc_policy));
    sqlite3_bind_int(stmt, 167, settings.get_int(lt::settings_pack::allowed_enc_level));
    sqlite3_bind_int(stmt, 168, settings.get_int(lt::settings_pack::inactive_down_rate));
    sqlite3_bind_int(stmt, 169, settings.get_int(lt::settings_pack::inactive_up_rate));
    sqlite3_bind_int(stmt, 170, settings.get_int(lt::settings_pack::urlseed_max_request_bytes));
    sqlite3_bind_int(stmt, 171, settings.get_int(lt::settings_pack::web_seed_name_lookup_retry));
    sqlite3_bind_int(stmt, 172, settings.get_int(lt::settings_pack::close_file_interval));
    sqlite3_bind_int(stmt, 173, settings.get_int(lt::settings_pack::utp_cwnd_reduce_timer));
    sqlite3_bind_int(stmt, 174, settings.get_int(lt::settings_pack::max_web_seed_connections));
    sqlite3_bind_int(stmt, 175, settings.get_int(lt::settings_pack::resolver_cache_timeout));
    sqlite3_bind_int(stmt, 176, settings.get_int(lt::settings_pack::send_not_sent_low_watermark));
    sqlite3_bind_int(stmt, 177, settings.get_int(lt::settings_pack::rate_choker_initial_threshold));
    sqlite3_bind_int(stmt, 178, settings.get_int(lt::settings_pack::upnp_lease_duration));
    sqlite3_bind_int(stmt, 179, settings.get_int(lt::settings_pack::max_concurrent_http_announces));
    sqlite3_bind_int(stmt, 180, settings.get_int(lt::settings_pack::dht_max_peers_reply));
    sqlite3_bind_int(stmt, 181, settings.get_int(lt::settings_pack::dht_search_branching));
    sqlite3_bind_int(stmt, 182, settings.get_int(lt::settings_pack::dht_max_fail_count));
    sqlite3_bind_int(stmt, 183, settings.get_int(lt::settings_pack::dht_max_torrents));
    sqlite3_bind_int(stmt, 184, settings.get_int(lt::settings_pack::dht_max_dht_items));
    sqlite3_bind_int(stmt, 185, settings.get_int(lt::settings_pack::dht_max_peers));
    sqlite3_bind_int(stmt, 186, settings.get_int(lt::settings_pack::dht_max_torrent_search_reply));
    sqlite3_bind_int(stmt, 187, settings.get_int(lt::settings_pack::dht_block_timeout));
    sqlite3_bind_int(stmt, 188, settings.get_int(lt::settings_pack::dht_block_ratelimit));
    sqlite3_bind_int(stmt, 189, settings.get_int(lt::settings_pack::dht_item_lifetime));
    sqlite3_bind_int(stmt, 190, settings.get_int(lt::settings_pack::dht_sample_infohashes_interval));
    sqlite3_bind_int(stmt, 191, settings.get_int(lt::settings_pack::dht_max_infohashes_sample_count));
    sqlite3_bind_int(stmt, 192, settings.get_int(lt::settings_pack::max_piece_count));

    res = sqlite3_step(stmt);

    if (res != SQLITE_DONE)
    {
        throw SQLiteException();
    }

    res = sqlite3_finalize(stmt);

    if (res != SQLITE_OK)
    {
        throw SQLiteException();
    }

    int id = static_cast<int>(sqlite3_last_insert_rowid(db));

    return std::shared_ptr<SettingsPack>(
        new SettingsPack(id, name, desc, settings));
}

std::shared_ptr<SettingsPack> SettingsPack::GetById(sqlite3* db, int id)
{
    sqlite3_stmt* stmt;
    int res = sqlite3_prepare_v2(
        db,
        "SELECT id,name,description,* FROM settings_pack WHERE id = $1",
        -1,
        &stmt,
        nullptr);

    if (res != SQLITE_OK)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to prepare SQLite statement: " << sqlite3_errmsg(db);
        throw SQLiteException();
    }

    res = sqlite3_bind_int(stmt, 1, id);

    if (res != SQLITE_OK)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to bind SQLite parameter: " << sqlite3_errmsg(db);
        throw SQLiteException();
    }

    std::shared_ptr<SettingsPack> result = nullptr;

    switch (sqlite3_step(stmt))
    {
    case SQLITE_ROW:
    {
        int id = sqlite3_column_int(stmt, 0);
        auto name = pt_sqlite3_column_std_string(stmt, 1);
        auto desc = pt_sqlite3_column_std_string(stmt, 2);
        lt::settings_pack sp;

        for (int i = 0; i < sqlite3_column_count(stmt); i++)
        {
            std::string columnName = sqlite3_column_name(stmt, i);

            if (Names().find(columnName) == Names().end())
            {
                continue;
            }

            int setting = lt::setting_by_name(columnName);

            if (setting == -1)
            {
                BOOST_LOG_TRIVIAL(warning) << "Unknown setting: " << columnName;
            }

            if (setting >= lt::settings_pack::string_type_base
                && setting < lt::settings_pack::max_string_setting_internal)
            {
                auto res = sqlite3_column_text(stmt, i);

                if (res != nullptr)
                {
                    sp.set_str(
                        setting,
                        std::string(
                            reinterpret_cast<const char*>(res),
                            sqlite3_column_bytes(stmt, i)));
                }
            }

            if (setting >= lt::settings_pack::bool_type_base
                && setting < lt::settings_pack::max_bool_setting_internal)
            {
                sp.set_bool(setting, sqlite3_column_int(stmt, i) == 1 ? true : false);
            }

            if (setting >= lt::settings_pack::int_type_base
                && setting < lt::settings_pack::max_int_setting_internal)
            {
                sp.set_int(setting, sqlite3_column_int(stmt, i));
            }
        }

        result.reset(new SettingsPack(id, name.value(), desc.value(), sp));

        break;
    }
    case SQLITE_DONE:
    {
        BOOST_LOG_TRIVIAL(warning) << "No settings_pack available with id " << id;
        break;
    }
    default:
    {
        BOOST_LOG_TRIVIAL(error) << "Error when reading settings_pack: " << sqlite3_errmsg(db);
        break;
    }
    }

    sqlite3_finalize(stmt);

    return result;
}
