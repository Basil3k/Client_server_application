﻿#include "Server.h"

void Server::ClientHandler(int index)
{
   CommunicationMessage message{};
   while (recv(m_connection[index], reinterpret_cast<char*>(&message), sizeof(message), NULL))
   {
      if (message.type == TypeOfMessage::MESSAGE_TO_CHOSEN_USER) // in some cases i use if else, and some where switch case. 
                                                                 // fix it to use one design
      {
         MessageToChosenUser(message, index);
      }
      if (message.type == TypeOfMessage::MESSAGE_TO_ALL_USERS)
      {
         MessageToAllUser(message, index);
      }
      if (message.type == TypeOfMessage::GET_USERS_LIST)
      {
         SendUserList(index);
      } // do something in "else" case
   }
   closesocket(m_connection[index]);
}

void Server::MessageToChosenUser(CommunicationMessage& message, int& index)
{
   for (size_t i = 0; i < m_client_counter; i++)
   {
      auto choised_user = m_users.find(message.target_user);
      // check that choised_user are not pointer to end dictionary
      if (i == choised_user->second)//if index(choised_user->second) == count index, send him message 
      {
         send(m_connection[i], message.msg, sizeof(message.msg), 0);
      } // if user was not found send message to sender about this situation
   }
}

void Server::MessageToAllUser(CommunicationMessage& message, int& index)
{
   for (size_t i = 0; i < m_client_counter; i++)
   {
      if (i == index)//if find the user who sent this message skip it
      {
         continue;
      }
      send(m_connection[i], message.msg, sizeof(message.msg), 0);
   }
}

void Server::SendUserList(int& index)
{
   for (size_t i = 0; i < m_client_counter; i++) // this iteration is redundant
   {
      if (i == index)
      {
         std::string users_list;
         for (auto it : m_users)
         {
            users_list.append(it.first + ", ");
         }
         users_list.pop_back();
         users_list.pop_back();
         send(m_connection[i], users_list.c_str(), users_list.size(), NULL); // enough use: "send(m_connection[index] ...)"
      }
   }
}

user_authorizacion Server::StringToUser(const std::string& user) // move somewhere out from this class, may be make it static
{
   user_authorizacion new_user;
   auto splitted_user = split(user, ' ');
   new_user.login = splitted_user.at(0);
   new_user.password = splitted_user.at(1);
   return new_user;
}

//here server compare login and password which client send him
bool Server::CompareUser(std::string login, std::string pass)
{
   std::fstream client_list("..\\ClientList.txt", std::ios::in); // move it to common.h and make it constant constexpr
   user_authorizacion check; // variable name does not correspond using
   std::map <std::string, std::string> users;
   std::string finded_pass;
   if (client_list)
   {
      while (!client_list.eof())//read all database // this code is identical, you can move it to separate method out from this class
      {
         std::string users_data; 
         std::getline(client_list, users_data);
         check = StringToUser(users_data);
         users[check.login] = check.password;
      }
      if (users.find(login) == users.end())
      {
         return false;
      }
      auto choised = users.find(login);
      finded_pass = choised->second;
   } // do something if cant open file
    // opened file dont closed (better to close by using close() method)
   return ((users.find(login) != users.end()) and (finded_pass == pass)); // change to "return finded_pass == pass"
}


void Server::CreateNewUser(std::string login, std::string pass)
{
   std::fstream client_list("..\\ClientList.txt", std::ios::in); // move it to common.h and make it constant constexpr
   std::map <std::string, std::string> all_users;
   AuthorizationMessage authorization;
   if (client_list)
   {  //server ask client enter login and password until client send unique login or his give up
      while (true)
      {
         while (!client_list.eof()) // this code is identical, you can move it to separate method out from this class
         {
            std::string users_data;
            std::getline(client_list, users_data);
            m_client_data = StringToUser(users_data);
            all_users[m_client_data.login] = m_client_data.password;
         }
         client_list.close();
         auto choised = all_users.find(login);
         if (choised != all_users.end())//if entered login is not unique(have in database)
         {
            // move "64" number to common.h and make it constant constexpr
            char msg[64] = "Oops, user with thath login already exists. Please try again.\n";
            send(m_new_connection, msg, 64, 0);
            recv(m_new_connection, reinterpret_cast<char*>(&authorization), sizeof(authorization), NULL);
            login = authorization.login;
            pass = authorization.password;
         }
         else
         {
            std::fstream client_list("..\\ClientList.txt", std::ios::app); // move it to common.h and make it constant constexpr
            if (client_list) // handle cases when when cant open this file and move working with file to separate method
            {
               client_list << std::endl << login << " " << pass;
               send(m_new_connection, "Authorization ok", sizeof("Authorization ok"), NULL);
               // move "Authorization ok" to common.h and make it constant constexpr
               break;
            }
         }
      }
   } // handle case when cant open file
}

void Server::MakeServer() // rename to "ConfigServer"
{
   m_DLL_version = MAKEWORD(2, 1);
   if (WSAStartup(m_DLL_version, &m_wsa_data) != 0)
   {
      std::cerr << "ERROR_LOAD_LIBRARY" << std::endl;
      exit(1);
   }
   m_sizeof_addr = sizeof(m_addr);
   m_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // magic number move it to common.h and make it constant constexpr
   m_addr.sin_port = htons(8484); // magic number move it to common.h and make it constant constexpr
   m_addr.sin_family = AF_INET;
   m_s_listen = socket(AF_INET, SOCK_STREAM, 0);
   bind(m_s_listen, reinterpret_cast<SOCKADDR*>(&m_addr), sizeof(m_addr));
   listen(m_s_listen, SOMAXCONN);
}

void Server::Listen()
{
   std::cout << "Server has been started ..." << std::endl;
   for (m_index = 0; m_index < 100; m_index++) // magic number move it to common.h and make it constant constexpr
                                               // may be should increase this number
   {
      m_new_connection = accept(m_s_listen, reinterpret_cast<SOCKADDR*>(&m_addr), &m_sizeof_addr);
      if (m_new_connection == 0) // check return code for positive case
      {
         std::cout << "Error #2\n"; // more information about error
      }
      else
      {     
         AuthorizationMessage authorization;
         UserAuthorization(authorization);
         if (CompareUser(authorization.login, authorization.password) == true) // "== true" unnecessary
         {
            std::cout << "Client Connected!\n";
            // move it to common.h and make it constant constexpr
            char msg[1024] = "Hi, this is an automatic message. You are connected to our chat.";
            send(m_new_connection, msg, sizeof(msg), 0);
            m_connection[m_index] = m_new_connection;
            m_users[authorization.login] = m_index;
            m_client_counter++;//this client counter used in ClientHandler when send message to client
            // creating thread for new user may be not good idea, but now i dont have another implementation
            CreateNewThread(m_index);//using this index, the server selects the user to send a message 
         }
         else
         {
            // move it to common.h and make it constant constexpr
            char msg[1024] = "You enter wrong login or password.\n";
            send(m_new_connection, msg, sizeof(msg), 0);
         }
      }
   }     
}

void Server::UserAuthorization(AuthorizationMessage& authorization)
{
   recv(m_new_connection, reinterpret_cast<char*>(&authorization), sizeof(authorization), NULL);
   if (authorization.type == TypeOfAuthorization::SELECTING_AN_EXISTING_USER)
   {
      m_users[authorization.login];
   }
   else if (authorization.type == TypeOfAuthorization::CREATE_NEW_USER)
   {
      CreateNewUser(authorization.login, authorization.password);
   } // do something in else case
}

void Server::CreateNewThread(int m_index)
{
   std::thread thr(&Server::ClientHandler, this, m_index);
   thr.detach();
}

void Server::StartServer()
{
   MakeServer();
   Listen();
}
