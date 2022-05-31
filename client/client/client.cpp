//For Client
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WS2tcpip.h>
#include<iostream>
#pragma comment(lib,"ws2_32.lib")


using namespace std;

class client
{
public:
	CONST INT kDefaultServerPort = 54000;
	SOCKET EstablishConnection(const char* pcAddr, u_short nPort)
	{
		u_long nRemoteAddr = inet_addr(pcAddr);
		// Create a stream socket
		SOCKET sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sd != INVALID_SOCKET) {
			sockaddr_in sinRemote;
			sinRemote.sin_family = AF_INET;
			sinRemote.sin_addr.s_addr = nRemoteAddr;
			sinRemote.sin_port = nPort;
			if (connect(sd, (sockaddr*)&sinRemote, sizeof(sockaddr_in)) ==
				SOCKET_ERROR) {
				sd = INVALID_SOCKET;
			}
		}

		return sd;
	}
	void Send(SOCKET TCPClientSocket) {
		// STEP-6 Send Data to the server

		int iSend;
		string mess;
		cout << "Mess to server: " << endl;
		cin >> mess;
		char* SenderBuffer;
		int iSenderBuffer = mess.length();
		iSend = send(TCPClientSocket, mess.c_str(), iSenderBuffer, 0);
		if (iSend == SOCKET_ERROR)
		{
			cout << "Sending Failed & Error No->" << WSAGetLastError() << endl;
		}
		cout << "Data sending success" << endl;

	}
	void Recv(SOCKET TCPClientSocket) {
		// STEP -5 Recv Data from Server

		int iRecv;
		char* RecvBuffer;
		RecvBuffer = (char*)calloc(1, sizeof(char*));
		int iRecvBuffer = sizeof(RecvBuffer);
		do
		{
			iRecv = recv(TCPClientSocket, RecvBuffer, iRecvBuffer, 0);
			if (iRecv == SOCKET_ERROR)
			{
				cout << "Receiving Failed & Error No->" << WSAGetLastError() << endl;
			}
			cout << "Receive fun success" << endl;
			cout << "Data Received -> " << RecvBuffer << endl;
		} while (iRecv > 0);

	}

	void DoWinsock(const char* pcAddr, int nPort) {
		// Connect to the server

		SOCKET sd = EstablishConnection(pcAddr, htons(nPort));
		if (sd == INVALID_SOCKET) {
			cout << " cant connect to server" << endl;
			return;
		}
		cout << "connected, socket " << sd << "." << endl;
		Send(sd);
		Recv(sd);

	}
private:

};




INT main(INT argc, CHAR* argv[])
{
	client cl;
	// Do we have enough command line arguments?
	if (argc < 2) {
		cerr << "usage: " << argv[0] << " <server-address> " <<
			"[server-port]" << endl << endl;
		cerr << "\tIf you don't pass server-port, it defaults to " <<
			cl.kDefaultServerPort << "." << endl;
		return 1;
	}

	// Get host and (optionally) port from the command line
	CONST CHAR* pcHost = argv[1];
	INT nPort = cl.kDefaultServerPort;
	if (argc >= 3) {
		nPort = atoi(argv[2]);
	}

	// Start Winsock up
	WSAData wsaData;
	INT nCode;
	if ((nCode = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		cerr << "WSAStartup() returned error code " << nCode << "." << endl;
	}
	
	cl.DoWinsock(pcHost, nPort);

	
	// Shut Winsock back down and take off.
	WSACleanup();
}


