<template>
  <main class="add-torrent dialog">
    <div class="content">
      <h3>Add torrent</h3>
      <hr>
      <div class="group">
        <label for="add-torrent-file">File</label>
        <input id="add-torrent-file" type="file" ref="torrentFile">
      </div>
      <div class="group">
        <label for="add-torrent-path">Save path</label>
        <input id="add-torrent-path" type="text" v-model="addSavePath">
      </div>
      <button @click="add">Add torrent</button>
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
