import Vue from 'vue'
import VueRouter from 'vue-router'
import prettyBytes from 'pretty-bytes'

import App from './App.vue'
import router from './router'
import store from './store'

Vue.config.productionTip = false
Vue.use(VueRouter)

Vue.filter('fileSize', (val) => {
  if (!val) { return '-'; }
  return prettyBytes(val)
});

Vue.filter('speed', (val) => {
  if (!val || val === 0) {
    return '-';
  }
  return `${prettyBytes(val)}/s`;
});

new Vue({
  render: h => h(App),
  router,
  store
}).$mount('#app')
