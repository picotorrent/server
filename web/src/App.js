import { Box, Button, Flex, IconButton, Input, InputGroup, InputLeftElement, Menu, MenuButton } from "@chakra-ui/react";
import { AddIcon, ChevronDownIcon, SearchIcon, SettingsIcon } from "@chakra-ui/icons";
import useSWR from 'swr'

import AddTorrentModal from './components/AddTorrentModal';
import { ReactComponent as Logo } from './logo.svg';

const rpcFetcher = method => fetch(
  '/api/jsonrpc',
  {
    method: 'POST',
    body: JSON.stringify({
      jsonrpc: '2.0',
      method
    })
  })
  .then(r => r.json())
  .then(r => {
    if (r.error) {
      throw new Error(r.error.message);
    }
    return r.result;
  })

function App() {
  const { torrents, error } = useSWR('session.getTorrents', rpcFetcher);

  return (
    <>
      <Flex justifyContent='center'>
        <Box w='580px'>
          <Flex alignItems='center' borderBottom='1px' borderColor='#ccc' py='2'>
            <Box w='24px'><Logo /></Box>
            <Box flex='1' mx='2'>
              <InputGroup size='sm'>
                <InputLeftElement
                  pointerEvents='none'
                  children={<SearchIcon />}
                />
                <Input size='sm' placeholder='Filter torrents...' />
              </InputGroup>
            </Box>
            <Flex>
              <AddTorrentModal />

              <Menu>
                <MenuButton size='xs' as={Button} rightIcon={<ChevronDownIcon />}>
                  <SettingsIcon />
                </MenuButton>
              </Menu>
            </Flex>
          </Flex>
          <Box>
            { !!error && <span>{error.toString()} </span> }

            { torrents && torrents.map(() => <div>ehj</div>)}
          </Box>
        </Box>
      </Flex>
    </>
  );
}

export default App;
