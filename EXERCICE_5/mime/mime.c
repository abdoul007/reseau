#include <string.h>
#include <ctype.h>
#include "mime.h"

#define DEFAULT_MIME_TYPE "application/octet-stream"


char *mime_type_get(char *filename)
{
    char *ext = strrchr(filename, '.');

    if (ext == NULL) {
        return DEFAULT_MIME_TYPE;
    }
    
    ext++;

	char * buffer  = malloc(sizeof(char*30));
	
	buffer = DEFAULT_MIME_TYPE;

    if (strcmp(ext, "html") == 0) 
    { 
		buffer = DEFAULT_MIME_TYPE; 
	}
	
    if (strcmp(ext, "jpeg") == 0 )
    { 
		buffer = DEFAULT_MIME_TYPE; 
	}
   
    if (strcmp(ext, "txt") == 0) 
    { 
		buffer = DEFAULT_MIME_TYPE; 
	}
    if (strcmp(ext, "gif") == 0) 
    { 
		buffer = DEFAULT_MIME_TYPE; 
	}
	
    if (strcmp(ext, "png") == 0) 
    { 
		buffer = DEFAULT_MIME_TYPE; 
	}

    return buffer;
}
