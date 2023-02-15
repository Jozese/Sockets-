#include "ClientSockets.h"

ClientSockets::ClientSockets(std::wstring inetAddr, unsigned int inetPort): inetAddr(inetAddr), inetPort(inetPort) {
	SSL_library_init();
	sslCtx = SSL_CTX_new(TLS_client_method());
	ssl = SSL_new(sslCtx);
	if (WSAStartup(wsaVersionRequested, &wsaData) != 0) {
		std::cout << "Winsock dll was not found! " << WSAGetLastError() << std::endl;
		WSACleanup(); // Unload dll
		
	}
	else { std::cout << "Winsock dll was found! " << "Status: " << wsaData.szSystemStatus << std::endl; }
}

int ClientSockets::socketSetupTcp()
{
	serverSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP); // TCP socket
	if (serverSocket == INVALID_SOCKET) {
		std::cout << "Error trying to set up TCP socket. " << WSAGetLastError() << std::endl;
		closesocket(serverSocket); // Terminate socket
		WSACleanup();
		return 0;
	}
	std::cout << "Socket set up successfully! " << std::endl;

	// Binding Ip and port
	sockaddr_in service;
	service.sin_family = AF_INET;
	InetPton(AF_INET, inetAddr.c_str(), &service.sin_addr.s_addr);
	service.sin_port = htons(inetPort);
	auto start = std::chrono::high_resolution_clock::now();
	if (connect(serverSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
		std::cout << "Failed to connect to server! " << WSAGetLastError() << std::endl;
		closesocket(serverSocket); 
		WSACleanup();
		return 0;
	}
	auto end = std::chrono::high_resolution_clock::now();
	pingToServer = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	std::cout << "Connected to remote host in " << pingToServer << "ms Ready to send and recieve data." << std::endl;

	return 0;
}

int ClientSockets::socketSendMessage(std::string message) {
	
	SSL_set_fd(ssl, serverSocket);
	if (SSL_connect(ssl) != 1) {
		std::cout << "Failed to establish ssl handshake." << SSL_get_error(ssl, sslError);
		closesocket(serverSocket);
		WSACleanup();
		return 0;
	}
	std::cout << "Ssl handshake successfully established." << std::endl;

	
	if (SSL_write(ssl, message.c_str(), message.length()) <= 0) {
		std::cout << "Failed sending message!" << SSL_get_error(ssl, sslError) << std::endl;
		SSL_shutdown(ssl);
		SSL_free(ssl);
		closesocket(serverSocket);
		WSACleanup();
		return 0;
	}
	std::cout << "OUT --> " << message << std::endl;
	return 1;
}

int ClientSockets::socketRecieveMessage(std::vector<char> buffer)
{
	int numBytes = 0;
	while (true)
	{
		sslError = SSL_read(ssl, &buffer[numBytes], buffer.size());
		if (numBytes >= sizeof(buffer)) {
			std::cout << "Received " << numBytes << " bytes. " << std::endl;
			break;
		}
		numBytes += sslError;
	}
	std::cout << "IN <-- ";
	for (int i = 0; i < numBytes; i++) {
		std::cout << buffer[i];
	}
	buffer.erase(buffer.begin(),buffer.end()); //Clear buffer after showing message without resizing

	return 1;
}

int ClientSockets::getPingToServer()
{
	return this->pingToServer;
}
