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
    conn.connect(server_hostname, server_port);
  } catch (const std::exception &e) {
    std::cerr << "Error connecting to server: " << e.what() << "\n";
    return 1;
  }

  // TODO: send slogin message
  conn.send(Message(TAG_SLOGIN, username));
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

  // TODO: loop reading commands from user, sending messages to
  //       server as appropriate
  while (true) {
    std::string line;
    std::getline(std::cin, line);
    std::istringstream iss(line);
    std::string command;
    iss >> command; 
    if (command == "/join") {
      std::string room_name;
      iss >> room_name;
      conn.send(Message(TAG_JOIN, room_name));
      conn.receive(msg); 
      
      // Expanded code for conn.check_ERR(msg)
      if (msg.tag == TAG_ERR) {
        std::cerr << msg.data;
        continue;
      }
      
      // Expanded code for conn.check_OK(msg)
      if (msg.tag != TAG_OK) {
        std::cerr << "Error: Unexpected message from the server. No OK received.\n";
        exit(1);
      }

    } else if (command == "/leave") {
      conn.send(Message(TAG_LEAVE, ""));
      conn.receive(msg); 
      if (msg.tag == TAG_ERR) {
        std::cerr << msg.data;
        continue;
      }
      if (msg.tag != TAG_OK) {
        std::cerr << "Error: Unexpected message from the server. No OK received.\n";
        exit(1);
      }

    } else if (command == "/quit") {
      conn.send(Message(TAG_QUIT, ""));
      conn.receive(msg); 
      
      if (msg.tag == TAG_ERR) {
        std::cerr << msg.data;
        continue;
      }
      if (msg.tag != TAG_OK) {
        std::cerr << "Error: Unexpected message from the server. No OK received.\n";
        exit(1);
      }
      break;  
    } else {
      if (command[0] == '/') {
        std::cerr << "Error: Invalid command\n";
        continue;
      }
      conn.send(Message(TAG_SENDALL, line));
      conn.receive(msg); 
    
      if (msg.tag == TAG_ERR) {
        std::cerr << msg.data;
        continue;
      }
      if (msg.tag != TAG_OK) {
        std::cerr << "Error: Unexpected message from the server. No OK received.\n";
        exit(1);
      }
    } 
}
  return 0;
}
