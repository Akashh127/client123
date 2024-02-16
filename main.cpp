#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <thread>
using namespace std;
#pragma comment(lib, "ws2_32.lib")

bool Initialize()
{
    WSADATA data;
    int result = WSAStartup(MAKEWORD(2, 2), &data);
    if (result != 0) {
        std::cout << "WSAStartup failed: " << result << std::endl;
        return false;
    }
    return true;
}

void SendMsg(SOCKET s)
{
    cout << "Enter your chat name:" << endl;
    string name;
    getline(cin, name);
    string message;

    while (1)
    {
        getline(cin, message);
        string msg = name + ": " + message; // Added missing '+'
        int bytesent = send(s, msg.c_str(), msg.length(), 0);

        if (bytesent == SOCKET_ERROR) {
            cout << "Error sending message" << endl;
            break;
        }

        if (message == "quit") {
            cout << "Stopping sending application" << endl;
            break;
        }
    }
}

void ReceiveMsg(SOCKET s)
{
    char buffer[4096];
    int recvlength; // Added variable to store the received length
    string msg = "";
    while (1) {
        recvlength = recv(s, buffer, sizeof(buffer), 0);

        if (recvlength <= 0) {
            cout << "Disconnected from the server" << endl;
            break;
        }
        else
        {
            msg = string(buffer, recvlength);
            cout << msg << endl;
        }
    }
}

int main()
{
    if (!Initialize()) {
        std::cout << "Winsock initialization failed" << std::endl;
        return 1;
    }

    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET) {
        std::cout << "Invalid socket created: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    int port = 12345;
    string serverAddress = "127.0.0.1";
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    if (inet_pton(AF_INET, serverAddress.c_str(), &(serverAddr.sin_addr)) != 1) {
        std::cout << "Invalid address: " << WSAGetLastError() << std::endl;
        closesocket(s);
        WSACleanup();
        return 1;
    }

    if (connect(s, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cout << "Not able to connect to server: " << WSAGetLastError() << std::endl;
        closesocket(s);
        WSACleanup();
        return 1;
    }

    std::cout << "Successfully connected to server" << std::endl;

    thread senderThread(SendMsg, s);
    thread receiverThread(ReceiveMsg, s);

    senderThread.join();
    receiverThread.join();

    closesocket(s);
    WSACleanup();

    return 0;
}
