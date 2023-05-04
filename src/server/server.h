#ifndef SERVERPD_SERVER_H
#define SERVERPD_SERVER_H

//////////////////
#include <iostream>
#include <string>
#include <ws2tcpip.h>
#include <sstream>


#define MAX_CLIENTS 10
#define DEFAULT_BUFLEN 4096

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void Server();
void ShowLastError();

#endif //SERVERPD_SERVER_H
