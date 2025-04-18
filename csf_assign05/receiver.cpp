#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

int main(int argc, char **argv) {
  if (argc != 5) {
    std::cerr << "Usage: ./receiver [server_address] [port] [username] [room]\n";
    return 1;
  }

  std::string server_hostname = argv[1];
  int server_port = std::stoi(argv[2]);
  std::string username = argv[3];
  std::string room_name = argv[4];

  // TODO: connect to server
  Connection conn;
  try {
    conn.connect(server_hostname, server_port);
  } catch (const std::exception &e) {
    std::cerr << "Error connecting to server: " << e.what() << "\n";
    return 1;
  }

  // TODO: send rlogin and join messages (expect a response from
  //       the server for each one)
  conn.send(Message(TAG_RLOGIN, username));
  Message msg;
  conn.receive(msg);
  if (msg.tag == TAG_ERR) {
    std::cerr << msg.data;
    return 1;
  }
  if (msg.tag != TAG_OK) {
    std::cerr << "Unexpected message from server: " << msg.data << "\n";
    return 1;
  }

  // TODO: loop waiting for messages from server
  //       (which should be tagged with TAG_DELIVERY)
  while (true) {
    conn.receive(msg);
    if (msg.tag == TAG_ERR) {
      std::cerr << msg.data;
      return 1;
    }
    if (msg.tag == TAG_DELIVERY) {
      std::cout << msg.data << "\n";
    } else {
      std::cerr << "Unexpected message from server: " << msg.data << "\n";
      return 1;
    }
  }
  return 0;
}
