<template>
  <div class="toast" role="alert" :id="id" :class="classes">
    <div class="header" v-if="title">
      <i class="icon bi" :class="'bi-'+icon" v-if="icon"></i>
      <div class="title">
        {{ title }}
      </div>
      <button class="close" title="Close" v-on:click="close">
        <i class="icon bi bi-x"></i>
      </button>
    </div>
    <div class="body">
      {{ message }}
    </div>
  </div>
</template>

<script>
export default {
  name: 'Toast',
  props: [
    'id',
    'classes',
    'icon',
    'title',
    'message'
  ],
  methods: {
    close: function () {
      this.$el.classList.add('is-closing');
      setTimeout(() => {
        this.$destroy();
        this.$el.parentNode.removeChild(this.$el);
      }, 500);
    }
  }
}
</script>