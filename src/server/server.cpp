#include "server.h"

void Server()
{
    // Initialze winsock
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);

    int wsOk = WSAStartup(ver, &wsData);
    if (wsOk != 0)
    {
        cerr << "Can't Initialize winsock! Quitting" << endl;
        return;
    }

    // Create a socket
    SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == INVALID_SOCKET)
    {
        cerr << "Can't create a socket! Quitting" << endl;
        return;
    }

    // Bind the ip address and port to a socket
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(13254);
    hint.sin_addr.S_un.S_addr = INADDR_ANY; // Could also use inet_pton ....

    bind(listening, (sockaddr*)&hint, sizeof(hint));

    // Tell Winsock the socket is for listening
    listen(listening, SOMAXCONN);

    // Create the master file descriptor set and zero it
    fd_set master;
    FD_ZERO(&master);

    // Add our first socket that we're interested in interacting with; the listening socket!
    // It's important that this socket is added for our server or else we won't 'hear' incoming
    // Connections
    FD_SET(listening, &master);

    // This will be changed by the \quit command (see below, bonus not in video!)
    bool running = true;

    //
    cout << "Server Starting!\n";

    while (running)
    {
        fd_set copy = master;

        // See who's talking to us
        int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

        // Loop through all the current connections / potential connect
        for (int i = 0; i < socketCount; i++)
        {
            // Makes things easy for us doing this assignment
            SOCKET sock = copy.fd_array[i];

            // Is it an inbound communication?
            if (sock == listening)
            {
                // Accept a new connection
                SOCKET client = accept(listening, nullptr, nullptr);

                // Add the new connection to the list of connected clients
                FD_SET(client, &master);

                // Send a welcome message to the connected client
                string connectionMessage = "-Connect";
                send(client, connectionMessage.c_str(), connectionMessage.size() + 1, 0);
            }
            else // It's an inbound message
            {
                char buf[4096];
                ZeroMemory(buf, 4096);

                // Receive message
                int bytesIn = recv(sock, buf, 4096, 0);

                if (bytesIn <= 0)
                {
                    // Drop the client
                    closesocket(sock);
                    FD_CLR(sock, &master);
                }
                else
                {
                    // Check to see if it's a command. \quit kills the server
                    string cmd = string(buf, bytesIn);
                    if (cmd == "123")
                    {
                        cout << "Kill Server!";
                        running = false;
                        break;
                    }

                    //
                    SOCKET outSock = master.fd_array[(int)buf[0] - 48];
                    if (outSock != listening && outSock != sock)
                    {
                        string command = buf;

                        send(outSock, command.c_str(), command.size() + 1, 0);
                    }
                }
            }
        }
    }

    // Remove the listening socket from the master file descriptor set and close it
    // to prevent anyone else trying to connect.
    FD_CLR(listening, &master);
    closesocket(listening);

    // Message to let users know what's happening.
    string msg = "Server is shutting down. Goodbye\r\n";

    while (master.fd_count > 0)
    {
        // Get the socket number
        SOCKET sock = master.fd_array[0];

        // Send the goodbye message
        send(sock, msg.c_str(), msg.size() + 1, 0);

        // Remove it from the master file list and close the socket
        FD_CLR(sock, &master);
        closesocket(sock);
    }

    // Cleanup winsock
    WSACleanup();

    system("pause");
}

void ShowLastError()
{
    wchar_t* s = nullptr;
    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                   FORMAT_MESSAGE_FROM_SYSTEM |
                   FORMAT_MESSAGE_IGNORE_INSERTS,
                   nullptr,
                   WSAGetLastError(),
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                   (LPWSTR)&s, 0, nullptr);
    fprintf(stderr, "%S\n", s);
    LocalFree(s);
}