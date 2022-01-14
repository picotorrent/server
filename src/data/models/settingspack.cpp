#include "settingspack.hpp"

#include <string>
#include <unordered_set>

#include <boost/log/trivial.hpp>

#include "../datareader.hpp"
#include "../sqliteexception.hpp"
#include "../statement.hpp"

#define BIND_SETTING_BOOL(pack, setting) sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, "$" # setting), pack.get_bool(lt::settings_pack:: setting) ? 1 : 0);
#define BIND_SETTING_INT(pack, setting) sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, "$" # setting), pack.get_int(lt::settings_pack:: setting));
#define BIND_SETTING_STR(pack, setting) sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "$" # setting), pack.get_str(lt::settings_pack:: setting).c_str(), -1, SQLITE_TRANSIENT);

namespace lt = libtorrent;
using pt::Server::Data::SettingsPack;
using pt::Server::Data::SQLiteException;
using pt::Server::Data::Statement;

std::shared_ptr<SettingsPack> SettingsPack::Create(
    sqlite3* db,
    const std::string& name,
    const std::string& description,
    const libtorrent::settings_pack& settings)
{
    int insertedId = -1;

    Statement::ForEach(
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
        "); SELECT last_insert_rowid() AS id;",
        [&](const Statement::Row& row)
        {
            insertedId = row.GetInt32("id");
        },
        [&](sqlite3_stmt* stmt)
        {
            sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "$name"), name.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "$description"), description.c_str(), -1, SQLITE_TRANSIENT);

            BIND_SETTING_STR(settings, user_agent)
            BIND_SETTING_STR(settings, announce_ip)
            BIND_SETTING_STR(settings, handshake_client_version)
            BIND_SETTING_STR(settings, peer_fingerprint)
            BIND_SETTING_STR(settings, outgoing_interfaces)
            BIND_SETTING_STR(settings, listen_interfaces)
            BIND_SETTING_STR(settings, proxy_hostname)
            BIND_SETTING_STR(settings, proxy_username)
            BIND_SETTING_STR(settings, proxy_password)
            BIND_SETTING_STR(settings, i2p_hostname)
            BIND_SETTING_STR(settings, dht_bootstrap_nodes)

            BIND_SETTING_BOOL(settings, allow_multiple_connections_per_ip)
            BIND_SETTING_BOOL(settings, send_redundant_have)
            BIND_SETTING_BOOL(settings, use_dht_as_fallback)
            BIND_SETTING_BOOL(settings, upnp_ignore_nonrouters)
            BIND_SETTING_BOOL(settings, use_parole_mode)
            BIND_SETTING_BOOL(settings, auto_manage_prefer_seeds)
            BIND_SETTING_BOOL(settings, dont_count_slow_torrents)
            BIND_SETTING_BOOL(settings, close_redundant_connections)
            BIND_SETTING_BOOL(settings, prioritize_partial_pieces)
            BIND_SETTING_BOOL(settings, rate_limit_ip_overhead)
            BIND_SETTING_BOOL(settings, announce_to_all_tiers)
            BIND_SETTING_BOOL(settings, announce_to_all_trackers)
            BIND_SETTING_BOOL(settings, prefer_udp_trackers)
            BIND_SETTING_BOOL(settings, disable_hash_checks)
            BIND_SETTING_BOOL(settings, volatile_read_cache)
            BIND_SETTING_BOOL(settings, no_atime_storage)
            BIND_SETTING_BOOL(settings, incoming_starts_queued_torrents)
            BIND_SETTING_BOOL(settings, report_true_downloaded)
            BIND_SETTING_BOOL(settings, strict_end_game_mode)
            BIND_SETTING_BOOL(settings, enable_outgoing_utp)
            BIND_SETTING_BOOL(settings, enable_incoming_utp)
            BIND_SETTING_BOOL(settings, enable_outgoing_tcp)
            BIND_SETTING_BOOL(settings, enable_incoming_tcp)
            BIND_SETTING_BOOL(settings, no_recheck_incomplete_resume)
            BIND_SETTING_BOOL(settings, anonymous_mode)
            BIND_SETTING_BOOL(settings, report_web_seed_downloads)
            BIND_SETTING_BOOL(settings, seeding_outgoing_connections)
            BIND_SETTING_BOOL(settings, no_connect_privileged_ports)
            BIND_SETTING_BOOL(settings, smooth_connects)
            BIND_SETTING_BOOL(settings, always_send_user_agent)
            BIND_SETTING_BOOL(settings, apply_ip_filter_to_trackers)
            BIND_SETTING_BOOL(settings, ban_web_seeds)
            BIND_SETTING_BOOL(settings, allow_partial_disk_writes)
            BIND_SETTING_BOOL(settings, support_share_mode)
            BIND_SETTING_BOOL(settings, report_redundant_bytes)
            BIND_SETTING_BOOL(settings, listen_system_port_fallback)
            BIND_SETTING_BOOL(settings, announce_crypto_support)
            BIND_SETTING_BOOL(settings, enable_upnp)
            BIND_SETTING_BOOL(settings, enable_natpmp)
            BIND_SETTING_BOOL(settings, enable_lsd)
            BIND_SETTING_BOOL(settings, enable_dht)
            BIND_SETTING_BOOL(settings, prefer_rc4)
            BIND_SETTING_BOOL(settings, auto_sequential)
            BIND_SETTING_BOOL(settings, enable_ip_notifier)
            BIND_SETTING_BOOL(settings, dht_prefer_verified_node_ids)
            BIND_SETTING_BOOL(settings, dht_restrict_routing_ips)
            BIND_SETTING_BOOL(settings, dht_restrict_search_ips)
            BIND_SETTING_BOOL(settings, dht_extended_routing_table)
            BIND_SETTING_BOOL(settings, dht_aggressive_lookups)
            BIND_SETTING_BOOL(settings, dht_privacy_lookups)
            BIND_SETTING_BOOL(settings, dht_enforce_node_id)
            BIND_SETTING_BOOL(settings, dht_ignore_dark_internet)
            BIND_SETTING_BOOL(settings, dht_read_only)
            BIND_SETTING_BOOL(settings, piece_extent_affinity)
            BIND_SETTING_BOOL(settings, validate_https_trackers)
            BIND_SETTING_BOOL(settings, ssrf_mitigation)
            BIND_SETTING_BOOL(settings, allow_idna)
            BIND_SETTING_BOOL(settings, enable_set_file_valid_data)
            BIND_SETTING_BOOL(settings, socks5_udp_send_local_ep)
            BIND_SETTING_BOOL(settings, proxy_hostnames)
            BIND_SETTING_BOOL(settings, proxy_peer_connections)
            BIND_SETTING_BOOL(settings, proxy_tracker_connections)
            BIND_SETTING_BOOL(settings, allow_i2p_mixed)

            BIND_SETTING_INT(settings, tracker_completion_timeout)
            BIND_SETTING_INT(settings, tracker_receive_timeout)
            BIND_SETTING_INT(settings, stop_tracker_timeout)
            BIND_SETTING_INT(settings, tracker_maximum_response_length)
            BIND_SETTING_INT(settings, piece_timeout)
            BIND_SETTING_INT(settings, request_timeout)
            BIND_SETTING_INT(settings, request_queue_time)
            BIND_SETTING_INT(settings, max_allowed_in_request_queue)
            BIND_SETTING_INT(settings, max_out_request_queue)
            BIND_SETTING_INT(settings, whole_pieces_threshold)
            BIND_SETTING_INT(settings, peer_timeout)
            BIND_SETTING_INT(settings, urlseed_timeout)
            BIND_SETTING_INT(settings, urlseed_pipeline_size)
            BIND_SETTING_INT(settings, urlseed_wait_retry)
            BIND_SETTING_INT(settings, file_pool_size)
            BIND_SETTING_INT(settings, max_failcount)
            BIND_SETTING_INT(settings, min_reconnect_time)
            BIND_SETTING_INT(settings, peer_connect_timeout)
            BIND_SETTING_INT(settings, connection_speed)
            BIND_SETTING_INT(settings, inactivity_timeout)
            BIND_SETTING_INT(settings, unchoke_interval)
            BIND_SETTING_INT(settings, optimistic_unchoke_interval)
            BIND_SETTING_INT(settings, num_want)
            BIND_SETTING_INT(settings, initial_picker_threshold)
            BIND_SETTING_INT(settings, allowed_fast_set_size)
            BIND_SETTING_INT(settings, suggest_mode)
            BIND_SETTING_INT(settings, max_queued_disk_bytes)
            BIND_SETTING_INT(settings, handshake_timeout)
            BIND_SETTING_INT(settings, send_buffer_low_watermark)
            BIND_SETTING_INT(settings, send_buffer_watermark)
            BIND_SETTING_INT(settings, send_buffer_watermark_factor)
            BIND_SETTING_INT(settings, choking_algorithm)
            BIND_SETTING_INT(settings, seed_choking_algorithm)
            BIND_SETTING_INT(settings, disk_io_write_mode)
            BIND_SETTING_INT(settings, disk_io_read_mode)
            BIND_SETTING_INT(settings, outgoing_port)
            BIND_SETTING_INT(settings, peer_tos)
            BIND_SETTING_INT(settings, active_downloads)
            BIND_SETTING_INT(settings, active_seeds)
            BIND_SETTING_INT(settings, active_checking)
            BIND_SETTING_INT(settings, active_dht_limit)
            BIND_SETTING_INT(settings, active_tracker_limit)
            BIND_SETTING_INT(settings, active_lsd_limit)
            BIND_SETTING_INT(settings, active_limit)
            BIND_SETTING_INT(settings, auto_manage_interval)
            BIND_SETTING_INT(settings, seed_time_limit)
            BIND_SETTING_INT(settings, auto_scrape_interval)
            BIND_SETTING_INT(settings, auto_scrape_min_interval)
            BIND_SETTING_INT(settings, max_peerlist_size)
            BIND_SETTING_INT(settings, max_paused_peerlist_size)
            BIND_SETTING_INT(settings, min_announce_interval)
            BIND_SETTING_INT(settings, auto_manage_startup)
            BIND_SETTING_INT(settings, seeding_piece_quota)
            BIND_SETTING_INT(settings, max_rejects)
            BIND_SETTING_INT(settings, recv_socket_buffer_size)
            BIND_SETTING_INT(settings, send_socket_buffer_size)
            BIND_SETTING_INT(settings, max_peer_recv_buffer_size)
            BIND_SETTING_INT(settings, read_cache_line_size)
            BIND_SETTING_INT(settings, write_cache_line_size)
            BIND_SETTING_INT(settings, optimistic_disk_retry)
            BIND_SETTING_INT(settings, max_suggest_pieces)
            BIND_SETTING_INT(settings, local_service_announce_interval)
            BIND_SETTING_INT(settings, dht_announce_interval)
            BIND_SETTING_INT(settings, udp_tracker_token_expiry)
            BIND_SETTING_INT(settings, num_optimistic_unchoke_slots)
            BIND_SETTING_INT(settings, max_pex_peers)
            BIND_SETTING_INT(settings, tick_interval)
            BIND_SETTING_INT(settings, share_mode_target)
            BIND_SETTING_INT(settings, upload_rate_limit)
            BIND_SETTING_INT(settings, download_rate_limit)
            BIND_SETTING_INT(settings, dht_upload_rate_limit)
            BIND_SETTING_INT(settings, unchoke_slots_limit)
            BIND_SETTING_INT(settings, connections_limit)
            BIND_SETTING_INT(settings, connections_slack)
            BIND_SETTING_INT(settings, utp_target_delay)
            BIND_SETTING_INT(settings, utp_gain_factor)
            BIND_SETTING_INT(settings, utp_min_timeout)
            BIND_SETTING_INT(settings, utp_syn_resends)
            BIND_SETTING_INT(settings, utp_fin_resends)
            BIND_SETTING_INT(settings, utp_num_resends)
            BIND_SETTING_INT(settings, utp_connect_timeout)
            BIND_SETTING_INT(settings, utp_loss_multiplier)
            BIND_SETTING_INT(settings, mixed_mode_algorithm)
            BIND_SETTING_INT(settings, listen_queue_size)
            BIND_SETTING_INT(settings, torrent_connect_boost)
            BIND_SETTING_INT(settings, alert_queue_size)
            BIND_SETTING_INT(settings, max_metadata_size)
            BIND_SETTING_INT(settings, hashing_threads)
            BIND_SETTING_INT(settings, checking_mem_usage)
            BIND_SETTING_INT(settings, predictive_piece_announce)
            BIND_SETTING_INT(settings, aio_threads)
            BIND_SETTING_INT(settings, tracker_backoff)
            BIND_SETTING_INT(settings, share_ratio_limit)
            BIND_SETTING_INT(settings, seed_time_ratio_limit)
            BIND_SETTING_INT(settings, peer_turnover)
            BIND_SETTING_INT(settings, peer_turnover_cutoff)
            BIND_SETTING_INT(settings, peer_turnover_interval)
            BIND_SETTING_INT(settings, connect_seed_every_n_download)
            BIND_SETTING_INT(settings, max_http_recv_buffer_size)
            BIND_SETTING_INT(settings, max_retry_port_bind)
            BIND_SETTING_INT(settings, out_enc_policy)
            BIND_SETTING_INT(settings, in_enc_policy)
            BIND_SETTING_INT(settings, allowed_enc_level)
            BIND_SETTING_INT(settings, inactive_down_rate)
            BIND_SETTING_INT(settings, inactive_up_rate)
            BIND_SETTING_INT(settings, urlseed_max_request_bytes)
            BIND_SETTING_INT(settings, web_seed_name_lookup_retry)
            BIND_SETTING_INT(settings, close_file_interval)
            BIND_SETTING_INT(settings, utp_cwnd_reduce_timer)
            BIND_SETTING_INT(settings, max_web_seed_connections)
            BIND_SETTING_INT(settings, resolver_cache_timeout)
            BIND_SETTING_INT(settings, send_not_sent_low_watermark)
            BIND_SETTING_INT(settings, rate_choker_initial_threshold)
            BIND_SETTING_INT(settings, upnp_lease_duration)
            BIND_SETTING_INT(settings, max_concurrent_http_announces)
            BIND_SETTING_INT(settings, dht_max_peers_reply)
            BIND_SETTING_INT(settings, dht_search_branching)
            BIND_SETTING_INT(settings, dht_max_fail_count)
            BIND_SETTING_INT(settings, dht_max_torrents)
            BIND_SETTING_INT(settings, dht_max_dht_items)
            BIND_SETTING_INT(settings, dht_max_peers)
            BIND_SETTING_INT(settings, dht_max_torrent_search_reply)
            BIND_SETTING_INT(settings, dht_block_timeout)
            BIND_SETTING_INT(settings, dht_block_ratelimit)
            BIND_SETTING_INT(settings, dht_item_lifetime)
            BIND_SETTING_INT(settings, dht_sample_infohashes_interval)
            BIND_SETTING_INT(settings, dht_max_infohashes_sample_count)
            BIND_SETTING_INT(settings, max_piece_count)
            BIND_SETTING_INT(settings, alert_mask)
            BIND_SETTING_INT(settings, proxy_type)
            BIND_SETTING_INT(settings, proxy_port)
            BIND_SETTING_INT(settings, i2p_port)
            BIND_SETTING_INT(settings, metadata_token_limit)
            BIND_SETTING_INT(settings, num_outgoing_ports)
        });

    return std::shared_ptr<SettingsPack>(new SettingsPack(insertedId, name, description, settings));
}

std::shared_ptr<SettingsPack> SettingsPack::GetById(sqlite3* db, int id)
{
    std::shared_ptr<SettingsPack> result = nullptr;

    Statement::ForEach(
        db,
        "SELECT id,name,description,"
        // strings
        "user_agent\n"
        "announce_ip\n"
        "handshake_client_version\n"
        "peer_fingerprint\n"
        "outgoing_interfaces\n"
        "listen_interfaces\n"
        "proxy_hostname\n"
        "proxy_username\n"
        "proxy_password\n"
        "i2p_hostname\n"
        "dht_bootstrap_nodes\n"
        //  bools
        "allow_multiple_connections_per_ip\n"
        "send_redundant_have\n"
        "use_dht_as_fallback\n"
        "upnp_ignore_nonrouters\n"
        "use_parole_mode\n"
        "auto_manage_prefer_seeds\n"
        "dont_count_slow_torrents\n"
        "close_redundant_connections\n"
        "prioritize_partial_pieces\n"
        "rate_limit_ip_overhead\n"
        "announce_to_all_tiers\n"
        "announce_to_all_trackers\n"
        "prefer_udp_trackers\n"
        "disable_hash_checks\n"
        "volatile_read_cache\n"
        "no_atime_storage\n"
        "incoming_starts_queued_torrents\n"
        "report_true_downloaded\n"
        "strict_end_game_mode\n"
        "enable_outgoing_utp\n"
        "enable_incoming_utp\n"
        "enable_outgoing_tcp\n"
        "enable_incoming_tcp\n"
        "no_recheck_incomplete_resume\n"
        "anonymous_mode\n"
        "report_web_seed_downloads\n"
        "seeding_outgoing_connections\n"
        "no_connect_privileged_ports\n"
        "smooth_connects\n"
        "always_send_user_agent\n"
        "apply_ip_filter_to_trackers\n"
        "ban_web_seeds\n"
        "allow_partial_disk_writes\n"
        "support_share_mode\n"
        "report_redundant_bytes\n"
        "listen_system_port_fallback\n"
        "announce_crypto_support\n"
        "enable_upnp\n"
        "enable_natpmp\n"
        "enable_lsd\n"
        "enable_dht\n"
        "prefer_rc4\n"
        "auto_sequential\n"
        "enable_ip_notifier\n"
        "dht_prefer_verified_node_ids\n"
        "dht_restrict_routing_ips\n"
        "dht_restrict_search_ips\n"
        "dht_extended_routing_table\n"
        "dht_aggressive_lookups\n"
        "dht_privacy_lookups\n"
        "dht_enforce_node_id\n"
        "dht_ignore_dark_internet\n"
        "dht_read_only\n"
        "piece_extent_affinity\n"
        "validate_https_trackers\n"
        "ssrf_mitigation\n"
        "allow_idna\n"
        "enable_set_file_valid_data\n"
        "socks5_udp_send_local_ep\n"
        "proxy_hostnames\n"
        "proxy_peer_connections\n"
        "proxy_tracker_connections\n"
        "allow_i2p_mixed\n"
        "ignore_resume_timestamps\n"
        // ints
        "tracker_completion_timeout\n"
        "tracker_receive_timeout\n"
        "stop_tracker_timeout\n"
        "tracker_maximum_response_length\n"
        "piece_timeout\n"
        "request_timeout\n"
        "request_queue_time\n"
        "max_allowed_in_request_queue\n"
        "max_out_request_queue\n"
        "whole_pieces_threshold\n"
        "peer_timeout\n"
        "urlseed_timeout\n"
        "urlseed_pipeline_size\n"
        "urlseed_wait_retry\n"
        "file_pool_size\n"
        "max_failcount\n"
        "min_reconnect_time\n"
        "peer_connect_timeout\n"
        "connection_speed\n"
        "inactivity_timeout\n"
        "unchoke_interval\n"
        "optimistic_unchoke_interval\n"
        "num_want\n"
        "initial_picker_threshold\n"
        "allowed_fast_set_size\n"
        "suggest_mode\n"
        "max_queued_disk_bytes\n"
        "handshake_timeout\n"
        "send_buffer_low_watermark\n"
        "send_buffer_watermark\n"
        "send_buffer_watermark_factor\n"
        "choking_algorithm\n"
        "seed_choking_algorithm\n"
        "disk_io_write_mode\n"
        "disk_io_read_mode\n"
        "outgoing_port\n"
        "peer_tos\n"
        "active_downloads\n"
        "active_seeds\n"
        "active_checking\n"
        "active_dht_limit\n"
        "active_tracker_limit\n"
        "active_lsd_limit\n"
        "active_limit\n"
        "auto_manage_interval\n"
        "seed_time_limit\n"
        "auto_scrape_interval\n"
        "auto_scrape_min_interval\n"
        "max_peerlist_size\n"
        "max_paused_peerlist_size\n"
        "min_announce_interval\n"
        "auto_manage_startup\n"
        "seeding_piece_quota\n"
        "max_rejects\n"
        "recv_socket_buffer_size\n"
        "send_socket_buffer_size\n"
        "max_peer_recv_buffer_size\n"
        "read_cache_line_size\n"
        "write_cache_line_size\n"
        "optimistic_disk_retry\n"
        "max_suggest_pieces\n"
        "local_service_announce_interval\n"
        "dht_announce_interval\n"
        "udp_tracker_token_expiry\n"
        "num_optimistic_unchoke_slots\n"
        "max_pex_peers\n"
        "tick_interval\n"
        "share_mode_target\n"
        "upload_rate_limit\n"
        "download_rate_limit\n"
        "dht_upload_rate_limit\n"
        "unchoke_slots_limit\n"
        "connections_limit\n"
        "connections_slack\n"
        "utp_target_delay\n"
        "utp_gain_factor\n"
        "utp_min_timeout\n"
        "utp_syn_resends\n"
        "utp_fin_resends\n"
        "utp_num_resends\n"
        "utp_connect_timeout\n"
        "utp_loss_multiplier\n"
        "mixed_mode_algorithm\n"
        "listen_queue_size\n"
        "torrent_connect_boost\n"
        "alert_queue_size\n"
        "max_metadata_size\n"
        "hashing_threads\n"
        "checking_mem_usage\n"
        "predictive_piece_announce\n"
        "aio_threads\n"
        "tracker_backoff\n"
        "share_ratio_limit\n"
        "seed_time_ratio_limit\n"
        "peer_turnover\n"
        "peer_turnover_cutoff\n"
        "peer_turnover_interval\n"
        "connect_seed_every_n_download\n"
        "max_http_recv_buffer_size\n"
        "max_retry_port_bind\n"
        "out_enc_policy\n"
        "in_enc_policy\n"
        "allowed_enc_level\n"
        "inactive_down_rate\n"
        "inactive_up_rate\n"
        "urlseed_max_request_bytes\n"
        "web_seed_name_lookup_retry\n"
        "close_file_interval\n"
        "utp_cwnd_reduce_timer\n"
        "max_web_seed_connections\n"
        "resolver_cache_timeout\n"
        "send_not_sent_low_watermark\n"
        "rate_choker_initial_threshold\n"
        "upnp_lease_duration\n"
        "max_concurrent_http_announces\n"
        "dht_max_peers_reply\n"
        "dht_search_branching\n"
        "dht_max_fail_count\n"
        "dht_max_torrents\n"
        "dht_max_dht_items\n"
        "dht_max_peers\n"
        "dht_max_torrent_search_reply\n"
        "dht_block_timeout\n"
        "dht_block_ratelimit\n"
        "dht_item_lifetime\n"
        "dht_sample_infohashes_interval\n"
        "dht_max_infohashes_sample_count\n"
        "max_piece_count\n"
        "alert_mask\n"
        "proxy_type\n"
        "proxy_port\n"
        "i2p_port\n"
        "metadata_token_limit\n"
        "num_outgoing_ports\n"
        "default_est_reciprocation_rate\n"
        "increase_est_reciprocation_rate\n"
        "decrease_est_reciprocation_rate\n"
        "utp_delayed_ack\n"
        "FROM settings_pack WHERE id = $1",
        [&result](const Statement::Row& row)
        {
            int id = row.GetInt32("id");
            std::string name = row.GetStdString("name");
            std::string desc = row.GetStdString("description");

            lt::settings_pack sp;
            // strings
            sp.set_str(lt::settings_pack::string_types::user_agent, row.GetStdString("user_agent"));
            sp.set_str(lt::settings_pack::string_types::announce_ip, row.GetStdString("announce_ip"));
            sp.set_str(lt::settings_pack::string_types::handshake_client_version, row.GetStdString("handshake_client_version"));
            sp.set_str(lt::settings_pack::string_types::peer_fingerprint, row.GetStdString("peer_fingerprint"));
            sp.set_str(lt::settings_pack::string_types::outgoing_interfaces, row.GetStdString("outgoing_interfaces"));
            sp.set_str(lt::settings_pack::string_types::listen_interfaces, row.GetStdString("listen_interfaces"));
            sp.set_str(lt::settings_pack::string_types::proxy_hostname, row.GetStdString("proxy_hostname"));
            sp.set_str(lt::settings_pack::string_types::proxy_username, row.GetStdString("proxy_username"));
            sp.set_str(lt::settings_pack::string_types::proxy_password, row.GetStdString("proxy_password"));
            sp.set_str(lt::settings_pack::string_types::i2p_hostname, row.GetStdString("i2p_hostname"));
            sp.set_str(lt::settings_pack::string_types::dht_bootstrap_nodes, row.GetStdString("dht_bootstrap_nodes"));
            // bools
            sp.set_bool(lt::settings_pack::bool_types::allow_multiple_connections_per_ip, row.GetBool("allow_multiple_connections_per_ip"));
            sp.set_bool(lt::settings_pack::bool_types::send_redundant_have, row.GetBool("send_redundant_have"));
            sp.set_bool(lt::settings_pack::bool_types::use_dht_as_fallback, row.GetBool("use_dht_as_fallback"));
            sp.set_bool(lt::settings_pack::bool_types::upnp_ignore_nonrouters, row.GetBool("upnp_ignore_nonrouters"));
            sp.set_bool(lt::settings_pack::bool_types::use_parole_mode, row.GetBool("use_parole_mode"));
            sp.set_bool(lt::settings_pack::bool_types::auto_manage_prefer_seeds, row.GetBool("auto_manage_prefer_seeds"));
            sp.set_bool(lt::settings_pack::bool_types::dont_count_slow_torrents, row.GetBool("dont_count_slow_torrents"));
            sp.set_bool(lt::settings_pack::bool_types::close_redundant_connections, row.GetBool("close_redundant_connections"));
            sp.set_bool(lt::settings_pack::bool_types::prioritize_partial_pieces, row.GetBool("prioritize_partial_pieces"));
            sp.set_bool(lt::settings_pack::bool_types::rate_limit_ip_overhead, row.GetBool("rate_limit_ip_overhead"));
            sp.set_bool(lt::settings_pack::bool_types::announce_to_all_tiers, row.GetBool("announce_to_all_tiers"));
            sp.set_bool(lt::settings_pack::bool_types::announce_to_all_trackers, row.GetBool("announce_to_all_trackers"));
            sp.set_bool(lt::settings_pack::bool_types::prefer_udp_trackers, row.GetBool("prefer_udp_trackers"));
            sp.set_bool(lt::settings_pack::bool_types::disable_hash_checks, row.GetBool("disable_hash_checks"));
            sp.set_bool(lt::settings_pack::bool_types::volatile_read_cache, row.GetBool("volatile_read_cache"));
            sp.set_bool(lt::settings_pack::bool_types::no_atime_storage, row.GetBool("no_atime_storage"));
            sp.set_bool(lt::settings_pack::bool_types::incoming_starts_queued_torrents, row.GetBool("incoming_starts_queued_torrents"));
            sp.set_bool(lt::settings_pack::bool_types::report_true_downloaded, row.GetBool("report_true_downloaded"));
            sp.set_bool(lt::settings_pack::bool_types::strict_end_game_mode, row.GetBool("strict_end_game_mode"));
            sp.set_bool(lt::settings_pack::bool_types::enable_outgoing_utp, row.GetBool("enable_outgoing_utp"));
            sp.set_bool(lt::settings_pack::bool_types::enable_incoming_utp, row.GetBool("enable_incoming_utp"));
            sp.set_bool(lt::settings_pack::bool_types::enable_outgoing_tcp, row.GetBool("enable_outgoing_tcp"));
            sp.set_bool(lt::settings_pack::bool_types::enable_incoming_tcp, row.GetBool("enable_incoming_tcp"));
            sp.set_bool(lt::settings_pack::bool_types::no_recheck_incomplete_resume, row.GetBool("no_recheck_incomplete_resume"));
            sp.set_bool(lt::settings_pack::bool_types::anonymous_mode, row.GetBool("anonymous_mode"));
            sp.set_bool(lt::settings_pack::bool_types::report_web_seed_downloads, row.GetBool("report_web_seed_downloads"));
            sp.set_bool(lt::settings_pack::bool_types::seeding_outgoing_connections, row.GetBool("seeding_outgoing_connections"));
            sp.set_bool(lt::settings_pack::bool_types::no_connect_privileged_ports, row.GetBool("no_connect_privileged_ports"));
            sp.set_bool(lt::settings_pack::bool_types::smooth_connects, row.GetBool("smooth_connects"));
            sp.set_bool(lt::settings_pack::bool_types::always_send_user_agent, row.GetBool("always_send_user_agent"));
            sp.set_bool(lt::settings_pack::bool_types::apply_ip_filter_to_trackers, row.GetBool("apply_ip_filter_to_trackers"));
            sp.set_bool(lt::settings_pack::bool_types::ban_web_seeds, row.GetBool("ban_web_seeds"));
            sp.set_bool(lt::settings_pack::bool_types::allow_partial_disk_writes, row.GetBool("allow_partial_disk_writes"));
            sp.set_bool(lt::settings_pack::bool_types::support_share_mode, row.GetBool("support_share_mode"));
            sp.set_bool(lt::settings_pack::bool_types::report_redundant_bytes, row.GetBool("report_redundant_bytes"));
            sp.set_bool(lt::settings_pack::bool_types::listen_system_port_fallback, row.GetBool("listen_system_port_fallback"));
            sp.set_bool(lt::settings_pack::bool_types::announce_crypto_support, row.GetBool("announce_crypto_support"));
            sp.set_bool(lt::settings_pack::bool_types::enable_upnp, row.GetBool("enable_upnp"));
            sp.set_bool(lt::settings_pack::bool_types::enable_natpmp, row.GetBool("enable_natpmp"));
            sp.set_bool(lt::settings_pack::bool_types::enable_lsd, row.GetBool("enable_lsd"));
            sp.set_bool(lt::settings_pack::bool_types::enable_dht, row.GetBool("enable_dht"));
            sp.set_bool(lt::settings_pack::bool_types::prefer_rc4, row.GetBool("prefer_rc4"));
            sp.set_bool(lt::settings_pack::bool_types::auto_sequential, row.GetBool("auto_sequential"));
            sp.set_bool(lt::settings_pack::bool_types::enable_ip_notifier, row.GetBool("enable_ip_notifier"));
            sp.set_bool(lt::settings_pack::bool_types::dht_prefer_verified_node_ids, row.GetBool("dht_prefer_verified_node_ids"));
            sp.set_bool(lt::settings_pack::bool_types::dht_restrict_routing_ips, row.GetBool("dht_restrict_routing_ips"));
            sp.set_bool(lt::settings_pack::bool_types::dht_restrict_search_ips, row.GetBool("dht_restrict_search_ips"));
            sp.set_bool(lt::settings_pack::bool_types::dht_extended_routing_table, row.GetBool("dht_extended_routing_table"));
            sp.set_bool(lt::settings_pack::bool_types::dht_aggressive_lookups, row.GetBool("dht_aggressive_lookups"));
            sp.set_bool(lt::settings_pack::bool_types::dht_privacy_lookups, row.GetBool("dht_privacy_lookups"));
            sp.set_bool(lt::settings_pack::bool_types::dht_enforce_node_id, row.GetBool("dht_enforce_node_id"));
            sp.set_bool(lt::settings_pack::bool_types::dht_ignore_dark_internet, row.GetBool("dht_ignore_dark_internet"));
            sp.set_bool(lt::settings_pack::bool_types::dht_read_only, row.GetBool("dht_read_only"));
            sp.set_bool(lt::settings_pack::bool_types::piece_extent_affinity, row.GetBool("piece_extent_affinity"));
            sp.set_bool(lt::settings_pack::bool_types::validate_https_trackers, row.GetBool("validate_https_trackers"));
            sp.set_bool(lt::settings_pack::bool_types::ssrf_mitigation, row.GetBool("ssrf_mitigation"));
            sp.set_bool(lt::settings_pack::bool_types::allow_idna, row.GetBool("allow_idna"));
            sp.set_bool(lt::settings_pack::bool_types::enable_set_file_valid_data, row.GetBool("enable_set_file_valid_data"));
            sp.set_bool(lt::settings_pack::bool_types::socks5_udp_send_local_ep, row.GetBool("socks5_udp_send_local_ep"));
            sp.set_bool(lt::settings_pack::bool_types::proxy_hostnames, row.GetBool("proxy_hostnames"));
            sp.set_bool(lt::settings_pack::bool_types::proxy_peer_connections, row.GetBool("proxy_peer_connections"));
            sp.set_bool(lt::settings_pack::bool_types::proxy_tracker_connections, row.GetBool("proxy_tracker_connections"));
            sp.set_bool(lt::settings_pack::bool_types::allow_i2p_mixed, row.GetBool("allow_i2p_mixed"));
            // ints
            sp.set_int(lt::settings_pack::int_types::tracker_completion_timeout, row.GetInt32("tracker_completion_timeout"));
            sp.set_int(lt::settings_pack::int_types::tracker_receive_timeout, row.GetInt32("tracker_receive_timeout"));
            sp.set_int(lt::settings_pack::int_types::stop_tracker_timeout, row.GetInt32("stop_tracker_timeout"));
            sp.set_int(lt::settings_pack::int_types::tracker_maximum_response_length, row.GetInt32("tracker_maximum_response_length"));
            sp.set_int(lt::settings_pack::int_types::piece_timeout, row.GetInt32("piece_timeout"));
            sp.set_int(lt::settings_pack::int_types::request_timeout, row.GetInt32("request_timeout"));
            sp.set_int(lt::settings_pack::int_types::request_queue_time, row.GetInt32("request_queue_time"));
            sp.set_int(lt::settings_pack::int_types::max_allowed_in_request_queue, row.GetInt32("max_allowed_in_request_queue"));
            sp.set_int(lt::settings_pack::int_types::max_out_request_queue, row.GetInt32("max_out_request_queue"));
            sp.set_int(lt::settings_pack::int_types::whole_pieces_threshold, row.GetInt32("whole_pieces_threshold"));
            sp.set_int(lt::settings_pack::int_types::peer_timeout, row.GetInt32("peer_timeout"));
            sp.set_int(lt::settings_pack::int_types::urlseed_timeout, row.GetInt32("urlseed_timeout"));
            sp.set_int(lt::settings_pack::int_types::urlseed_pipeline_size, row.GetInt32("urlseed_pipeline_size"));
            sp.set_int(lt::settings_pack::int_types::urlseed_wait_retry, row.GetInt32("urlseed_wait_retry"));
            sp.set_int(lt::settings_pack::int_types::file_pool_size, row.GetInt32("file_pool_size"));
            sp.set_int(lt::settings_pack::int_types::max_failcount, row.GetInt32("max_failcount"));
            sp.set_int(lt::settings_pack::int_types::min_reconnect_time, row.GetInt32("min_reconnect_time"));
            sp.set_int(lt::settings_pack::int_types::peer_connect_timeout, row.GetInt32("peer_connect_timeout"));
            sp.set_int(lt::settings_pack::int_types::connection_speed, row.GetInt32("connection_speed"));
            sp.set_int(lt::settings_pack::int_types::inactivity_timeout, row.GetInt32("inactivity_timeout"));
            sp.set_int(lt::settings_pack::int_types::unchoke_interval, row.GetInt32("unchoke_interval"));
            sp.set_int(lt::settings_pack::int_types::optimistic_unchoke_interval, row.GetInt32("optimistic_unchoke_interval"));
            sp.set_int(lt::settings_pack::int_types::num_want, row.GetInt32("num_want"));
            sp.set_int(lt::settings_pack::int_types::initial_picker_threshold, row.GetInt32("initial_picker_threshold"));
            sp.set_int(lt::settings_pack::int_types::allowed_fast_set_size, row.GetInt32("allowed_fast_set_size"));
            sp.set_int(lt::settings_pack::int_types::suggest_mode, row.GetInt32("suggest_mode"));
            sp.set_int(lt::settings_pack::int_types::max_queued_disk_bytes, row.GetInt32("max_queued_disk_bytes"));
            sp.set_int(lt::settings_pack::int_types::handshake_timeout, row.GetInt32("handshake_timeout"));
            sp.set_int(lt::settings_pack::int_types::send_buffer_low_watermark, row.GetInt32("send_buffer_low_watermark"));
            sp.set_int(lt::settings_pack::int_types::send_buffer_watermark, row.GetInt32("send_buffer_watermark"));
            sp.set_int(lt::settings_pack::int_types::send_buffer_watermark_factor, row.GetInt32("send_buffer_watermark_factor"));
            sp.set_int(lt::settings_pack::int_types::choking_algorithm, row.GetInt32("choking_algorithm"));
            sp.set_int(lt::settings_pack::int_types::seed_choking_algorithm, row.GetInt32("seed_choking_algorithm"));
            sp.set_int(lt::settings_pack::int_types::disk_io_write_mode, row.GetInt32("disk_io_write_mode"));
            sp.set_int(lt::settings_pack::int_types::disk_io_read_mode, row.GetInt32("disk_io_read_mode"));
            sp.set_int(lt::settings_pack::int_types::outgoing_port, row.GetInt32("outgoing_port"));
            sp.set_int(lt::settings_pack::int_types::peer_tos, row.GetInt32("peer_tos"));
            sp.set_int(lt::settings_pack::int_types::active_downloads, row.GetInt32("active_downloads"));
            sp.set_int(lt::settings_pack::int_types::active_seeds, row.GetInt32("active_seeds"));
            sp.set_int(lt::settings_pack::int_types::active_checking, row.GetInt32("active_checking"));
            sp.set_int(lt::settings_pack::int_types::active_dht_limit, row.GetInt32("active_dht_limit"));
            sp.set_int(lt::settings_pack::int_types::active_tracker_limit, row.GetInt32("active_tracker_limit"));
            sp.set_int(lt::settings_pack::int_types::active_lsd_limit, row.GetInt32("active_lsd_limit"));
            sp.set_int(lt::settings_pack::int_types::active_limit, row.GetInt32("active_limit"));
            sp.set_int(lt::settings_pack::int_types::auto_manage_interval, row.GetInt32("auto_manage_interval"));
            sp.set_int(lt::settings_pack::int_types::seed_time_limit, row.GetInt32("seed_time_limit"));
            sp.set_int(lt::settings_pack::int_types::auto_scrape_interval, row.GetInt32("auto_scrape_interval"));
            sp.set_int(lt::settings_pack::int_types::auto_scrape_min_interval, row.GetInt32("auto_scrape_min_interval"));
            sp.set_int(lt::settings_pack::int_types::max_peerlist_size, row.GetInt32("max_peerlist_size"));
            sp.set_int(lt::settings_pack::int_types::max_paused_peerlist_size, row.GetInt32("max_paused_peerlist_size"));
            sp.set_int(lt::settings_pack::int_types::min_announce_interval, row.GetInt32("min_announce_interval"));
            sp.set_int(lt::settings_pack::int_types::auto_manage_startup, row.GetInt32("auto_manage_startup"));
            sp.set_int(lt::settings_pack::int_types::seeding_piece_quota, row.GetInt32("seeding_piece_quota"));
            sp.set_int(lt::settings_pack::int_types::max_rejects, row.GetInt32("max_rejects"));
            sp.set_int(lt::settings_pack::int_types::recv_socket_buffer_size, row.GetInt32("recv_socket_buffer_size"));
            sp.set_int(lt::settings_pack::int_types::send_socket_buffer_size, row.GetInt32("send_socket_buffer_size"));
            sp.set_int(lt::settings_pack::int_types::max_peer_recv_buffer_size, row.GetInt32("max_peer_recv_buffer_size"));
            sp.set_int(lt::settings_pack::int_types::read_cache_line_size, row.GetInt32("read_cache_line_size"));
            sp.set_int(lt::settings_pack::int_types::write_cache_line_size, row.GetInt32("write_cache_line_size"));
            sp.set_int(lt::settings_pack::int_types::optimistic_disk_retry, row.GetInt32("optimistic_disk_retry"));
            sp.set_int(lt::settings_pack::int_types::max_suggest_pieces, row.GetInt32("max_suggest_pieces"));
            sp.set_int(lt::settings_pack::int_types::local_service_announce_interval, row.GetInt32("local_service_announce_interval"));
            sp.set_int(lt::settings_pack::int_types::dht_announce_interval, row.GetInt32("dht_announce_interval"));
            sp.set_int(lt::settings_pack::int_types::udp_tracker_token_expiry, row.GetInt32("udp_tracker_token_expiry"));
            sp.set_int(lt::settings_pack::int_types::num_optimistic_unchoke_slots, row.GetInt32("num_optimistic_unchoke_slots"));
            sp.set_int(lt::settings_pack::int_types::max_pex_peers, row.GetInt32("max_pex_peers"));
            sp.set_int(lt::settings_pack::int_types::tick_interval, row.GetInt32("tick_interval"));
            sp.set_int(lt::settings_pack::int_types::share_mode_target, row.GetInt32("share_mode_target"));
            sp.set_int(lt::settings_pack::int_types::upload_rate_limit, row.GetInt32("upload_rate_limit"));
            sp.set_int(lt::settings_pack::int_types::download_rate_limit, row.GetInt32("download_rate_limit"));
            sp.set_int(lt::settings_pack::int_types::dht_upload_rate_limit, row.GetInt32("dht_upload_rate_limit"));
            sp.set_int(lt::settings_pack::int_types::unchoke_slots_limit, row.GetInt32("unchoke_slots_limit"));
            sp.set_int(lt::settings_pack::int_types::connections_limit, row.GetInt32("connections_limit"));
            sp.set_int(lt::settings_pack::int_types::connections_slack, row.GetInt32("connections_slack"));
            sp.set_int(lt::settings_pack::int_types::utp_target_delay, row.GetInt32("utp_target_delay"));
            sp.set_int(lt::settings_pack::int_types::utp_gain_factor, row.GetInt32("utp_gain_factor"));
            sp.set_int(lt::settings_pack::int_types::utp_min_timeout, row.GetInt32("utp_min_timeout"));
            sp.set_int(lt::settings_pack::int_types::utp_syn_resends, row.GetInt32("utp_syn_resends"));
            sp.set_int(lt::settings_pack::int_types::utp_fin_resends, row.GetInt32("utp_fin_resends"));
            sp.set_int(lt::settings_pack::int_types::utp_num_resends, row.GetInt32("utp_num_resends"));
            sp.set_int(lt::settings_pack::int_types::utp_connect_timeout, row.GetInt32("utp_connect_timeout"));
            sp.set_int(lt::settings_pack::int_types::utp_loss_multiplier, row.GetInt32("utp_loss_multiplier"));
            sp.set_int(lt::settings_pack::int_types::mixed_mode_algorithm, row.GetInt32("mixed_mode_algorithm"));
            sp.set_int(lt::settings_pack::int_types::listen_queue_size, row.GetInt32("listen_queue_size"));
            sp.set_int(lt::settings_pack::int_types::torrent_connect_boost, row.GetInt32("torrent_connect_boost"));
            sp.set_int(lt::settings_pack::int_types::alert_queue_size, row.GetInt32("alert_queue_size"));
            sp.set_int(lt::settings_pack::int_types::max_metadata_size, row.GetInt32("max_metadata_size"));
            sp.set_int(lt::settings_pack::int_types::hashing_threads, row.GetInt32("hashing_threads"));
            sp.set_int(lt::settings_pack::int_types::checking_mem_usage, row.GetInt32("checking_mem_usage"));
            sp.set_int(lt::settings_pack::int_types::predictive_piece_announce, row.GetInt32("predictive_piece_announce"));
            sp.set_int(lt::settings_pack::int_types::aio_threads, row.GetInt32("aio_threads"));
            sp.set_int(lt::settings_pack::int_types::tracker_backoff, row.GetInt32("tracker_backoff"));
            sp.set_int(lt::settings_pack::int_types::share_ratio_limit, row.GetInt32("share_ratio_limit"));
            sp.set_int(lt::settings_pack::int_types::seed_time_ratio_limit, row.GetInt32("seed_time_ratio_limit"));
            sp.set_int(lt::settings_pack::int_types::peer_turnover, row.GetInt32("peer_turnover"));
            sp.set_int(lt::settings_pack::int_types::peer_turnover_cutoff, row.GetInt32("peer_turnover_cutoff"));
            sp.set_int(lt::settings_pack::int_types::peer_turnover_interval, row.GetInt32("peer_turnover_interval"));
            sp.set_int(lt::settings_pack::int_types::connect_seed_every_n_download, row.GetInt32("connect_seed_every_n_download"));
            sp.set_int(lt::settings_pack::int_types::max_http_recv_buffer_size, row.GetInt32("max_http_recv_buffer_size"));
            sp.set_int(lt::settings_pack::int_types::max_retry_port_bind, row.GetInt32("max_retry_port_bind"));
            sp.set_int(lt::settings_pack::int_types::out_enc_policy, row.GetInt32("out_enc_policy"));
            sp.set_int(lt::settings_pack::int_types::in_enc_policy, row.GetInt32("in_enc_policy"));
            sp.set_int(lt::settings_pack::int_types::allowed_enc_level, row.GetInt32("allowed_enc_level"));
            sp.set_int(lt::settings_pack::int_types::inactive_down_rate, row.GetInt32("inactive_down_rate"));
            sp.set_int(lt::settings_pack::int_types::inactive_up_rate, row.GetInt32("inactive_up_rate"));
            sp.set_int(lt::settings_pack::int_types::urlseed_max_request_bytes, row.GetInt32("urlseed_max_request_bytes"));
            sp.set_int(lt::settings_pack::int_types::web_seed_name_lookup_retry, row.GetInt32("web_seed_name_lookup_retry"));
            sp.set_int(lt::settings_pack::int_types::close_file_interval, row.GetInt32("close_file_interval"));
            sp.set_int(lt::settings_pack::int_types::utp_cwnd_reduce_timer, row.GetInt32("utp_cwnd_reduce_timer"));
            sp.set_int(lt::settings_pack::int_types::max_web_seed_connections, row.GetInt32("max_web_seed_connections"));
            sp.set_int(lt::settings_pack::int_types::resolver_cache_timeout, row.GetInt32("resolver_cache_timeout"));
            sp.set_int(lt::settings_pack::int_types::send_not_sent_low_watermark, row.GetInt32("send_not_sent_low_watermark"));
            sp.set_int(lt::settings_pack::int_types::rate_choker_initial_threshold, row.GetInt32("rate_choker_initial_threshold"));
            sp.set_int(lt::settings_pack::int_types::upnp_lease_duration, row.GetInt32("upnp_lease_duration"));
            sp.set_int(lt::settings_pack::int_types::max_concurrent_http_announces, row.GetInt32("max_concurrent_http_announces"));
            sp.set_int(lt::settings_pack::int_types::dht_max_peers_reply, row.GetInt32("dht_max_peers_reply"));
            sp.set_int(lt::settings_pack::int_types::dht_search_branching, row.GetInt32("dht_search_branching"));
            sp.set_int(lt::settings_pack::int_types::dht_max_fail_count, row.GetInt32("dht_max_fail_count"));
            sp.set_int(lt::settings_pack::int_types::dht_max_torrents, row.GetInt32("dht_max_torrents"));
            sp.set_int(lt::settings_pack::int_types::dht_max_dht_items, row.GetInt32("dht_max_dht_items"));
            sp.set_int(lt::settings_pack::int_types::dht_max_peers, row.GetInt32("dht_max_peers"));
            sp.set_int(lt::settings_pack::int_types::dht_max_torrent_search_reply, row.GetInt32("dht_max_torrent_search_reply"));
            sp.set_int(lt::settings_pack::int_types::dht_block_timeout, row.GetInt32("dht_block_timeout"));
            sp.set_int(lt::settings_pack::int_types::dht_block_ratelimit, row.GetInt32("dht_block_ratelimit"));
            sp.set_int(lt::settings_pack::int_types::dht_item_lifetime, row.GetInt32("dht_item_lifetime"));
            sp.set_int(lt::settings_pack::int_types::dht_sample_infohashes_interval, row.GetInt32("dht_sample_infohashes_interval"));
            sp.set_int(lt::settings_pack::int_types::dht_max_infohashes_sample_count, row.GetInt32("dht_max_infohashes_sample_count"));
            sp.set_int(lt::settings_pack::int_types::max_piece_count, row.GetInt32("max_piece_count"));
            sp.set_int(lt::settings_pack::int_types::alert_mask, row.GetInt32("alert_mask"));
            sp.set_int(lt::settings_pack::int_types::proxy_type, row.GetInt32("proxy_type"));
            sp.set_int(lt::settings_pack::int_types::proxy_port, row.GetInt32("proxy_port"));
            sp.set_int(lt::settings_pack::int_types::i2p_port, row.GetInt32("i2p_port"));
            sp.set_int(lt::settings_pack::int_types::metadata_token_limit, row.GetInt32("metadata_token_limit"));
            sp.set_int(lt::settings_pack::int_types::num_outgoing_ports, row.GetInt32("num_outgoing_ports"));

            result.reset(new SettingsPack(id, name, desc, sp));
        },
        [&id](sqlite3_stmt* stmt)
        {
            sqlite3_bind_int(stmt, 1, id);
        });

    return result;
}
