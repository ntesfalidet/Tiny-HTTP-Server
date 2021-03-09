/*
 * http_server.h
 *
 * Constants and extern declarations for http server
 *
 */

#ifndef HTTP_SERVER_H_
#define HTTP_SERVER_H_

#include <stdbool.h>
#include "properties.h"

/** maximum buffer size */
#define MAXBUF 256

/** web newline sequence */
#define CRLF "\r\n"

/** http server config properties */
struct http_server_conf {
	/** debug flag */
	bool debug;

	/** path to web content directory */
	const char *content_base;

	/** name of http server */
	const char* server_name;

	/** host name or IP address of http server */
	const char* server_host;

	/** port number of http server */
	int server_port;

	/** http response protocol */
	const char* server_protocol;
};

/**  external declaration of server config */
extern struct http_server_conf server;


#endif /* HTTP_SERVER_H */
