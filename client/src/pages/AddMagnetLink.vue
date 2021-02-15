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
    const {
      default_save_path
    } = await this.$rpc('config.get', [ 'default_save_path' ]);

    this.addSavePath = default_save_path;

    const { uri } = this.$route.query;

    if (uri) {
      this.addMagnetUri = uri;
    }
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
