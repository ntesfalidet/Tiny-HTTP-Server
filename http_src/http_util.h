/*
 * http_util.h
 *
 * Functions used to implement http operations.
 *
 */

#ifndef HTTP_UTIL_H_
#define HTTP_UTIL_H_

#include "properties.h"

/**
 * Reads request headers from request stream until empty line.
 *
 * @param istream the socket input stream
 * @param request headers
 * @throws IOException if error occurs
 */
void readRequestHeaders(FILE *istream, Properties *requestHeader);

/**
 * Send bytes for status to response output stream.
 *
 * @param status the response status
 * @param statusMsg the response message
 *   (NULL for default message)
 */
void sendResponseStatus(FILE *ostream, int status, const char *statusMsg);

/**
 * Send bytes for headers to response output stream.
 *
 * @param responseHeaders the header name value pairs
 * @param responseCharset the response charset
 * @throws IOException if errors occur
 */
void sendResponseHeaders(FILE *ostream, Properties *responseHeaders);

/**
 * Set error response and error page to the response output stream.
 *
 * @param ostream the output socket stream
 * @param status the response status
 * @param statusMsg the response message
 *   (NULL for default response message)
 * @param responseHeaders the response headers
 */
void sendStatusResponse(FILE* ostream, int status, const char *statusMsg, Properties *responseHeaders);

/**
 * Decode a URI string by replacing %xx with the
 * corresponding character code and '+' with " ".
 * @param escUrl the esc URI
 * @param uri the decoded URI
 * @return the URL if successful, NULL if error
 */
char *unescapeUri(const char *escUri, char *uri);

/**
 * Resolves server URI to file system path.
 * @param uri the request URI
 * @param fspath the file system path
 * @return the file system path
 */
char *resolveUri(const char *uri, char *fspath);

/**
 * Decode query string.
 *
 * @parma query the query string (e.g. a=b&c=d&e=f);
 * @param queryProps the query parameters
 */
void decodeQuery(const char *query, Properties *queryProps);

/**
 * Debug request by printing request and request headers
 *
 * @param request the request line
 * @param requestHeaders the request headers
 */
void debugRequest(const char *request, Properties *requestHeaders);

#endif /* HTTP_UTIL_H_ */
