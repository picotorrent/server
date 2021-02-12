<template>
  <div class="settings-profiles">
    <h3 class="title">Profiles</h3>

    <template v-if="activeProfile">
      <h5>Active profile: {{ activeProfile.name }}</h5>
      <p>This is your current profile.</p>
      <div class="group">
        <div class="item">
          <label for="currentProxy">Proxy</label>
          <select id="currentProxy" v-model.number="currentProxy">
            <option :value="null">None</option>
            <option v-for="proxy in proxies" :key="proxy.id" :value="proxy.id">{{ proxy.name }}</option>
          </select>
        </div>
      </div>
    </template>

    <button class="mt-1" @click="save">Save profile</button>
  </div>
</template>

<script>
export default {
  computed: {
    activeProfile () {
      return this.profiles.filter(p => p.is_active)[0];
    }
  },
  data () {
    return {
      profiles: [],
      proxies: [],
      currentProxy: null
    }
  },
  async mounted () {
    this.profiles = await this.$rpc('profiles.getAll');
    this.proxies  = await this.$rpc('proxy.getAll');
    this.currentProxy = this.activeProfile.proxy_id;
  },
  methods: {
    async save () {
      await this.$rpc('profiles.update', {
        id: this.activeProfile.id,
        proxy_id: this.currentProxy
      });
    }
  }
}
</script>
