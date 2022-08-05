import { createReactQueryHooks } from '@trpc/react';
import type { AppRouter } from '../../../server';

export const trpc = createReactQueryHooks<AppRouter>();
