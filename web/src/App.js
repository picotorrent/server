import { Box, Button, Checkbox, Flex, Input, InputGroup, InputLeftElement, Progress, Table, Tbody, Td, Th, Thead, Tr } from "@chakra-ui/react";
import { SearchIcon, SettingsIcon } from "@chakra-ui/icons";
import filesize from 'filesize';
import useSWR from 'swr';

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
    return r.result || [];
  })

function App() {
  const { data, error } = useSWR('session.getTorrents', rpcFetcher, { refreshInterval: 1000 });

  return (
    <>
      <Box>
        <Flex justifyContent="center" borderBottom='1px' borderColor='#ccc'>
          <Box flex="1" maxWidth="580px" mx="2">
            <Flex alignItems='center' py='2'>
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
                <Button size="xs">
                  <SettingsIcon />
                </Button>
              </Flex>
            </Flex>
          </Box>
        </Flex>

        <Box mt="1">
          { !!error && <span>{error.toString()} </span> }
          <Table size="sm">
            <Thead>
              <Tr>
                <Th w="16px"><Checkbox /></Th>
                <Th>Name</Th>
                <Th isNumeric>Size</Th>
                <Th isNumeric>DL</Th>
                <Th isNumeric>UL</Th>
                <Th>State</Th>
                <Th>Progress</Th>
                <Th></Th>
              </Tr>
            </Thead>
            <Tbody>
              { !!data && data.map(torrent =>
              <Tr key={torrent.info_hash}>
                <Td><Checkbox /></Td>
                <Td>{torrent.name}</Td>
                <Td isNumeric>{filesize(torrent.total_wanted)}</Td>
                <Td isNumeric>{torrent.dl > 1024 ? filesize(torrent.dl) + "/s" : "-"}</Td>
                <Td isNumeric>{torrent.ul > 1024 ? filesize(torrent.ul) + "/s" : "-"}</Td>
                <Td>{torrent.state}</Td>
                <Td>
                  <Progress value={torrent.progress*100} minWidth="100px" />
                </Td>
                <Td>

                </Td>
              </Tr>
              )}
            </Tbody>
          </Table>
        </Box>
      </Box>
    </>
  );
}

export default App;
