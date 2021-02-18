<template>
  <div class="settings-proxy">
    <h3 class="title">Proxy</h3>

    <h5>Proxy configurations</h5>
    <p>These are the current proxy configurations. To change your current proxy configuration, go to <router-link to="/settings/profiles">the Profile settings</router-link>.</p>
    <div class="table-control">
      <table>
        <thead>
          <tr>
            <th scope="col">Name</th>
            <th scope="col">Type</th>
            <th scope="col">Host</th>
            <th scope="col">Port</th>
            <th scope="col" class="actions">Actions</th>
          </tr>
        </thead>
        <tbody>
          <tr v-for="proxy in proxies" :key="proxy.id">
            <td>{{ proxy.name }}</td>
            <td>{{ proxy.type }}</td>
            <td>{{ proxy.hostname }}</td>
            <td>{{ proxy.port }}</td>
            <td class="actions">
              <button class="remove red" title="Remove" @click="remove(li.id)"><i class="bi bi-trash"></i></button>
            </td>
          </tr>
        </tbody>
      </table>
    </div>

    <h5>Add proxy configuration</h5>
    <div class="group">
      <div class="item">
        <label for="friendlyName">Friendly name</label>
        <input type="text" id="friendlyName" v-model="addModel.name" placeholder="Mullvad">
      </div>
      <div class="item">
        <label for="type">Type</label>
        <select id="type" v-model.number="addModel.type">
          <option value="1">SOCKS4</option>
          <option value="2">SOCKS5</option>
          <option value="3">SOCKS5 (with auth)</option>
          <option value="4">HTTP</option>
          <option value="5">HTTP (with auth)</option>
        </select>
      </div>
    </div>
    <div class="group">
      <div class="item">
        <label for="host">Host</label>
        <input type="text" id="host" v-model="addModel.hostname" placeholder="10.8.0.1">
      </div>
      <div class="item">
        <label for="port">Port</label>
        <input type="number" id="port" v-model.number="addModel.port" placeholder="6881">
      </div>
    </div>
    <div class="group" v-if="authEnabled">
      <div class="item">
        <label for="username">Username</label>
        <input type="text" id="username" v-model="addModel.username">
      </div>
      <div class="item">
        <label for="password">Password</label>
        <input type="password" id="password" v-model="addModel.password">
      </div>
    </div>
    <div class="check">
      <input type="checkbox" id="ssl" v-model="addModel.proxy_hostnames">
      <label for="ssl">Proxy hostnames?</label>
    </div>
    <div class="check">
      <input type="checkbox" id="outgoing" v-model="addModel.proxy_peer_connections">
      <label for="outgoing">Proxy peer connections?</label>
    </div>
    <div class="check">
      <input type="checkbox" id="local" v-model="addModel.proxy_tracker_connections">
      <label for="local">Proxy tracker connections?</label>
    </div>
    <button class="mt-1 icon" @click="add">
      <i class="bi bi-plus-square"></i>
      Add new proxy
    </button>
  </div>
</template>

<script>
export default {
  computed: {
    authEnabled () {
      return this.addModel.type === '3'
        || this.addModel.type === '5';
    }
  },
  data () {
    return {
      addModel: {
        name: null,
        type: null,
        hostname: null,
        port: null,
        username: null,
        password: null,
        proxy_hostnames: true,
        proxy_peer_connections: true,
        proxy_tracker_connections: true
      },
      proxies: []
    }
  },
  async mounted () {
    this.proxies = await this.$rpc('proxy.getAll');
  },
  methods: {
    async remove(id) {
      await this.$rpc('proxy.remove', [ id ]);
      this.proxies = await this.$rpc('proxy.getAll');
    },
    async add() {
      await this.$rpc('proxy.create', this.addModel);
      this.proxies = await this.$rpc('proxy.getAll');
    }
  }
}
</script>
