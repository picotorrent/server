import './App.css';
import { Alert, AlertDescription, AlertIcon, AlertTitle, Box, Button, Flex, HStack, Icon, IconButton, Image, Input, InputGroup, InputLeftElement, InputRightElement, Kbd, Menu, MenuButton, MenuGroup, MenuItem, MenuList, Modal, ModalBody, ModalCloseButton, ModalContent, ModalFooter, ModalHeader, ModalOverlay, Progress, Radio, RadioGroup, Stack, Tag, TagLabel, Text, useDisclosure, VStack } from '@chakra-ui/react';
import { HamburgerIcon, SearchIcon } from '@chakra-ui/icons';
import useSwr from 'swr';
import { ChangeEvent, useState } from 'react';
import { MdFolder, MdDownload, MdPeople, MdSettings, MdUpload, MdCheckCircle, MdCloudUpload, MdOutlinePause, MdOutlineRemove, MdOutlineFolder, MdLabelOutline, MdTag } from 'react-icons/md';

interface Torrent {
  flags: number;
  info_hash: string;
  dl: number;
  name: string;
  save_path: string;
  ul: number;
  num_peers: number;
  num_seeds: number;
  progress: number;
  state: number;
}

interface TorrentListItemProps {
  torrent: Torrent;
}

function AddTorrentMenuItem(props: any) {
  const { isOpen, onOpen, onClose } = useDisclosure();

  const [error, setError] = useState<Error>();
  const [loading, setLoading] = useState(false);
  const [type, setType] = useState('1');
  const [torrentFile, setTorrentFile] = useState<File | null>();

  const handleChange = (e: ChangeEvent<HTMLInputElement>) => {
    setTorrentFile(e.target.files?.item(0));
  };

  function add() {
    setLoading(true);

    const reader = new FileReader();
    reader.onload = function (ev) {
      const idx = reader.result?.toString().indexOf('base64,')!;
      const data = reader.result?.toString().substring(idx + 'base64,'.length);

      fetch('/api/jsonrpc', {
        body: JSON.stringify({
          jsonrpc: '2.0',
          method: 'session.addTorrent',
          id: 1337,
          params: {
            save_path: '/tmp',
            ti: data
          }
        }),
        method: 'POST',
      })
      .then(res => res.json())
      .then(res => {
        setLoading(false);

        if (res.error) {
          throw new Error(res.error.message);
        }

        onClose();
      })
      .catch(err => {
        setError(err);
      });
    };
    reader.readAsDataURL(torrentFile!);
  }

  return (
    <>
      <MenuItem onClick={onOpen}>Add torrent</MenuItem>

      <Modal isOpen={isOpen} onClose={onClose} size='xl'>
        <ModalOverlay />
        <ModalContent>
          <ModalHeader>Add torrent</ModalHeader>
          <ModalCloseButton />
          <ModalBody>
            <VStack align={'stretch'}>
              <Box>
                <RadioGroup onChange={setType} value={type}>
                  <Stack direction='row'>
                    <Radio value='1' mr='3'>File</Radio>
                    <Radio value='2'>Magnet link</Radio>
                  </Stack>
                </RadioGroup>
                <Box mt='2'>
                  { type == '1' &&
                    <Input type="file" onChange={handleChange} />
                  }
                  { type == '2' &&
                    <Input placeholder='magnet:?xt=urn:btih:...' />
                  }
                </Box>
              </Box>
              <Box>
                <Text>Save path</Text>
                <Input />
              </Box>
            </VStack>
            { error &&
              <Alert mr='3' mt='3' status='error' borderRadius='md'>
                <AlertIcon />
                <Box>
                  <AlertTitle>Error!</AlertTitle>
                  <AlertDescription>{error.message}</AlertDescription>
                </Box>
              </Alert>
            }
          </ModalBody>
          <ModalFooter>
            <Button onClick={add} isDisabled={loading}>Add</Button>
          </ModalFooter>
        </ModalContent>
      </Modal>
    </>
  )
}

function rpc(method: string, params: Object | Array<any>) {
  return fetch('/api/jsonrpc', {
    body: JSON.stringify({
      jsonrpc: '2.0',
      method,
      params,
      id: 1337
    }),
    method: 'POST'
  })
  .then(res => res.json());
}

function pause(ih: string) {
  rpc('torrents.pause', [ ih ]);
}

function TorrentListItem(props: TorrentListItemProps) {
  return (
    <Box className='TorrentListItem' p='2' borderColor={'gray.200'} borderRadius='lg' borderWidth={'thin'}>
      <Flex alignItems={'center'}>
        <Box flex='1'>
          <Flex>
            <Text fontSize='sm' flex={1}>
              <Flex alignItems='center'>
                {props.torrent.name}
                {props.torrent.state===5 &&
                  <Icon as={MdCloudUpload} color='blue.400' ml='1' />
                }
              </Flex>
            </Text>
            <Tag size='sm'>Linux ISOs</Tag>
          </Flex>
          <HStack spacing='3' mt='1'>
            <Flex alignItems={'center'} flex='1'>
              <Icon as={MdFolder} size='xs' mr='1' color='gray.400' />
              <Text fontSize={'xs'} color='gray.500'>{props.torrent.save_path}</Text>
            </Flex>
            <Flex alignItems='end'>
              <Icon as={MdDownload} size='xs' mr='1' color='gray.400' />
              <Text fontSize='xs' color='gray.500'>{props.torrent.dl}</Text>
            </Flex>
            <Flex alignItems='end'>
              <Icon as={MdUpload} size='xs' mr='1' color='gray.400' />
              <Text fontSize='xs' color='gray.500'>{props.torrent.ul}</Text>
            </Flex>
            <Flex alignItems='center'>
              <Icon as={MdPeople} size='xs' mr='1' color='gray.400' />
              <Text fontSize='xs' color='gray.500'>{props.torrent.num_peers + props.torrent.num_seeds}</Text>
            </Flex>
           </HStack>
          <Progress value={props.torrent.progress*100} size='xs' mt='1' colorScheme={
            props.torrent.state === 5
              ? 'blue'
              : 'pink'
          } />
        </Box>
        <Menu isLazy>
          <MenuButton
            as={IconButton}
            aria-label='Torrent menu'
            icon={<MdSettings />}
            ml='3'
            size='sm'
            variant='ghost'
          />
          <MenuList>
            <MenuGroup title='Actions'>
              <MenuItem icon={<MdOutlineFolder />} onClick={() => pause(props.torrent.info_hash)}>Move</MenuItem>
              <MenuItem icon={<MdOutlineRemove />} onClick={() => pause(props.torrent.info_hash)}>Remove</MenuItem>
            </MenuGroup>
            <MenuGroup title='Other'>
              <MenuItem icon={<MdTag />}>Tags</MenuItem>
            </MenuGroup>
          </MenuList>
        </Menu>
      </Flex>
    </Box>
  )
}

interface TorrentListProps {
  torrents: Torrent[]
}

function TorrentList(props: TorrentListProps) {
  if (!props?.torrents?.length) {
    return (
      <Alert status='info'>
        <AlertIcon />
        <AlertTitle>No torrents added yet.</AlertTitle>
      </Alert>
    )
  }

  return (
    <>
      { props.torrents.map((torrent) => {
        return (<TorrentListItem key={torrent.info_hash} torrent={torrent} />)
      }) }
    </>
  )
}

const jsonrpc = (key: string) => fetch('/api/jsonrpc', {
  body: JSON.stringify({
    jsonrpc: '2.0',
    method: key,
    id: 1337,
    params: []
  }),
  method: 'POST'
}).then((res) => res.json());

function App() {
  const {
    data,
    error
  } = useSwr('session.listTorrents', jsonrpc, { refreshInterval: 1000 });

  return (
    <Flex mt='20px' justifyContent={'center'}>
      <Box w='640px'>
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
              <Input placeholder='Search torrents...' />
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
              <MenuList>
                <AddTorrentMenuItem />
                <MenuItem>Settings</MenuItem>
              </MenuList>
            </Menu>
          </Flex>
        </Flex>
        { error &&
          <Alert status='error'>
            <AlertIcon />
            <AlertTitle>Cannot connect to Pika API.</AlertTitle>
          </Alert>
        }
        { !error && data && data.result?.length &&
          <TorrentList torrents={data.result as Torrent[]} />
        }
      </Box>
    </Flex>
  );
}

export default App;
