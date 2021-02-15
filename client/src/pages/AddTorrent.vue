<template>
  <main class="add-torrent dialog">
    <div class="content form">
      <h3 class="title">Add torrent</h3>
      <div class="group">
        <div class="item">
          <label for="add-torrent-file">File</label>
          <input id="add-torrent-file" type="file" ref="torrentFile">
        </div>
      </div>
      <div class="group mb-1">
        <div class="item">
          <label for="add-torrent-path">Save path</label>
          <input id="add-torrent-path" type="text" placeholder="C:\Downloads\Torrents\" v-model="addSavePath">
        </div>
      </div>
      <button class="mr-1" @click="add">Add torrent file</button>
      <router-link to="/add-magnet-link" tag="button" class="link">Or add magnet link</router-link>
    </div>
  </main>
</template>

<script>
import axios from 'axios';

export default {
  data () {
    return {
      addSavePath: null
    }
  },
  async mounted () {
    const res = await axios.post('/api/jsonrpc', {
      jsonrpc: '2.0',
      method: 'config.get',
      params: [ 'default_save_path' ]
    });

    this.addSavePath = res.data.result.default_save_path;
  },
  methods: {
    async add() {
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

      this.$router.push('/');
    }
  }
}
</script>
