#pragma once
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)
#include <winsock2.h>

#include <iostream> // all dont using includs move to .cpp file
#include <vector>
#include <sstream>
#include <fstream>
#include <map>
#include <thread>

#include "..\Utils.h"

;class  Client
{
public:
   void ConnectClientToServer();

private:
   void ClientHandler();
   void CreateNewUser();
   bool ConnectToServer();
   void ChoisedInitialization();
   void AuthorizationInServer();
   void CreateClientHandlerThread();
   void CreateSendMessageThread();
   void SendMessageToServer();
   
   SOCKADDR_IN m_addr{};
   WORD m_DLLVersion {};
   SOCKET m_connection {};
   WSAData m_wsa_data {};
};
