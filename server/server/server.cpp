//For Server
//#include<WinSock2.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
#include<iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <process.h>
#include "Header.h"
#pragma comment(lib,"ws2_32.lib")


using namespace std;

const int kBufferSize = 1024;

// Default port, host to connect to on the server
const int kDefaultServerPort = 54000;
// File
FILE* f;
const char fileName[] = "C:\\Users\\vuhon\\OneDrive - actvn.edu.vn\\works\\Socket\\WinSocket\\log.txt";

//// SetUpListener /////////////////////////////////////////////////////
// Sets up a listener on the given interface and port, returning the
// listening socket if successful; if not, returns INVALID_SOCKET.

SOCKET SetUpListener(const char* pcAddress, int nPort)
{
	
	u_long nInterfaceAddr = inet_addr(pcAddress);

	if (nInterfaceAddr != INADDR_NONE) {
		SOCKET sd = socket(AF_INET, SOCK_STREAM, 0);
		if (sd != INVALID_SOCKET) {
			sockaddr_in sinInterface;
			sinInterface.sin_family = AF_INET;
			sinInterface.sin_addr.S_un.S_addr = nInterfaceAddr;
			sinInterface.sin_port = nPort;
			if (bind(sd, (sockaddr*)&sinInterface, sizeof(sockaddr_in)) != SOCKET_ERROR) {
				cout << "Binding success" << endl;
				listen(sd, SOMAXCONN);
				cout << "Listening" << endl;
				return sd;
			}
			else {
				cout <<"bind() failed" << endl;
			}
		}
	}

	return INVALID_SOCKET;
}
//// AcceptConnections /////////////////////////////////////////////////
// Spins forever waiting for connections.  For each one that comes in, 
// we create a thread to handle it and go back to waiting for
// connections.  If an error occurs, we return.

void AcceptConnections(SOCKET ListeningSocket)
{
	sockaddr_in sinRemote;
	int nAddrSize = sizeof(sinRemote);
	
	while (1) {
		SOCKET sd = accept(ListeningSocket, (sockaddr*)&sinRemote, &nAddrSize);
		char ip[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &sinRemote.sin_addr, ip, INET_ADDRSTRLEN);
		int port = ntohs(sinRemote.sin_port);
		if (sd != INVALID_SOCKET) {
			
			fprintf(f, "ip: %s, port: %d, ", ip, port);
			cout << "Accepted connection from " << ip << ":" << port << "." << endl;
			DWORD nThreadID;
			CreateThread(0, 0, EchoHandler, (void*)sd, 0, &nThreadID);
		}
		else {
			cout << "accept() failed" << endl;
			return;
		}
	}
}

//// EchoIncomingPackets ///////////////////////////////////////////////
// Bounces any incoming packets back to the client.  We return false
// on errors, or true if the client closed the socket normally.

bool EchoIncomingPackets(SOCKET sd)
{
	// Read data from client
	char acReadBuffer [kBufferSize];
	int nReadBytes;
	
	do {
		nReadBytes = recv(sd, acReadBuffer, kBufferSize, 0);
		if (nReadBytes > 0) {
			cout << acReadBuffer << "Received " << nReadBytes << " bytes from client." << endl;
			fprintf(f, "long: %d \n", nReadBytes);
			fclose(f);
			int nSentBytes = 0;
			while (nSentBytes < nReadBytes) {
				int nTemp = send(sd, acReadBuffer + nSentBytes,
					nReadBytes - nSentBytes, 0);
				if (nTemp > 0) {
					cout << "Sent " << nTemp << " bytes back to client." << endl;
					nSentBytes += nTemp;
				}
				else if (nTemp == SOCKET_ERROR) {
					return false;
				}
				else {
					// Client closed connection before we could reply to
					// all the data it sent, so bomb out early.
					cout << "Peer unexpectedly dropped connection!" << endl;
					return true;
				}
			}
		}
		else if (nReadBytes == SOCKET_ERROR) {
			return false;
		}
	} while (nReadBytes != 0);

	cout << "Connection closed by peer." << endl;
	return true;
}


//// EchoHandler ///////////////////////////////////////////////////////
// Handles the incoming data by reflecting it back to the sender.

DWORD WINAPI EchoHandler(void* sd_)
{
	int nRetval = 0;
	SOCKET sd = (SOCKET)sd_;

	if (!EchoIncomingPackets(sd)) {
		cout << endl <<"Echo incoming packets failed" << endl;
		nRetval = 3;
	}

	cout << "Shutting connection down..." << flush;
	if (ShutdownConnection(sd)) {
		cout << "Connection is down." << endl;
	}
	else {
		cout << endl << "Connection shutdown failed"<< endl;
		nRetval = 3;
	}

	return nRetval;
}

//// ShutdownConnection ////////////////////////////////////////////////
// Gracefully shuts the connection sd down.  Returns true if we're
// successful, false otherwise.

bool ShutdownConnection(SOCKET sd)
{
	
	if (shutdown(sd, SD_SEND) == SOCKET_ERROR) {
		return false;
	}
	// Close the socket.
	if (closesocket(sd) == SOCKET_ERROR) {
		return false;
	}

	return true;
}

//// DoWinsock /////////////////////////////////////////////////////////
// The module's driver function -- we just call other functions and
// interpret their results.

int DoWinsock(const char* pcAddress, int nPort)
{
	cout << "Establishing the listener..." << endl;
	SOCKET ListeningSocket = SetUpListener(pcAddress, htons(nPort));
	if (ListeningSocket == INVALID_SOCKET) {
		cout << "establish listener" << endl;
		return 3;
	}

	cout << "Waiting for connections..." << flush;
	while (1) {
		AcceptConnections(ListeningSocket);
		cout << "Acceptor restarting..." << endl;
	}
}

//main//

int main(int argc, char* argv[])
{
	// Do we have enough command line arguments?
	if (argc < 2) {
		cerr << "usage: " << argv[0] << " <server-address> " <<
			"[server-port]" << endl << endl;
		cerr << "\tIf you don't pass server-port, it defaults to " <<
			kDefaultServerPort << "." << endl;
		return 1;
	}

	// Get host and (optionally) port from the command line
	const char* pcHost = argv[1];
	int nPort = kDefaultServerPort;
	if (argc >= 3) {
		nPort = atoi(argv[2]);
	}

	// Start Winsock up
	WSAData wsaData;
	int nCode;
	if ((nCode = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		cerr << "WSAStartup() returned error code " << nCode << "." << endl;
	}
	f = fopen(fileName, "a+");
	DoWinsock(pcHost, nPort);
	// Shut Winsock back down and take off.
	WSACleanup();
}
