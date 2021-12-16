# PicoTorrent Server is a high performance BitTorrent server

Built to handle tens of thousands of torrents, PicoTorrent Server is an
excellent BitTorrent server. Perfect for a seedbox setup where performance
and ease of use is key.

## Features

 * Full support for BitTorrent 2.0 ([BEP-52](http://bittorrent.org/beps/bep_0052.html)),
   v1, v2 and v1+v2 hybrid torrents.
 * A simple, clean, embedded web UI for management.
 * All data is stored in a SQLite database which makes backups trivial.
 * A Prometheus exporter for monitoring session statistics.

 ## Getting started

 PicoTorrent Server is distributed via Docker which makes it easy to run and
 distribute.

 ```sh
 docker run -p 8080:1337 ghcr.io/picotorrent/server:{VERSION}
 ```

 ### Using `docker-compose`

 This is an example file with more advanced usage - the container is set to
 read-only and volumes are mapped in for data storage.

```yaml
version: '3'

services:
  picotorrent:
    image: ghcr.io/picotorrent/server:{VERSION}
    ports:
      - 8080:1337
    environment:
      PICOTORRENT_DB_FILE: /data/PicoTorrent.sqlite
    volumes:
      - /mnt/downloads:/downloads
      - /var/lib/picotorrent:/data
```

### Configuration

Use these environment variables to configure the behavior of PicoTorrent
Server.

 * `PICOTORRENT_DB_FILE` - the path to a SQLite database to use for data storage.
   The file will be created if it does not exist. _Defaults to right next to the
   PicoTorrentServer binary_.
 * `PICOTORRENT_HTTP_HOST` - the IP address to listen on for the HTTP server.
   _Defaults to `127.0.0.1`_.
 * `PICOTORRENT_HTTP_PORT` - the port to use for the HTTP server. _Defaults to `1337`_.
 * `PICOTORRENT_WEBROOT_PATH` - the path to the web UI. If unset, no web UI will
   be served. _Defaults to `/app/client` in the Docker container which is where
   the bundled web UI is located_.

#### Prometheus

 * `PICOTORRENT_PROMETHEUS_EXPORTER` - if this is set, the Prometheus exporter
   is enabled and will serve metrics on `/metrics`. _Defaults to unset_.

 ## Setting up for development

 Build PicoTorrent Server then run it.

```sh
$ mkdir build && cd build
$ cmake -G Ninja
$ ninja
$ ./PicoTorrentServer
```

Then start the Vue client.

```sh
$ cd client
$ npm i
$ npm run serve
```

Open `http://localhost:8080` in your browser. All API requests are forwarded to
the server process with the help of the Vue proxy.

In production scenarios, the Vue client is hosted by PicoTorrent itself.
