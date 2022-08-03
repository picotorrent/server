const http = require('http');
const native = require('bindings')('native');

const session = new native.Session({
  foo: 123
});

const server = http.createServer((req, res) => {
    console.log(req, res);
});

server.listen(
    process.env.PIKA_HTTP_PORT || 3000,
    () => console.log('> Pika HTTP started'));
