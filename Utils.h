#pragma once

constexpr uint32_t MAX_LOGIN_SIZE = 64;
constexpr uint32_t MAX_PASSWORD_SIZE = 64;

enum class TypeOfMessage
{
   MESSAGE_TO_CHOSEN_USER,
   MESSAGE_TO_ALL_USERS,
   GET_USERS_LIST,
   GET_COMMANDS_LIST
};

struct CommunicationMessage
{
   char msg[1024]{ " " };
   char target_user[64] {" "};
   TypeOfMessage type;
};

enum class TypeOfAuthorization
{
   SELECTING_AN_EXISTING_USER,
   CREATE_NEW_USER
};

struct AuthorizationMessage
{
   TypeOfAuthorization type;
   char login[MAX_LOGIN_SIZE];
   char password[MAX_PASSWORD_SIZE];
};
