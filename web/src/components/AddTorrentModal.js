import { useState } from 'react';
import { AddIcon } from "@chakra-ui/icons";
import {
  Box,
  Button,
  FormLabel,
  IconButton,
  Input,
  Modal,
  ModalBody,
  ModalCloseButton,
  ModalContent,
  ModalFooter,
  ModalHeader,
  ModalOverlay,
  useDisclosure } from "@chakra-ui/react";

export default function AddTorrentModal() {
  const { isOpen, onOpen, onClose } = useDisclosure();

  const [ magnetLink, setMagnetLink ] = useState('');
  const [ savePath, setSavePath ] = useState('/tmp');

  function add() {
    fetch('/api/jsonrpc', {
      method: 'POST',
      body: JSON.stringify({
        jsonrpc: '2.0',
        method: 'session.addMagnetLink',
        params: {
          magnet_uri: magnetLink,
          save_path: savePath
        }
      })
    }).then(onClose);
  }

  return (
    <>
      <IconButton onClick={onOpen} mr='1' size='xs' icon={<AddIcon />} />

      <Modal isOpen={isOpen} onClose={onClose}>
        <ModalOverlay />
        <ModalContent>
          <ModalHeader>Add torrent</ModalHeader>
          <ModalCloseButton />
          <ModalBody>
            <FormLabel htmlFor='magnetLink'>Magnet link</FormLabel>
            <Input id='magnetLink' value={magnetLink} onChange={(ev) => setMagnetLink(ev.target.value)} />

            <Box mt='3'>
            <FormLabel htmlFor='savePath'>Save path</FormLabel>
            <Input id='savePath' value={savePath} onChange={(ev) => setSavePath(ev.target.value)} />
            </Box>
          </ModalBody>

          <ModalFooter>
            <Button onClick={add}>Add</Button>
          </ModalFooter>
        </ModalContent>
      </Modal>
    </>
  );
}
