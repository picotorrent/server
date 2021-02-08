import Vue from 'vue'
import Vuex from 'vuex'

import torrents from './modules/torrents'
import createWebSocketPlugin from './plugins/ws'

Vue.use(Vuex)

const store = new Vuex.Store({
  modules: {
    torrents
  },
  plugins: [ createWebSocketPlugin() ]
});

export default store;
