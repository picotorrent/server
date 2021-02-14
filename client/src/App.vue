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
  }
}
</script>

<style lang="scss">
@import './scss/style.scss';
@import '../node_modules/bootstrap-icons/font/bootstrap-icons.css';
</style>