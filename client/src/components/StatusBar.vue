<template>
  <div class="backdrop status-bar">
    <div class="item torrents">
      <i class="bi bi-file-earmark"></i>
      <span>Torrents</span> {{ torrentsCount }}
    </div>
    <div class="item download">
      <i class="bi bi-download"></i>
      <span>Download</span> {{ torrentsTotalDl | speed }}
    </div>
    <div class="item upload">
      <i class="bi bi-upload"></i>
      <span>Upload</span> {{ torrentsTotalUl | speed }}
    </div>
    <div class="item nodes">
      <i class="bi bi-hdd-network"></i>
      <span>DHT nodes</span> {{ sessionStats('dht.dht_nodes') }}
    </div>
    <div class="graphs">
      <div class="download">
        <div class="label">
          <span class="down">
            <i class="bi bi-arrow-down"></i> 25,0 MB/s
          </span>
          <span class="up">
            <i class="bi bi-arrow-up"></i> 1,0 MB/s
          </span>
        </div>
        <div class="graph">
          <canvas id="chart-download"></canvas>
        </div>
      </div>
      <div class="upload">
        <span>Upload: 1,0 MB/s</span>
        <div class="graph">
          <canvas id="chart-upload"></canvas>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
import { mapGetters } from 'vuex'

export default {
  name: 'StatusBar',
  computed: {
    ...mapGetters({
      sessionStats: 'session/byId',
      torrentsCount: 'torrents/count',
      torrentsTotalDl: 'torrents/dl_total',
      torrentsTotalUl: 'torrents/ul_total'
    })
  }
}
</script>
