#pragma once
#ifndef __FUNCTION_H___
#define __FUNCTION_H___

typedef struct _message
{
	char m_buffer[100];
	int m_light_level;
	int m_hum;
	int m_temp;
} MESSAGE;

//#define DEBUG_MODE

#endif