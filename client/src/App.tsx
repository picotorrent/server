import './App.css';
import { Box, CircularProgress, CircularProgressLabel, Flex, HStack, IconButton, Image, Input, InputGroup, InputLeftElement, InputRightElement, Kbd, Menu, MenuButton, Progress, Text } from '@chakra-ui/react';
import { HamburgerIcon, SearchIcon } from '@chakra-ui/icons';

interface Torrent {
  name: string;
}

interface TorrentItem {
  item: Torrent;
}

function TorrentListItem(props: TorrentItem) {
  return (
    <Box className='TorrentListItem' p='2' borderColor={'gray.200'} borderRadius='lg' borderWidth={'thin'}>
      <Text fontSize='sm'>
        {props.item.name}
      </Text>
      <Progress value={20} size='xs' />
    </Box>
  )
}

function App() {
  fetch('/api/jsonrpc').then(console.log).catch(console.error);

  return (
    <Flex mt='20px' justifyContent={'center'}>
      <Box w={600}>
        <Flex alignItems={'center'} borderBottom={'1px solid #f0f0f0'} mb='3' pb={'3'}>
          <Box flex={0.5}>
            <Image src='/logo.png' boxSize={'40px'} />
          </Box>
          <Box flex={5}>
            <InputGroup>
              <InputLeftElement
                pointerEvents={'none'}
                children={<SearchIcon color={'gray.300'} />}
              />
              <Input placeholder='Pika command palette ...' />
              <InputRightElement pointerEvents={'none'} width='6em'>
                <HStack>
                  <Kbd>Ctrl</Kbd>
                  <Kbd>K</Kbd>
                </HStack>
              </InputRightElement>
            </InputGroup>
          </Box>
          <Flex justifyContent={'end'} flex={0.5}>
            <Menu>
              <MenuButton
                as={IconButton}
                aria-label='Menu'
                icon={<HamburgerIcon />}
                size='sm'
                variant='outline'
              />
            </Menu>
          </Flex>
        </Flex>
        <TorrentListItem item={{
          name: 'ubuntu-21.04-desktop-amd64.iso'
        }} />
      </Box>
    </Flex>
  );
}

export default App;
