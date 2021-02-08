import VueRouter from 'vue-router';

import About from '@/pages/About';
import AddTorrent from '@/pages/AddTorrent';
import Home from '@/pages/Home';
import Settings from '@/pages/Settings';

export default new VueRouter({
  routes: [
    { path: '/', component: Home },
    { path: '/about', component: About },
    { path: '/add-torrent', component: AddTorrent },
    { path: '/settings', component: Settings },
  ]
});
