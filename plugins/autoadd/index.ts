import { Router } from "express";

export function router(): Router {
  return Router()
    .get('/trpc', (_, res) => res.end('Hello!'));
}
