import * as express from 'express';
import path = require('path');
import * as trpc from '@trpc/server';
import * as trpcExpress from '@trpc/server/adapters/express';

import { appRouter } from "./server";

// created for each request
const createContext = ({
  req,
  res,
}: trpcExpress.CreateExpressContextOptions) => ({}) // no context

export function router(): express.Router {
  return express.Router()
    .use('/trpc', trpcExpress.createExpressMiddleware({router: appRouter, createContext}))
    .use(express.static('node_modules/@picotorrent/pika-plugin-autoadd-client/build'));
}
