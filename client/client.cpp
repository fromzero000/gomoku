#include <iostream>
#include <vector>
#include <sstream>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")
constexpr int BUF_SIZE = 1024;

void ErrorHandling(const std::string& message, SOCKET hServSock = INVALID_SOCKET);

int main(int argc, char *argv[])
{
    WSAData wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error");

    SOCKET hServSock;
    sockaddr_in servAddr{};

    std::string message;
    int strLen;
    
    hServSock = socket(PF_INET, SOCK_STREAM, 0);
    if (hServSock == INVALID_SOCKET)
        ErrorHandling("socket() error");

    servAddr.sin_family = AF_INET;
    int inetPtonResult = inet_pton(AF_INET, argv[1], &servAddr.sin_addr);
    //visual studio에서 inetPton 사용시 유니코드 관련 에러 발생
    if (inetPtonResult != 1)
    {
        if (inetPtonResult == 0)
            ErrorHandling("Invalid IP Address", hServSock);
        else
        {
            int errCode = WSAGetLastError();
            ErrorHandling("inet_pton() error, error Code : "+std::to_string(errCode), hServSock);
        }
    }
    servAddr.sin_port = htons(atoi(argv[2]));

    if (connect(hServSock, (sockaddr*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
        ErrorHandling("connect() error", hServSock);
    else
        std::cout << "Connected...\n";

    while (1)
    {
        std::cout << "Input Message(Q to quit) : ";
        std::getline(std::cin, message);
        if (message == "Q" || message == "q")
            break;

        message.resize(message.length());
        send(hServSock, message.data(), message.length(), 0);
        message.resize(BUF_SIZE);
        strLen = recv(hServSock, message.data(), BUF_SIZE, 0);
        if (strLen == -1)
            ErrorHandling("recv() error", hServSock);
        message.resize(strLen);
        std::cout << "Echo from server : " << message << '\n';
    }
    closesocket(hServSock);
    WSACleanup();
    return 0;
}

void ErrorHandling(const std::string& message, SOCKET hServSock)
{
    std::cerr << message << '\n';
    if (hServSock != INVALID_SOCKET) closesocket(hServSock);
    WSACleanup();
    exit(1);
}