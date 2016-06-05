#include "repl_server.h"

#include <include/v8.h>

#include "json.hpp"

#include "readerwriterqueue.h"
#include "atomicops.h"

#include "shell.h"

using namespace v8;

using namespace moodycamel;
using json = nlohmann::json;

std::atomic<bool>           ReplServer::terminateServer(false);
int                         ReplServer::_main_debug_client_socket = -1;

void ReplServer::Start() {
    std::cout << "Begin debugger thread" << std::endl;
    
    int sockfd, client_socket, portno;
    socklen_t clilen;
    sockaddr_in serv_addr, cli_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(sockfd >= 0);

    int yes=1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    portno = debugging_port;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;;
    serv_addr.sin_port = htons(portno);
    int bindResult = bind(sockfd, reinterpret_cast<sockaddr *>(&serv_addr), sizeof(serv_addr));

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    while (1) {
        int rv;
        while (rv == 0) {
            if (terminateServer) return;

            fd_set readfds;
            struct timeval tv;
            tv.tv_sec = 1;
            FD_ZERO(&readfds);
            FD_SET(sockfd, &readfds);

            rv = select(sockfd+1, &readfds, NULL, NULL, &tv);
        }

        client_socket = accept(sockfd, reinterpret_cast<sockaddr *>(&cli_addr), &clilen);
        assert(client_socket >= 0);
        _main_debug_client_socket = client_socket;
        std::cout << "Client connected to debugger" << std::endl;

        SendBuffer(client_socket, "Type: connect\n");
        SendBuffer(client_socket,
                std::string("V8-Version: ") + std::string(V8::GetVersion()) + std::string("\n"));
        SendBuffer(client_socket, "Protocol-Version: 1\n");
        SendBuffer(client_socket, std::string("Embedding-Host: ZEngine\n"));
        SendBuffer(client_socket, "Content-Length: 0\n");
        SendBuffer(client_socket, "\n");

        while (1) {
            std::string request = GetRequest(client_socket);

            if (request.empty())
                continue;

            json j_request = json::parse(request);
            std::string jsExpression = j_request["arguments"]["expression"];

            Shell::UnevaluatedQueue.enqueue(jsExpression);
            while (true) {
                std::string evaluated;
                bool succeeded = Shell::EvaluatedQueue.try_dequeue(evaluated);
                if (succeeded) {
                    SendMessage(client_socket, evaluated);
                    break;
                }
            }

        }

    }

}
