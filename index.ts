import express from 'express';
import { InfoHash, Session, Torrent } from '@picotorrent/pika-native';
import * as trpcExpress from '@trpc/server/adapters/express';
import migrate from './migrate';
import logger from './logger';
import { appRouter } from './server';
import db from './database';

const session = new Session({});
const torrents = new Map<InfoHash, Torrent>();

session.on('torrents:added', async (err, torrent) => {
  if (err) {
    console.log('Failed to add torrent: ', err);
    return;
  }

  torrents.set(torrent.info_hash(), torrent);

  const exists = 1;

  if (exists > 0) {
    // Ignore torrent since it was most likely loaded from db.
    return;
  }

  // New torrent. Insert into database.
});

async function load() {
  const params = db.prepare("SELECT resume_data_buf FROM addtorrentparams ORDER BY queue_position ASC").all();

  logger.info(`Loading ${params.length} torrents`);

  for (const p of params) {
    try {
      session.loadTorrent(p.resume_data_buf);
    } catch (err) {
      console.log('Error when loading torrent: ', err);
    }
  }
}

Promise.resolve()
  .then(migrate(db))
  .then(load)
  .then(async () => {
    const createContext = ({
      req,
      res,
    }: trpcExpress.CreateExpressContextOptions) => ({});

    const app = express();

    app.use('/', express.static('node_modules/@picotorrent/pika-client/build'))

    app.use('/trpc', trpcExpress.createExpressMiddleware({
      router: appRouter,
      createContext,
    }));

    app.listen(
        process.env.PIKA_HTTP_PORT || 1337,
        () => logger.info('Pika HTTP started'));
    })
  .catch(console.error);

process.on('SIGTERM', () => {
  console.log('SIGTERM');
});

process.on('SIGINT', () => {
  logger.info('shutting down');
  process.exit(0);
});
