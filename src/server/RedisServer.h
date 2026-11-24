#pragma once

#include "../parser/CommandParser.h"
#include "../commands/CommandHandler.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
using socket_t = SOCKET;
#else
#include <sys/socket.h>
using socket_t = int;
#endif

class RedisServer
{
private:
    CommandParser& parser;
    CommandHandler& handler;

public:
    RedisServer(CommandParser& parser,
                CommandHandler& handler);

    void start();
    void handleClient(socket_t clientSocket);
};
