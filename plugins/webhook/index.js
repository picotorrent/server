export default async function webhook(pika) {
  await pika.db.migrate(__dirname + '/db');

  pika.on('torrents:added', ({ torrent }) => {
      
  });

  pika.on('shutdown', () => {
    // cancel all requests in transit
  });
}
