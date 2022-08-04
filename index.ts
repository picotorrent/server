import express from 'express';
import { InfoHash, Session, Torrent } from '@picotorrent/pika-native';
import { Sequelize, DataTypes, Op } from 'sequelize';
import * as trpc from '@trpc/server';
import * as trpcExpress from '@trpc/server/adapters/express';
import { z } from 'zod';
import path from 'path';
import { readdir } from 'fs/promises';

const sequelize = new Sequelize('sqlite:/home/viktor/pika.db', {
  logging: false
});

const AddTorrentParams = sequelize.define('AddTorrentParams', {
  info_hash_v1: {
    type: DataTypes.TEXT,
    allowNull: true
  },
  resume_data_buf: {
    type: DataTypes.BLOB,
    allowNull: false
  }
});

const session = new Session({});
const torrents = new Map<InfoHash, Torrent>();

session.on('torrents:added', async (err, torrent) => {
  if (err) {
    console.log('Failed to add torrent: ', err);
    return;
  }

  torrents.set(torrent.info_hash(), torrent);

  const exists = await AddTorrentParams.count({
    where: {
      info_hash_v1: {
        [Op.eq]: torrent.info_hash()[0]
      }
    }
  });

  if (exists > 0) {
    // Ignore torrent since it was most likely loaded from db.
    return;
  }

  // New torrent. Insert into database.
});

const createContext = ({
  req,
  res,
}: trpcExpress.CreateExpressContextOptions) => ({}) // no context

const router = trpc.router()
  .query('torrents.list', {
    async resolve(req) {
      const result = [];
      for (const [hash,torrent] of torrents) {
        result.push(hash[0]);
      }
      return result;
    }
  })
  .query('torrents.search', {
    input: z.string(),
    async resolve(req) {
    }
  });

async function load() {
  const params = await AddTorrentParams.findAll({
    attributes: ['resume_data_buf'],
    order: [
      ['queue_position', 'ASC']
    ]
  });

  console.log('Loading', params.length, 'torrents');

  for (const p of params) {
    const buf = p.getDataValue('resume_data_buf');

    try {
      session.loadTorrent(buf);
    } catch (err) {
      console.log('Error when loading torrent: ', err);
    }
  }
}

load()
  .then(async () => {
    const plugins = path.join(__dirname, 'plugins');
    let rr = trpc.router();

    const app = express();

    for (const plugin of await readdir(plugins)) {
      const pluginPath = path.join(__dirname, 'plugins', plugin);

      const loadedPlugin = await import(pluginPath) as any;

      if ('router' in loadedPlugin) {
        app.use(`/plugins/${plugin}`, loadedPlugin.router())
      }
    }

    app.use('/trpc', trpcExpress.createExpressMiddleware({
      router: rr,
      createContext,
    }));

    app.listen(
        process.env.PIKA_HTTP_PORT || 3000,
        () => console.log('Pika HTTP started'));
    })
  .catch(console.error);

process.on('SIGTERM', () => {
  console.log('SIGTERM');
});

process.on('SIGINT', () => {
  console.log('shutting down');
  process.exit(0);
});
