<template>
  <header>
    <!-- Header: Left -->
    <div class="left">
      <button class="logo" href="./" title="PicoTorrent">
        <svg xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" viewBox="0 0 32 32">
          <path fill="var(--header-color)" d="M24.43,12c-.77-1.56-1.65-3.14-2.54-4.61l-3.77,3.69L10,19,5.44,23.5l-4,4A15.65,15.65,0,0,0,4,32l4.11-4A11.06,11.06,0,0,0,27.06,20.2C27.06,18.11,26,15.08,24.43,12ZM16,26a6,6,0,0,1-4.14-1.63l8.9-8.73a11.38,11.38,0,0,1,1.32,4.27A6.09,6.09,0,0,1,16,26ZM5,21.06c0-.28,0-.57,0-.86C4.94,14.1,14.34,0,16,0c.68,0,2.66,2.37,4.76,5.59Z"/>
        </svg> 
      </button>
      <template v-if="ws">
        <button class="item icon"><i class="bi bi-plus-circle"></i> <span>Add torrent</span></button>
        <button class="item control"><i class="bi bi-play"></i></button>
        <button class="item control"><i class="bi bi-pause"></i></button>
        <button class="item control"><i class="bi bi-stop"></i></button>
      </template>
    </div>
    <!-- Header: Center -->
    <div class="center" v-if="ws">
      <div class="filter">
        <i class="bi bi-terminal"></i>
        <label for="filter">Filter torrents</label>
        <input id="filter" type="text" placeholder="Filter torrents...">
      </div>
    </div>
    <!-- Header: Right -->
    <div class="right">
      <button to="/settings" class="item" title="Settings"><i class="bi bi-sliders"></i></button>
      <button to="/about" class="item" title="About"><i class="bi bi-info-square"></i></button>
    </div>
  </header>
</template>

<script>
export default {
  name: 'Header'
}
</script>