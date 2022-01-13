#include "0001_initialsetup.hpp"

#include <boost/log/trivial.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/settings_pack.hpp>

using pt::Server::Data::Migrations::InitialSetup;

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

    sqlite3_stmt* stmt;
    res = sqlite3_prepare_v2(
        db,
        "INSERT INTO settings_pack ("
            "name,"
            "description,"
            "user_agent,"
            "announce_ip,"
            "handshake_client_version,"
            "peer_fingerprint,"
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
            "max_piece_count"
        ") VALUES ("
            "$name,"
            "$description,"
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
            "$max_piece_count"
        ");",
        -1,
        &stmt,
        nullptr);

    if (res != SQLITE_OK) { return res; }

    auto df = lt::default_settings();

#define BIND_SETTING_BOOL(pack, setting) sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, "$" # setting), df.get_bool(lt::settings_pack:: setting) ? 1 : 0);
#define BIND_SETTING_INT(pack, setting) sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, "$" # setting), df.get_int(lt::settings_pack:: setting));
#define BIND_SETTING_STR(pack, setting) sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "$" # setting), df.get_str(lt::settings_pack:: setting).c_str(), -1, SQLITE_TRANSIENT);

    sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "$name"), "Default", -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "$description"), "Default settings", -1, SQLITE_TRANSIENT);
    BIND_SETTING_STR(df, user_agent);
    BIND_SETTING_STR(df, announce_ip);
    BIND_SETTING_STR(df, handshake_client_version);
    BIND_SETTING_STR(df, peer_fingerprint);
    // bool
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

    res = sqlite3_step(stmt);
    if (res != SQLITE_DONE) { return res; }

    BOOST_LOG_TRIVIAL(info) << "Creating proxy table";

    res = sqlite3_exec(
        db,
        "CREATE TABLE proxy ("
            "id                        INTEGER PRIMARY KEY,"
            "name                      TEXT    NOT NULL,"
            "type                      INTEGER NOT NULL,"
            "hostname                  TEXT    NOT NULL,"
            "port                      INTEGER NOT NULL,"
            "username                  TEXT    NULL,"
            "password                  TEXT    NULL,"
            "proxy_hostnames           INTEGER NOT NULL CHECK(proxy_hostnames           IN (0,1)),"
            "proxy_peer_connections    INTEGER NOT NULL CHECK(proxy_peer_connections    IN (0,1)),"
            "proxy_tracker_connections INTEGER NOT NULL CHECK(proxy_tracker_connections IN (0,1))"
        ");",
        nullptr,
        nullptr,
        nullptr);

    if (res != SQLITE_OK) { return res; }

    BOOST_LOG_TRIVIAL(info) << "Creating profiles table";

    res = sqlite3_exec(
        db,
        "CREATE TABLE profiles ("
            "id                  INTEGER PRIMARY KEY,"
            "name                TEXT NOT NULL,"
            "description         TEXT NULL,"
            "is_active           INTEGER NOT NULL DEFAULT 0,"
            "proxy_id            INTEGER     NULL REFERENCES proxy(id),"
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
            "torrent_data   BLOB NULL,"
            "timestamp      INTEGER NOT NULL DEFAULT (strftime('%s'))"
        ");",
        nullptr,
        nullptr,
        nullptr);

    if (res != SQLITE_OK) { return res; }

    BOOST_LOG_TRIVIAL(info) << "Creating magnet links table";

    res = sqlite3_exec(
        db,
        "CREATE TABLE magnet_uris ("
            "info_hash TEXT PRIMARY KEY,"
            "save_path TEXT NOT NULL,"
            "uri       TEXT NOT NULL,"
            "timestamp INTEGER NOT NULL DEFAULT (strftime('%s')),"
            "FOREIGN KEY(info_hash) REFERENCES torrents(info_hash) ON DELETE CASCADE"
        ");",
        nullptr,
        nullptr,
        nullptr);

    if (res != SQLITE_OK) { return res; }

    BOOST_LOG_TRIVIAL(info) << "Creating config table";

    res = sqlite3_exec(
        db,
        "CREATE TABLE config ("
            "key       TEXT    PRIMARY KEY,"
            "value     TEXT    NULL"
        ");",
        nullptr,
        nullptr,
        nullptr);

    if (res != SQLITE_OK) { return res; }

    BOOST_LOG_TRIVIAL(info) << "Creating listen interfaces table";

    res = sqlite3_exec(
        db,
        "CREATE TABLE listen_interfaces ("
            "id   INTEGER        PRIMARY KEY,"
            "host TEXT           NOT NULL,"
            "port INTEGER        NOT NULL,"
            "is_local    INTEGER NOT NULL CHECK(is_local    IN (0,1)),"
            "is_outgoing INTEGER NOT NULL CHECK(is_outgoing IN (0,1)),"
            "is_ssl      INTEGER NOT NULL CHECK(is_ssl      IN (0,1))"
        ");",
        nullptr,
        nullptr,
        nullptr);

    if (res != SQLITE_OK) { return res; }

    BOOST_LOG_TRIVIAL(info) << "Inserting default listen interfaces";

    res = sqlite3_exec(
        db,
        "INSERT INTO listen_interfaces (host, port, is_local, is_outgoing, is_ssl) "
        "VALUES ('0.0.0.0', 6881, 0, 1, 0), ('[::]', 6881, 0, 1, 0);",
        nullptr,
        nullptr,
        nullptr);

    if (res != SQLITE_OK) { return res; }

    return SQLITE_OK;
}
