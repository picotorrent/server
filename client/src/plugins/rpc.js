import axios from 'axios';

async function makeRequest(method, params) {
  const response = await axios.post('/api/jsonrpc', {
    jsonrpc: '2.0',
    method,
    params
  });

  return response.data.result;
}

export default {
  install (Vue) {
    Vue.prototype.$rpc = async function rpc(method, args) {
      if (Array.isArray(method)) {
        // Array is [ method, args ]
        const result = [];

        for (const exec of method) {
          result.push(await makeRequest(exec[0], exec[1] || []));
        }

        return result;
      }

      return await makeRequest(method, args || []);
    }
  }
}
