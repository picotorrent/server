function config (settings) {
    settings.enable_dht = false;
}

plugin = function (pika) {
    pika.on('session.configure', config);

    const timer = pika.timer(function () {
        pika.log('tick');
        timer.cancel();
    }, 5000);
}
