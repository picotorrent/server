CREATE TABLE plugin_webhook_hooks (
    id  INTEGER PRIMARY KEY,
    url TEXT                NOT NULL,
    timeout INTEGER         NOT NULL DEFAULT 30
);
