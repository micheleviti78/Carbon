/**
 ******************************************************************************
 * @file           test.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Jan. 2024
 * @brief          TCP/IP client to test tracing
 ******************************************************************************
 * @attention
 * Copyright (c) 2024 Michele Viti.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <carbon/trace_format.hpp>

#include <iostream>

using namespace CARBON;

/*
** client.c -- a stream socket client demo
*/

#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <arpa/inet.h>

#define PORT "18888" // the port client will be connecting to

#define MAXDATASIZE 100 // max number of bytes we can get at once

static constexpr auto BUF_SIZE =
    sizeof(TracePacketHeader) + sizeof(TraceTaskSwitchedInEvent);

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(int argc, char *argv[]) {
    int sockfd, numbytes;
    uint8_t buf[BUF_SIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2) {
        fprintf(stderr, "usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) ==
            -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("client: connect");
            close(sockfd);
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s,
              sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

    while (1) {
        int bytesLeft = sizeof(TracePacketHeader);
        int bufOffset = 0;

        while (bytesLeft != 0) {
            if ((numbytes = recv(sockfd, buf + bufOffset, bytesLeft, 0)) ==
                -1) {
                perror("recv");
                exit(1);
            }

            bytesLeft -= numbytes;
            bufOffset += numbytes;

            if (bytesLeft < 0) {
                perror("data size received wrong");
                exit(1);
            }
        }

        TracePacketHeader *packetHeaderPtr =
            reinterpret_cast<TracePacketHeader *>(buf);

        std::cout << "NEW STREAM" << std::endl;
        std::cout << "cpuId " << packetHeaderPtr->cpuId << std::endl;
        std::cout << "packetSizeBits " << packetHeaderPtr->packetSizeBits
                  << std::endl;
        std::cout << "streamId " << packetHeaderPtr->streamId << std::endl;
        std::cout << "magic " << packetHeaderPtr->magic << std::endl;

        int bytesLeftStream =
            (packetHeaderPtr->packetSizeBits >> 3) - sizeof(TracePacketHeader);

        std::cout << "data stream size " << bytesLeftStream << std::endl;

        if (bytesLeftStream < 0 || bytesLeftStream < sizeof(TraceEventHeader)) {
            perror("too few data stream byte");
            exit(1);
        }

        bufOffset = 0;

        while (bytesLeftStream != 0) {
            bytesLeft = sizeof(TraceEventHeader);
            bufOffset = 0;

            while (bytesLeft != 0) {
                if ((numbytes = recv(sockfd, buf + bufOffset, bytesLeft, 0)) ==
                    -1) {
                    perror("recv");
                    exit(1);
                }

                bytesLeftStream -= numbytes;
                bytesLeft -= numbytes;
                bufOffset += numbytes;

                if (bytesLeft < 0) {
                    perror("TraceEventHeader received wrong");
                    exit(1);
                }

                if (bytesLeftStream < 0) {
                    perror("Stream received wrong");
                    exit(1);
                }
            }

            TraceEventHeader *traceEventHeaderPtr =
                reinterpret_cast<TraceEventHeader *>(buf);

            TraceEventID id =
                static_cast<TraceEventID>(traceEventHeaderPtr->id);

            std::cout << std::endl;
            std::cout << "HEADER NEW EVENT" << std::endl;
            std::cout << "id " << traceEventHeaderPtr->id << std::endl;
            std::cout << "eventSizeBits " << traceEventHeaderPtr->eventSizeBits
                      << std::endl;
            std::cout << "timestamp " << traceEventHeaderPtr->timestamp
                      << std::endl;

            bytesLeft = (traceEventHeaderPtr->eventSizeBits >> 3) -
                        sizeof(TraceEventHeader);

            if (bytesLeft < 0) {
                perror("too few event data byte");
                exit(1);
            }

            bufOffset = sizeof(TraceEventHeader);

            while (bytesLeft != 0) {
                if ((numbytes = recv(sockfd, buf + bufOffset, bytesLeft, 0)) ==
                    -1) {
                    perror("recv");
                    exit(1);
                }

                bytesLeftStream -= numbytes;
                bytesLeft -= numbytes;
                bufOffset += numbytes;

                if (bytesLeft < 0) {
                    perror("TraceEvent received wrong");
                    exit(1);
                }

                if (bytesLeftStream < 0) {
                    perror("Stream received wrong");
                    exit(1);
                }
            }

            switch (id) {
            case TraceEventID::Free:
            case TraceEventID::Malloc:
            case TraceEventID::PerfCnt:
                break;
            case TraceEventID::TaskSwitchedIn: {
                TraceTaskSwitchedInEvent *eventPtr =
                    reinterpret_cast<TraceTaskSwitchedInEvent *>(buf);
                std::cout << "TaskSwitchedIn: number " << eventPtr->number
                          << std::endl;
                break;
            }
            case TraceEventID::TaskSwitchedOut: {
                TraceTaskSwitchedOutEvent *eventPtr =
                    reinterpret_cast<TraceTaskSwitchedOutEvent *>(buf);
                std::cout << "TaskSwitchedOut: number " << eventPtr->number
                          << std::endl;
                break;
            }
            case TraceEventID::Tasks:
                break;
            }
        }

        std::cout << std::endl;
    }

    close(sockfd);

    return 0;
}
