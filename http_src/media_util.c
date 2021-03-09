/*
 * media_util.c
 *
 * Functions for processing media types.
 *
 */

#include "media_util.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "string_util.h"
#include "http_server.h"
#include "file_util.h"
#include "properties.h"

/** default media type */
static const char *DEFAULT_MEDIA_TYPE = "application/octet-stream";
static Properties* contentTypes;

/**
 * Return a media type for a given filename.
 *
 * @param filename the name of the file
 * @param mediaType output buffer for mime type
 * @return pointer to media type string
 */
char *getMediaType(const char *filename, char *mediaType)
{
    // special-case directory based on trailing '/'
    if (filename[strlen(filename)-1] == '/') {
        strcpy(mediaType, "text/directory");
        return mediaType;
    }

    // get file extension
    char ext[MAXBUF];
    if (getExtension(filename, ext) == NULL) {
        // default if no extension
        strcpy(mediaType, DEFAULT_MEDIA_TYPE);
        return mediaType;
    }

    // lower-case extension
    strlower(ext, ext);

    strcpy(mediaType, DEFAULT_MEDIA_TYPE);

    findProperty(contentTypes, 0, ext, mediaType);

    return mediaType;
}

int readMediaTypes(const char *filename) {
    int size = 0;

    contentTypes = newProperties();

    fprintf(stderr, "Getting Properties\n");

    FILE* propStream = fopen(filename, "r");
    if (propStream == NULL) {
        return -1;
    }
    char buf[MAXBUF];

    // get next line
    while (fgets(buf, MAXBUF, propStream) != NULL) {
        if (buf[0] == '#') { // ignore comment
            continue;
        }

        char *valp = strchr(buf, '\t');

        if (valp != NULL) {
            *valp++ = '\0';

            // trim newline characters
            trim_newline(valp);

            char *ptr = strtok(valp, " ");
            while(ptr != NULL) {
                trim_newline(ptr);
                char* new = trim_tabs(ptr);

                // make property entry
                putProperty(contentTypes, new, buf);
                ptr = strtok(NULL, " ");
                size++;
            }
        }
    }
    fclose(propStream);
    return size;
}
