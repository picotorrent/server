import './App.css';
import { Box, Flex, HStack, IconButton, Image, Input, InputGroup, InputLeftElement, InputRightElement, Kbd, Menu, MenuButton } from '@chakra-ui/react';
import { HamburgerIcon, SearchIcon } from '@chakra-ui/icons';

function App() {
  fetch('/api/jsonrpc').then(console.log).catch(console.error);

  return (
    <Flex mt={20} justifyContent={'center'}>
      <Box w={600}>
        <Flex alignItems={'center'} borderBottom={'1px solid #f0f0f0'} pb={'3'}>
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
      </Box>
    </Flex>
  );
}

export default App;
