import { Box, Flex, Icon } from "@chakra-ui/react";
import { BsDroplet } from "react-icons/bs";
import { trpc } from "../utils/trpc";

function Hello() {
  return (
    <Flex height="100%" mt="20" justifyContent="center">
      <Box w="md">
        <Flex mb="5" alignItems="center" justifyContent="center">
          <Icon as={BsDroplet} fontSize="48px" />
        </Flex>

        <p>Time to set up pika</p>
      </Box>
    </Flex>
  )
}

export default function Home() {
  const hello = trpc.useQuery(["config.get", ["pika.hello"]]);

  if (!hello.data) {
    return <div>Loading...</div>
  }

  if (!hello.data["pika.hello"]) {
    return <Hello />
  }

  return (
    <Box>
      Ready to go!
    </Box>
  )
}
