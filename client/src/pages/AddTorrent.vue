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
export default {
  data () {
    return {
      addSavePath: null
    }
  },
  async mounted () {
    const {
      default_save_path
    } = await this.$rpc('config.get', [ 'default_save_path' ]);

    this.addSavePath = default_save_path
  },
  methods: {
    async add() {
      if (this.addSavePath == null){
        if (Math.floor(Math.random() * 1000) == 69){
          alert("pwease pwovide a save path uwu"); // replace with toast notifications
        } else{
          alert("You must provide a save path.");  // replace with toast notifications
        }
      } else{
      const buffer = new Uint8Array(await this.$refs.torrentFile.files[0].arrayBuffer());
      const tempBuffer = Array.prototype.map.call(buffer, function (ch) {
        return String.fromCharCode(ch);
      }).join('');

      await this.$rpc('session.addTorrent', {
        data: btoa(tempBuffer),
        save_path: this.addSavePath
      });

      this.$router.push('/');
      }
    }
  }
}
</script>
