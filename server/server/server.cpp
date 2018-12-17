#include <winsock2.h>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <windows.h>

#pragma comment(lib, "rpcrt4.lib")  // UuidCreate - Minimum supported OS Win 2000

// my own
#include "../../common/common.h"

const uint32_t TICKS_PER_SECOND = 60;
const float	SECONDS_PER_TICK = 1.0f / float(TICKS_PER_SECOND);
const float	CLIENT_TIMEOUT = 500.0f;

struct Client_Connection
{
    Client_Connection(float timeSinceLastRequest, IP_Endpoint endpointInfo, MyUUID clientUUID):
        m_timeSinceLastRequest(timeSinceLastRequest), m_endpointInfo(endpointInfo), m_clientUUID(clientUUID), m_removed(false){}
    
    MyUUID m_clientUUID;
    float m_timeSinceLastRequest;
    IP_Endpoint m_endpointInfo;
    bool m_removed;
};

struct Player_State
{
    Player_State() {}
    Player_State(MyUUID clientUUID, float x, float y, uint32_t updateRound) : m_clientUuid(clientUUID), m_x(x), m_y(y), m_updateRound(updateRound)
    {}
    MyUUID m_clientUuid;
    float m_x, m_y;
    uint32_t m_updateRound;
};

struct Player_Input
{
    Player_Input(MyUUID clientUUID, bool up, bool down, bool left, bool right) : m_clientUUID(clientUUID), m_up(up), m_down(down), m_left(left), m_right(right)
    {}
    MyUUID m_clientUUID;
    bool m_up, m_down, m_left, m_right;
};

static float time_since(LARGE_INTEGER t, LARGE_INTEGER frequency)
{
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);

    return float(now.QuadPart - t.QuadPart) / float(frequency.QuadPart);
}

int main()
{
    WORD winsock_version = 0x202;
    WSADATA winsock_data;
    if (WSAStartup(winsock_version, &winsock_data))
    {
        std::cout<< "WSAStartup failed: " << WSAGetLastError() << std::endl;
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (sock == INVALID_SOCKET)
    {
        std::cout << "socket failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    SOCKADDR_IN local_address;
    local_address.sin_family = AF_INET;
    local_address.sin_port = htons(9999);
    local_address.sin_addr.s_addr = INADDR_ANY;
    if (bind(sock, (SOCKADDR*)&local_address, sizeof(local_address)) == SOCKET_ERROR)
    {
        std::cout << "bind failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // put socket in non-blocking mode
    u_long enabled = 1;
    ioctlsocket(sock, FIONBIO, &enabled);

    UINT sleep_granularity_ms = 1;
    bool sleep_granularity_was_set = timeBeginPeriod(sleep_granularity_ms) == TIMERR_NOERROR;

    LARGE_INTEGER clock_frequency;
    QueryPerformanceFrequency(&clock_frequency);

    std::string buffer;
    
    std::unordered_map<std::string, std::shared_ptr<Client_Connection>> clientEndpoints;
    std::unordered_map<std::string, Player_State> playerStates;
    std::vector<std::shared_ptr<Client_Connection>> clientConnections;
    
    int32_t player_x = 0;
    int32_t player_y = 0;
    uint32_t updateRound = 0;
    bool is_running = 1;
    while (is_running)
    {
        LARGE_INTEGER tick_start_time;
        QueryPerformanceCounter(&tick_start_time);

        // player inputs to be filled in while recvfrom
        std::vector<Player_Input> playersInputs;
        ++updateRound;
        if (updateRound == UINT32_MAX)
        {
            updateRound = 1;
        }

        while (true)
        {
            // get input packet from player
            int flags = 0;
            SOCKADDR_IN from;
            int from_size = sizeof(from);
            int bytes_received = recvfrom(sock, &buffer[0], SOCKET_BUFFER_SIZE, flags, (SOCKADDR*)&from, &from_size);

            if (bytes_received == SOCKET_ERROR)
            {
                int error = WSAGetLastError();
                if (error != WSAEWOULDBLOCK)
                {
                    std::cout << "recvfrom returned SOCKET_ERROR, WSAGetLastError(): " << error << std::endl;
                }
                break;
            }

            IP_Endpoint from_endpoint;
            from_endpoint.address = from.sin_addr.S_un.S_addr;
            from_endpoint.port = from.sin_port;

            // process input
            switch (buffer[0])
            {
                case static_cast<uint8_t>(Client_Message::Join) :
                {
                    std::cout << "Client_Message::Join from " << from_endpoint.address << ":" << from_endpoint.port << std::endl;

                    // generate new client uuid
                    UUID uuid;
                    UuidCreate(&uuid);
                    MyUUID clientUuid(uuid);

                    buffer[0] = static_cast<char>(Server_Message::Join_Result);
                    std::cout << "new client assigned with uuid: " << clientUuid.m_uuidString << std::endl;
                    
                    buffer[1] = 1;
                    memcpy(&buffer[2], &uuid, sizeof(UUID));

                    flags = 0;
                    if (sendto(sock, &buffer[0], buffer.size(), flags, (SOCKADDR*)&from, from_size) != SOCKET_ERROR)
                    {
                        std::shared_ptr<Client_Connection> clientConnection = std::make_shared<Client_Connection>(0.0f, from_endpoint, clientUuid);
                        clientEndpoints[clientUuid.m_uuidString] = clientConnection;
                        Player_State playerState(clientUuid, 0, 0, 0);
                        playerStates[clientUuid.m_uuidString] = playerState;
                        clientConnections.push_back(clientConnection);
                    }
                    else
                    {
                        std::cout << "sendto failed: " << WSAGetLastError() << std::endl;
                    }
                }
                break;

                case static_cast<uint8_t>(Client_Message::Leave) :
                {
                    UUID uuid;
                    memcpy(&uuid, &buffer[1], sizeof(UUID));
                    MyUUID clientUuid(uuid);

                    if (clientEndpoints[clientUuid.m_uuidString]->m_endpointInfo == from_endpoint)
                    {
                        clientEndpoints[clientUuid.m_uuidString]->m_removed = true;
                        clientEndpoints.erase(clientUuid.m_uuidString);

                        std::cout 
                            << "Client_Message::Leave from " 
                            << clientUuid.m_uuidString << " "
                            << from_endpoint.address 
                            << ":" << from_endpoint.port 
                            << std::endl;
                    }
                }
                break;
                case static_cast<uint8_t>(Client_Message::Input) :
                {
                    UUID uuid;
                    memcpy(&uuid, &buffer[1], sizeof(UUID));
                    MyUUID clientUuid(uuid);

                    std::cout << bytes_received << " bytes received from: " << clientUuid.m_uuidString << std::endl;

                    if (clientEndpoints[clientUuid.m_uuidString]->m_endpointInfo == from_endpoint)
                    {
                        uint32_t bytes_read = 1 + sizeof(UUID);
                        uint8_t input = buffer[bytes_read];

                        Player_Input playerInput(clientUuid, false, false, false, false);
                        switch (input)
                        {
                            case 'w':
                                playerInput.m_up = true;
                                break;

                            case 'a':
                                playerInput.m_left = true;
                                break;

                            case 's':
                                playerInput.m_down = true;
                                break;

                            case 'd':
                                playerInput.m_right = true;
                                break;

                            default:
                                std::cout << "Client_Message::Unhandled input: " << input;
                                break;
                        }

                        clientEndpoints[clientUuid.m_uuidString]->m_timeSinceLastRequest = 0.0f;
                        playersInputs.push_back(playerInput);
                        std::cout << "Client_Message::Input: " << input << std::endl;
                    }
                    else
                    {
                        std::cout << "Client_Message::Input discarded, was from: " << from_endpoint.address << ":" << from_endpoint.port
                            << "but expected: " << clientEndpoints[clientUuid.m_uuidString]->m_endpointInfo.address << ":"
                            << clientEndpoints[clientUuid.m_uuidString]->m_endpointInfo.port << std::endl;
                    }
                    
                }
                break;
                default:
                    std::cout << "unhandled input: " << buffer[0] << std::endl;
                    break;
            }
        }
        // create state packet
        buffer[0] = static_cast<uint8_t>(Server_Message::State);
        uint32_t bytes_written = 1;
        for (const auto & playerInput : playersInputs)
        {
            Player_State playerState = playerStates[playerInput.m_clientUUID.m_uuidString];
            if (playerInput.m_up)
            {
                playerState.m_x += 1;
            }
            if (playerInput.m_down)
            {
                playerState.m_x -= 1;
            }
            if (playerInput.m_right)
            {
                playerState.m_y += 1;
            }
            if (playerInput.m_left)
            {
                playerState.m_x -= 1;
            }
            playerState.m_updateRound = updateRound;
            memcpy(&buffer[bytes_written], &playerInput.m_clientUUID, sizeof(playerInput.m_clientUUID));
            bytes_written += sizeof(playerInput.m_clientUUID);

            memcpy(&buffer[bytes_written], &playerState.m_x, sizeof(playerState.m_x));
            bytes_written += sizeof(playerState.m_x);

            memcpy(&buffer[bytes_written], &playerState.m_y, sizeof(playerState.m_y));
            bytes_written += sizeof(playerState.m_y);
        }

        // send back to clients
        int flags = 0;
        SOCKADDR_IN to;
        to.sin_family = AF_INET;
        to.sin_port = htons(9999);
        int to_length = sizeof(to);

        for (const auto &clientConnection : clientConnections)
        {
            // increment the time since last request
            if (playerStates[clientConnection->m_clientUUID.m_uuidString].m_updateRound != updateRound)
            {
                clientConnection->m_timeSinceLastRequest += SECONDS_PER_TICK;
                if (clientConnection->m_timeSinceLastRequest > CLIENT_TIMEOUT)
                {
                    std::cout << "client with uuid: " << clientConnection->m_clientUUID.m_uuidString << " has timed out." << std::endl;
                    clientConnection->m_removed = true;
                }
                if (clientConnection->m_removed) 
                {
                    continue;
                }
                to.sin_addr.S_un.S_addr = clientConnection->m_endpointInfo.address;
                to.sin_port = clientConnection->m_endpointInfo.address;
                if (sendto(sock, &buffer[0], bytes_written, flags, (SOCKADDR*)&to, to_length) == SOCKET_ERROR)
                {
                    std::cout << "send to failed: " << WSAGetLastError() << std::endl;
                }
            }
        }

        float time_taken_s = time_since(tick_start_time, clock_frequency);

        while (time_taken_s < SECONDS_PER_TICK)
        {
            if (sleep_granularity_was_set)
            {
                DWORD time_to_wait_ms = DWORD((SECONDS_PER_TICK - time_taken_s) * 1000);
                if (time_to_wait_ms > 0)
                {
                    Sleep(time_to_wait_ms);
                }
            }

            time_taken_s = time_since(tick_start_time, clock_frequency);
        }
    }
    system("PAUSE");
    WSACleanup();
    return 0;
}