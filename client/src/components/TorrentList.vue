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
          <th scope="col" class="name">Name</th>
          <th scope="col" class="size">Size</th>
          <th scope="col" class="download">DL</th>
          <th scope="col" class="upload">UL</th>
          <th scope="col" class="status">State</th>
          <th scope="col" class="progress">Progress</th>
          <th scope="col" class="actions">Actions</th>
        </tr>
      </thead>
      <tbody>
        <tr v-for="(torrent, infoHash) in torrents" :key="infoHash" :class="torrentStatus(torrent.state)">
          <td class="checkbox">
            <div class="check">
              <input :id="'check-torrent-' + infoHash" type="checkbox">
              <label :for="'check-torrent-' + infoHash" class="aria">Select torrent: {{ torrent.name }}</label>
            </div>
          </td>
          <td class="name">
            {{ torrent.name }}
          </td>
          <td class="size">
            <i class="bi bi-file-zip"></i>
            {{ torrent.total_wanted | fileSize }}
          </td>
          <td class="download">
            <i class="bi bi-download"></i>
            {{ torrent.dl | speed }}
          </td>
          <td class="upload">
            <i class="bi bi-upload"></i>
            {{ torrent.ul | speed }}
          </td>
          <td class="status">
            <i class="bi icon-status"></i>
            {{ torrentStatus(torrent.state) }}
          </td>
          <td class="progress">
            <progress max="100" :value="torrent.progress * 100" >{{ torrent.progress * 100 }}</progress>
          </td>
          <td class="actions">
            <button class="remove red" title="Remove" @click="remove(infoHash)">
              <i class="bi bi-trash"></i>
            </button>
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
    },
    torrentStatus(state) {
      switch (state) {        
        // checking_files
        case 1: return 'checking_files';
        // downloading_metadata
        case 2: return 'downloading_metadata';
        // downloading
        case 3: return 'downloading';
        // finished
        case 4: return 'finished';
        // seeding
        case 5: return 'seeding';
        // checking_resume_dat
        case 7: return 'checking_resume_dat';
      }
      return 'undefined';
    }
  }
}
</script>
