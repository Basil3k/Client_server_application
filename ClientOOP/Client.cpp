#include "Client.h"

void Client::ClientHandler() //in this method receives a message from the server and prints it to the console 
{
   CommunicationMessage message{};
   while (recv(m_connection, message.msg, sizeof(message.msg), NULL))
   {
      std::cout << "You receive new message: " << message.msg << std::endl;
   }
   closesocket(m_connection);
}

void Client::CreateNewUser()
{
   while (true)
   {
     AuthorizationMessage authorization;
      std::cout << "Enter you login" << std::endl;
      std::cin >> authorization.login;
      std::cout << "Enter you password" << std::endl;
      std::cin >> authorization.password;
      authorization.type = TypeOfAuthorization::CREATE_NEW_USER;
      //here client send new login and password to server 
      send(m_connection, reinterpret_cast<char*>(&authorization), sizeof(authorization), NULL);
      // server compare they and if dont find entered login in his database save users login and password 
      char server_answer[64]; // magic number move it to common.h and make it constant constexpr
      recv(m_connection, server_answer, sizeof(server_answer), NULL);
      //then send message about it
      std::cout << server_answer << std::endl;
      auto compare_new_user = "Authorization ok"; // move this variable to common.h and make it constant constexpr
      //here we compare server message if it right client application send in console message about it 
      if (!(std::strcmp(server_answer, compare_new_user)))
      {
         std::cout << "We saved your login and passwod." << std::endl;
         break;
      }// implement case where user change his mind to create new user
   }
}

bool Client::ConnectToServer()
{
   m_DLLVersion = MAKEWORD(2, 1);
   if (WSAStartup(m_DLLVersion, &m_wsa_data) != 0)
   {
      std::cerr << "ERROR_LOAD_LIBRARY" << std::endl;
      exit(1);
   }
   m_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//IP-addres // magic number move it to common.h and make it constant constexpr
   m_addr.sin_port = htons(8484);//port number // magic number move it to common.h and make it constant constexpr
   m_addr.sin_family = AF_INET;//type of socket(for internet protocols)
   m_connection = socket(m_addr.sin_family, SOCK_STREAM, NULL);//try connect to server 
   return (connect(m_connection, reinterpret_cast<SOCKADDR*>(&m_addr), sizeof(m_addr)) != 0);
}

void Client::ChoisedInitialization()
{
   std::cout << "Write 1 if you want enter you login and password" << std::endl;
   std::cout << "Write 2 if you want enter new login and password" << std::endl;
   size_t choise;
   std::cin >> choise;
   switch (choise)
   {
   case (1):
   {
      AuthorizationInServer();
   }
   break;
   case (2): 
   {
      CreateNewUser();
   }
   break;
   default:
      std::cout << "Wrong choise." << std::endl;
      break;
   }
   // this step should be only if chosen value from expected values list
   // and implement case if chosen value not in expected list
   CreateClientHandlerThread();//after autorization create thread for recv message from server
}

void Client::CreateClientHandlerThread()
{
   std::thread client_handler_thread(&Client::ClientHandler, this);
   client_handler_thread.detach();
}

void Client::CreateSendMessageThread()
{
   std::thread send_message_thread(&Client::SendMessageToServer, this);
   while (true) {} // this statement don't let correctly disconnect from the server
}

void Client::AuthorizationInServer()
{
   AuthorizationMessage authorization;
   std::cout << "Enter you login" << std::endl;
   std::cin >> authorization.login;
   std::cout << "Enter you password" << std::endl;
   std::cin >> authorization.password;
   authorization.type = TypeOfAuthorization::SELECTING_AN_EXISTING_USER;
   send(m_connection, reinterpret_cast<char*>(&authorization), sizeof(authorization), NULL);
}

void Client::SendMessageToServer()
{
   ChoisedInitialization();
   CommunicationMessage message;
   size_t operating_mode{}; // use same style
   size_t ansver; // use same style
   //here user can choise several types for sended message. User can send private message to choised user or to all connected users
   while (true) // too tricky logic. it would be simplified. maybe split into multiple methods
   {
      if (operating_mode == 1)
      {
         std::cout << "Enter message to user. Or <<!reset>> to reset.\n";
         std::cin >> message.msg;
         if(message.msg[0] != '!')
         {
            send(m_connection, reinterpret_cast<char*>(&message), sizeof(message), NULL);
         }
         else if (message.msg[1] == 'r')
         {
            operating_mode = 0;
         }
      }
      if (operating_mode == 0)
      {
         std::cout << std::endl 
                   << "If you wanna send message to chosen user enter 1.\n"
                   << "If you wanna send message to all connected users enter 2.\n"
                   << "If you wanna get conneted users list enter 3.\n";
         
         std::cin >> ansver; 
      }
      if ((ansver == 1) and (operating_mode != 1))
      {
         message.type = TypeOfMessage::MESSAGE_TO_CHOSEN_USER;
         operating_mode = 1;
         std::cout << "Enter target user.\n";
         std::cin >> message.target_user;
      }
      else if ((ansver == 2) and (operating_mode != 1))
      {
         message.type = TypeOfMessage::MESSAGE_TO_ALL_USERS;
         operating_mode = 2;
         std::cout << "Enter message to all connected users. Or <<!reset>> to reset.\n";
         std::cin >> message.msg;
         if (message.msg[0] != '!') // move compare order
         {
            send(m_connection, reinterpret_cast<char*>(&message), sizeof(message), NULL);
         }
         else if (message.msg[1] == 'r') // move compare order
         {
            operating_mode = 0;
         }
      }
      else if (ansver == 3)
      {
         message.type = TypeOfMessage::GET_USERS_LIST;
         send(m_connection, reinterpret_cast<char*>(&message), sizeof(message), NULL);
         char users_list[1024]; // magic number move it to common.h and make it constant constexpr
         recv(m_connection, users_list, sizeof(users_list), NULL);//recive user list here, not in the ClientHandler
         std::cout << "Users, which connected now in the server: " << users_list << std::endl;
      }
   }
}

void Client::ConnectClientToServer()
{
   if (ConnectToServer() != 0)
   {
      std::cerr << "Error: failed connect to server." << std::endl;
      exit(1);
   }
   std::cout << "Connected to server!" << std::endl;
   CreateSendMessageThread();
}
