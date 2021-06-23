# PicoTorrent Server is a high performance BitTorrent server

Built to handle tens of thousands of torrents, PicoTorrent Server is an
excellent BitTorrent server. Perfect for a seedbox setup where performance
and ease of use is key.

## Features

 * Full support for BitTorrent 2.0 ([BEP-52](http://bittorrent.org/beps/bep_0052.html)),
   v1, v2 and v1+v2 hybrid torrents.
 * A simple, clean, embedded web UI for management.
 * All data is stored in a SQLite database which makes backups trivial.

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
