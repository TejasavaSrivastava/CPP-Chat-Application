#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<tchar.h>
#include<thread>
#include<vector>
using namespace std;

#pragma comment(lib, "ws2_32.lib")

/*
* Steps to write the sever code:
	1. Initializing winsock Library
	2. Create the socket
	3. Get the IP and port on which the application is running.
	4. Bind the Ip and port with the socket
	5. Start listening on the socket
	6. acccept the call
	7. Recieve and send
	8 Close the socket
	9. Clean up the winsock

*/
bool Initialize() {
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}

void InteractClient(SOCKET clientsocket, vector<SOCKET>& clients) {
	//send recieve:
	char buffer[2096];
	cout << "Client connected" << endl;
	while (true) {
		int bytesrecv = recv(clientsocket, buffer, sizeof(buffer), 0); //byte recved store the actual length of data which client has sent.
		string message(buffer, bytesrecv);
		if (bytesrecv <= 0) {
			cout << "Cleint Disconnected!!" << endl;
			break;
		}
		cout << "Message from client: " << message << endl;
		for (auto client : clients) {
			if(client!=clientsocket)
				send(client, message.c_str(), message.length(), 0);
		}
	}
	auto it = find(clients.begin(), clients.end(), clientsocket);
	if (it != clients.end())clients.erase(it);
	closesocket(clientsocket);
}

int main() {

	if (!Initialize()) {
		cout << "Intialize failed\n";
		return 1;
	}


	cout << "server program" << endl;

	//creating socket:

	SOCKET listensocket = socket(AF_INET, SOCK_STREAM, 0); //AF_INET is for IPv4 and AF_INET6 is for IPv6, SOCKSTREAM is used for TCP data0 lets the service provider decide the protocol

	// always check for the return of the data to ensure error free code while creating API

	if (listensocket == INVALID_SOCKET) {
		cout << "  socket creation failed" << endl;
		return 1;
	}

	// create address structure  which contain family, port

	sockaddr_in	serveraddr;
	serveraddr.sin_family = AF_INET;
	int port = 12345;
	serveraddr.sin_port = htons(port); //htons api converts the port number to network readable format htons means host to network short

	//Convert the IP address (0.0.0.0) put it inside the sin_family sturcture in binary format.
	if (InetPton(AF_INET, _T("0.0.0.0"), &serveraddr.sin_addr) != 1) {
		cout << "setting address structure failed" << endl;
		closesocket(listensocket);
		WSACleanup();
		return 1;
	}
	
	//binding the IP and port to socket:
	if (bind(listensocket, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {
		cout << "Bind failed" << endl;
		closesocket(listensocket);
		WSACleanup();
		return 1;
	}

	//listen on the socket:
	if (listen(listensocket, SOMAXCONN) == SOCKET_ERROR) {
		cout << "Error in listening on the socket" << endl;
		closesocket(listensocket);
		WSACleanup();
		return 1;
	}

	cout << "Server is listening on port: " << port << endl;
	vector<SOCKET> clients;
	//Start accepting the connection from the client:
	while (true) {
		SOCKET clientsocket = accept(listensocket, nullptr, nullptr); //client socket is specific to the client which has been conected on the server

		if (clientsocket == INVALID_SOCKET) {
			cout << "Invalid client socket created!" << endl;
		}
		clients.push_back(clientsocket);
		//recieving data form client:
		thread t1(InteractClient, clientsocket, std::ref(clients));
		t1.detach();
	}
	
	

	//Final cleanup!
	closesocket(listensocket);
	WSACleanup();
	return 0;
}