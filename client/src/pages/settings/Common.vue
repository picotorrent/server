<template>
  <div class="settings-common">
    <h3 class="title">General</h3>

    <h4>User Interface</h4>

    <h5>Language</h5>
    <div class="group">
      <div class="item">
        <label for="language">Define the UI language</label>
        <select id="language" name="language">
          <option value="">English</option>
          <option value="">Spanish</option>
        </select>
      </div>
    </div>
    
    <h5>Theme</h5>
    <div class="group">
      <div class="item">
        <label for="language">Define the UI theme</label>
        <select id="language" name="language">
          <option value="light">Light</option>
          <option value="dark">Dark</option>
          <option value="default">Default browser theme</option>
        </select>
      </div>
    </div>

    <h4>Miscellaneous</h4>

    <h5>Labels</h5>
    <div class="check">
      <input id="label-bg" type="checkbox" name="">
      <label for="label-bg">
        Use color of label as background in torrent list
      </label>
    </div>
  </div>
</template>
