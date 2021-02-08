<template>
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
</template>

<script>
export default {
  data () {
    return {
      addSavePath: null
    }
  },
  methods: {
    async add() {

    }
  }
}
</script>
