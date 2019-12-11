#pragma once

#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <fstream>
#include <map>
#include <thread>

#include "..\Utils.h"
#include "..\Split.h"

#include "UserAuthorizacion.h"

#pragma warning(disable: 4996)

class Server
{
public:
   void StartServer();
   Server() {};
private:
   Server(const Server& server) = delete;
   Server& operator=(const Server&) = delete;
   void ClientHandler(int index);
   void MessageToChosenUser(CommunicationMessage& message, int& index);
   void MessageToAllUser(CommunicationMessage& message, int& index);
   void SendUserList(int& index);
   void MakeServer();
   user_authorizacion StringToUser(const std::string& user);
   bool CompareUser(std::string login, std::string pass);
   void CreateNewUser(std::string login, std::string pass);
   void UserAuthorization(AuthorizationMessage& authorization);
   void CreateNewThread(int m_index);
   void Listen();

   size_t m_client_counter{};
   int m_index{};
   int m_sizeof_addr{};
   std::map <std::string, int> m_users;
   user_authorizacion m_client_data{};
   SOCKADDR_IN m_addr;
   WORD m_DLL_version;
   SOCKET m_connection[100];
   WSAData m_wsa_data{};
   SOCKET m_s_listen;
   SOCKET m_new_connection{};
};