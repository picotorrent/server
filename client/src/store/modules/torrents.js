import Vue from 'vue';

const state = {
  torrents: {}
};

const getters = {
  all: state => state.torrents,
  byId: state => id => {
    return state.torrents[id]
  },
  count: state => Object.keys(state.torrents).length,

  dl_total: state => Object.values(state.torrents)
    .map(t => t.dl)
    .reduce((a, b) => a + b, 0),

  ul_total: state => Object.values(state.torrents)
    .map(t => t.ul)
    .reduce((a, b) => a + b, 0)
}

const actions = {
  addTorrent({ commit }, infoHash, torrent) {
    commit('ADD_TORRENT', infoHash, torrent);
  },

  addMany({ commit }, torrents) {
    commit('ADD_TORRENTS', torrents);
  },

  updateMany({ commit }, torrents) {
    commit('UPDATE_TORRENTS', torrents);
  }
}

const mutations = {
  ADD_TORRENT(state, infoHash, torrent) {
      Vue.set(state.torrents, infoHash, torrent);
  },

  ADD_TORRENTS(state, torrents) {
    for (const infoHash in torrents) {
      Vue.set(state.torrents, infoHash, torrents[infoHash]);
    }
  },

  REMOVE_BY_ID(state, infoHash) {
    Vue.delete(state.torrents, infoHash);
  },

  UPDATE_TORRENTS(state, torrents) {
    for (const infoHash in torrents) {
      Vue.set(state.torrents, infoHash, torrents[infoHash]);
    }
  }
}

export default {
  namespaced: true,
  state,
  getters,
  actions,
  mutations
}
