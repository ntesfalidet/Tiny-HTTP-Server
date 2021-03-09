/*
 * methods.c
 *
 * Functions that implement HTTP methods, including
 * GET, HEAD, PUT, POST, and DELETE.
 *
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <unistd.h>
#include <dirent.h>

#include "http_codes.h"
#include "http_methods.h"
#include "http_server.h"
#include "http_util.h"
#include "time_util.h"
#include "media_util.h"
#include "properties.h"
#include "string_util.h"
#include "file_util.h"

/**
 * This function is responsible for listing the contents of a directory as a formatted HTML page (extension of GET).
 * Since the specified path to the file is a directory (pathDir), the function generates the listing of the
 * directory as a temporary file and returns a FILE* type.
 * @param pathDir specified path to directory
 * @param uri the request URI
 * @return returns a temporary file stream of type FILE* representing the listing of directories
 */
static FILE* listing_directories(const char *pathDir, const char *uri) {
    // create temporary file
    FILE* directoriesListingTmpFile = tmpfile();
    // open path directory for read
    DIR* openPathDir = opendir(pathDir);

    if (directoriesListingTmpFile == NULL) {
        return NULL;
    }
    if (openPathDir == NULL) {
        fprintf(stderr, "Directory %s can't be opened for read.\n", openPathDir);
    }

    // outputs the first portion of the formatted html page (header portion)
    char* htmlFirst = "<html>\n"
                      "<head><title>index of %s</title></head>\n"
                      "<body>\n"
                      "<h1>Index of %s</h1>\n"
                      "<table>\n"
                      "<tr>\n"
                      "<th valign=\"top\"></th>\n"
                      "<th>Name</th>\n"
                      "<th>Last modified</th>\n"
                      "<th>Size</th>\n"
                      "<th>Description (file type)</th>\n"
                      "</tr>\n"
                      "<tr>\n"
                      "<td colspan=\"5\"><hr></td>\n"
                      "</tr>\n";

    fprintf(directoriesListingTmpFile, htmlFirst, uri, uri);

    struct dirent *dirEnt;
    //char *entryInPathDir[MAXBUF];

    char *dirEntName;
    char filepath[MAXPATHLEN];
    struct stat sb;
    char time[MAXBUF];

    while ((dirEnt = readdir(openPathDir)) != NULL) {
        // for current directory
        if (strcmp(dirEnt->d_name, ".") == 0) {
            continue;
        }

        // for parent directory
        if (strcmp(dirEnt->d_name, "..") == 0) {
            // if the directory is a root directory
            if (strcmp(uri, "/") == 0) {
                continue;
            }
            dirEntName = "Parent Directory";
        }

        else {
            dirEntName = dirEnt->d_name;
        }

        makeFilePath(pathDir, dirEnt->d_name, filepath);
        stat(filepath, &sb);
        milliTimeToShortHM_Date_Time(sb.st_mtim.tv_sec, time);

        // outputs the entries (entries of directory) portion of the formatted html page
        char* htmlEntry = "<tr>\n"
                          "<td></td>\n"
                          "<td><a href=\"%s\">%s</a></td>\n"
                          "<td align=\"right\">%s</td>\n"
                          "<td align=\"right\">%lu</td>\n"
                          "<td align=\"right\">%s</td>\n"
                          "<td></td>\n"
                          "</tr>\n";

        char entryLink[MAXBUF];
        char entryMode[MAXBUF];

        // if entry is a parent directory entry in a non-root directory
        if (strcmp(dirEnt->d_name, "..") == 0) {
            strcpy(entryLink, "../");
        }

        // if entry is not a parent directory entry (either a file entry or directory entry in a directory)
        else {
            strcpy(entryLink, dirEntName);
            if (S_ISDIR(sb.st_mode)) {
                strcat(entryLink, "/");
            }
        }

        if (S_ISDIR(sb.st_mode)) {
            strcpy(entryMode, "directory");
        }

        if (S_ISREG(sb.st_mode)) {
            strcpy(entryMode, "file");
        }

        fprintf(directoriesListingTmpFile, htmlEntry, entryLink, dirEntName, time, (size_t)sb.st_size, entryMode);

        // outputs the last portion of the formatted html page (footer portion)
        char* htmlLast = "<tr><td colspan=\"5\"><hr></td></tr>\n"
                         "</body>\n"
                         "</html>\n";
        fprintf(directoriesListingTmpFile, "%s", htmlLast);
    }
    closedir(openPathDir);
    fflush(directoriesListingTmpFile);
    rewind(directoriesListingTmpFile);
    return directoriesListingTmpFile;
}



/**
 * Handle GET or HEAD request.
 *
 * @param stream the socket stream
 * @param uri the request URI
 * @param requestHeaders the request headers
 * @param responseHeaders the response headers
 * @param sendContent send content (GET)
 */
static void do_get_or_head(FILE *stream, const char *uri, Properties *requestHeaders, Properties *responseHeaders, bool sendContent) {
	// get path to URI in file system
	char filePath[MAXPATHLEN];
	resolveUri(uri, filePath);
	FILE *contentStream = NULL;

	// ensure file exists
	struct stat sb;
	if (stat(filePath, &sb) != 0) {
		sendStatusResponse(stream, Http_NotFound, NULL, responseHeaders);
		return;
	}
	// directory path ends with '/'
	if (S_ISDIR(sb.st_mode) && strendswith(filePath, "/")) {
		// not allowed for this method

		//sendStatusResponse(stream, Http_MethodNotAllowed, NULL, responseHeaders);
		contentStream = listing_directories(filePath, uri);
		if (contentStream == NULL) {
		    sendStatusResponse(stream, Http_MethodNotAllowed, NULL, responseHeaders);
		    return;
		}

		//return;
		fileStat(contentStream, &sb);
	}

	else if (!S_ISREG(sb.st_mode)) { // error if not regular file
		sendStatusResponse(stream, Http_NotFound, NULL, responseHeaders);
		return;
	}

	// record the file length
	char buf[MAXBUF];
	size_t contentLen = (size_t)sb.st_size;
	sprintf(buf,"%lu", contentLen);
	putProperty(responseHeaders,"Content-Length", buf);

	// record the last-modified date/time
	time_t timer = sb.st_mtim.tv_sec;
	putProperty(responseHeaders,"Last-Modified",
				milliTimeToRFC_1123_Date_Time(timer, buf));

	// get mime type of file
	getMediaType(filePath, buf);
	if (strcmp(buf, "text/directory") == 0) {
		// some browsers interpret text/directory as a VCF file
		strcpy(buf,"text/html");
	}
	putProperty(responseHeaders, "Content-type", buf);

	// send response
	sendResponseStatus(stream, Http_OK, NULL);

	// Send response headers
	sendResponseHeaders(stream, responseHeaders);

	if (sendContent) {  // for GET
	    if (contentStream == NULL) {
	        contentStream = fopen(filePath, "r");
	    }

		//contentStream = fopen(filePath, "r");
		copyFileStreamBytes(contentStream, stream, contentLen);
		fclose(contentStream);
	}
}

/**
 * Handle GET request.
 *
 * @param stream the socket stream
 * @param uri the request URI
 * @param requestHeaders the request headers
 * @param responseHeaders the response headers
 * @param headOnly only perform head operation
 */
void do_get(FILE *stream, const char *uri, Properties *requestHeaders, Properties *responseHeaders) {
	do_get_or_head(stream, uri, requestHeaders, responseHeaders, true);
}

/**
 * Handle HEAD request.
 *
 * @param the socket stream
 * @param uri the request URI
 * @param requestHeaders the request headers
 * @param responseHeaders the response headers
 */
void do_head(FILE *stream, const char *uri, Properties *requestHeaders, Properties *responseHeaders) {
	do_get_or_head(stream, uri, requestHeaders, responseHeaders, false);
}

/**
 * Handle DELETE request.
 *
 * @param the socket stream
 * @param uri the request URI
 * @param requestHeaders the request headers
 * @param responseHeaders the response headers
 */
void do_delete(FILE *stream, const char *uri, Properties *requestHeaders, Properties *responseHeaders) {
    // get path to URI in file system
    char filePath[MAXPATHLEN];
    resolveUri(uri, filePath);
    FILE *contentStream = NULL;
    // ensure file exists
    struct stat sb;
    if (stat(filePath, &sb) != 0) {
        sendStatusResponse(stream, Http_NotFound, NULL, responseHeaders);
        return;
    }

    // directory path ends with '/'
    if (S_ISDIR(sb.st_mode) && strendswith(filePath, "/")) {
        //allow deleting a non-empty directory
        // Determine size of the directory contents
        DIR *dir = opendir(filePath);
        int size = 0;
        struct dirent *ent;
        while ((ent = readdir(dir))) {
            if (!strcmp(ent->d_name, ".") || !(strcmp(ent->d_name, "..")))
                continue;
            size++;
            break;
        }
        closedir(dir);
        if (size != 0) {
            // not allowed for this method
            sendStatusResponse(stream, Http_MethodNotAllowed, NULL, responseHeaders);
            return;
        }
    } else if (!S_ISREG(sb.st_mode)) { // error if not regular file
        sendStatusResponse(stream, Http_NotFound, NULL, responseHeaders);
        return;
    }
    if (remove(filePath) == 0) {
        printf(stderr, "Deleted successfully\n");
        //sendResponseStatus(stream, Http_OK, NULL);
        sendStatusResponse(stream, Http_OK, NULL, responseHeaders);
    }
    else {
        sendStatusResponse(stream, Http_NotFound, NULL, responseHeaders);
    }
}

/**
 * Handle PUT request.
 * @param the socket stream
 * @param uri the request URI
 * @param requestHeaders the request headers
 * @param responseHeaders the response headers
 */
void do_put(FILE *stream, const char* uri, Properties *requestHeaders, Properties *responseHeaders){
    // get path to URI in file system
    char filePath[MAXPATHLEN];
    resolveUri(uri, filePath);

    // contentStream for the PUT operation where file stream bytes will be copied from
    // contentStream to stream
    FILE *contentStream = NULL;
    char buf[MAXBUF];

    // check content_length
    int contentLen;
    char val[MAXBUF];
    if (findProperty(requestHeaders, 0, "Content-Length", val) == SIZE_MAX) {
        sendStatusResponse(stream, Http_LengthRequired, NULL, responseHeaders);
        return;
    }
    contentLen = atoi(val);

    struct stat sb;

    // if our file exists
    if (stat(filePath, &sb) == 0) {
        // if the end of our file path to an existing file is a directory
        if (S_ISDIR(sb.st_mode) && strendswith(filePath, "/")) {
            // not allowed for this method
            sendStatusResponse(stream, Http_MethodNotAllowed, NULL, responseHeaders);
            return;
        }
        // if the end of our file path to an existing file is not a regular file
        else if (!S_ISREG(sb.st_mode)) { // error if not regular file
            sendStatusResponse(stream, Http_MethodNotAllowed, NULL, responseHeaders);
            return;
        }

        // write request body to the file
        contentStream = fopen(filePath, "w");
        // if the file cannot be opened
        if (contentStream == NULL) {
            sendStatusResponse(stream, Http_MethodNotAllowed, NULL, responseHeaders);
            return;
        }
        copyFileStreamBytes(stream, contentStream, contentLen);
        fclose(contentStream);
        sendStatusResponse(stream, Http_OK, NULL, responseHeaders);
    }

    // if our file does not exist
    else {
        char *pathOfFile = getPath(filePath, buf);
        // if getting the path to file is NULL
        if (pathOfFile == NULL) {
            sendStatusResponse(stream, Http_MethodNotAllowed, NULL, responseHeaders);
            return;
        }
        // if creating intermediate directories fails
        if (mkdirs(pathOfFile, 0777) != 0){
        //if (mkdirs(pathOfFile, sb.st_mode) < 0){
            sendStatusResponse(stream, Http_MethodNotAllowed, NULL, responseHeaders);
            return;
        }

        // write request body to the file
        contentStream = fopen(filePath, "w");
        // if the file cannot be opened
        if (contentStream == NULL) {
            sendStatusResponse(stream, Http_MethodNotAllowed, NULL, responseHeaders);
            return;
        }
        copyFileStreamBytes(stream, contentStream, contentLen);
        fclose(contentStream);
        putProperty(responseHeaders,"Location", filePath);
        sendStatusResponse(stream, Http_Created, NULL, responseHeaders);
    }
}

/**
 * Handle POST request.
 * @param the socket stream
 * @param uri the request URI
 * @param requestHeaders the request headers
 * @param responseHeaders the response headers
 */
void do_post(FILE *stream, const char* uri, Properties *requestHeaders, Properties *responseHeaders){
    // get path to URI in file system
    char collectionDirPath[MAXPATHLEN];
    resolveUri(uri, collectionDirPath);
    char filePath[MAXPATHLEN];

    // contentStream for the POST operation where file stream bytes will be copied from
    // contentStream to stream
    FILE *contentStream = NULL;
    char buf[MAXBUF];

    // check content_length
    int contentLen;
    char val[MAXBUF];
    if (findProperty(requestHeaders, 0, "Content-Length", val) == SIZE_MAX) {
        sendStatusResponse(stream, Http_LengthRequired, NULL, responseHeaders);
        return;
    }
    contentLen = atoi(val);

    // contentTypeString should hold the string to Content-type: eg. application/x-www-form-urlencoded,
    // multipart/form-data, text/plain, etc.
    char contentTypeString[MAXBUF];
    // this string will be appended to the end of a file with a unique name
    char extensionString[MAXBUF];
    findProperty(requestHeaders, 0, "Content-type", contentTypeString);

    // first 19 characters
    char multipartTypeString[MAXBUF];
    strncpy(multipartTypeString, contentTypeString, 19);
    multipartTypeString[19] = '\0'; // null terminating

    if (strcmp(contentTypeString, "application/x-www-form-urlencoded") == 0) {
        strcpy(extensionString, ".urlencoded");
    }
    else if (strcmp(multipartTypeString, "multipart/form-data") == 0) {
        strcpy(extensionString, ".mime");
    }
    else if (strcmp(contentTypeString, "text/plain") == 0) {
        strcpy(extensionString, ".txt");
    }
    // use the .bin extension for any other content type
    else {
        strcpy(extensionString, ".bin");
    }

    struct stat sb;

    // if the path to a collection directory exists
    if (stat(collectionDirPath, &sb) == 0) {
        // if the path to a collection directory is not a directory
        if (!S_ISDIR(sb.st_mode)) {
            // not allowed for this method
            sendStatusResponse(stream, Http_MethodNotAllowed, NULL, responseHeaders);
            return;
        }

        if (strendswith(collectionDirPath, "/")) {
            sendStatusResponse(stream, Http_MethodNotAllowed, NULL, responseHeaders);
            return;
        }

        strcat(collectionDirPath, "/");
        // creates a file (in the collection directory) with a unique name based on a template string
        // appends the appropriate extension (based on the Content-type)
        // at the end of the file
        strcpy(filePath, collectionDirPath);
        strcat(filePath, "XXXXXXXXXX");
        strcat(filePath, extensionString);
        mkstemps(filePath, strlen(extensionString));

        // write request body to the file
        contentStream = fopen(filePath, "w");
        // if the file cannot be opened
        if (contentStream == NULL) {
            sendStatusResponse(stream, Http_MethodNotAllowed, NULL, responseHeaders);
            return;
        }
        copyFileStreamBytes(stream, contentStream, contentLen);
        fclose(contentStream);
        putProperty(responseHeaders,"Location", filePath);
        sendStatusResponse(stream, Http_Created, NULL, responseHeaders);
    }

    // if the path to a collection directory does not exist
    else {
        if (strendswith(collectionDirPath, "/")) {
            sendStatusResponse(stream, Http_MethodNotAllowed, NULL, responseHeaders);
            return;
        }

        strcat(collectionDirPath, "/");
        // creates a file (in the collection directory) with a unique name based on a template string
        // appends the appropriate extension (based on the Content-type)
        // at the end of the file
        strcpy(filePath, collectionDirPath);
        strcat(filePath, "XXXXXXXXXX");
        strcat(filePath, extensionString);
        mkstemps(filePath, strlen(extensionString));

        char *pathOfFile = getPath(filePath, buf);
        // if getting the path to file is NULL
        if (pathOfFile == NULL) {
            sendStatusResponse(stream, Http_MethodNotAllowed, NULL, responseHeaders);
            return;
        }
        // if creating intermediate directories fails
        if (mkdirs(pathOfFile, 0777) != 0){
            sendStatusResponse(stream, Http_MethodNotAllowed, NULL, responseHeaders);
            return;
        }

        // write request body to the file
        contentStream = fopen(filePath, "w");
        // if the file cannot be opened
        if (contentStream == NULL) {
            sendStatusResponse(stream, Http_MethodNotAllowed, NULL, responseHeaders);
            return;
        }
        copyFileStreamBytes(stream, contentStream, contentLen);
        fclose(contentStream);
        putProperty(responseHeaders,"Location", filePath);
        sendStatusResponse(stream, Http_Created, NULL, responseHeaders);
    }
}
