#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include "csvmonkey.h"

long long ustime() {
    struct timeval tv;
    long long ust;

    gettimeofday(&tv, NULL);
    ust = ((long long)tv.tv_sec)*1000000;
    ust += tv.tv_usec;
    return ust;
}

ssize_t readCallback(void *context, void *buffer, size_t nbytes) {
    return fread(buffer, 1, nbytes, (FILE*)context);
}

const char *toCursorTypeString(csvCursorType type) {
    switch(type) {
        case CSV_MAPPED:
            return "mapped";
        case CSV_BUFFERED:
            return "buffered";
        case CSV_CALLBACK:
            return "callback";
        default:
            fprintf(stderr, "Error:  Unknown cursor type!\n");
            exit(-1);
    }
}

csvCursorType getCursorType(const char *input) {
    if (!strncmp(input, "mapped", sizeof("mapped")-1)) {
        return CSV_MAPPED;
    } else if (!strncmp(input, "buffered", sizeof("buffered")-1)) {
        return CSV_BUFFERED;
    } else if (!strncmp(input, "callback", sizeof("callback")-1)) {
        return CSV_CALLBACK;
    }

    fprintf(stderr, "Error:  Invalid type (valid: 'mapped', 'buffered', 'callback')\n");
    exit(-1);
}

int main(int argc, const char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage:  %s <filename> <cursor type>\n", argv[0]);
        fprintf(stderr, "  -- Valid types: 'mapped', 'buffered', or 'callback'\n");
        exit(-1);
    }

    const char *file = argv[1];
    unsigned long long st, et;
    csvCursorType type = getCursorType(argv[2]);
    FILE *fp = NULL;
    size_t rows = 0, totlen = 0;
    csvContext *context;
    csvRow row;

    if (type == CSV_MAPPED || type == CSV_BUFFERED) {
        context = csv_open_file(file, type, NULL);
    } else {
        fp = fopen(file, "r");
        if (fp == NULL) {
            fprintf(stderr, "Error:  Cannot open file '%s'\n", file);
            exit(-1);
        }

        context = csv_open_stream(readCallback, fp, NULL);
    }

    st = ustime();

    while (csv_read_row(context, &row)) {
        for (int i = 0; i < row.columns; i++) {
            totlen += row.column[i].size;
        }
        
        rows++;
    }

    et = ustime();

    printf("%s: %zu rows with %zu column bytes in %lld us\n",
            toCursorTypeString(type), rows, totlen, et - st);
}
