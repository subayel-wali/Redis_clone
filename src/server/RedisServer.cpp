#include "RedisServer.h"

#include <iostream>
#include <thread>

#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#define CLOSE_SOCKET closesocket
#define IS_INVALID_SOCKET(s) ((s) == INVALID_SOCKET)
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define CLOSE_SOCKET close
#define IS_INVALID_SOCKET(s) ((s) < 0)
#endif

using namespace std;

RedisServer::RedisServer(CommandParser& parser,
                         CommandHandler& handler)
    : parser(parser),
      handler(handler)
{
}

void RedisServer::handleClient(socket_t clientSocket)
{
    while(true)
    {
        char buffer[1024];

        int bytesReceived = ::recv(
            clientSocket,
            buffer,
            sizeof(buffer) - 1,
            0
        );

        if(bytesReceived <= 0)
        {
            cout << "Client Disconnected\n";
            break;
        }
        if(buffer[0]=='E' && buffer[1]=='X' && buffer[2]=='I' && buffer[3]=='T'){
            cout << "Client Disconnected\n";
            break;
        }

        buffer[bytesReceived] = '\0';

        auto tokens = parser.parse(buffer);

        string response = handler.execute(tokens);

        response += "\n";

        ::send(
            clientSocket,
            response.c_str(),
            static_cast<int>(response.size()),
            0
        );
    }

    CLOSE_SOCKET(clientSocket);
}

void RedisServer::start()
{
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cerr << "WSAStartup failed\n";
        return;
    }
#endif

    socket_t serverSocket = ::socket(AF_INET, SOCK_STREAM, 0);

    if (IS_INVALID_SOCKET(serverSocket))
    {
#ifdef _WIN32
        WSACleanup();
#endif
        return;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(6379);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (::bind(serverSocket,
               (sockaddr*)&serverAddress,
               sizeof(serverAddress)) != 0)
    {
        CLOSE_SOCKET(serverSocket);
#ifdef _WIN32
        WSACleanup();
#endif
        return;
    }

    if (::listen(serverSocket, 5) != 0)
    {
        CLOSE_SOCKET(serverSocket);
#ifdef _WIN32
        WSACleanup();
#endif
        return;
    }

    cout << "Server is listening on port 6379...\n";

    while(true)
    {
        socket_t clientSocket = ::accept(serverSocket, nullptr, nullptr);

        if (IS_INVALID_SOCKET(clientSocket))
            continue;

        cout << "Client Connected\n";

        thread clientThread(
            &RedisServer::handleClient,
            this,
            clientSocket
        );
            
        clientThread.detach();
    }

    CLOSE_SOCKET(serverSocket);
#ifdef _WIN32
    WSACleanup();
#endif
}
