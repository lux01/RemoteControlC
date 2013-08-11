#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Windows headers
#ifdef _WIN32
#include <windows.h>

// Windows sucks
#define snprintf _snprintf

#endif

// Mongoose headers
#include "mongoose.h"

#include "mouse.h"
#include "routes.h"

const char *customRoutes[] = {
	"POST /move",
	"POST /left_click",
	"POST /right_click"
};

int (*routeHandlers[]) (struct mg_connection *conn, const struct mg_request_info *ri) = {
	get_move,
	get_left_click,
	get_right_click
};

const int numRoutes = 3;

// Handle a GET mouse move
int get_move(struct mg_connection *conn, const struct mg_request_info *ri) {
	const char* failMessage = "Requires query string of the form `?x=XXX&y=YYY' where XXX and YYY denote integers.";
	const char* successMessage = "ok";
	long x = 0, y = 0;
	int numParsed = 0;

	// If we have a query string, continue
	if(ri->query_string) {

		if((numParsed = sscanf(ri->query_string, "x=%d&y=%d", &x, &y)) == 2) {
			// We're guaranteed to have parsed x and y successfully
			mouse_move(x, y);
			mg_printf(conn,
				"HTTP/1.1 200 OK\r\n"
				"Content-Type: text/plain; charset=utf-8\r\n"
				"Content-Length: %lu\r\n"
				"\r\n"
				"%s",
				strlen(successMessage),
				successMessage);
			return 1;
		}
	}

	mg_printf(conn,
		"HTTP/1.1 400 Bad Request\r\n"
		"Content-Type: text/plain; charset=utf-8\r\n"
		"Content-Length: %lu\r\n"
		"\r\n"
		"%s",
		strlen(failMessage),
		failMessage);
	return 1;
}

int get_left_click(struct mg_connection *conn, const struct mg_request_info *ri) {
	const char* successMessage = "ok";
	mouse_left_click();
	mg_printf(conn,
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/plain; charset=utf-8\r\n"
		"Content-Length: %lu\r\n"
		"\r\n"
		"%s",
		strlen(successMessage),
		successMessage);
	return 1;
}

int get_right_click(struct mg_connection *conn, const struct mg_request_info *ri) {
	const char* successMessage = "ok";
	mouse_right_click();
	mg_printf(conn,
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/plain; charset=utf-8\r\n"
		"Content-Length: %lu\r\n"
		"\r\n"
		"%s",
		strlen(successMessage),
		successMessage);
	return 1;
}

int begin_request_handler(struct mg_connection *conn) {
	const struct mg_request_info *ri = mg_get_request_info(conn);
	time_t timer;
	char timeBuffer[50], routeBuffer[2048];
	struct tm* tm_info;
	int i;

	time(&timer);
	tm_info = localtime(&timer);
	strftime(timeBuffer, 50, "%Y-%m-%d %H:%M:%S", tm_info);

	// Attempt to serve the requested URL as a local file
	printf("[%s] Requested: %s\n", timeBuffer, ri->uri);

	// Check to see if we have a custom route for the requested method
	snprintf(routeBuffer, 2048, "%s %s", ri->request_method, ri->uri);
	for(i = 0; i < numRoutes; i++) {
		if( strcmp( routeBuffer, customRoutes[i] ) == 0 ) {
			return (*routeHandlers[i])(conn, ri);
		}
	}

	return 0;
}
