import * as trpc from '@trpc/server';

export function trpcRouter() {
  return trpc.router()
    .query('list', {
      resolve(ctx) {
        return "list of webhooks";
      }
    });
}

export async function webhook(pika: any) {
  await pika.db.migrate(__dirname + '/db');

  pika.on('torrents:added', ({ torrent }: any) => {
      
  });

  pika.on('shutdown', () => {
    // cancel all requests in transit
  });
}
