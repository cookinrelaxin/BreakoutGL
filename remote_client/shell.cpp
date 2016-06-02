#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <string>
#include <sstream>
#include <iostream>
#include <vector>

extern "C" {

    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <errno.h>
    #include <unistd.h>

}

#include "linenoise.h"
#include "json.hpp"
using json = nlohmann::json;

char* PORT("5858");
char* HOST("localhost");

int MAXDATASIZE(1024);

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void completion(const char *buf, linenoiseCompletions *lc) {
    if (buf[0] == 'h') {
        linenoiseAddCompletion(lc, "hello");
        linenoiseAddCompletion(lc, "hello there");
    }
}

char *hints(const char *buf, int *color, int *bold) {
    if (!strcasecmp(buf,"hello")) {
        *color = 35;
        *bold = 0;
        return " World";
    }
    return nullptr;
}

std::string get_header(int sockfd) {
    char buf[MAXDATASIZE];
    int numbytes;

    std::string response;

    while (numbytes = recv(sockfd, buf, sizeof(buf), 0)) {
        for (int i(0); i<numbytes; ++i) {
            response.push_back(buf[i]);
        }
        const char last = response[response.size()-1];
        const char second_last = response[response.size()-2];
        if (last == '\n' and second_last == '\n')
            return response;
    }
}

std::vector<std::string>& split(const std::string &s,
                                char delim,
                                std::vector<std::string> & elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

std::string send_js(int sockfd, std::string expression) {
    json j = {
        {"seq", 117},
        {"type", "request"},
        {"command", "evaluate"},
        {"arguments", {
            {"expression", expression }
        }}
    };
    const std::string serialized = j.dump();

    int numbytes;
    const char* header = std::string(
            std::string("Content-Length:")
          + std::to_string(serialized.size())
          + std::string("\n\n")).c_str();

    // std::cout << "header: " << header << std::endl;


    // std::cout << "send header" << std::endl;
    int sent = 0;
    while (sent < strlen(header)) {
        numbytes = write(sockfd, header+sent, strlen(header)-sent);
        if (numbytes == -1)
            throw std::runtime_error("Can't send header");
        sent += numbytes;
    }

    const char* payload = serialized.c_str();

    // std::cout << "send payload" << std::endl;
    sent = 0;
    while (sent < strlen(payload)) {
        numbytes = write(sockfd, payload+sent, strlen(payload)-sent);
        if (numbytes == -1)
            throw std::runtime_error("Can't send payload");
        sent += numbytes;
    }

    char buf[MAXDATASIZE];

    // std::cout << "receive response" << std::endl;
    std::string response;
    while (numbytes = read(sockfd, buf, sizeof(buf))) {
        // std::cout << response << std::endl;
        for (int i(0); i<numbytes; ++i) {
            response.push_back(buf[i]);
        }
        if (numbytes < sizeof(buf))
            break;
    }
    memset(buf, 0, sizeof(buf));

    // std::cout << "response: " << response << std::endl;

    return response;

}

int main(int argc, char** argv) {
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2) {
        fprintf(stderr, "usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            // perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            // perror("client: connect");
            close(sockfd);
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client:: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr*)p->ai_addr), s, sizeof s);

    // printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo);

    std::string header = get_header(sockfd);

    // printf("client: received '%s'\n", header.c_str());

    // printf(send_js(sockfd, "p = new Vector3(1,2,3)").c_str());
    // send_js(sockfd, "p = new Vector3(1,2,3)");


    char *line;
    char *prgname = argv[0];
    
    linenoiseSetCompletionCallback(completion);
    linenoiseHistoryLoad("history.txt");

    std::cout << "Connected to ZEngine at " << s << std::endl;
    while ((line = linenoise("ZEngine> ")) != NULL) {
        if (line[0] != '\0' && line[0] != '/') {
            // printf("echo: '%s'\n", line);
            printf("%s\n", send_js(sockfd, line).c_str());
            // printf(send_js(sockfd, line).c_str());
            linenoiseHistoryAdd(line);
            linenoiseHistorySave("history.txt");
        }
        else if (!strncmp(line, "/historylen", 11)) {
            int len = atoi(line+11);
            linenoiseHistorySetMaxLen(len);
        }
        else if (line[0] == '/') {
            printf("Unrecognized command: %s\n", line);
        }
        free(line);
    }

    close(sockfd);
    return 0;
}
