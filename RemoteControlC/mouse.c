#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32

// Windows headers
#include <windows.h>

#elif __linux

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <fcntl.h>
#include <sys/time.h>

#endif

#include "mouse.h"

#ifdef _WIN32
void mouse_init() {
    // Windows mouse config does not require any initialisation
    return;
}

void mouse_cleanup() {
    // Windows mouse config requires no cleanup
    return;
}

void mouse_move(int x, int y) {
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

#elif __linux

static char * uidev_name = "uinput-mouse";
static int uinput_fd;

void mouse_init() {
    struct uinput_user_dev uidev;

    // Attempt to open the uinput device
    uinput_fd = open("/dev/uinput", O_WRONLY);
    if(uinput_fd < 0) {
        fprintf(stderr, "Error: %s (%i). Ensure that you have the uinput kernel module available at /dev/uinput, and that this is writable by the current user.\n",
                strerror(errno), errno);
        exit(EXIT_FAILURE);
    }

    // Prepare the uinput device descriptor
    memset(&uidev, 0, sizeof(uidev));
    strcpy(uidev.name, uidev_name);
    write(uinput_fd, &uidev, sizeof(uidev));

    // Tell uinput what events we want to support.
    // EV_SYN to mark the end of events
    ioctl(uinput_fd, UI_SET_EVBIT, EV_SYN);

    // EV_REL for relative axis events (mouse movement)
    ioctl(uinput_fd, UI_SET_EVBIT, EV_REL);
    // EV_KEY for mouse button, we need this for the OS to recognise the input as a mouse.
    ioctl(uinput_fd, UI_SET_EVBIT, EV_KEY);

    // We need to specify the relative axes we want to support, and the keys we support
    ioctl(uinput_fd, UI_SET_RELBIT, REL_X);
    ioctl(uinput_fd, UI_SET_RELBIT, REL_Y);
    ioctl(uinput_fd, UI_SET_KEYBIT, BTN_MOUSE);
    ioctl(uinput_fd, UI_SET_KEYBIT, BTN_LEFT);
    ioctl(uinput_fd, UI_SET_KEYBIT, BTN_RIGHT);

    // Request the creation of the device
    if(ioctl(uinput_fd, UI_DEV_CREATE, 0) < 0) {
        // Failed to create the device, error and exit
        fprintf(stderr, "Error: %s (%i). Coudln't create the input device.\n", strerror(errno), errno);
        close(uinput_fd);
        exit(EXIT_FAILURE);
    }
}

void mouse_cleanup() {
    ioctl(uinput_fd, UI_DEV_DESTROY);
    close(uinput_fd);
}

void mouse_move(int x, int y) {
    struct input_event event;

    memset(&event, 0, sizeof(event));
    gettimeofday(&event.time, NULL);

    event.type = EV_REL;
    event.code = REL_X;
    event.value = x;
    write(uinput_fd, &event, sizeof(event));

    event.type = EV_REL;
    event.code = REL_Y;
    event.value = y;
    write(uinput_fd, &event, sizeof(event));

    event.type = EV_SYN;
    event.code = SYN_REPORT;
    event.value = 0;
    write(uinput_fd, &event, sizeof(event));
}

void mouse_left_click() {
    struct input_event event;

    memset(&event, 0, sizeof(event));
    gettimeofday(&event.time, NULL);

    event.type = EV_KEY;
    event.code = BTN_LEFT;
    event.value = 1;
    write(uinput_fd, &event, sizeof(event));

    event.type = EV_KEY;
    event.code = BTN_LEFT;
    event.value = 0;
    write(uinput_fd, &event, sizeof(event));

    event.type = EV_SYN;
    event.code = SYN_REPORT;
    event.value = 0;
    write(uinput_fd, &event, sizeof(event));
}

void mouse_right_click() {
    struct input_event event;

    memset(&event, 0, sizeof(event));
    gettimeofday(&event.time, NULL);

    event.type = EV_KEY;
    event.code = BTN_RIGHT;
    event.value = 1;
    write(uinput_fd, &event, sizeof(event));

    event.type = EV_KEY;
    event.code = BTN_RIGHT;
    event.value = 0;
    write(uinput_fd, &event, sizeof(event));

    event.type = EV_SYN;
    event.code = SYN_REPORT;
    event.value = 0;
    write(uinput_fd, &event, sizeof(event));
}


#endif
