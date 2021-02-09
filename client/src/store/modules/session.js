import Vue from 'vue';

const state = {
  stats: {}
};

const getters = {
  all: state => state.stats,
  byId: state => id => {
    return state.stats[id]
  }
}

const mutations = {
  UPDATE_STATS(state, stats) {
    for (const key in stats) {
      Vue.set(state.stats, key, stats[key]);
    }
  }
}

export default {
  namespaced: true,
  state,
  getters,
  mutations
}
