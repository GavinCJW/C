#pragma once

#include"cjw.hpp"

#define SEND_LENGTH 1024
#define RCV_LENGTH 2048

class HTTP {
private:

public:
	inline HTTP() = delete;
	static std::string ReturnData(int type, char * ipAddr, char * host, int port, char * msg) {
		SOCKET mSocket;
		WSADATA mWsaData;
		SOCKADDR_IN mSockAddr;

		std::string sendBuf;
		std::string str;
		char *rcvBuf(new char[RCV_LENGTH]());

		WSAStartup(MAKEWORD(2, 1), &mWsaData);

		if ((mSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			_ERROR_(HTTP_SOKET_ERROR);

		mSockAddr.sin_family = AF_INET;
		mSockAddr.sin_port = htons(port);
		inet_pton(AF_INET, host, &(mSockAddr.sin_addr.S_un.S_addr));

		if ((connect(mSocket, (struct sockaddr *)&mSockAddr, sizeof(mSockAddr))) < 0)
			_ERROR_(HTTP_CONNECT_ERROR);

		if (type == 1) 
			sendBuf = "POST "+std::string(ipAddr)+" HTTP/1.1\r\n"
				"Host: "+ std::string(host) +":"+std::to_string(port)+"\r\n"
				"Content-Length: "+std::to_string(strlen(msg))+"\r\n"
				"\r\n"
				+std::string(msg);
		else if (type == 2) 
			sendBuf = "GET " + std::string(ipAddr) + "?" + std::string(msg) + " HTTP/1.1\r\n"
				"Host: " + std::string(host) + ":" + std::to_string(port) + "\r\n"
				"\r\n";

		if (send(mSocket, sendBuf.data(), SEND_LENGTH, 0) < 0)
			_ERROR_(HTTP_SEND_ERROR);

		while (recv(mSocket, rcvBuf, 2048, 0) != 0)
			str += rcvBuf;

		delete(rcvBuf);
		closesocket(mSocket);
		WSACleanup();

		return str;
	}

};