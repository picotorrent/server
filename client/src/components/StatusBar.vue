<template>
  <div class="backdrop status-bar">
    <div class="item">
      <i class="bi bi-file-earmark"></i>
      <span>Torrents</span> 
    </div>
    <div class="item">
      <i class="bi bi-download"></i>
      <span>Download</span> 25,0 MB/s
    </div>
    <div class="item">
      <i class="bi bi-upload"></i>
      <span>Upload</span> 1,0 MB/s
    </div>
    <div class="item">
      <i class="bi bi-hdd-network"></i>
      <span>DHT</span> 280 node(s)
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
export default {
  name: 'Statusbar'
}
</script>