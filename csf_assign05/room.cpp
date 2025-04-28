#include "guard.h"
#include "message.h"
#include "message_queue.h"
#include "user.h"
#include "room.h"

Room::Room(const std::string &room_name)
  : room_name(room_name) {
  // TODO: initialize the mutex
  pthread_mutex_init(&lock, nullptr);
}

Room::~Room() {
  // TODO: destroy the mutex
  pthread_mutex_destroy(&lock);
}

void Room::add_member(User *user) {
  // TODO: add User to the room
  Guard guard(lock);
  members.insert(user);
}

void Room::remove_member(User *user) {
  // TODO: remove User from the room
  Guard guard(lock);
  members.erase(user);
}

void Room::broadcast_message(const std::string &sender_username, const std::string &message_text) {
  // TODO: send a message to every (receiver) User in the room
   std::string room_name_to_input = room_name;
   room_name_to_input.resize(room_name_to_input.size ()- 1);
   std::string uname = sender_username;
   uname.resize(uname.size()-1);
   std::string message = room_name_to_input + ":" + uname + ":" + message_text;

   // Make a copy of the members set under the lock to minimize lock duration
   UserSet members_copy;
   {
     Guard g(lock);
     members_copy = members; // Copy the members set
   }
 
   // Send the message to every user in the room
   for (User *user : members_copy) {
     user->mqueue.enqueue(new Message(TAG_DELIVERY, message));
   }
}
