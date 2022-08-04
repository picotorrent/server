declare module '@picotorrent/pika-native' {
  export class Session {
    constructor(options: any);

    loadTorrent(buf: Buffer): void;
    on(event: 'torrents:added', callback: (err: Error, torrent: Torrent) => void): void;
  }

  export class Torrent {
    info_hash(): InfoHash;
  }

  export type InfoHash = [(string|null),(string|null)];
}
