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
  Room* room = nullptr;
  if (userType == TAG_RLOGIN) {
    conn->receive(received);
    if (received.tag != TAG_JOIN) {
      toSend.tag = TAG_ERR;
      toSend.data = "not join commmand";
    } else if (received.data.length() == 0) {
      toSend.tag = TAG_ERR;
      toSend.data = "no romm name";
    } else {
      toSend.tag = TAG_OK;
      toSend.data = "received";
      room = (aux->server)->find_or_create_room(received.data); //come back to this
    }
    
    conn->send(toSend);
    while (1) {
      //add a delivery system from room to receiver
      Message* message = currUser->mqueue.dequeue();
      if (message == nullptr) {
        continue;
      } else {
        conn->send(*message);
      }
    }

  } else if (userType == TAG_SLOGIN) {
    while (1) {
      conn->receive(received);
      if (received.tag == TAG_JOIN) {
        if (received.data.length() == 0) {
          toSend.tag == TAG_ERR;
          toSend.data == "no room name";
        } else {
          toSend.tag == TAG_OK;
          toSend.data == "received";
          room = (aux->server)->find_or_create_room(received.data);
          room->add_member(currUser);
        }
      } else if (received.tag == TAG_LEAVE) {
        if (room == nullptr) {
          toSend.tag = TAG_ERR;
          toSend.data = "not in a room";
        } else {
          toSend.tag == TAG_OK;
          toSend.data == "received";
          room->remove_member(currUser);
          room = nullptr;
        }

      } else if (received.tag == TAG_SENDALL) {
        if (room == nullptr) {
          toSend.tag = TAG_ERR;
          toSend.data = "not in a room";
        } else if (received.data.length() == 0) {
          toSend.tag = TAG_ERR;
          toSend.data = "no message to send";
        } else {
          toSend.tag == TAG_OK;
          toSend.data == "received";
          room->broadcast_message(currUser->username,received.data);
        }
      } else {
        toSend.tag = TAG_ERR;
        toSend.data = "unknown tag";
      }
      conn->send(toSend);
    }
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
  assert(ret = 0);
}

Server::~Server() {
  // TODO: destroy mutex
  int ret = pthread_mutex_destroy(&m_lock);
  assert(ret == 0);
}

bool Server::listen() {
  // TODO: use open_listenfd to create the server socket, return true
  //       if successful, false if not
  int serverfd = open_listenfd((char *) m_port); //Review later
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
    Connection conn(clientfd);
    aux->connection = &conn;
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
