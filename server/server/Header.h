#pragma once
#include<iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <process.h>
SOCKET SetUpListener(const char* pcAddress, int nPort);
void AcceptConnections(SOCKET ListeningSocket);
bool EchoIncomingPackets(SOCKET sd);
DWORD WINAPI EchoHandler(void* sd_);
bool ShutdownConnection(SOCKET sd);
int DoWinsock(const char* pcAddress, int nPort);