<template>
  <main class="add-torrent dialog">
    <div class="content form">
      <h3 class="title">Add magnet link</h3>
      <div class="group">
        <div class="item">
          <label for="add-torrent-link">Link</label>
          <input id="add-torrent-link" type="text" placeholder="magnet:" v-model="addMagnetUri">
        </div>
      </div>
      <div class="group mb-1">
        <div class="item">
          <label for="add-torrent-path">Save path</label>
          <input id="add-torrent-path" type="text" placeholder="C:\Downloads\Torrents\" v-model="addSavePath">
        </div>
      </div>
      <button class="mr-1" @click="add">Add magnet link</button>
      <router-link to="/add-torrent" tag="button" class="link">Or add torrent file</router-link>
    </div>
  </main>
</template>

<script>
export default {
  data () {
    return {
      addMagnetUri: null,
      addSavePath: null
    }
  },
  async mounted () {
    this.addSavePath = '/tmp';

    const { uri } = this.$route.query;

    if (uri) {
      this.addMagnetUri = uri;
    }
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
      await this.$rpc('session.addMagnetLink', {
        magnet_uri: this.addMagnetUri,
        save_path: this.addSavePath
      });

      this.$router.push('/');
      }
    }
  }
}
</script>
