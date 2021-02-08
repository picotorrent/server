module.exports = {
  devServer: {
    proxy: {
      '^/api': {
        target: 'http://localhost:1337',
        ws: true
      }
    }
  },
  css: {
    loaderOptions: {
      sass: {
        implementation: require('sass')
      }
    }
  }
};
