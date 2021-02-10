<template>
  <main class="settings">
    <div class="sidebar">
      <h2 class="title">Settings</h2>
      <div class="menu">
        <router-link to="/settings/common" active-class="active" class="item">
          <i class="bi bi-star"></i>
          <span>Common</span>
        </router-link>
        <a href="#" class="item">
          <i class="bi bi-grid"></i>
          <span>General</span>
        </a>
        <router-link to="/settings/downloads" active-class="active" class="item">
          <i class="bi bi-download"></i>
          <span>Downloads</span>
        </router-link>
        <router-link to="/settings/connection" active-class="active" class="item">
          <i class="bi bi-hdd-network"></i>
          <span>Connection</span>
        </router-link>
        <router-link to="/settings/proxy" active-class="active" class="item">
          <i class="bi bi-hdd-rack"></i>
          <span>Proxy</span>
        </router-link>
        <a href="#" class="item">
          <i class="bi bi-lightning"></i>
          <span>Advanced</span>
        </a>
      </div>
    </div>
    <div class="content">
      <router-view />
    </div>
  </main>
</template>
