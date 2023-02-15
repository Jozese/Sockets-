#pragma once
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "libcrypto.lib")
#pragma comment(lib, "Ws2_32.lib")
#include "includes.h"

class ClientSockets
{
private:
	std::wstring inetAddr;
	unsigned int inetPort = 0;
	WSADATA wsaData;
	int pingToServer;
	int sslError;
	WORD wsaVersionRequested = MAKEWORD(2,2);
	SOCKET serverSocket = INVALID_SOCKET;
	SSL_CTX* sslCtx;
	SSL* ssl;
public:
	ClientSockets(std::wstring inetAddr, unsigned int inetPort);
	int socketSetupTcp(); // Tcp socket setup and binding
	int socketSendMessage(std::string message); // Send Message over TLS
	int socketRecieveMessage(std::vector<char> buffer);
	int getPingToServer();
};

