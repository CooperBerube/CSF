#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

int main(int argc, char **argv) {
  if (argc != 4) {
    std::cerr << "Usage: ./sender [server_address] [port] [username]\n";
    return 1;
  }

  std::string server_hostname;
  int server_port;
  std::string username;

  server_hostname = argv[1];
  server_port = std::stoi(argv[2]);
  username = argv[3];

  // TODO: connect to server
  Connection conn;
  try {
    // Attempt to establish a connection using the provided hostname and port
    conn.connect(server_hostname, server_port);
  } catch (const std::exception &e) {
    // Handle connection failure by printing an error message and exiting
    std::cerr << "Error connecting to server: " << e.what() << "\n";
    return 1;
  }

  // Send the SLOGIN message to authenticate or initialize the session
  conn.send(Message(TAG_SLOGIN, username));
  Message message;
  conn.receive(message);

  // Handle server response for SLOGIN
  if (message.tag == TAG_ERR) {
    // Print server error message if authentication fails
    std::cerr << message.data;
    return 1;
  }
  if (message.tag != TAG_OK) {
    // Ensure the server sends an OK response; otherwise, terminate with an error
    std::cerr << "Unexpected message from server: " << message.data << "\n";
    return 1;
  }

  // Enter a loop to read commands from the user and send messages to the server
  while (true) {
    std::string line;
    // Read a line of input from the user
    std::getline(std::cin, line);
    std::istringstream iss(line);
    std::string command;
    iss >> command;

    if (command == "/join") {
      // Handle the /join command to join a specific room
      std::string room;
      iss >> room;
      conn.send(Message(TAG_JOIN, room));
      conn.receive(message);

      // Handle server response for the /join command
      if (message.tag == TAG_ERR) {
        // Print server error message if joining fails
        std::cerr << message.data;
        continue;
      }
      if (message.tag != TAG_OK) {
        // Ensure the server sends an OK response; otherwise, terminate with an error
        std::cerr << "Error: Unexpected message from the server. No OK received.\n";
        return 1;
          }

      } else if (command == "/quit") {
        // Handle the /quit command to leave the server and exit
        conn.send(Message(TAG_QUIT, ""));
        conn.receive(message);

        // Handle server response for the /quit command
        if (message.tag == TAG_ERR) {
          // Print server error message if quitting fails
          std::cerr << message.data;
          continue;
        }
        if (message.tag != TAG_OK) {
          // Ensure the server sends an OK response; otherwise, terminate with an error
          std::cerr << "Error: Unexpected message from the server. No OK received.\n";
          return 1;
        }
        break;  // Exit the loop after successfully quitting
      } else if (command == "/leave") {
        // Handle the /leave command to leave the current room
        conn.send(Message(TAG_LEAVE, ""));
        conn.receive(message);

        // Handle server response for the /leave command
        if (message.tag == TAG_ERR) {
          // Print server error message if leaving fails
          std::cerr << message.data;
          continue;
        }
        if (message.tag != TAG_OK) {
          // Ensure the server sends an OK response; otherwise, terminate with an error
          std::cerr << "Error: Unexpected message from the server. No OK received.\n";
          return 1;
      }
    } else {
        // Handle sending messages to all participants if no command is detected
        if (command[0] == '/') {
          // Print an error if the command is invalid
          std::cerr << "Error: Command is Invalid \n";
          continue;
        }
        conn.send(Message(TAG_SENDALL, line));
        conn.receive(message);

        // Handle server response for sending a message
        if (message.tag == TAG_ERR) {
          // Print server error message if sending fails
          std::cerr << message.data;
          continue;
          }
        if (message.tag != TAG_OK) {
           // Ensure the server sends an OK response; otherwise, terminate with an error
          std::cerr << "Error: Unexpected message from the server. No OK received.\n";
          exit(1);
        }
      }
  }
  return 0;
}