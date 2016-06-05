#ifndef REPL_SERVER_H
#define REPL_SERVER_H

#include <cassert>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <string>
#include <iostream>
#include <atomic>

class ReplServer {
    public:
        static void Start();
        static std::atomic<bool> terminateServer;
    private:        

    static const int debugging_port = 5858;
    static constexpr char* const kContentLength = "Content-Length";
    static const int kContentLengthSize = strlen(kContentLength);
    static int _main_debug_client_socket;

    static void SendBuffer(int socket, const std::string& message) {
        std::string payload = message;    
        if (payload.empty())
            payload = "(empty)";
        int n = send(socket, payload.c_str(), payload.size(), 0);
    }

    static void SendMessage(int conn, const std::string& message) {
        SendBuffer(conn, message); 
    }

    static std::string GetRequest(int socket) {
        int received;
        int content_length(0);
        while (true) {
            const int kHeaderBufferSize(80);
            char header_buffer[kHeaderBufferSize];
            int header_buffer_position(0);
            char c =  '\0';

            while(c != '\n') {
                received = recv(socket, &c, 1, 0);
                //assert(received >= 0);
                if (header_buffer_position < kHeaderBufferSize)
                    header_buffer[header_buffer_position++] = c;
            }

            if (header_buffer_position == 1)
                break;

            assert(header_buffer_position > 0);
            assert(header_buffer_position <= kHeaderBufferSize);
            header_buffer[header_buffer_position - 1] = '\0';

            char* key = header_buffer;
            char* value = nullptr;
            for (int i(0); header_buffer[i] != '\0'; i++)
                if (header_buffer[i] == ':') {
                    header_buffer[i] = '\0';
                    value = header_buffer + i + 1;
                    while (*value == ' ')
                        value++;
                    break;
                }

            if (strcmp(key, kContentLength) == 0) {
                if (value == nullptr || strlen(value) > 7)
                    return std::string();
                for (int i = 0; value[i] != '\0'; i++) {
                    if (value[i] < '0' || value[i] > '9')
                        return std::string();
                    content_length = 10 * content_length + (value[i] - '0');
                }
            }
            else
                std::cout << key
                          << ": "
                          << (value != nullptr ? value : "(no value)")
                          << std::endl;
        }

        if (content_length == 0)
            return std::string();

        std::string buffer;
        buffer.resize(content_length);
        received = recv(socket, &buffer[0], content_length, 0);
        if (received < content_length) {
            std::cout << "Error request data size" << std::endl;
            return std::string();
        }
        buffer[content_length] = '\0';
        return buffer;
    }
};

#endif
