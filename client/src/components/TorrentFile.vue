<template>
  <div class="backdrop torrent-file">
    <!-- File: Name -->
    <h2>CentOS-7-x86_64-DVD-2020</h2>
    <!-- File: Status-bar -->
    <div class="status-bar">        
      <div class="item tooltip">
        <i class="bi bi-clock"></i>
        <span class="title">ETA</span> 22m 14s
      </div>
      <div class="item tooltip">
        <i class="bi bi-download"></i>
        <span class="title">Download</span> 25,0 MB/s
      </div>
      <div class="item tooltip">
        <i class="bi bi-upload"></i>
        <span class="title">Upload</span> 1,0 MB/s
      </div>
      <div class="item tooltip">
        <i class="bi bi-arrow-up-circle"></i>
        <span class="title">Seeds</span> 10 of 20
      </div>
      <div class="item tooltip">
        <i class="bi bi-arrow-down-circle"></i>
        <span class="title">Leechers</span> 1 of 230
      </div>
      <div class="item tooltip">
        <i class="bi bi-arrow-left-right"></i>
        <span class="title">Ratio</span> 0.000
      </div>
      <div class="item tooltip">
        <i class="bi bi-globe"></i>
        <span class="title">Availability</span> 124.18
      </div>
    </div>
    <!-- File: Overview -->
    <h3>Overview</h3>
    <div class="row">
      <div class="col">
        <div class="label">Name</div>
        <div class="value">CentOS-7-x86_64-DVD-2020</div>
      </div>
      <div class="col">
        <div class="label">Save path</div>
        <div class="value">C:\Torrents\</div>
      </div>
      <div class="col">
        <div class="label">Comment</div>
        <div class="value">CentOS CD from cdimage.centos.org</div>
      </div>
      <div class="col">
        <div class="label">Private</div>
        <div class="value">No</div>
      </div>
      <div class="col">
        <div class="label">Last download</div>
        <div class="value">5m 1s</div>
      </div>
      <div class="col">
        <div class="label">Total downloaded</div>
        <div class="value">33.3 MB</div>
      </div>
      <div class="col">
        <div class="label">Info HASH</div>
        <div class="value">09c3041fff40e10f7c95ab4a647d7253c53bff91</div>
      </div>
      <div class="col">
        <div class="label">Pieces</div>
        <div class="value">19 of 3789</div>
      </div>
      <div class="col">
        <div class="label">Size</div>
        <div class="value">3.59 GB</div>
      </div>
      <div class="col">
        <div class="label">Ratio</div>
        <div class="value">0.0000</div>
      </div>
      <div class="col">
        <div class="label">Last upload</div>
        <div class="value">5m 1s</div>
      </div>
      <div class="col">
        <div class="label">Total uploaded</div>
        <div class="value">33.3 MB</div>
      </div>
    </div>  
    <!-- File: Piece progress -->
    <h3>Piece progress</h3>
    <div class="progress">
      <div class="bar" role="progressbar" style="width: 70%" aria-valuenow="100" aria-valuemin="0" aria-valuemax="100"></div>
    </div>
    <!-- Tabs -->
    <div class="tabs">
      <div class="menu">
        <button class="item active">Files</button>
        <button class="item">Peers</button>
        <button class="item">Trackers</button>
      </div>
      <div class="content">
        <div class="tab show" id="tab-files">
          <div class="tree">
            <div class="folder">
              <button class="root">CentOS-7-x86-DVD-2020</button>
              <div class="item">
                <div class="name">
                  CentOS-7-x86-DVD-2020.iso
                </div>
                <div class="size">
                  3.59 GB
                </div>
                <div class="progress">
                  <div class="bar" role="progressbar" style="width: 70%" aria-valuenow="100" aria-valuemin="0" aria-valuemax="100"></div>
                </div>
                <div class="priority">
                  Normal
                </div>
              </div>
            </div>
            <div class="folder">
              <button class="root">CentOS-7-x64-DVD-2020</button>
              <div class="item">
                <div class="name">
                  CentOS-7-x864-DVD-2020.iso
                </div>
                <div class="size">
                  3.59 GB
                </div>
                <div class="progress">
                  <div class="bar" role="progressbar" style="width: 70%" aria-valuenow="100" aria-valuemin="0" aria-valuemax="100"></div>
                </div>
                <div class="priority">
                  Normal
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'TorrentFile'
}
</script>