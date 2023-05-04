#ifndef SERVERPD_SERVER_H
#define SERVERPD_SERVER_H

//////////////////
#include <iostream>
#include <string>
#include <ws2tcpip.h>
#include <sstream>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void Server();
void ShowLastError();

#endif //SERVERPD_SERVER_H
