<template>
  <div class="settings-common">
    <h3 class="title">General</h3>

    <h4>Miscellaneous</h4>

    <h5>Magnet URI handling</h5>
    <div class="group">
      <div class="item">
        <label for="protocolHandlerUrl">Protocol handler URL</label>
        <input id="protocolHandlerUrl" type="text" v-model="protocolHandlerUrl">
      </div>
    </div>
    <button class="mt-1" @click.prevent="registerProtocolHandler">Register protocol handler</button>
  </div>
</template>

<script>
export default {
  data () {
    return {
      protocolHandlerUrl: `${location.protocol}//${location.host}/#/add-magnet-link?uri=%s`
    }
  },
  methods: {
    registerProtocolHandler () {
      window.navigator.registerProtocolHandler(
        'magnet',
        this.protocolHandlerUrl,
        'PicoTorrent');
    }
  }
}
</script>
