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
   char msg[1024]{ " " }; // magic number move it to common.h and make it constant constexpr
   char target_user[64] {" "}; // magic number move it to common.h and make it constant constexpr
   TypeOfMessage type;
};

enum class TypeOfAuthorization // add some value for defaulting initialization
{
   SELECTING_AN_EXISTING_USER,
   CREATE_NEW_USER
};

struct AuthorizationMessage
{
   TypeOfAuthorization type; // using uniform ("{}") initialization
   char login[MAX_LOGIN_SIZE];
   char password[MAX_PASSWORD_SIZE];
};
