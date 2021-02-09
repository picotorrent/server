<template>
  <div id="app">
    <Header />
    <router-view />
  </div>
</template>

<script>
import Header from '@/components/Header';

export default {
  name: 'App',
  components: {
    Header,
  },
  mounted () {
    // navigator.registerProtocolHandler('magnet', 'http://localhost:8080/ting/%s', 'PicoTorrent')
  }
}
</script>

<style lang="scss">
@import './scss/style.scss';
@import '../node_modules/bootstrap-icons/font/bootstrap-icons.css';
</style>