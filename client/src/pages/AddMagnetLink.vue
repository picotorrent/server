<template>
  <main class="add-torrent dialog">
    <div class="content form">
      <h3>Add magnet link</h3>
      <hr>
      <div class="group">
        <div class="item">
          <label for="add-torrent-link">Link</label>
          <input id="add-torrent-link" type="text" v-model="addMagnetUri">
        </div>
      </div>
      <div class="group">
        <div class="item">
          <label for="add-torrent-path">Save path</label>
          <input id="add-torrent-path" type="text" placeholder="C:\Downloads\Torrents\" v-model="addSavePath">
        </div>
      </div>
      <button @click="add">Add magnet link</button>
    </div>
  </main>
</template>

<script>
import axios from 'axios';

export default {
  data () {
    return {
      addMagnetUri: null,
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
      await this.$rpc('session.addMagnetLink', {
        magnet_uri: this.addMagnetUri,
        save_path: this.addSavePath
      });

      this.$router.push('/');
    }
  }
}
</script>
