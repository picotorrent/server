<template>
  <div class="backdrop status-bar">
    <!-- Torrents -->
    <div class="item torrents">
      <i class="bi bi-file-earmark"></i>
      <span>Torrents</span> {{ torrentsCount }}
    </div>
    <!-- Nodes -->
    <div class="item nodes">
      <i class="bi bi-hdd-network"></i>
      <span>DHT nodes</span> {{ sessionStats('dht.dht_nodes') }}
    </div>
    <!-- Download -->
    <div class="item download">
      <i class="bi bi-download"></i>
      <span>Download</span> {{ torrentsTotalDl | speed }}
    </div>
    <!-- Upload -->
    <div class="item upload">
      <i class="bi bi-upload"></i>
      <span>Upload</span> {{ torrentsTotalUl | speed }}
    </div>
    <!-- Graphs -->
    <div class="graphs">
      <!-- Graph: Dwnload & Upload -->
      <div class="both">
        <div class="label">
          <div class="dl">
            <span class="aria">Download</span>
            <i class="bi bi-arrow-down"></i> 25,0 MB/s
          </div>
          <div class="ul">
            <span class="aria">Upload</span>
            <i class="bi bi-arrow-up"></i> 1,0 MB/s
          </div>
        </div>
        <div class="graph">
          <canvas id="chart-both"></canvas>
        </div>
      </div>
      <!-- Graph: Dwnload -->
      <div class="download hide">
        <div class="label">
          <div class="dl">
            <span class="aria">Download</span>
            <i class="bi bi-arrow-down"></i> 25,0 MB/s
          </div>
        </div>
        <div class="graph">
          <canvas id="chart-download"></canvas>
        </div>
      </div>
      <!-- Graph: Upload -->
      <div class="upload hide">
        <div class="label">
          <div class="ul">
            <span class="aria">Upload</span>
            <i class="bi bi-arrow-up"></i> 1,0 MB/s
          </div>
        </div>
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
