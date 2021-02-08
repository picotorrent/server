<template>
  <div class="backdrop torrents">
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
          <td class="size">{{ torrent.total_wanted | fileSize }}</td>
          <td class="download">{{ torrent.dl | speed }}</td>
          <td class="upload">{{ torrent.ul | speed }}</td>
          <td class="status">{{ torrent.state }}</td>
          <td class="progress"><progress max="100" :value="torrent.progress * 100" style="width: 100%;" /></td>
          <td class="actions">
            <button @click="remove(infoHash)">Remove</button>
          </td>
        </tr>
      </tbody>
    </table>
  </div>
</template>

<script>
import axios from 'axios';
import prettyBytes from 'pretty-bytes';
import { mapGetters } from 'vuex';

export default {
  name: 'Torrents',
  computed: {
    ...mapGetters({
      torrents: 'torrents/all'
    })
  },
  filters: {
    fileSize (val) {
      if (!val) { return '-'; }
      return prettyBytes(val)
    },
    speed (val) {
      if (!val || val === 0) {
        return '-';
      }
      return `${prettyBytes(val)}/s`;
    }
  },
  methods: {
    async remove (infoHash) {
      await axios.post('/api/jsonrpc', {
        method: 'session.removeTorrent',
        params: [ infoHash ]
      });
    }
  }
}
</script>