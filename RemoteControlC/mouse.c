#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "mouse.h"

void mouse_move(long x, long y) {
	POINT p;
	long absX = 0;
	long absY = 0;
	GetCursorPos(&p);

	absX = (long) (p.x + x) * 65535.0f / GetSystemMetrics(SM_CXSCREEN);
	absY = (long) (p.y + y) * 65535.0f / GetSystemMetrics(SM_CYSCREEN);

	mouse_event( MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE,
		absX,
		absY,
		0,
		0);

}

void mouse_left_click() {
	mouse_event( MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

void mouse_right_click() {
	mouse_event( MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
}