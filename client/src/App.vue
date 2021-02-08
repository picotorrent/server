<template>
  <div id="app">
    <Header />
    <main>
      <router-view />
      <!-- WebSockets error -->
      <div v-if="false">
        <div class="content error">
          <i class="icon bi bi-wifi-off"></i>
          <h2>WebSockets error</h2>
          <h4>Can't connect to the server</h4>
        </div>
      </div>
    </main>
  </div>
</template>

<script>
import axios from 'axios';
import Header from '@/components/Header';

export default {
  name: 'App',
  components: {
    Header,
  },
  data () {
    return {
      addSavePath: null,
      torrents: {},
      ws: null
    }
  },
  mounted () {
    // navigator.registerProtocolHandler('magnet', 'http://localhost:8080/ting/%s', 'PicoTorrent')
  },
  methods: {
    async remove (infoHash) {
      await axios.post('/api/jsonrpc', {
        method: 'session.removeTorrent',
        params: [ infoHash ]
      });
    }
  }
}
</script>

<style lang="scss">
@import './scss/style.scss';
@import '../node_modules/bootstrap-icons/font/bootstrap-icons.css';
</style>