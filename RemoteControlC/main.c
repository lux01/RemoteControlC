#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Windows libraries
#include <winsock2.h>

// We use the Mongoose server to provide a threaded HTTP server: https://github.com/valenok/mongoose
#include "mongoose.h"

#include "routes.h"
#include "mouse.h"

const char* username = "user\0";
const char* realm = "RemoteControlC\0";

size_t my_gets(char *buff, size_t size){
    size_t result = 0;
    if(NULL != fgets(buff, size, stdin)) {
        char *nlPtr = strchr(buff, '\n');
        if(nlPtr) *nlPtr = '\0';
        result = strlen(buff);
    }
    return result;
}

void generate_htpasswd(char* password) {
	char *strBuff;
	char digest[33];
	int len;
	FILE *pFile;

	if(strlen(password) == 0) return;

	// Generate the digest to hash
	len = strlen(username) + strlen(realm) + strlen(password) + 2 + 1; // we need 2 : characters, plus null
	strBuff = (char*)malloc(len * sizeof(char));
	strcpy(strBuff, username);
	strcat(strBuff, ":");
	strcat(strBuff, realm);
	strcat(strBuff, ":");
	strcat(strBuff, password);

	// Calculate the digest
	mg_md5(digest, strBuff, NULL);

	// Clear the undigested string from memory
	free(strBuff);

	// Write the digest file
	pFile = fopen("resources/.htpasswd", "w");
	if(pFile != NULL) {
		len = strlen(username) + strlen(realm) + 2 + 32 + 1; // 2 for the :, 32 for the md5 hash, 1 for null
		strBuff = (char*)malloc(len * sizeof(char));
		strcpy(strBuff, username);
		strcat(strBuff, ":");
		strcat(strBuff, realm);
		strcat(strBuff, ":");
		strcat(strBuff, digest);

		fwrite(strBuff, sizeof(char), len-1, pFile);
		fwrite("\r\n", sizeof(char), 2, pFile);
		fclose(pFile);

		// Clear the digested string from memory
        free(strBuff);

		printf(".htpasswd generated successfully!\n\nUsername: \t\t%s\nPassword: \t\t%s\n", username, password);
	} else {
		printf("Failed to generate .htpasswd!\n");
		return;
	}
}

// Attempt to deduce the IP address of the server
char* deduce_ip() {
	char ac[80];
	struct hostent *phe;
	int i;

	if(gethostname(ac, sizeof(ac)) == SOCKET_ERROR) {
		printf("Error %i when getting local host name.\n", WSAGetLastError());
		return NULL;
	}

	phe = gethostbyname(ac);
	if(phe == 0) {
		printf("Bad host name lookup.\n");
		return NULL;
	}

	for(i = 0; phe->h_addr_list[i] != 0; i++) {
		struct in_addr addr;
		memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
		return inet_ntoa(addr);
	}

	return NULL;
}

int main(void) {
	struct mg_context *ctx;
	struct mg_callbacks callbacks;
	struct WSAData wsaData;
	char *localIp;
	char passwdBuff[52];

	const char *options[] = {
		"listening_ports", "8080",
		"num_threads", "1",
		"authentication_domain", realm,
		"document_root", "resources/",
		NULL
	};

	memset(&callbacks, 0, sizeof(callbacks));
	callbacks.begin_request = begin_request_handler;

	// Attempt to deduce the local IP
	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return -1;
	localIp = deduce_ip();
	WSACleanup();

	if(localIp != NULL) {
		printf("Starting remote control server on IP\n"
          "+------------------+\n"
          "| %-16s |\n"
          "+------------------+\n", localIp);
	}

	// Generate the htpasswd file
	printf("Please enter password (%i characters max., leave blank for none): ",
        (sizeof passwdBuff)-2); // 2 for \n\0
	my_gets(passwdBuff, sizeof passwdBuff);
	generate_htpasswd(passwdBuff);

	// Start Mongoose
	ctx = mg_start(&callbacks, NULL, options);
	printf("Listening ports: \t%s\n\n", mg_get_option(ctx, "listening_ports"));

	printf("Press <ENTER> to terminate.\n");

	getchar();

	printf("Exiting...\n");
	mg_stop(ctx);

	return EXIT_SUCCESS;
}
