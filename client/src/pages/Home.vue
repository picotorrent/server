<template>
  <main class="home">
    <StatusBar />
    <TorrentList />
    <!-- WIP -->
    <File class="hide" />
    <!-- /WIP -->
  </main>
</template>

<script>
import StatusBar from '@/components/StatusBar';
import TorrentList from '@/components/TorrentList';
import File from '@/components/File';

export default {
  components: {
    StatusBar,
    TorrentList,
    File
  }
}
</script>
