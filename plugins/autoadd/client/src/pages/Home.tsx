import { Box, Button, Flex, Heading, Modal, ModalBody, ModalCloseButton, ModalContent, ModalFooter, ModalHeader, ModalOverlay, useDisclosure } from '@chakra-ui/react';
import { trpc } from '../utils/trpc';

export default function Home() {
  const { isOpen, onOpen, onClose } = useDisclosure();
  const folders = trpc.useQuery(['folders.list']);

  if (!folders.data) {
    return <div>Loading...</div>
  }

  return (
    <>
      <Box>
        <Flex justifyContent={'space-between'}>
          <Heading>Folders</Heading>
          <Button
            onClick={onOpen}
          >
            Add folder
          </Button>
        </Flex>
      </Box>
        <Modal isOpen={isOpen} onClose={onClose}>
        <ModalOverlay />
        <ModalContent>
          <ModalHeader>Modal Title</ModalHeader>
          <ModalCloseButton />
          <ModalBody>
            Do tings
          </ModalBody>

          <ModalFooter>
            <Button colorScheme='blue' mr={3} onClick={onClose}>
              Close
            </Button>
            <Button variant='ghost'>Secondary Action</Button>
          </ModalFooter>
        </ModalContent>
      </Modal>
    </>
  )
}
