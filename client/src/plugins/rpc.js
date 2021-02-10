import axios from 'axios';

export default {
  install (Vue) {
    Vue.prototype.$rpc = async function rpc(method, args) {
      const response = await axios.post('/api/jsonrpc', {
        jsonrpc: '2.0',
        method,
        params: args || []
      });

      return response.data.result;
    }
  }
}
