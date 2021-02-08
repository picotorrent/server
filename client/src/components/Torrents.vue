<template>
  <div class="backdrop torrents" v-if="ws">
    <table v-if="torrents">
      <thead>
        <tr>
          <th scope="col">Name</th>
          <th scope="col">Size</th>
          <th scope="col">DL</th>
          <th scope="col">UL</th>
          <th scope="col">State</th>
          <th scope="col">Progress</th>
          <th></th>
        </tr>
      </thead>
      <tbody>
        <tr v-for="(torrent, infoHash) in torrents" :key="infoHash">
          <td class="name">{{ torrent.name }}</td>
          <td class="size">{{ torrent.size_wanted }}</td>
          <td class="download">{{ torrent.dl }}</td>
          <td class="upload">{{ torrent.ul }}</td>
          <td class="status">{{ torrent.state }}</td>
          <td class="progress"><progress max="100" :value="torrent.progress * 100" style="width: 100%;" /></td>
          <td class="actions">
            <button @click="remove(infoHash)">Remove</button>
          </td>
        </tr>
      </tbody>
    </table>
    <div class="empty" v-if="!torrents">
      <div class="content">
        <h4 class="bold">There are no torrents</h4>
        <p>Looks so empty around here...</p>
        <h5>Add torrent</h5>
        <div class="group">
          <label for="add-torrent">File</label>
          <input id="add-torrent-file" type="file" ref="torrentFile">
        </div>
        <div class="group">
          <label for="add-torrent-path">Save path</label>
          <input id="add-torrent-path" type="text" v-model="addSavePath">
        </div>
        <button @click="add">Add torrent</button>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'Torrents'
}
</script>