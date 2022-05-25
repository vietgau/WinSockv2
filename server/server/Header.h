#pragma once
#include<iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <process.h>
SOCKET SetUpListener(CONST CHAR* pcAddress, int nPort);
VOID AcceptConnections(SOCKET ListeningSocket);
BOOL EchoIncomingPackets(SOCKET sd);
DWORD WINAPI EchoHandler(VOID* sd_);
BOOL ShutdownConnection(SOCKET sd);
INT DoWinsock(CONST CHAR* pcAddress, INT nPort);