function config(settings) {
    settings.enable_dht = false;
}

plugin = function (pika) {
    pika.on('session.configure', config);
}
