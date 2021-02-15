<template>
  <main class="home">
    <StatusBar />
    <TorrentList />
    <!-- WIP -->
    <TorrentFile class="hide" />
    <!-- /WIP -->
  </main>
</template>

<script>
import StatusBar from '@/components/StatusBar';
import TorrentList from '@/components/TorrentList';
import TorrentFile from '@/components/TorrentFile';

export default {
  components: {
    StatusBar,
    TorrentList,
    TorrentFile
  }
}
</script>
