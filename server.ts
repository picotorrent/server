import * as trpc from "@trpc/server";
import { z } from "zod";
import db from "./database";

export const appRouter = trpc.router()
  .query('config.get', {
    input: z.array(z.string()),
    async resolve(req) {
      const stmt = db.prepare('SELECT value FROM config WHERE key = ?');
      const response: {[key: string]: any} = {};

      for (const key of req.input) {
        const result = stmt.bind(key).get();
        response[key] = result ? JSON.parse(result) : null;
      }

      return response;
    }
  })
  .query("torrents.list", {
    async resolve(req) {
      return "";
    }
  })
  .query("torrents.search", {
    input: z.string(),
    async resolve(req) {
    }
  });

export type AppRouter = typeof appRouter;
