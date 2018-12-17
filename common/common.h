#pragma once
#include <stdint.h>

const uint16_t PORT = 9999;
const uint32_t SOCKET_BUFFER_SIZE = 1024;

enum class Client_Message : uint8_t
{
    Join,		// tell server we're new here
    Leave,		// tell server we're leaving
    Input 		// tell server our user input
};

enum class Server_Message : uint8_t
{
    Join_Result,// tell client they're accepted/rejected
    State 		// tell client game state
};

struct IP_Endpoint
{
    uint32_t address;
    uint16_t port;
};
bool operator==(const IP_Endpoint& a, const IP_Endpoint& b) { return a.address == b.address && a.port == b.port; }

struct MyUUID
{
    MyUUID() {}
    MyUUID(UUID uuid) : m_uuid(uuid)
    {
        UuidToStringA(&m_uuid, (RPC_CSTR*)&m_uuidString);
    }
    ~MyUUID()
    {
        RpcStringFreeA((RPC_CSTR*)&m_uuidString);
    }
    UUID m_uuid;
    std::string m_uuidString;
};