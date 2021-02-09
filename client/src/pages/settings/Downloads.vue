<template>
  <div>
    <h3 class="title">Downloads</h3>
    <hr>
    <h5>Default save path</h5>
    <p>The default path where downloads will be saved.</p>
    <input v-model="defaultSavePath" type="text">
    <hr>
    <button @click="save">Save</button>
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
