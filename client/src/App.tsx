import { useState } from 'react';
import { QueryClient, QueryClientProvider } from 'react-query';
import { trpc } from './utils/trpc';
import { ChakraProvider } from "@chakra-ui/react";

import "./App.css";
import Home from "./pages/Home";

export function App() {
  const [queryClient] = useState(() => new QueryClient());
  const [trpcClient] = useState(() =>
    trpc.createClient({
      url: '/trpc',
    }),
  );

  return (
    <ChakraProvider>
      <trpc.Provider client={trpcClient} queryClient={queryClient}>
        <QueryClientProvider client={queryClient}>
          <Home />
        </QueryClientProvider>
      </trpc.Provider>
    </ChakraProvider>
  )
}
