#pragma once
#ifndef __SMART_H__
#define __SMART_H__
//#pragma comment (lib, "ws2_32.lib");
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
//#include <WinSock2.h>
//#include <Windows.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "function.h"

#define RCVBUFSIZE 32
#define ENDMSG "@END Message"
#define LOGOUT "quit"
#define CLTEND 0x05
#define OUTMSG "OUT"

#define MAXPENDING 5 // 최대 허용 대기 가능 인원
#define MAXUSER 3 // 최대 허용 접속 가능 인원.

#endif

