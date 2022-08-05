import { createReactQueryHooks } from '@trpc/react';
import type { AppRouter } from '../../../host/server';

export const trpc = createReactQueryHooks<AppRouter>();
