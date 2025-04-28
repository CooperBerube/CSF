#include <pthread.h>
#include <iostream>
#include <sstream>
#include <memory>
#include <set>
#include <vector>
#include <cctype>
#include <cassert>
#include "message.h"
#include "connection.h"
#include "user.h"
#include "room.h"
#include "guard.h"
#include "server.h"

////////////////////////////////////////////////////////////////////////
// Server implementation data types
////////////////////////////////////////////////////////////////////////
struct ConnInfo {
  Connection* connection;
  const int* port;
  int clientfd;
  Server* server;
};


// TODO: add any additional data types that might be helpful
//       for implementing the Server member functions

void chat_with_receiver(ConnInfo* aux, Connection* conn, User* currUser) {
  Message received;
  Message toSend;
  Room* room = nullptr;
  conn->receive(received);
    
  if (received.tag != TAG_JOIN) {
    toSend.tag = TAG_ERR;
    toSend.data = "not join commmand";
  } else if (received.data.length() == 0) {
    toSend.tag = TAG_ERR;
    toSend.data = "no room name";
  } else {
    toSend.tag = TAG_OK;
    toSend.data = "received";
    room = (aux->server)->find_or_create_room(received.data); //come back to this
    room->add_member(currUser);
  }
  conn->send(toSend);
  while (1) {
    //add a delivery system from room to receiver
    Message* ref = (currUser->mqueue.dequeue());
    if (ref == nullptr) {
      
    } else {
      Message& message = *ref;
      conn->send(message);
      //std::cout << "message sent to a receiver: " << message.tag + ":" << message.data;
    }
  }
  return;
}

void chat_with_sender(ConnInfo* aux, Connection* conn, User* currUser) {
  Message received;
  Room* room = nullptr;
  while (1) {
    conn->receive(received);
    //std::cout << "received on server from sender: " << received.tag << ":" << received.data;
    if (received.tag == TAG_JOIN) {
      if (received.data.length() == 0) {
        conn->send(Message(TAG_ERR, "no room name\n"));
      } else {
        conn->send(Message(TAG_OK, "received\n"));
        room = (aux->server)->find_or_create_room(received.data);
        room->add_member(currUser);
      }
    } else if (received.tag == TAG_LEAVE) {
      if (room == nullptr) {
        conn->send(Message(TAG_ERR, "not in a room\n"));
      } else {
        conn->send(Message(TAG_OK, "received\n"));
        room->remove_member(currUser);
        room = nullptr;
      }
    } else if (received.tag == TAG_SENDALL) {
      if (room == nullptr) {
        conn->send(Message(TAG_ERR, "not in a room\n"));
      } else if (received.data.length() == 0) {
        conn->send(Message(TAG_ERR, "no message to send\n"));
      } else {
        room->broadcast_message(currUser->username,received.data);
        conn->send(Message(TAG_OK, "received\n"));
        //std::cout << "message queued: " << received.data;
      } 
    } else if (received.tag == TAG_QUIT) {
      if (room != nullptr) {
        room->remove_member(currUser);
        room = nullptr;
      }
      conn->send(Message(TAG_OK, "received\n"));
      break;
    }else {
      conn->send(Message(TAG_ERR, "unkown tag\n"));
    }
  }
  return;
}

////////////////////////////////////////////////////////////////////////
// Client thread functions
////////////////////////////////////////////////////////////////////////

namespace {

void *worker(void *arg) {
  pthread_detach(pthread_self());
  // NOTES: Add user object and room in this part too?
  
  // TODO: use a static cast to convert arg from a void* to
  //       whatever pointer type describes the object(s) needed
  //       to communicate with a client (sender or receiver)
  ConnInfo *aux = static_cast<ConnInfo*>(arg); 
  Message received;
  Message toSend;
  Connection* conn = aux->connection;
  std::string username;
  
  // TODO: read login message (should be tagged either with
  //       TAG_SLOGIN or TAG_RLOGIN), send response

  conn->receive(received);
  User* currUser;
  if (received.tag == TAG_RLOGIN || received.tag == TAG_SLOGIN) {
    toSend.tag = TAG_OK;
    toSend.data = "received";
    if (received.data.length() == 0) {
      toSend.tag = TAG_ERR;
      toSend.data = "no username";
    } else {
      currUser = new User(received.data);
    }
  } else {
    toSend.tag = TAG_ERR;
    toSend.data = "error";
  }
  conn->send(toSend);
  
  // TODO: depending on whether the client logged in as a sender or
  //       receiver, communicate with the client (implementing
  //       separate helper functions for each of these possibilities
  //       is a good idea)
  std::string userType = received.tag;
  if (userType == TAG_RLOGIN) {
    chat_with_receiver(aux, conn, currUser);
  } else if (userType == TAG_SLOGIN) {
    chat_with_sender(aux, conn, currUser);
  }
  

  close(aux->clientfd);
  free(aux);

  return nullptr;
}

}

////////////////////////////////////////////////////////////////////////
// Server member function implementation
////////////////////////////////////////////////////////////////////////

Server::Server(int port)
  : m_port(port)
  , m_ssock(-1) {
  // TODO: initialize mutex
  int ret = pthread_mutex_init(&m_lock, nullptr);
  assert(ret == 0);
}

Server::~Server() {
  // TODO: destroy mutex
  int ret = pthread_mutex_destroy(&m_lock);
  assert(ret == 0);
}

bool Server::listen() {
  // TODO: use open_listenfd to create the server socket, return true
  //       if successful, false if not
  const char * string_port = (std::to_string(m_port)).c_str();
  int serverfd = open_listenfd(string_port); //reviewed, maybe double check
  if (serverfd < 0) {
    return false;
  } else {
    m_ssock = serverfd;
    return true;
  }

}

void Server::handle_client_requests() {
  // TODO: infinite loop calling accept or Accept, starting a new
  //       pthread for each connected client
  while(1) {
    int clientfd = accept(m_ssock, nullptr, nullptr);
    if (clientfd < 0) {
      std::cerr << "Error accepting client" << std::endl;
    }
    struct ConnInfo *aux = static_cast<struct ConnInfo *>(malloc(sizeof(struct ConnInfo)));
    aux->connection = new Connection(clientfd);
    aux->port = &m_port;
    aux->clientfd = clientfd;
    aux->server = this;
    pthread_t curr_thread;
    if(pthread_create(&curr_thread, nullptr, worker, aux) != 0) {
      std::cerr << "Error pthread problem" << std::endl; //fix
    }
  }
}

Room *Server::find_or_create_room(const std::string &room_name) {
  // TODO: return a pointer to the unique Room object representing
  //       the named chat room, creating a new one if necessary
  auto it = m_rooms.find(room_name);
  if (it == m_rooms.end()) {
    m_rooms[room_name] = new Room(room_name); //check to see how we should alocate this
    return m_rooms[room_name];
  } else {
    return m_rooms[room_name];
  }
}
