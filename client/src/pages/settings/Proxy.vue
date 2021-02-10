<template>
  <div>
    <h3 class="title">Proxy</h3>
    <hr>
    <h5>Proxy configurations</h5>
    <p>These are the current proxy configurations stored.</p>
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

    <h5>Add proxy configuration</h5>
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
    <div>
      <label for="local">Is local?</label>
      <input type="checkbox" id="local" v-model="addLocal">
    </div>
    <div><button @click="add" :disabled="!addEnabled">Add</button></div>
  </div>
</template>

<script>
export default {
  computed: {
    addEnabled () {
      return this.addHost && this.addPort;
    }
  },
  data () {
    return {
      listenInterfaces: [],
      addHost: null,
      addPort: null,
      addSsl: false,
      addOutgoing: true,
      addLocal: false
    }
  },
  async mounted () {
    this.listenInterfaces = await this.$rpc('listenInterfaces.getAll');
  },
  methods: {
    async remove(id) {
      await this.$rpc('listenInterfaces.remove', [ id ]);
      this.listenInterfaces = await this.$rpc('listenInterfaces.getAll');
    },
    async add() {
      await this.$rpc('listenInterfaces.create', {
        host: this.addHost,
        port: parseInt(this.addPort, 10),
        is_local: this.addLocal,
        is_outgoing: this.addOutgoing,
        is_ssl: this.addSsl
      });

      this.addHost = null;
      this.addPort = null;
      this.addLocal = false;
      this.addOutgoing = true;
      this.addSsl = false;

      this.listenInterfaces = await this.$rpc('listenInterfaces.getAll');
    }
  }
}
</script>
