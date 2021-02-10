<template>
  <div>
    <h3 class="title">Connection</h3>
    <hr>
    <h5>Listen interfaces</h5>
    <p>These are the listen interfaces that PicoTorrent Server will bind to.</p>
    <table>
      <thead>
        <tr>
          <th scope="col">Host</th>
          <th scope="col">Port</th>
          <th scope="col" class="actions">Actions</th>
        </tr>
      </thead>
      <tbody>
        <tr v-for="li in listenInterfaces" :key="li.id">
          <td>{{ li.host }}</td>
          <td>{{ li.port }}</td>
          <td class="actions">
            <button class="remove" title="Remove" @click="remove(li.id)"><i class="bi bi-trash"></i></button>
          </td>
        </tr>
      </tbody>
    </table>

    <h5>Add listen interface</h5>
    <hr>
    <p><label for="host">Device name (or IP)</label></p>
    <input type="text" id="host" v-model="addHost" placeholder="127.0.0.1, eth0">
    <p><label for="port">Listen port</label></p>
    <input type="number" id="port" v-model="addPort" placeholder="6881">
    <div>
      <label for="ssl">Is SSL?</label>
      <input type="checkbox" id="ssl" v-model="addSsl">
    </div>
    <div>
      <label for="outgoing">Is outgoing?</label>
      <input type="checkbox" id="outgoing" v-model="addOutgoing">
    </div>
    <div><button @click="add">Add</button></div>
  </div>
</template>

<script>
export default {
  data () {
    return {
      listenInterfaces: [],
      addHost: null,
      addPort: null,
      addSsl: false,
      addOutgoing: false
    }
  },
  async mounted () {
    this.listenInterfaces = await this.$rpc('listenInterfaces.getAll');
  },
  methods: {
    async remove(id) {
      await this.$rpc('listenInterfaces.remove', [ id ]);
    },
    async add() {
      await this.$rpc('listenInterfaces.add', {
        host: this.addHost,
        port: this.addPort,
        is_ssl: this.addSsl,
        is_outgoing: this.addOutgoing
      });
    }
  }
}
</script>
