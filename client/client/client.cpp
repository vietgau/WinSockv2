//For Client
#include <WS2tcpip.h>
#include<iostream>
#pragma comment(lib,"ws2_32.lib")

using namespace std;

static SOCKET TCPClientSocket;
void Connect() {

	//Step-1 WSAStartup Fun------------------------------------

	WSADATA Winsockdata;
	int iWsaStartup;
	int iWsaCleanup;
	iWsaStartup = WSAStartup(MAKEWORD(2, 2), &Winsockdata);
	if (iWsaStartup != 0)
	{
		cout << "WSAStartup Failed" << endl;
	}
	cout << "WSAStartup Success" << endl;

	//Step -2 Socket Creation------------------------------------


	TCPClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (TCPClientSocket == INVALID_SOCKET)
	{
		cout << "TCP Client Socket Creation failed" << WSAGetLastError() << endl;
	}
	cout << "TCP client socket creation success";
	// STEP -3 Fill the structure-------------------------------

	struct sockaddr_in TCPServerAdd;
	string ipAddress = "127.0.0.1";			// IP Address of the server
	TCPServerAdd.sin_family = AF_INET;
	TCPServerAdd.sin_port = htons(54000);
	inet_pton(AF_INET, ipAddress.c_str(), &TCPServerAdd.sin_addr);
	
	// STEP-4 Connect fun---------------------------------------------

	int iConnect;
	iConnect = connect(TCPClientSocket, (SOCKADDR*)&TCPServerAdd, sizeof(TCPServerAdd));
	if (iConnect == SOCKET_ERROR)
	{
		cout << "Connection failed & Error No ->" << WSAGetLastError() << endl;
	}
	cout << "Connection success" << endl;
}
void Recv() {
	// STEP -5 Recv Data from Server

	int iRecv;
	char* RecvBuffer;
	RecvBuffer = (char*)(malloc(1 * sizeof(char)));
	int iRecvBuffer = strlen(RecvBuffer) + 1;
	iRecv = recv(TCPClientSocket, RecvBuffer, iRecvBuffer, 0);
	if (iRecv == SOCKET_ERROR)
	{
		cout << "Receiving Failed & Error No->" << WSAGetLastError() << endl;
	}
	cout << "Receive fun success" << endl;
	cout << "Data Received -> " << RecvBuffer << endl;
}
void Send() {
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
BOOL Disconnect() {
	//STEP - 7 Close Socket
	if (FALSE)
	{
		int iCloseSocket;
		iCloseSocket = closesocket(TCPClientSocket);
		if (iCloseSocket == SOCKET_ERROR)
		{
			cout << "Closing Failed & Error No->" << WSAGetLastError() << endl;
		}
		cout << "Closing Socket success" << endl;
		return FALSE;
	}
	return TRUE;


	system("PAUSE");
}
int main()
{


	cout << "\t\t------TCP Client-------" << endl;
	cout << endl;
	Connect();
	while (Disconnect())
	{
		
		Send();
		Recv();
	}
	
}