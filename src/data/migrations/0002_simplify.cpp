#include "0002_simplify.hpp"

#include <libtorrent/session.hpp>
#include <libtorrent/settings_pack.hpp>

#include "../statement.hpp"

#define BIND_SETTING_BOOL(pack, setting) sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, "$" # setting), df.get_bool(lt::settings_pack:: setting) ? 1 : 0);
#define BIND_SETTING_INT(pack, setting) sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, "$" # setting), df.get_int(lt::settings_pack:: setting));
#define BIND_SETTING_STR(pack, setting) sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "$" # setting), df.get_str(lt::settings_pack:: setting).c_str(), -1, SQLITE_TRANSIENT);

namespace lt = libtorrent;
using pt::Server::Data::Migrations::Simplify;

int Simplify::Migrate(sqlite3 *db)
{
    // delete existing settings packs
    int res = sqlite3_exec(db, "DELETE FROM settings_pack", nullptr, nullptr, nullptr);

    if (res != SQLITE_OK)
    {
        return res;
    }

    // add all settings pack columns
    res = sqlite3_exec(
        db,
        // string types
        "ALTER TABLE settings_pack ADD COLUMN outgoing_interfaces TEXT NOT NULL;\n"
        "ALTER TABLE settings_pack ADD COLUMN listen_interfaces   TEXT NOT NULL;\n"
        "ALTER TABLE settings_pack ADD COLUMN proxy_hostname      TEXT NOT NULL;\n"
        "ALTER TABLE settings_pack ADD COLUMN proxy_username      TEXT NOT NULL;\n"
        "ALTER TABLE settings_pack ADD COLUMN proxy_password      TEXT NOT NULL;\n"
        "ALTER TABLE settings_pack ADD COLUMN i2p_hostname        TEXT NOT NULL;\n"
        "ALTER TABLE settings_pack ADD COLUMN dht_bootstrap_nodes TEXT NOT NULL;\n"

        // bool types
        "ALTER TABLE settings_pack ADD COLUMN socks5_udp_send_local_ep  INTEGER NOT NULL CHECK(socks5_udp_send_local_ep  IN (0,1));\n"
        "ALTER TABLE settings_pack ADD COLUMN proxy_hostnames           INTEGER NOT NULL CHECK(proxy_hostnames           IN (0,1));\n"
        "ALTER TABLE settings_pack ADD COLUMN proxy_peer_connections    INTEGER NOT NULL CHECK(proxy_peer_connections    IN (0,1));\n"
        "ALTER TABLE settings_pack ADD COLUMN proxy_tracker_connections INTEGER NOT NULL CHECK(proxy_tracker_connections IN (0,1));\n"
        "ALTER TABLE settings_pack ADD COLUMN allow_i2p_mixed           INTEGER NOT NULL CHECK(allow_i2p_mixed           IN (0,1));\n"
        // deprecated
        "ALTER TABLE settings_pack ADD COLUMN ignore_resume_timestamps  INTEGER NOT NULL CHECK(ignore_resume_timestamps  IN (0,1));\n"

        // int types
        "ALTER TABLE settings_pack ADD COLUMN alert_mask                      INTEGER NOT NULL;\n"
        "ALTER TABLE settings_pack ADD COLUMN proxy_type                      INTEGER NOT NULL;\n"
        "ALTER TABLE settings_pack ADD COLUMN proxy_port                      INTEGER NOT NULL;\n"
        "ALTER TABLE settings_pack ADD COLUMN i2p_port                        INTEGER NOT NULL;\n"
        "ALTER TABLE settings_pack ADD COLUMN metadata_token_limit            INTEGER NOT NULL;\n"
        "ALTER TABLE settings_pack ADD COLUMN num_outgoing_ports              INTEGER NOT NULL;\n"
        // deprecated
        "ALTER TABLE settings_pack ADD COLUMN default_est_reciprocation_rate  INTEGER NOT NULL;\n"
        "ALTER TABLE settings_pack ADD COLUMN increase_est_reciprocation_rate INTEGER NOT NULL;\n"
        "ALTER TABLE settings_pack ADD COLUMN decrease_est_reciprocation_rate INTEGER NOT NULL;\n"
        "ALTER TABLE settings_pack ADD COLUMN utp_delayed_ack                 INTEGER NOT NULL;\n"
        ,
        nullptr,
        nullptr,
        nullptr);

    if (res != SQLITE_OK)
    {
        return res;
    }

    sqlite3_stmt* stmt;
    res = sqlite3_prepare_v2(
        db,
        "INSERT INTO settings_pack ("
        "name,"
        "description,"
        // string
        "user_agent,"
        "announce_ip,"
        "handshake_client_version,"
        "peer_fingerprint,"
        "outgoing_interfaces,"
        "listen_interfaces,"
        "proxy_hostname,"
        "proxy_username,"
        "proxy_password,"
        "i2p_hostname,"
        "dht_bootstrap_nodes,"
        // bool
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
        "socks5_udp_send_local_ep,"
        "proxy_hostnames,"
        "proxy_peer_connections,"
        "proxy_tracker_connections,"
        "allow_i2p_mixed,"
        "ignore_resume_timestamps,"
        // int
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
        "max_piece_count,"
        "alert_mask,"
        "proxy_type,"
        "proxy_port,"
        "i2p_port,"
        "metadata_token_limit,"
        "num_outgoing_ports,"
        "default_est_reciprocation_rate,"
        "increase_est_reciprocation_rate,"
        "decrease_est_reciprocation_rate,"
        "utp_delayed_ack"
        ") VALUES ("
        "$name,"
        "$description,"
        // string
        "$outgoing_interfaces,"
        "$listen_interfaces,"
        "$proxy_hostname,"
        "$proxy_username,"
        "$proxy_password,"
        "$i2p_hostname,"
        "$dht_bootstrap_nodes,"
        "$user_agent,"
        "$announce_ip,"
        "$handshake_client_version,"
        "$peer_fingerprint,"
        // bool
        "$allow_multiple_connections_per_ip,"
        "$send_redundant_have,"
        "$use_dht_as_fallback,"
        "$upnp_ignore_nonrouters,"
        "$use_parole_mode,"
        "$auto_manage_prefer_seeds,"
        "$dont_count_slow_torrents,"
        "$close_redundant_connections,"
        "$prioritize_partial_pieces,"
        "$rate_limit_ip_overhead,"
        "$announce_to_all_tiers,"
        "$announce_to_all_trackers,"
        "$prefer_udp_trackers,"
        "$disable_hash_checks,"
        "$volatile_read_cache,"
        "$no_atime_storage,"
        "$incoming_starts_queued_torrents,"
        "$report_true_downloaded,"
        "$strict_end_game_mode,"
        "$enable_outgoing_utp,"
        "$enable_incoming_utp,"
        "$enable_outgoing_tcp,"
        "$enable_incoming_tcp,"
        "$no_recheck_incomplete_resume,"
        "$anonymous_mode,"
        "$report_web_seed_downloads,"
        "$seeding_outgoing_connections,"
        "$no_connect_privileged_ports,"
        "$smooth_connects,"
        "$always_send_user_agent,"
        "$apply_ip_filter_to_trackers,"
        "$ban_web_seeds,"
        "$allow_partial_disk_writes,"
        "$support_share_mode,"
        "$report_redundant_bytes,"
        "$listen_system_port_fallback,"
        "$announce_crypto_support,"
        "$enable_upnp,"
        "$enable_natpmp,"
        "$enable_lsd,"
        "$enable_dht,"
        "$prefer_rc4,"
        "$auto_sequential,"
        "$enable_ip_notifier,"
        "$dht_prefer_verified_node_ids,"
        "$dht_restrict_routing_ips,"
        "$dht_restrict_search_ips,"
        "$dht_extended_routing_table,"
        "$dht_aggressive_lookups,"
        "$dht_privacy_lookups,"
        "$dht_enforce_node_id,"
        "$dht_ignore_dark_internet,"
        "$dht_read_only,"
        "$piece_extent_affinity,"
        "$validate_https_trackers,"
        "$ssrf_mitigation,"
        "$allow_idna,"
        "$enable_set_file_valid_data,"
        "$socks5_udp_send_local_ep,"
        "$proxy_hostnames,"
        "$proxy_peer_connections,"
        "$proxy_tracker_connections,"
        "$allow_i2p_mixed,"
        "0," // ignore_resume_timestamps
        // int
        "$tracker_completion_timeout,"
        "$tracker_receive_timeout,"
        "$stop_tracker_timeout,"
        "$tracker_maximum_response_length,"
        "$piece_timeout,"
        "$request_timeout,"
        "$request_queue_time,"
        "$max_allowed_in_request_queue,"
        "$max_out_request_queue,"
        "$whole_pieces_threshold,"
        "$peer_timeout,"
        "$urlseed_timeout,"
        "$urlseed_pipeline_size,"
        "$urlseed_wait_retry,"
        "$file_pool_size,"
        "$max_failcount,"
        "$min_reconnect_time,"
        "$peer_connect_timeout,"
        "$connection_speed,"
        "$inactivity_timeout,"
        "$unchoke_interval,"
        "$optimistic_unchoke_interval,"
        "$num_want,"
        "$initial_picker_threshold,"
        "$allowed_fast_set_size,"
        "$suggest_mode,"
        "$max_queued_disk_bytes,"
        "$handshake_timeout,"
        "$send_buffer_low_watermark,"
        "$send_buffer_watermark,"
        "$send_buffer_watermark_factor,"
        "$choking_algorithm,"
        "$seed_choking_algorithm,"
        "$disk_io_write_mode,"
        "$disk_io_read_mode,"
        "$outgoing_port,"
        "$peer_tos,"
        "$active_downloads,"
        "$active_seeds,"
        "$active_checking,"
        "$active_dht_limit,"
        "$active_tracker_limit,"
        "$active_lsd_limit,"
        "$active_limit,"
        "$auto_manage_interval,"
        "$seed_time_limit,"
        "$auto_scrape_interval,"
        "$auto_scrape_min_interval,"
        "$max_peerlist_size,"
        "$max_paused_peerlist_size,"
        "$min_announce_interval,"
        "$auto_manage_startup,"
        "$seeding_piece_quota,"
        "$max_rejects,"
        "$recv_socket_buffer_size,"
        "$send_socket_buffer_size,"
        "$max_peer_recv_buffer_size,"
        "$read_cache_line_size,"
        "$write_cache_line_size,"
        "$optimistic_disk_retry,"
        "$max_suggest_pieces,"
        "$local_service_announce_interval,"
        "$dht_announce_interval,"
        "$udp_tracker_token_expiry,"
        "$num_optimistic_unchoke_slots,"
        "$max_pex_peers,"
        "$tick_interval,"
        "$share_mode_target,"
        "$upload_rate_limit,"
        "$download_rate_limit,"
        "$dht_upload_rate_limit,"
        "$unchoke_slots_limit,"
        "$connections_limit,"
        "$connections_slack,"
        "$utp_target_delay,"
        "$utp_gain_factor,"
        "$utp_min_timeout,"
        "$utp_syn_resends,"
        "$utp_fin_resends,"
        "$utp_num_resends,"
        "$utp_connect_timeout,"
        "$utp_loss_multiplier,"
        "$mixed_mode_algorithm,"
        "$listen_queue_size,"
        "$torrent_connect_boost,"
        "$alert_queue_size,"
        "$max_metadata_size,"
        "$hashing_threads,"
        "$checking_mem_usage,"
        "$predictive_piece_announce,"
        "$aio_threads,"
        "$tracker_backoff,"
        "$share_ratio_limit,"
        "$seed_time_ratio_limit,"
        "$peer_turnover,"
        "$peer_turnover_cutoff,"
        "$peer_turnover_interval,"
        "$connect_seed_every_n_download,"
        "$max_http_recv_buffer_size,"
        "$max_retry_port_bind,"
        "$out_enc_policy,"
        "$in_enc_policy,"
        "$allowed_enc_level,"
        "$inactive_down_rate,"
        "$inactive_up_rate,"
        "$urlseed_max_request_bytes,"
        "$web_seed_name_lookup_retry,"
        "$close_file_interval,"
        "$utp_cwnd_reduce_timer,"
        "$max_web_seed_connections,"
        "$resolver_cache_timeout,"
        "$send_not_sent_low_watermark,"
        "$rate_choker_initial_threshold,"
        "$upnp_lease_duration,"
        "$max_concurrent_http_announces,"
        "$dht_max_peers_reply,"
        "$dht_search_branching,"
        "$dht_max_fail_count,"
        "$dht_max_torrents,"
        "$dht_max_dht_items,"
        "$dht_max_peers,"
        "$dht_max_torrent_search_reply,"
        "$dht_block_timeout,"
        "$dht_block_ratelimit,"
        "$dht_item_lifetime,"
        "$dht_sample_infohashes_interval,"
        "$dht_max_infohashes_sample_count,"
        "$max_piece_count,"
        "$alert_mask,"
        "$proxy_type,"
        "$proxy_port,"
        "$i2p_port,"
        "$metadata_token_limit,"
        "$num_outgoing_ports,"
        "0," // default_est_reciprocation_rate
        "0," // increase_est_reciprocation_rate
        "0," // decrease_est_reciprocation_rate
        "0" // utp_delayed_ack
        "); SELECT last_insert_rowid();",
        -1,
        &stmt,
        nullptr);

    if (res != SQLITE_OK) { return res; }

    auto df = lt::default_settings();
    df.set_int(lt::settings_pack::alert_mask, lt::alert_category::all);

    sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "$name"), "Default", -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "$description"), "The default settings from libtorrent", -1, SQLITE_TRANSIENT);

    BIND_SETTING_STR(df, user_agent)
    BIND_SETTING_STR(df, announce_ip)
    BIND_SETTING_STR(df, handshake_client_version)
    BIND_SETTING_STR(df, peer_fingerprint)
    BIND_SETTING_STR(df, outgoing_interfaces)
    BIND_SETTING_STR(df, listen_interfaces)
    BIND_SETTING_STR(df, proxy_hostname)
    BIND_SETTING_STR(df, proxy_username)
    BIND_SETTING_STR(df, proxy_password)
    BIND_SETTING_STR(df, i2p_hostname)
    BIND_SETTING_STR(df, dht_bootstrap_nodes)

    BIND_SETTING_BOOL(df, allow_multiple_connections_per_ip)
    BIND_SETTING_BOOL(df, send_redundant_have)
    BIND_SETTING_BOOL(df, use_dht_as_fallback)
    BIND_SETTING_BOOL(df, upnp_ignore_nonrouters)
    BIND_SETTING_BOOL(df, use_parole_mode)
    BIND_SETTING_BOOL(df, auto_manage_prefer_seeds)
    BIND_SETTING_BOOL(df, dont_count_slow_torrents)
    BIND_SETTING_BOOL(df, close_redundant_connections)
    BIND_SETTING_BOOL(df, prioritize_partial_pieces)
    BIND_SETTING_BOOL(df, rate_limit_ip_overhead)
    BIND_SETTING_BOOL(df, announce_to_all_tiers)
    BIND_SETTING_BOOL(df, announce_to_all_trackers)
    BIND_SETTING_BOOL(df, prefer_udp_trackers)
    BIND_SETTING_BOOL(df, disable_hash_checks)
    BIND_SETTING_BOOL(df, volatile_read_cache)
    BIND_SETTING_BOOL(df, no_atime_storage)
    BIND_SETTING_BOOL(df, incoming_starts_queued_torrents)
    BIND_SETTING_BOOL(df, report_true_downloaded)
    BIND_SETTING_BOOL(df, strict_end_game_mode)
    BIND_SETTING_BOOL(df, enable_outgoing_utp)
    BIND_SETTING_BOOL(df, enable_incoming_utp)
    BIND_SETTING_BOOL(df, enable_outgoing_tcp)
    BIND_SETTING_BOOL(df, enable_incoming_tcp)
    BIND_SETTING_BOOL(df, no_recheck_incomplete_resume)
    BIND_SETTING_BOOL(df, anonymous_mode)
    BIND_SETTING_BOOL(df, report_web_seed_downloads)
    BIND_SETTING_BOOL(df, seeding_outgoing_connections)
    BIND_SETTING_BOOL(df, no_connect_privileged_ports)
    BIND_SETTING_BOOL(df, smooth_connects)
    BIND_SETTING_BOOL(df, always_send_user_agent)
    BIND_SETTING_BOOL(df, apply_ip_filter_to_trackers)
    BIND_SETTING_BOOL(df, ban_web_seeds)
    BIND_SETTING_BOOL(df, allow_partial_disk_writes)
    BIND_SETTING_BOOL(df, support_share_mode)
    BIND_SETTING_BOOL(df, report_redundant_bytes)
    BIND_SETTING_BOOL(df, listen_system_port_fallback)
    BIND_SETTING_BOOL(df, announce_crypto_support)
    BIND_SETTING_BOOL(df, enable_upnp)
    BIND_SETTING_BOOL(df, enable_natpmp)
    BIND_SETTING_BOOL(df, enable_lsd)
    BIND_SETTING_BOOL(df, enable_dht)
    BIND_SETTING_BOOL(df, prefer_rc4)
    BIND_SETTING_BOOL(df, auto_sequential)
    BIND_SETTING_BOOL(df, enable_ip_notifier)
    BIND_SETTING_BOOL(df, dht_prefer_verified_node_ids)
    BIND_SETTING_BOOL(df, dht_restrict_routing_ips)
    BIND_SETTING_BOOL(df, dht_restrict_search_ips)
    BIND_SETTING_BOOL(df, dht_extended_routing_table)
    BIND_SETTING_BOOL(df, dht_aggressive_lookups)
    BIND_SETTING_BOOL(df, dht_privacy_lookups)
    BIND_SETTING_BOOL(df, dht_enforce_node_id)
    BIND_SETTING_BOOL(df, dht_ignore_dark_internet)
    BIND_SETTING_BOOL(df, dht_read_only)
    BIND_SETTING_BOOL(df, piece_extent_affinity)
    BIND_SETTING_BOOL(df, validate_https_trackers)
    BIND_SETTING_BOOL(df, ssrf_mitigation)
    BIND_SETTING_BOOL(df, allow_idna)
    BIND_SETTING_BOOL(df, enable_set_file_valid_data)
    BIND_SETTING_BOOL(df, socks5_udp_send_local_ep)
    BIND_SETTING_BOOL(df, proxy_hostnames)
    BIND_SETTING_BOOL(df, proxy_peer_connections)
    BIND_SETTING_BOOL(df, proxy_tracker_connections)
    BIND_SETTING_BOOL(df, allow_i2p_mixed)

    BIND_SETTING_INT(df, tracker_completion_timeout)
    BIND_SETTING_INT(df, tracker_receive_timeout)
    BIND_SETTING_INT(df, stop_tracker_timeout)
    BIND_SETTING_INT(df, tracker_maximum_response_length)
    BIND_SETTING_INT(df, piece_timeout)
    BIND_SETTING_INT(df, request_timeout)
    BIND_SETTING_INT(df, request_queue_time)
    BIND_SETTING_INT(df, max_allowed_in_request_queue)
    BIND_SETTING_INT(df, max_out_request_queue)
    BIND_SETTING_INT(df, whole_pieces_threshold)
    BIND_SETTING_INT(df, peer_timeout)
    BIND_SETTING_INT(df, urlseed_timeout)
    BIND_SETTING_INT(df, urlseed_pipeline_size)
    BIND_SETTING_INT(df, urlseed_wait_retry)
    BIND_SETTING_INT(df, file_pool_size)
    BIND_SETTING_INT(df, max_failcount)
    BIND_SETTING_INT(df, min_reconnect_time)
    BIND_SETTING_INT(df, peer_connect_timeout)
    BIND_SETTING_INT(df, connection_speed)
    BIND_SETTING_INT(df, inactivity_timeout)
    BIND_SETTING_INT(df, unchoke_interval)
    BIND_SETTING_INT(df, optimistic_unchoke_interval)
    BIND_SETTING_INT(df, num_want)
    BIND_SETTING_INT(df, initial_picker_threshold)
    BIND_SETTING_INT(df, allowed_fast_set_size)
    BIND_SETTING_INT(df, suggest_mode)
    BIND_SETTING_INT(df, max_queued_disk_bytes)
    BIND_SETTING_INT(df, handshake_timeout)
    BIND_SETTING_INT(df, send_buffer_low_watermark)
    BIND_SETTING_INT(df, send_buffer_watermark)
    BIND_SETTING_INT(df, send_buffer_watermark_factor)
    BIND_SETTING_INT(df, choking_algorithm)
    BIND_SETTING_INT(df, seed_choking_algorithm)
    BIND_SETTING_INT(df, disk_io_write_mode)
    BIND_SETTING_INT(df, disk_io_read_mode)
    BIND_SETTING_INT(df, outgoing_port)
    BIND_SETTING_INT(df, peer_tos)
    BIND_SETTING_INT(df, active_downloads)
    BIND_SETTING_INT(df, active_seeds)
    BIND_SETTING_INT(df, active_checking)
    BIND_SETTING_INT(df, active_dht_limit)
    BIND_SETTING_INT(df, active_tracker_limit)
    BIND_SETTING_INT(df, active_lsd_limit)
    BIND_SETTING_INT(df, active_limit)
    BIND_SETTING_INT(df, auto_manage_interval)
    BIND_SETTING_INT(df, seed_time_limit)
    BIND_SETTING_INT(df, auto_scrape_interval)
    BIND_SETTING_INT(df, auto_scrape_min_interval)
    BIND_SETTING_INT(df, max_peerlist_size)
    BIND_SETTING_INT(df, max_paused_peerlist_size)
    BIND_SETTING_INT(df, min_announce_interval)
    BIND_SETTING_INT(df, auto_manage_startup)
    BIND_SETTING_INT(df, seeding_piece_quota)
    BIND_SETTING_INT(df, max_rejects)
    BIND_SETTING_INT(df, recv_socket_buffer_size)
    BIND_SETTING_INT(df, send_socket_buffer_size)
    BIND_SETTING_INT(df, max_peer_recv_buffer_size)
    BIND_SETTING_INT(df, read_cache_line_size)
    BIND_SETTING_INT(df, write_cache_line_size)
    BIND_SETTING_INT(df, optimistic_disk_retry)
    BIND_SETTING_INT(df, max_suggest_pieces)
    BIND_SETTING_INT(df, local_service_announce_interval)
    BIND_SETTING_INT(df, dht_announce_interval)
    BIND_SETTING_INT(df, udp_tracker_token_expiry)
    BIND_SETTING_INT(df, num_optimistic_unchoke_slots)
    BIND_SETTING_INT(df, max_pex_peers)
    BIND_SETTING_INT(df, tick_interval)
    BIND_SETTING_INT(df, share_mode_target)
    BIND_SETTING_INT(df, upload_rate_limit)
    BIND_SETTING_INT(df, download_rate_limit)
    BIND_SETTING_INT(df, dht_upload_rate_limit)
    BIND_SETTING_INT(df, unchoke_slots_limit)
    BIND_SETTING_INT(df, connections_limit)
    BIND_SETTING_INT(df, connections_slack)
    BIND_SETTING_INT(df, utp_target_delay)
    BIND_SETTING_INT(df, utp_gain_factor)
    BIND_SETTING_INT(df, utp_min_timeout)
    BIND_SETTING_INT(df, utp_syn_resends)
    BIND_SETTING_INT(df, utp_fin_resends)
    BIND_SETTING_INT(df, utp_num_resends)
    BIND_SETTING_INT(df, utp_connect_timeout)
    BIND_SETTING_INT(df, utp_loss_multiplier)
    BIND_SETTING_INT(df, mixed_mode_algorithm)
    BIND_SETTING_INT(df, listen_queue_size)
    BIND_SETTING_INT(df, torrent_connect_boost)
    BIND_SETTING_INT(df, alert_queue_size)
    BIND_SETTING_INT(df, max_metadata_size)
    BIND_SETTING_INT(df, hashing_threads)
    BIND_SETTING_INT(df, checking_mem_usage)
    BIND_SETTING_INT(df, predictive_piece_announce)
    BIND_SETTING_INT(df, aio_threads)
    BIND_SETTING_INT(df, tracker_backoff)
    BIND_SETTING_INT(df, share_ratio_limit)
    BIND_SETTING_INT(df, seed_time_ratio_limit)
    BIND_SETTING_INT(df, peer_turnover)
    BIND_SETTING_INT(df, peer_turnover_cutoff)
    BIND_SETTING_INT(df, peer_turnover_interval)
    BIND_SETTING_INT(df, connect_seed_every_n_download)
    BIND_SETTING_INT(df, max_http_recv_buffer_size)
    BIND_SETTING_INT(df, max_retry_port_bind)
    BIND_SETTING_INT(df, out_enc_policy)
    BIND_SETTING_INT(df, in_enc_policy)
    BIND_SETTING_INT(df, allowed_enc_level)
    BIND_SETTING_INT(df, inactive_down_rate)
    BIND_SETTING_INT(df, inactive_up_rate)
    BIND_SETTING_INT(df, urlseed_max_request_bytes)
    BIND_SETTING_INT(df, web_seed_name_lookup_retry)
    BIND_SETTING_INT(df, close_file_interval)
    BIND_SETTING_INT(df, utp_cwnd_reduce_timer)
    BIND_SETTING_INT(df, max_web_seed_connections)
    BIND_SETTING_INT(df, resolver_cache_timeout)
    BIND_SETTING_INT(df, send_not_sent_low_watermark)
    BIND_SETTING_INT(df, rate_choker_initial_threshold)
    BIND_SETTING_INT(df, upnp_lease_duration)
    BIND_SETTING_INT(df, max_concurrent_http_announces)
    BIND_SETTING_INT(df, dht_max_peers_reply)
    BIND_SETTING_INT(df, dht_search_branching)
    BIND_SETTING_INT(df, dht_max_fail_count)
    BIND_SETTING_INT(df, dht_max_torrents)
    BIND_SETTING_INT(df, dht_max_dht_items)
    BIND_SETTING_INT(df, dht_max_peers)
    BIND_SETTING_INT(df, dht_max_torrent_search_reply)
    BIND_SETTING_INT(df, dht_block_timeout)
    BIND_SETTING_INT(df, dht_block_ratelimit)
    BIND_SETTING_INT(df, dht_item_lifetime)
    BIND_SETTING_INT(df, dht_sample_infohashes_interval)
    BIND_SETTING_INT(df, dht_max_infohashes_sample_count)
    BIND_SETTING_INT(df, max_piece_count)
    BIND_SETTING_INT(df, alert_mask)
    BIND_SETTING_INT(df, proxy_type)
    BIND_SETTING_INT(df, proxy_port)
    BIND_SETTING_INT(df, i2p_port)
    BIND_SETTING_INT(df, metadata_token_limit)
    BIND_SETTING_INT(df, num_outgoing_ports)

    res = sqlite3_step(stmt);
    if (res != SQLITE_DONE) { return res; }

    Data::Statement::ForEach(
        db,
        "UPDATE settings_pack SET "
        "proxy_hostname            = (SELECT hostname                  FROM proxy WHERE id = (SELECT proxy_id FROM profiles WHERE is_active = 1)),"
        "proxy_port                = (SELECT port                      FROM proxy WHERE id = (SELECT proxy_id FROM profiles WHERE is_active = 1)),"
        "proxy_type                = (SELECT type                      FROM proxy WHERE id = (SELECT proxy_id FROM profiles WHERE is_active = 1)),"
        "proxy_username            = (SELECT username                  FROM proxy WHERE id = (SELECT proxy_id FROM profiles WHERE is_active = 1)),"
        "proxy_password            = (SELECT password                  FROM proxy WHERE id = (SELECT proxy_id FROM profiles WHERE is_active = 1)),"
        "proxy_hostnames           = (SELECT proxy_hostnames           FROM proxy WHERE id = (SELECT proxy_id FROM profiles WHERE is_active = 1)),"
        "proxy_peer_connections    = (SELECT proxy_peer_connections    FROM proxy WHERE id = (SELECT proxy_id FROM profiles WHERE is_active = 1)),"
        "proxy_tracker_connections = (SELECT proxy_tracker_connections FROM proxy WHERE id = (SELECT proxy_id FROM profiles WHERE is_active = 1))"
        ,
        [](auto){},
        [](auto){});

    std::stringstream incoming;
    std::stringstream outgoing;

    Data::Statement::ForEach(
        db,
        "SELECT host,port,is_local,is_outgoing,is_ssl FROM listen_interfaces",
        [&incoming, &outgoing](Data::Statement::Row const& row)
        {
            incoming << ","
                     << row.GetStdString(0)
                     << ":"
                     << row.GetInt32(1)
                     << (row.GetInt32(2) == 1 ? "l" : "")
                     << (row.GetInt32(4) == 1 ? "s" : "");

            if (row.GetInt32(3) == 1)
            {
                outgoing << ","
                         << row.GetStdString(0);
            }
        },
        [](auto){});

    res = sqlite3_exec(db, "DROP TABLE listen_interfaces", nullptr, nullptr, nullptr);
    if (res != SQLITE_OK) { return res; }

    res = sqlite3_exec(db, "DROP TABLE profiles", nullptr, nullptr, nullptr);
    if (res != SQLITE_OK) { return res; }

    res = sqlite3_exec(db, "DROP TABLE proxy", nullptr, nullptr, nullptr);
    if (res != SQLITE_OK) { return res; }

    // create sessions table
    res = sqlite3_exec(
        db,
        "CREATE TABLE sessions ("
            "id               INTEGER PRIMARY KEY,"
            "settings_pack_id INTEGER NOT NULL UNIQUE REFERENCES settings_pack(id),"
            "name             TEXT    NOT NULL,"
            "description      TEXT        NULL,"
            "created_at       INTEGER NOT NULL DEFAULT (strftime('%s')),"
            "params_data      BLOB        NULL"
        ");",
        nullptr,
        nullptr,
        nullptr);

    if (res != SQLITE_OK)
    {
        return res;
    }

    // create default session
    res = sqlite3_exec(
        db,
        "INSERT INTO sessions (name, description, settings_pack_id, params_data) "
        "VALUES ('Default', 'Default libtorrent session', (SELECT id FROM settings_pack WHERE name = 'Default'), (SELECT data FROM session_params ORDER BY timestamp DESC LIMIT 1));",
        nullptr,
        nullptr,
        nullptr);

    if (res != SQLITE_OK)
    {
        return res;
    }

    if (!incoming.str().empty())
    {
        Data::Statement::ForEach(
            db,
            "UPDATE settings_pack SET listen_interfaces = $1 WHERE id = (SELECT settings_pack_id FROM sessions WHERE name = 'Default')",
            [](auto const&) {},
            [&incoming](sqlite3_stmt* stmt)
            {
                sqlite3_bind_text(stmt, 1, incoming.str().substr(1).c_str(), static_cast<int>(incoming.str().substr(1).size()), SQLITE_TRANSIENT);
            });
    }

    if (!outgoing.str().empty())
    {
        Data::Statement::ForEach(
            db,
            "UPDATE settings_pack SET outgoing_interfaces = $1 WHERE id = (SELECT settings_pack_id FROM sessions WHERE name = 'Default')",
            [](auto const&) {},
            [&outgoing](sqlite3_stmt* stmt)
            {
                sqlite3_bind_text(stmt, 1, outgoing.str().substr(1).c_str(), static_cast<int>(outgoing.str().substr(1).size()), SQLITE_TRANSIENT);
            });
    }

    // Drop session_params
    res = sqlite3_exec(db, "DROP TABLE session_params", nullptr, nullptr, nullptr);
    if (res != SQLITE_OK) { return res; }

    // Add FK from torrents to session
    res = sqlite3_exec(db, "ALTER TABLE torrents ADD COLUMN session_id INTEGER NULL REFERENCES sessions(id)", nullptr, nullptr, nullptr);
    if (res != SQLITE_OK) { return res; }

    // Update torrents, set default session id
    res = sqlite3_exec(db, "UPDATE torrents SET session_id = (SELECT id FROM sessions WHERE name = 'Default')", nullptr, nullptr, nullptr);
    if (res != SQLITE_OK) { return res; }

    // create new torrents table
    res = sqlite3_exec(
        db,
        "CREATE TABLE torrents_new ("
            "info_hash      TEXT PRIMARY KEY,"
            "session_id     INTEGER NOT NULL REFERENCES sessions(id),"
            "queue_position INTEGER NOT NULL,"
            "resume_data    BLOB NULL,"
            "torrent_data   BLOB NULL,"
            "timestamp      INTEGER NOT NULL DEFAULT (strftime('%s'))"
        ");", nullptr, nullptr, nullptr);

    if (res != SQLITE_OK) { return res; }

    res = sqlite3_exec(
        db,
        "INSERT INTO torrents_new SELECT * from torrents; DROP TABLE torrents; ALTER TABLE torrents_new RENAME TO torrents;",
        nullptr,
        nullptr,
        nullptr);

    if (res != SQLITE_OK) { return res; }

    return SQLITE_OK;
}
