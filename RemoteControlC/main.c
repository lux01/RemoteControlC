#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#elif __linux
#include <unistd.h>
#endif

// We use the Mongoose server to provide a threaded HTTP server: https://github.com/valenok/mongoose
#include "mongoose.h"
#include "getopt.h"
#include "routes.h"
#include "mouse.h"
#include "network.h"

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

void generate_htpasswd(struct options * opts) {
	char *strBuff;
	char digest[33];
	int len;
	FILE *pFile;
    #ifdef _WIN32
	DWORD dwAttrib;
    #endif

    // If no password is specified, attempt to delete any existing
    // .htpasswd file, and then exit
	if(strlen(opts->password) == 0 || opts->disableAuth) {
	    #ifdef _WIN32
            dwAttrib = GetFileAttributes("resources\\.htpasswd");
			if( dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY) ) {
	    #elif __linux
            if(access( "resources/.htpasswd", F_OK) != -1) {
		#endif
                if(remove("resources/.htpasswd") != 0)
                    fprintf(stderr, "Failed to delete .htpasswd, please remove it by hand to disable password protection.\n");
            }
	    printf("\n");

	    return;
    }

	// Generate the digest to hash
	len = strlen(opts->username) + strlen(realm) + strlen(opts->password) + 2 + 1; // we need 2 : characters, plus null
	strBuff = (char*)malloc(len * sizeof(char));
	strcpy(strBuff, opts->username);
	strcat(strBuff, ":");
	strcat(strBuff, realm);
	strcat(strBuff, ":");
	strcat(strBuff, opts->password);

	// Calculate the digest
	mg_md5(digest, strBuff, NULL);

	// Clear the undigested string from memory
	free(strBuff);

	// Write the digest file
	pFile = fopen("resources/.htpasswd", "w");
	if(pFile != NULL) {
		len = strlen(opts->username) + strlen(realm) + 2 + 32 + 1; // 2 for the :, 32 for the md5 hash, 1 for null
		strBuff = (char*)malloc(len * sizeof(char));
		strcpy(strBuff, opts->username);
		strcat(strBuff, ":");
		strcat(strBuff, realm);
		strcat(strBuff, ":");
		strcat(strBuff, digest);

		fwrite(strBuff, sizeof(char), len-1, pFile);
		fwrite("\r\n", sizeof(char), 2, pFile);
		fclose(pFile);

		// Clear the digested string from memory
        free(strBuff);

		printf(".htpasswd generated successfully!\n\nUsername: \t\t%s\nPassword: \t\t%s\n", opts->username, opts->password);
	} else {
		printf("Failed to generate .htpasswd!\n");
		return;
	}
}

int main(int argc, char **argv) {
	struct mg_context *ctx;
	struct mg_callbacks callbacks;
	struct options opts;
	int parseRet;
	unsigned long passbuffSize = 52;

	const char *options[] = {
		"listening_ports", "8080",
		"num_threads", "1",
		"authentication_domain", realm,
		"document_root", "resources/",
		NULL
	};

	// Parse command-line arguments
    set_default_opts(&opts);
    parseRet = parse_opts(argc, argv, &opts);
    // Initialise the mouse handling code
    mouse_init();

    // Print the system IPs
    print_system_ips();

	// Prompt the user for a password if necessary.
	if(!(parseRet & P_PASS || parseRet & P_NOAU)) {
        opts.password = (char *) malloc(passbuffSize * sizeof(char));
        printf("Please enter password (%lu characters max., leave blank for none): ",
            passbuffSize-2); // 2 for \n\0
        my_gets(opts.password, passbuffSize);
    }
    // Generate (or remove) the .htpasswd file.
    generate_htpasswd(&opts);

    // Prepare mongoose
	memset(&callbacks, 0, sizeof(callbacks));
	callbacks.begin_request = begin_request_handler;
    options[1] = opts.ports;

	// Start Mongoose
	ctx = mg_start(&callbacks, NULL, options);
	printf("Listening ports: \t%s\n\n", mg_get_option(ctx, "listening_ports"));

	printf("Press <ENTER> to terminate.\n");

    // Wait for an enter to exit.
	getchar();

    // Star cleaning up
	printf("Exiting...\n");
	mg_stop(ctx);
	mouse_cleanup();

	return EXIT_SUCCESS;
}
