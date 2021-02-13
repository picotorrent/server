import VueRouter from 'vue-router';

import About from '@/pages/About';
import AddMagnetLink from '@/pages/AddMagnetLink';
import AddTorrent from '@/pages/AddTorrent';
import Home from '@/pages/Home';
import SettingsCommon from '@/pages/settings/Common';

import SettingsConnectionLayout from '@/pages/settings/connection/Layout';
import SettingsConnectionIndex from '@/pages/settings/connection/Index';
import SettingsConnectionAddListenIntercace from '@/pages/settings/connection/AddListenInterface';

import SettingsDownloads from '@/pages/settings/Downloads';
import SettingsLayout from '@/pages/settings/Layout';
import SettingsProfiles from '@/pages/settings/Profiles';
import SettingsProxy from '@/pages/settings/Proxy';

export default new VueRouter({
  routes: [
    { path: '/', component: Home },
    { path: '/about', component: About },
    { path: '/add-magnet-link', component: AddMagnetLink },
    { path: '/add-torrent', component: AddTorrent },
    {
      path: '/settings',
      component: SettingsLayout,
      redirect: '/settings/common',
      children: [
        { path: 'common',     component: SettingsCommon },
        {
          path: 'connection',
          component: SettingsConnectionLayout,
          children: [
            { path: '', component: SettingsConnectionIndex },
            { path: 'add-listen-interface', component: SettingsConnectionAddListenIntercace }
          ]
        },
        { path: 'downloads',  component: SettingsDownloads  },
        { path: 'profiles',   component: SettingsProfiles   },
        { path: 'proxy',      component: SettingsProxy      }
      ]
    }
  ]
});
