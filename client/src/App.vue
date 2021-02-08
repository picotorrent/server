<template>
  <div id="app"> 
    <Header />
    <main>
      <Statusbar />
      <Torrents />
      <File class="hide" />
      <!-- WebSockets error -->
      <div class="empty">
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
import Vue from 'vue';
import Header from './components/Header';
import Statusbar from './components/Statusbar';
import Torrents from './components/Torrents';
import File from './components/File';

export default {
  name: 'App',
  components: {
    Header,
    Statusbar,
    Torrents,
    File,
  },
  data () {
    return {
      addSavePath: null,
      torrents: {},
      ws: null
    }
  },
  mounted () {
    this.ws = new WebSocket(`ws://${location.host}/api/ws`);
    this.ws.onmessage = (ev) => {
      const data = JSON.parse(ev.data);

      switch (data.type) {
        case 'init':
          for (const infoHash in data.torrents) {
            Vue.set(this.torrents, infoHash, data.torrents[infoHash]);
          }
          break;

        case 'torrent.added':
          Vue.set(this.torrents, data.torrent.info_hash, data.torrent);
          break;

        case 'torrent.removed':
          Vue.delete(this.torrents, data.info_hash);
          break;

        case 'torrent.updated':
          for (const infoHash in data.torrents) {
            Vue.set(this.torrents, infoHash, data.torrents[infoHash]);
          }
          break;
      }
    }
  },
  methods: {
    async add () {
      const buffer = new Uint8Array(await this.$refs.torrentFile.files[0].arrayBuffer());
      const tempBuffer = Array.prototype.map.call(buffer, function (ch) {
        return String.fromCharCode(ch);
      }).join('');

      await axios.post('/api/jsonrpc', {
        method: 'session.addTorrent',
        params: {
          data: btoa(tempBuffer),
          save_path: this.addSavePath
        }
      });
    },

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