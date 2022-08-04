import sqlite3 from 'sqlite3';
import http from 'http';
import { Session } from '@picotorrent/pika-native';

const db = new sqlite3.Database(":memory:");

const session = new Session({});
const torrents = new Map();

session.on('torrents:added', (err: any, torrent: any) => {
  if (err) {
    console.log('Failed to add torrent: ', err);
    return;
  }

  db.each(
    'SELECT 1 AS count FROM addtorrentparams WHERE info_hash_v1 = $v1',
    { $v1: torrent.info_hash()[0] },
    (err, { count }: any) => {
      if (err) {
        return;
      }

      torrents.set(torrent.info_hash()[0], torrent);

      if (count === 1) {
        // Ignore torrent since it was most likely loaded from db.
        return;
      }

      // New torrent. Insert.
    });
});

db.each('SELECT resume_data_buf FROM addtorrentparams ORDER BY queue_position ASC', (err, row: any) => {
  if (err) {
    console.log('Error when reading params: ', err);
    return;
  }

  try {
    session.loadTorrent(row.resume_data_buf);
  } catch (err) {
    console.log('Error when loading torrent: ', err);
  }
});

const server = http.createServer((req, res) => {
    console.log(req, res);
});

server.listen(
    process.env.PIKA_HTTP_PORT || 3000,
    () => console.log('> Pika HTTP started'));


process.on('SIGTERM', () => {
  console.log('SIGTERM');
});

process.on('SIGINT', () => {
  console.log('shutting down');
  process.exit(0);
});
