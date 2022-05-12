//pika: { "name": "prometheus-exporter" }

var metrics = null;

plugin = function (pika, use) {
    const httpServer = use('http/server');

    httpServer.get('/metrics', function (_, res) {
        res.contentType('text/plain; version=0.0.4')
            .status(metrics === null ? 400 : 200)
            .send(metrics || 'No metrics available');
    });

    pika.on('session.stats', function (stats) {
        metrics = null;

        const keys = Object.keys(stats);

        for (var i = 0; i < keys.length; i++) {
            const key = keys[i];
            metrics += 'pika_' + key.replace(/\./gi, '_') + ' ' + stats[key] + '\n';
        }
    });
}
