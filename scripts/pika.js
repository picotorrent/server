function configGenerator(pika) {
    return function config(settings) {
        pika.log('Overriding settings');

        pika.log(
            JSON.stringify(
                Object.keys(settings)));

        settings.enable_dht = true;
    }
}

plugin = function (pika) {
    pika.on('session.configure', configGenerator(pika));

    pika.timer(function () {
        pika.log('tick');
        this.cancel();
    }, 1000);
}
