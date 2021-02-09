<template>
  <header>
    <!-- Header: Left -->
    <div class="left">
      <router-link class="logo" to="/" title="PicoTorrent">
        <svg xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" viewBox="0 0 32 32">
          <path fill="var(--header-color)" d="M24.43,12c-.77-1.56-1.65-3.14-2.54-4.61l-3.77,3.69L10,19,5.44,23.5l-4,4A15.65,15.65,0,0,0,4,32l4.11-4A11.06,11.06,0,0,0,27.06,20.2C27.06,18.11,26,15.08,24.43,12ZM16,26a6,6,0,0,1-4.14-1.63l8.9-8.73a11.38,11.38,0,0,1,1.32,4.27A6.09,6.09,0,0,1,16,26ZM5,21.06c0-.28,0-.57,0-.86C4.94,14.1,14.34,0,16,0c.68,0,2.66,2.37,4.76,5.59Z"/>
        </svg> 
      </router-link>
      <router-link to="/add-torrent" class="item tooltip icon" title="Add torrent">
        <i class="bi bi-plus-circle"></i>
        <span>Add torrent</span>
        <span class="title">Add torrent</span>
      </router-link>
      <button class="item control tooltip" title="Resume">
        <i class="bi bi-play"></i>
        <span class="title">Resume</span>
      </button>
      <button class="item control tooltip" title="Pause">
        <i class="bi bi-pause"></i>
        <span class="title">Pause</span>
      </button>
      <button class="item control tooltip" title="Stop">
        <i class="bi bi-stop"></i>
        <span class="title">Stop</span>
      </button>
    </div>
    <!-- Header: Center -->
    <div class="center">
      <div class="filter">
        <i class="bi bi-terminal"></i>
        <label for="filter">Filter torrents</label>
        <input id="filter" type="text" placeholder="Filter torrents...">
      </div>
    </div>
    <!-- Header: Right -->
    <div class="right">
      <router-link to="/settings" class="item tooltip" title="Settings">
        <i class="bi bi-sliders"></i>
        <span class="title">Settings</span>
      </router-link>
      <router-link to="/about" class="item tooltip" title="About">
        <i class="bi bi-info-circle"></i>
        <span class="title">About</span>
      </router-link>
    </div>
  </header>
</template>

<script>
export default {
  name: 'Header'
}
</script>