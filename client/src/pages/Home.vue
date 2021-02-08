<template>
  <div>
    <StatusBar />
    <TorrentList />
    <File class="hide" />
  </div>
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
