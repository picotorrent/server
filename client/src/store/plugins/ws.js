export default function createWebSocketPlugin() {
  return store => {
    const scheme = location.protocol === 'http:'
      ? 'ws:'
      : 'wss:';

    const ws = new WebSocket(`${scheme}//${location.host}/api/ws`);

    ws.onmessage = (ev) => {
      const data = JSON.parse(ev.data);

      switch (data.type) {
        case 'init':
          store.commit('torrents/ADD_TORRENTS', data.torrents);
          break;

        case 'session.stats':
          store.commit('session/UPDATE_STATS', data.stats);
          break;

        case 'torrent.added':
          store.commit('torrents/ADD_TORRENT', data.torrent);
          break;

        case 'torrent.removed':
          store.commit('torrents/REMOVE_BY_ID', data.info_hash);
          break;

        case 'torrent.updated':
          store.commit('torrents/UPDATE_TORRENTS', data.torrents);
          break;
      }
    }
  }
}
