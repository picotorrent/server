import Vue from 'vue'
import Vuex from 'vuex'

import session from './modules/session'
import torrents from './modules/torrents'
import createWebSocketPlugin from './plugins/ws'

Vue.use(Vuex)

const store = new Vuex.Store({
  modules: {
    session,
    torrents
  },
  plugins: [ createWebSocketPlugin() ]
});

export default store;
