<template>
  <div>
    <h5>Add listen interface</h5>
    <div class="group">
      <div class="item">
        <label for="host">Device name (or IP)</label>  
        <input type="text" id="host" v-model="addHost" placeholder="127.0.0.1, eth0">
      </div>
      <div class="item">
        <label for="port">Listen port</label>
        <input type="number" id="port" v-model.number="addPort" placeholder="6881">
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

    <button class="mt-1 mr-1" @click="add" :disabled="!addEnabled">
      Add new listen interface
    </button>
    <router-link class="mt-1 link" to="/settings/connection/" tag="button">
      Go back
    </router-link>
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
      addHost: null,
      addPort: null,
      addSsl: false,
      addOutgoing: true,
      addLocal: false
    }
  },
  methods: {
    async add() {
      await this.$rpc('listenInterfaces.create', {
        host: this.addHost,
        port: this.addPort,
        is_local: this.addLocal,
        is_outgoing: this.addOutgoing,
        is_ssl: this.addSsl
      });
      
      this.$router.push('/settings/connection');
    }
  }
}
</script>
