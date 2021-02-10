import VueRouter from 'vue-router';

import About from '@/pages/About';
import AddTorrent from '@/pages/AddTorrent';
import Home from '@/pages/Home';
import SettingsCommon from '@/pages/settings/Common';
import SettingsConnection from '@/pages/settings/Connection';
import SettingsDownloads from '@/pages/settings/Downloads';
import SettingsLayout from '@/pages/settings/Layout';
import SettingsProxy from '@/pages/settings/Proxy';

export default new VueRouter({
  routes: [
    { path: '/', component: Home },
    { path: '/about', component: About },
    { path: '/add-torrent', component: AddTorrent },
    {
      path: '/settings',
      component: SettingsLayout,
      redirect: '/settings/common',
      children: [
        { path: 'common',     component: SettingsCommon     },
        { path: 'connection', component: SettingsConnection },
        { path: 'downloads',  component: SettingsDownloads  },
        { path: 'proxy',      component: SettingsProxy      }
      ]
    }
  ]
});
