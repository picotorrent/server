export default function createWebSocketPlugin() {
  return store => {
    const ws = new WebSocket(`ws://${location.host}/api/ws`)
    ws.onopen = () => {
    }

    ws.onmessage = (ev) => {
      const data = JSON.parse(ev.data);

      switch (data.type) {
        case 'init':
          store.commit('torrents/ADD_TORRENTS', data.torrents);
          break;

        case 'session.stats':
          store.commit('session/UPDATE_STATS', data.stats);
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
