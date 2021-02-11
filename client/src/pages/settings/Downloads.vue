<template>
  <div class="settings-downloads">
    <h3 class="title">Downloads</h3>

    <h5>General</h5>
    <p>The default path where downloads will be saved.</p>
    <div class="group">
      <div class="item">
        <label for="default-save-path">Default save path</label>
        <input id="default-save-path" type="text" placeholder="C:\Downloads\Torrents\" v-model="defaultSavePath">
      </div>
    </div>
    <button class="mt-1" @click="save">Save settings</button>
  </div>
</template>

<script>
import axios from 'axios';

export default {
  data () {
    return {
      defaultSavePath: null
    }
  },
  async mounted () {
    const res = await axios.post('/api/jsonrpc', {
      jsonrpc: '2.0',
      method: 'config.get',
      params: [ 'default_save_path' ]
    });

    this.defaultSavePath = res.data.result.default_save_path;
  },
  methods: {
    async save() {
      await axios.post('/api/jsonrpc', {
        jsonrpc: '2.0',
        method: 'config.set',
        params: {
          default_save_path: this.defaultSavePath
        }
      });
    }
  }
}
</script>
