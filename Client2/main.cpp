#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<thread>
#include<string>

using namespace std;

#pragma comment(lib, "ws2_32.lib");

/*
To create client code the following steps needs to be followed:
	1. Initialize winsock
	2. Create the socket
	3. connect the sserver
	4. send and recieve the data
	5. close the ssocket.

*/
bool Initialize() {
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}

void SendMessageonChat(SOCKET s) {
	cout << "Enter Your  UserName: ";
	string name;
	getline(cin, name);
	string message;

	while (1) {
		getline(cin, message);
		string msg = name + " : " + message;
		int bytesent = send(s, msg.c_str(), msg.length(), 0);
		if (bytesent == SOCKET_ERROR) {
			cout << "Error sending messsage" << endl;
			break;
		}
		if (message == "quit") {
			cout << "Disconnecting from the chat!" << endl;
			break;
		}
	}
	closesocket(s);
	WSACleanup();
}

void RecieveMessage(SOCKET s) {
	char buffer[4096];
	int recvlen;
	while (1) {
		recvlen = recv(s, buffer, sizeof(buffer), 0);
		if (recvlen <= 0) {
			cout << "Disconnected from the server!" << endl;
			break;
		}
		else {
			string message(buffer, recvlen);
			cout << message << endl;
		}
	}
	closesocket(s);
	WSACleanup();
}

int main() {
	cout << "Client program started!" << endl;

	if (!Initialize()) {
		cout << "Initializing winsock failed" << endl;
		return 1;
	}

	//creating socket;
	SOCKET s;
	s = socket(AF_INET, SOCK_STREAM, 0);

	if (s == INVALID_SOCKET) {
		cout << "Socket creation failed" << endl;
		return 1;
	}
	string serverip = "127.0.0.1";
	int port = 12345;
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	inet_pton(AF_INET, serverip.c_str(), &serveraddr.sin_addr);


	//connecting to the server:

	if (connect(s, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {
		cout << "Server connection failed" << endl;
		cout << "Error: " << WSAGetLastError() << endl;
		closesocket(s);
		WSACleanup();
		return 1;
	}

	cout << "Successfully connected to the server!" << endl;

	//send and recieve:

	thread senderthread(SendMessageonChat, s);
	thread recieverthread(RecieveMessage, s);

	senderthread.join();
	recieverthread.join();

	return 0;
}
