plugin = function (pika, use) {
    const cfg = use('config/scoped');
    const log = use('logger');

    log.info(cfg.foo);
}
