<template>
  <div>
    <fieldset>
      <legend>Add torrent</legend>
      File: <input type="file" ref="torrentFile"><br>
      Save path: <input type="text" v-model="addSavePath"><br>
      <button @click="add">Add torrent</button>
    </fieldset>

    <fieldset>
      <legend>Torrents</legend>
      <table width="100%">
        <thead>
          <tr>
            <th>Name</th>
            <th align="right">Size</th>
            <th align="right">DL</th>
            <th align="right">UL</th>
            <th align="right">State</th>
            <th>Progress</th>
            <th></th>
          </tr>
        </thead>
          <tbody>
          <tr v-for="(torrent, infoHash) in torrents" :key="infoHash">
            <td>{{ torrent.name }}</td>
            <td align="right">{{ torrent.size_wanted }}</td>
            <td align="right">{{ torrent.dl }}</td>
            <td align="right">{{ torrent.ul }}</td>
            <td align="right">{{ torrent.state }}</td>
            <td><progress max="100" :value="torrent.progress * 100" style="width: 100%;" /></td>
            <td align="right">
              <button @click="remove(infoHash)">Remove</button>
            </td>
          </tr>
        </tbody>
      </table>
    </fieldset>
  </div>
</template>

<script>
import axios from 'axios';
import Vue from 'vue';

export default {
  name: 'App',
  data () {
    return {
      addSavePath: null,
      torrents: {},
      ws: null
    }
  },
  mounted () {
    this.ws = new WebSocket(`ws://${location.host}/api/ws`);
    this.ws.onmessage = (ev) => {
      const data = JSON.parse(ev.data);

      switch (data.type) {
        case 'init':
          for (const infoHash in data.torrents) {
            Vue.set(this.torrents, infoHash, data.torrents[infoHash]);
          }
          break;

        case 'torrent.added':
          Vue.set(this.torrents, data.torrent.info_hash, data.torrent);
          break;

        case 'torrent.removed':
          Vue.delete(this.torrents, data.info_hash);
          break;

        case 'torrent.updated':
          for (const infoHash in data.torrents) {
            Vue.set(this.torrents, infoHash, data.torrents[infoHash]);
          }
          break;
      }
    }
  },
  methods: {
    async add () {
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
    },

    async remove (infoHash) {
      await axios.post('/api/jsonrpc', {
        method: 'session.removeTorrent',
        params: [ infoHash ]
      });
    }
  }
}
</script>
