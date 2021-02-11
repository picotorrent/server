<template>
  <div class="settings-connection">
    <h3 class="title">Connection</h3>

    <h5>Listen interfaces</h5>
    <p>These are the listen interfaces that PicoTorrent Server will bind to.</p>
    <div class="table-control">
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
    </div>

    <h5>Add listen interface</h5>
    <div class="group">
      <div class="item">
        <label for="host">Device name (or IP)</label>  
        <input type="text" id="host" v-model="addHost" placeholder="127.0.0.1, eth0">
      </div>
      <div class="item">
        <label for="port">Listen port</label>
        <input type="number" id="port" v-model="addPort" placeholder="6881">
      </div>
    </div>
    
    <div class="check">
      <input type="checkbox" id="ssl" v-model="addSsl">
      <label for="ssl">Is SSL?</label>
    </div>
    <div class="check">
      <input type="checkbox" id="outgoing" v-model="addOutgoing">
      <label for="outgoing">Is outgoing?</label>
    </div>
    <div class="check">
      <input type="checkbox" id="local" v-model="addLocal">
      <label for="local">Is local?</label>
    </div>

    <button class="mt-1" @click="add" :disabled="!addEnabled">
    Add new listen interface
    </button>
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
