<template>
  <div>
    <h3 class="title">Connection</h3>
    <hr>
    <h5>Listen interfaces</h5>
    <p>These are the listen interfaces that PicoTorrent Server will bind to.</p>
    <table>
      <thead>
        <tr>
          <th>Host</th>
          <th>Port</th>
        </tr>
      </thead>
      <tbody>
        <tr v-for="li in listenInterfaces" :key="li.id">
          <td>{{ li.host }}</td>
          <td>{{ li.port }}</td>
        </tr>
      </tbody>
    </table>
  </div>
</template>

<script>
import axios from 'axios';

export default {
  data () {
    return {
      listenInterfaces: []
    }
  },
  async mounted () {
    const res = await axios.post('/api/jsonrpc', {
      jsonrpc: '2.0',
      method: 'listenInterfaces.getAll',
      params: []
    });

    this.listenInterfaces = res.data.result;
  },
  methods: {
    async save() {
    }
  }
}
</script>
