/*
 * author:  Xinrun Zhang
 * date:    Mar 03, 2021 17:12:40
 */

#ifndef SRC_SERVER_H
#define SRC_SERVER_h

#include<iostream>
#include<unistd.h>          // for fork(), exec()
#include<stdio.h>

#include<sys/socket.h>      // for socket()
#include<netinet/in.h>      // for sockaddr_in
#include<arpa/inet.h>       // for inet_pton()         
#include<string.h>          // for memset()
#include<sys/select.h>      // for select()
#include<sys/epoll.h>       // for epoll
#include<sys/time.h>        // for timeval
#include<netdb.h>           // getnameinfo
#include<string>

class server
{
private:
    /* data */
public:
    /* constructor */
    server(/* args */);

    /* destructor */
    ~server();

    /* member functions */
};

#endif