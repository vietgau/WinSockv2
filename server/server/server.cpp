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

CONST INT kDefaultServerPort = 54000;
// File
FILE* f;
CONST CHAR fileName[] = "C:\\Users\\vuhon\\OneDrive - actvn.edu.vn\\works\\Socket\\WinSockv2\\log.txt";


class server
{
public:
	SOCKET SetUpListener(CONST CHAR* pcAddress, INT nPort)
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
					cout << "bind() failed" << endl;
				}
			}
		}

		return INVALID_SOCKET;
	}
	//// AcceptConnections /////////////////////////////////////////////////
	// Spins forever waiting for connections.  For each one that comes in, 
	// we create a thread to handle it and go back to waiting for
	// connections.  If an error occurs, we return.
	 
	static DWORD WINAPI threadEchoHandler(VOID* sd_) {
		server* This = (server*)sd_;
		return This->EchoHandler(sd_);
	}
	 
	VOID AcceptConnections(SOCKET ListeningSocket)
	{
		sockaddr_in sinRemote;
		INT nAddrSize = sizeof(sinRemote);
		f = fopen(fileName, "a+");

		while (1) {

			SOCKET sd = accept(ListeningSocket, (sockaddr*)&sinRemote, &nAddrSize);
			CHAR ip[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &sinRemote.sin_addr, ip, INET_ADDRSTRLEN);
			INT port = ntohs(sinRemote.sin_port);
			if (sd != INVALID_SOCKET) {

				fprintf(f, "ip: %s, port: %d, ", ip, port);
				cout << "Accepted connection from " << ip << ":" << port << "." << endl;
				DWORD nThreadID;
				CreateThread(0, 0, threadEchoHandler, (VOID*)sd, 0, &nThreadID);
			}
			else {
				cout << "accept() failed" << endl;
				continue;
			}
		}
	}

	//// EchoIncomingPackets ///////////////////////////////////////////////
	// Bounces any incoming packets back to the client.  We return FALSE
	// on errors, or TRUE if the client closed the socket normally.

	BOOL RecvAndSend(SOCKET sd)
	{
		// Read data from client
		CHAR* ReadBuffer;
		ReadBuffer = (char*)calloc(1, sizeof(char*));
		INT kBufferSize = sizeof(ReadBuffer) + 1;
		INT nReadBytes;

		do {
			nReadBytes = recv(sd, ReadBuffer, kBufferSize, 0);
			if (nReadBytes > 0) {
				cout << ReadBuffer << " Received " << nReadBytes << " bytes from client." << endl;
				fprintf(f, "long: %d \n", nReadBytes);
				fclose(f);
				INT nSentBytes = 0;
				int iSend;
				string mess;
				cout << "Mess to client: " << endl;
				cin >> mess;
				char* SenderBuffer;
				int iSenderBuffer = mess.length();
				iSend = send(sd, mess.c_str(), iSenderBuffer, 0);
				if (iSend == SOCKET_ERROR)
				{
					cout << "Sending Failed & Error No->" << WSAGetLastError() << endl;
				}
				cout << "Data sending success" << endl;
			}
			else if (nReadBytes == SOCKET_ERROR) {
				return FALSE;
			}
		} while (nReadBytes != 0);

		cout << "Connection closed by peer." << endl;
		return TRUE;
	}


	//// EchoHandler ///////////////////////////////////////////////////////
	// Handles the incoming data by reflecting it back to the sender.

	DWORD WINAPI EchoHandler(VOID* sd_)
	{
		INT nRetval = 0;
		SOCKET sd = (SOCKET)sd_;

		if (!RecvAndSend(sd)) {
			cout << endl << "Echo incoming packets failed" << endl;
			nRetval = 3;
		}

		cout << "Shutting connection down..." << flush;
		if (ShutdownConnection(sd)) {
			cout << "Connection is down." << endl;
		}
		else {
			cout << endl << "Connection shutdown failed" << endl;
			nRetval = 3;
		}

		return nRetval;
	}

	//// ShutdownConnection ////////////////////////////////////////////////
	// Gracefully shuts the connection sd down.  Returns TRUE if we're
	// successful, FALSE otherwise.

	BOOL ShutdownConnection(SOCKET sd)
	{

		if (shutdown(sd, SD_SEND) == SOCKET_ERROR) {
			return FALSE;
		}
		// Close the socket.
		if (closesocket(sd) == SOCKET_ERROR) {
			return FALSE;
		}

		return TRUE;
	}

	//// DoWinsock /////////////////////////////////////////////////////////
	// The module's driver function -- we just call other functions and
	// interpret their results.

	INT DoWinsock(CONST CHAR* pcAddress, INT nPort)
	{
		cout << "Establishing the listener..." << endl;
		SOCKET ListeningSocket = SetUpListener(pcAddress, htons(nPort));
		if (ListeningSocket == INVALID_SOCKET) {
			cout << "establish listener" << endl;
			return 3;
		}

		cout << "Waiting for connections..." << flush;
		AcceptConnections(ListeningSocket);
		cout << "Acceptor restarting..." << endl;
	}


private:

};




//// SetUpListener /////////////////////////////////////////////////////
// Sets up a listener on the given interface and port, returning the
// listening socket if successful; if not, returns INVALID_SOCKET.


//main//

INT main(INT argc, CHAR* argv[])
{
	// Do we have enough command line arguments?
	server sv;
	if (argc < 2) {
		cerr << "usage: " << argv[0] << " <server-address> " <<
			"[server-port]" << endl << endl;
		cerr << "\tIf you don't pass server-port, it defaults to " <<
			kDefaultServerPort << "." << endl;
		return 1;
	}

	// Get host and (optionally) port from the command line
	CONST CHAR* pcHost = argv[1];
	INT nPort = kDefaultServerPort;
	if (argc >= 3) {
		nPort = atoi(argv[2]);
	}

	// Start Winsock up
	WSAData wsaData;
	INT nCode;
	if ((nCode = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		cerr << "WSAStartup() returned error code " << nCode << "." << endl;
	}
	
	sv.DoWinsock(pcHost, nPort);
	// Shut Winsock back down and take off.
	WSACleanup();
}
