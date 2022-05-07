function configGenerator(pika) {
    return function config(settings) {
        pika.log('Overriding settings');

        pika.log(
            JSON.stringify(
                Object.keys(settings)));

        settings.enable_dht = false;

        pika.timer(function ()  {
            pika.log(settings.enable_dht);
            this.cancel();
        }, 1000);
    }
}

plugin = function (pika) {
    pika.on('session.configure', configGenerator(pika));
}
