import { Database } from "better-sqlite3";
import { readdir, readFile } from "fs/promises";
import path from "path";
import logger from "./logger";

async function listMigrations() {
  const sql = path.join(__dirname, 'sql');

  return readdir(sql)
    .then(res => res.map(val => path.join(sql, val)))
}

function getUserVersion(db: Database) {
  return db.prepare("PRAGMA user_version;").get().user_version;
}

export default function(db: Database) {
  return async function () {
    const version = getUserVersion(db);
    const migrations = await listMigrations();

    if (version == migrations.length) {
        return;
    }

    logger.info(`Migrating from version ${version} to ${migrations.length}`);

    for (let i = version; i < migrations.length; i++) {
        const buf = await readFile(migrations[i]);
        db.exec(buf.toString('utf8'));
    }
    }
}
