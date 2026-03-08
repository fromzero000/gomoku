#include <iostream>
#include <vector>
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")
constexpr int BUF_SIZE = 1024;
void ErrorHandling(const std::string& message, SOCKET hServSock = INVALID_SOCKET, SOCKET hClntSock = INVALID_SOCKET);

int main(int argc, char* argv[])
{
    WSAData wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error");

    SOCKET hServSock, hClntSock;
    sockaddr_in servAddr{}, clntAddr{};
    int clntAddrSize;
    std::string message;
    hServSock = socket(PF_INET, SOCK_STREAM, 0);
    if(hServSock == INVALID_SOCKET)
        ErrorHandling("socket() error");
    
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(atoi(argv[1]));

    if (bind(hServSock, (sockaddr *)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
        ErrorHandling("bind() error");

    if(listen(hServSock, 1) == SOCKET_ERROR)
        ErrorHandling("listen() error");
    clntAddrSize = sizeof(clntAddr);
    hClntSock = accept(hServSock, (sockaddr*)&clntAddr, &clntAddrSize);
    if (hClntSock == -1)
        ErrorHandling("accept() error");
    else
        std::cout << "Connected client\n";
    /*통신 테스트용 코드 (로직 추가 예정)*/
    while (1)
    {
        message.resize(BUF_SIZE);
        int receivedDataLen = recv(hClntSock, message.data(), BUF_SIZE, 0);
        if (receivedDataLen <= 0)
            break;
        message.resize(receivedDataLen);
        std::cout << "Received from client : " << message << '\n';
        send(hClntSock, message.data(), receivedDataLen, 0);
    }
    /*통신 테스트용 코드 종료*/
    closesocket(hClntSock);
    closesocket(hServSock);
    WSACleanup();
    return 0;
}

void ErrorHandling(const std::string& message, SOCKET hServSock, SOCKET hClntSock)
{
    std::cerr << message << '\n';
    if (hServSock != INVALID_SOCKET) closesocket(hServSock);
    if (hClntSock!= INVALID_SOCKET) closesocket(hClntSock);
    WSACleanup();
    exit(1);
}