#include "settingspackgetbyid.hpp"

#include <boost/log/trivial.hpp>

#include "../data/models/settingspack.hpp"

namespace lt = libtorrent;
using json = nlohmann::json;
using pt::Server::Data::SettingsPack;
using pt::Server::RPC::SettingsPackGetByIdCommand;

SettingsPackGetByIdCommand::SettingsPackGetByIdCommand(sqlite3* db)
    : m_db(db)
{
}

json SettingsPackGetByIdCommand::Execute(const json& params)
{
    auto settingsPack = SettingsPack::GetById(m_db, params[0]);

    json result =
        {
            { "id", settingsPack->Id() },
            { "name", settingsPack->Name() },
            { "description", settingsPack->Description() },
            { "settings", json::object() }
        };

    // strings
    result["settings"]["user_agent"] = settingsPack->Settings().get_str(lt::settings_pack::string_types::user_agent);
    result["settings"]["announce_ip"] = settingsPack->Settings().get_str(lt::settings_pack::string_types::announce_ip);
    result["settings"]["handshake_client_version"] = settingsPack->Settings().get_str(lt::settings_pack::string_types::handshake_client_version);
    result["settings"]["peer_fingerprint"] = settingsPack->Settings().get_str(lt::settings_pack::string_types::peer_fingerprint);
    result["settings"]["outgoing_interfaces"] = settingsPack->Settings().get_str(lt::settings_pack::string_types::outgoing_interfaces);
    result["settings"]["listen_interfaces"] = settingsPack->Settings().get_str(lt::settings_pack::string_types::listen_interfaces);
    result["settings"]["proxy_hostname"] = settingsPack->Settings().get_str(lt::settings_pack::string_types::proxy_hostname);
    result["settings"]["proxy_username"] = settingsPack->Settings().get_str(lt::settings_pack::string_types::proxy_username);
    result["settings"]["proxy_password"] = settingsPack->Settings().get_str(lt::settings_pack::string_types::proxy_password);
    result["settings"]["i2p_hostname"] = settingsPack->Settings().get_str(lt::settings_pack::string_types::i2p_hostname);
    result["settings"]["dht_bootstrap_nodes"] = settingsPack->Settings().get_str(lt::settings_pack::string_types::dht_bootstrap_nodes);
    // bools
    result["settings"]["allow_multiple_connections_per_ip"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::allow_multiple_connections_per_ip);
    result["settings"]["send_redundant_have"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::send_redundant_have);
    result["settings"]["use_dht_as_fallback"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::use_dht_as_fallback);
    result["settings"]["upnp_ignore_nonrouters"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::upnp_ignore_nonrouters);
    result["settings"]["use_parole_mode"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::use_parole_mode);
    result["settings"]["auto_manage_prefer_seeds"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::auto_manage_prefer_seeds);
    result["settings"]["dont_count_slow_torrents"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::dont_count_slow_torrents);
    result["settings"]["close_redundant_connections"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::close_redundant_connections);
    result["settings"]["prioritize_partial_pieces"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::prioritize_partial_pieces);
    result["settings"]["rate_limit_ip_overhead"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::rate_limit_ip_overhead);
    result["settings"]["announce_to_all_tiers"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::announce_to_all_tiers);
    result["settings"]["announce_to_all_trackers"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::announce_to_all_trackers);
    result["settings"]["prefer_udp_trackers"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::prefer_udp_trackers);
    result["settings"]["disable_hash_checks"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::disable_hash_checks);
    result["settings"]["volatile_read_cache"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::volatile_read_cache);
    result["settings"]["no_atime_storage"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::no_atime_storage);
    result["settings"]["incoming_starts_queued_torrents"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::incoming_starts_queued_torrents);
    result["settings"]["report_true_downloaded"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::report_true_downloaded);
    result["settings"]["strict_end_game_mode"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::strict_end_game_mode);
    result["settings"]["enable_outgoing_utp"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::enable_outgoing_utp);
    result["settings"]["enable_incoming_utp"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::enable_incoming_utp);
    result["settings"]["enable_outgoing_tcp"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::enable_outgoing_tcp);
    result["settings"]["enable_incoming_tcp"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::enable_incoming_tcp);
    result["settings"]["no_recheck_incomplete_resume"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::no_recheck_incomplete_resume);
    result["settings"]["anonymous_mode"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::anonymous_mode);
    result["settings"]["report_web_seed_downloads"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::report_web_seed_downloads);
    result["settings"]["seeding_outgoing_connections"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::seeding_outgoing_connections);
    result["settings"]["no_connect_privileged_ports"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::no_connect_privileged_ports);
    result["settings"]["smooth_connects"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::smooth_connects);
    result["settings"]["always_send_user_agent"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::always_send_user_agent);
    result["settings"]["apply_ip_filter_to_trackers"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::apply_ip_filter_to_trackers);
    result["settings"]["ban_web_seeds"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::ban_web_seeds);
    result["settings"]["allow_partial_disk_writes"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::allow_partial_disk_writes);
    result["settings"]["support_share_mode"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::support_share_mode);
    result["settings"]["report_redundant_bytes"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::report_redundant_bytes);
    result["settings"]["listen_system_port_fallback"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::listen_system_port_fallback);
    result["settings"]["announce_crypto_support"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::announce_crypto_support);
    result["settings"]["enable_upnp"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::enable_upnp);
    result["settings"]["enable_natpmp"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::enable_natpmp);
    result["settings"]["enable_lsd"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::enable_lsd);
    result["settings"]["enable_dht"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::enable_dht);
    result["settings"]["prefer_rc4"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::prefer_rc4);
    result["settings"]["auto_sequential"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::auto_sequential);
    result["settings"]["enable_ip_notifier"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::enable_ip_notifier);
    result["settings"]["dht_prefer_verified_node_ids"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::dht_prefer_verified_node_ids);
    result["settings"]["dht_restrict_routing_ips"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::dht_restrict_routing_ips);
    result["settings"]["dht_restrict_search_ips"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::dht_restrict_search_ips);
    result["settings"]["dht_extended_routing_table"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::dht_extended_routing_table);
    result["settings"]["dht_aggressive_lookups"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::dht_aggressive_lookups);
    result["settings"]["dht_privacy_lookups"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::dht_privacy_lookups);
    result["settings"]["dht_enforce_node_id"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::dht_enforce_node_id);
    result["settings"]["dht_ignore_dark_internet"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::dht_ignore_dark_internet);
    result["settings"]["dht_read_only"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::dht_read_only);
    result["settings"]["piece_extent_affinity"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::piece_extent_affinity);
    result["settings"]["validate_https_trackers"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::validate_https_trackers);
    result["settings"]["ssrf_mitigation"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::ssrf_mitigation);
    result["settings"]["allow_idna"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::allow_idna);
    result["settings"]["enable_set_file_valid_data"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::enable_set_file_valid_data);
    result["settings"]["socks5_udp_send_local_ep"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::socks5_udp_send_local_ep);
    result["settings"]["proxy_hostnames"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::proxy_hostnames);
    result["settings"]["proxy_peer_connections"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::proxy_peer_connections);
    result["settings"]["proxy_tracker_connections"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::proxy_tracker_connections);
    result["settings"]["allow_i2p_mixed"] = settingsPack->Settings().get_bool(lt::settings_pack::bool_types::allow_i2p_mixed);
    // ints
    result["settings"]["tracker_completion_timeout"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::tracker_completion_timeout);
    result["settings"]["tracker_receive_timeout"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::tracker_receive_timeout);
    result["settings"]["stop_tracker_timeout"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::stop_tracker_timeout);
    result["settings"]["tracker_maximum_response_length"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::tracker_maximum_response_length);
    result["settings"]["piece_timeout"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::piece_timeout);
    result["settings"]["request_timeout"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::request_timeout);
    result["settings"]["request_queue_time"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::request_queue_time);
    result["settings"]["max_allowed_in_request_queue"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::max_allowed_in_request_queue);
    result["settings"]["max_out_request_queue"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::max_out_request_queue);
    result["settings"]["whole_pieces_threshold"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::whole_pieces_threshold);
    result["settings"]["peer_timeout"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::peer_timeout);
    result["settings"]["urlseed_timeout"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::urlseed_timeout);
    result["settings"]["urlseed_pipeline_size"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::urlseed_pipeline_size);
    result["settings"]["urlseed_wait_retry"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::urlseed_wait_retry);
    result["settings"]["file_pool_size"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::file_pool_size);
    result["settings"]["max_failcount"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::max_failcount);
    result["settings"]["min_reconnect_time"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::min_reconnect_time);
    result["settings"]["peer_connect_timeout"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::peer_connect_timeout);
    result["settings"]["connection_speed"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::connection_speed);
    result["settings"]["inactivity_timeout"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::inactivity_timeout);
    result["settings"]["unchoke_interval"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::unchoke_interval);
    result["settings"]["optimistic_unchoke_interval"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::optimistic_unchoke_interval);
    result["settings"]["num_want"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::num_want);
    result["settings"]["initial_picker_threshold"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::initial_picker_threshold);
    result["settings"]["allowed_fast_set_size"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::allowed_fast_set_size);
    result["settings"]["suggest_mode"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::suggest_mode);
    result["settings"]["max_queued_disk_bytes"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::max_queued_disk_bytes);
    result["settings"]["handshake_timeout"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::handshake_timeout);
    result["settings"]["send_buffer_low_watermark"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::send_buffer_low_watermark);
    result["settings"]["send_buffer_watermark"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::send_buffer_watermark);
    result["settings"]["send_buffer_watermark_factor"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::send_buffer_watermark_factor);
    result["settings"]["choking_algorithm"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::choking_algorithm);
    result["settings"]["seed_choking_algorithm"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::seed_choking_algorithm);
    result["settings"]["disk_io_write_mode"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::disk_io_write_mode);
    result["settings"]["disk_io_read_mode"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::disk_io_read_mode);
    result["settings"]["outgoing_port"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::outgoing_port);
    result["settings"]["peer_tos"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::peer_tos);
    result["settings"]["active_downloads"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::active_downloads);
    result["settings"]["active_seeds"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::active_seeds);
    result["settings"]["active_checking"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::active_checking);
    result["settings"]["active_dht_limit"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::active_dht_limit);
    result["settings"]["active_tracker_limit"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::active_tracker_limit);
    result["settings"]["active_lsd_limit"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::active_lsd_limit);
    result["settings"]["active_limit"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::active_limit);
    result["settings"]["auto_manage_interval"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::auto_manage_interval);
    result["settings"]["seed_time_limit"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::seed_time_limit);
    result["settings"]["auto_scrape_interval"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::auto_scrape_interval);
    result["settings"]["auto_scrape_min_interval"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::auto_scrape_min_interval);
    result["settings"]["max_peerlist_size"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::max_peerlist_size);
    result["settings"]["max_paused_peerlist_size"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::max_paused_peerlist_size);
    result["settings"]["min_announce_interval"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::min_announce_interval);
    result["settings"]["auto_manage_startup"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::auto_manage_startup);
    result["settings"]["seeding_piece_quota"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::seeding_piece_quota);
    result["settings"]["max_rejects"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::max_rejects);
    result["settings"]["recv_socket_buffer_size"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::recv_socket_buffer_size);
    result["settings"]["send_socket_buffer_size"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::send_socket_buffer_size);
    result["settings"]["max_peer_recv_buffer_size"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::max_peer_recv_buffer_size);
    result["settings"]["read_cache_line_size"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::read_cache_line_size);
    result["settings"]["write_cache_line_size"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::write_cache_line_size);
    result["settings"]["optimistic_disk_retry"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::optimistic_disk_retry);
    result["settings"]["max_suggest_pieces"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::max_suggest_pieces);
    result["settings"]["local_service_announce_interval"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::local_service_announce_interval);
    result["settings"]["dht_announce_interval"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::dht_announce_interval);
    result["settings"]["udp_tracker_token_expiry"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::udp_tracker_token_expiry);
    result["settings"]["num_optimistic_unchoke_slots"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::num_optimistic_unchoke_slots);
    result["settings"]["max_pex_peers"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::max_pex_peers);
    result["settings"]["tick_interval"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::tick_interval);
    result["settings"]["share_mode_target"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::share_mode_target);
    result["settings"]["upload_rate_limit"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::upload_rate_limit);
    result["settings"]["download_rate_limit"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::download_rate_limit);
    result["settings"]["dht_upload_rate_limit"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::dht_upload_rate_limit);
    result["settings"]["unchoke_slots_limit"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::unchoke_slots_limit);
    result["settings"]["connections_limit"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::connections_limit);
    result["settings"]["connections_slack"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::connections_slack);
    result["settings"]["utp_target_delay"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::utp_target_delay);
    result["settings"]["utp_gain_factor"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::utp_gain_factor);
    result["settings"]["utp_min_timeout"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::utp_min_timeout);
    result["settings"]["utp_syn_resends"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::utp_syn_resends);
    result["settings"]["utp_fin_resends"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::utp_fin_resends);
    result["settings"]["utp_num_resends"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::utp_num_resends);
    result["settings"]["utp_connect_timeout"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::utp_connect_timeout);
    result["settings"]["utp_loss_multiplier"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::utp_loss_multiplier);
    result["settings"]["mixed_mode_algorithm"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::mixed_mode_algorithm);
    result["settings"]["listen_queue_size"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::listen_queue_size);
    result["settings"]["torrent_connect_boost"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::torrent_connect_boost);
    result["settings"]["alert_queue_size"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::alert_queue_size);
    result["settings"]["max_metadata_size"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::max_metadata_size);
    result["settings"]["hashing_threads"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::hashing_threads);
    result["settings"]["checking_mem_usage"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::checking_mem_usage);
    result["settings"]["predictive_piece_announce"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::predictive_piece_announce);
    result["settings"]["aio_threads"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::aio_threads);
    result["settings"]["tracker_backoff"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::tracker_backoff);
    result["settings"]["share_ratio_limit"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::share_ratio_limit);
    result["settings"]["seed_time_ratio_limit"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::seed_time_ratio_limit);
    result["settings"]["peer_turnover"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::peer_turnover);
    result["settings"]["peer_turnover_cutoff"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::peer_turnover_cutoff);
    result["settings"]["peer_turnover_interval"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::peer_turnover_interval);
    result["settings"]["connect_seed_every_n_download"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::connect_seed_every_n_download);
    result["settings"]["max_http_recv_buffer_size"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::max_http_recv_buffer_size);
    result["settings"]["max_retry_port_bind"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::max_retry_port_bind);
    result["settings"]["out_enc_policy"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::out_enc_policy);
    result["settings"]["in_enc_policy"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::in_enc_policy);
    result["settings"]["allowed_enc_level"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::allowed_enc_level);
    result["settings"]["inactive_down_rate"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::inactive_down_rate);
    result["settings"]["inactive_up_rate"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::inactive_up_rate);
    result["settings"]["urlseed_max_request_bytes"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::urlseed_max_request_bytes);
    result["settings"]["web_seed_name_lookup_retry"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::web_seed_name_lookup_retry);
    result["settings"]["close_file_interval"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::close_file_interval);
    result["settings"]["utp_cwnd_reduce_timer"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::utp_cwnd_reduce_timer);
    result["settings"]["max_web_seed_connections"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::max_web_seed_connections);
    result["settings"]["resolver_cache_timeout"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::resolver_cache_timeout);
    result["settings"]["send_not_sent_low_watermark"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::send_not_sent_low_watermark);
    result["settings"]["rate_choker_initial_threshold"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::rate_choker_initial_threshold);
    result["settings"]["upnp_lease_duration"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::upnp_lease_duration);
    result["settings"]["max_concurrent_http_announces"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::max_concurrent_http_announces);
    result["settings"]["dht_max_peers_reply"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::dht_max_peers_reply);
    result["settings"]["dht_search_branching"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::dht_search_branching);
    result["settings"]["dht_max_fail_count"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::dht_max_fail_count);
    result["settings"]["dht_max_torrents"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::dht_max_torrents);
    result["settings"]["dht_max_dht_items"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::dht_max_dht_items);
    result["settings"]["dht_max_peers"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::dht_max_peers);
    result["settings"]["dht_max_torrent_search_reply"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::dht_max_torrent_search_reply);
    result["settings"]["dht_block_timeout"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::dht_block_timeout);
    result["settings"]["dht_block_ratelimit"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::dht_block_ratelimit);
    result["settings"]["dht_item_lifetime"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::dht_item_lifetime);
    result["settings"]["dht_sample_infohashes_interval"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::dht_sample_infohashes_interval);
    result["settings"]["dht_max_infohashes_sample_count"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::dht_max_infohashes_sample_count);
    result["settings"]["max_piece_count"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::max_piece_count);
    result["settings"]["alert_mask"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::alert_mask);
    result["settings"]["proxy_type"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::proxy_type);
    result["settings"]["proxy_port"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::proxy_port);
    result["settings"]["i2p_port"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::i2p_port);
    result["settings"]["metadata_token_limit"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::metadata_token_limit);
    result["settings"]["num_outgoing_ports"] = settingsPack->Settings().get_int(lt::settings_pack::int_types::num_outgoing_ports);

    return Ok(result);
}
