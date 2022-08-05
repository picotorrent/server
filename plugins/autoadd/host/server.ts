import * as trpc from '@trpc/server';

type Context = {};

export const appRouter = trpc.router<Context>()
  .query('folders.list', {
    async resolve(ctx) {
      return [ 'list', 'of', 'folders' ]
    }
  });

export type AppRouter = typeof appRouter;
