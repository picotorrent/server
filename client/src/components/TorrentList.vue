<template>
  <div class="backdrop torrent-list">
    <table v-if="torrents">
      <thead>
        <tr>
          <th scope="col" class="checkbox"> 
            <div class="check">
              <input id="check-all-torrents" type="checkbox">
              <label for="check-all-torrents" class="aria">Select all torrents</label>
            </div>
          </th>
          <th scope="col">Name</th>
          <th scope="col">Size</th>
          <th scope="col">DL</th>
          <th scope="col">UL</th>
          <th scope="col">State</th>
          <th scope="col">Progress</th>
          <th scope="col" class="actions">Actions</th>
        </tr>
      </thead>
      <tbody>
        <tr v-for="(torrent, infoHash) in torrents" :key="infoHash" :class="torrent.state">
          <td class="checkbox">
            <div class="check">
              <input :id="'check-torrent-' + infoHash" type="checkbox">
              <label :for="'check-torrent-' + infoHash" class="aria">Select torrent: {{ torrent.name }}</label>
            </div>
          </td>
          <td class="name">{{ torrent.name }}</td>
          <td class="size">{{ torrent.total_wanted | fileSize }}</td>
          <td class="download">{{ torrent.dl | speed }}</td>
          <td class="upload">{{ torrent.ul | speed }}</td>
          <td class="status">{{ torrent.state }}</td>
          <td class="progress">
            <progress max="100" :value="torrent.progress * 100" >{{ torrent.progress * 100 }}</progress>
          </td>
          <td class="actions">
            <button class="remove" title="Remove" @click="remove(infoHash)"><i class="bi bi-trash"></i></button>
          </td>
        </tr>
      </tbody>
    </table>
  </div>
</template>

<script>
import { mapGetters } from 'vuex';

export default {
  name: 'Torrents',
  computed: {
    ...mapGetters({
      torrents: 'torrents/all'
    })
  },
  methods: {
    async remove (infoHash) {
      await this.$rpc('session.removeTorrent',[ infoHash ]);
    }
  }
}
</script>
