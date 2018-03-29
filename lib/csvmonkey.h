#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

#ifdef __cplusplus 
extern "C" {
#else
#include <stdbool.h>
#endif

/* Opaque type to our CsvMonkey context */ 
typedef struct csvContext csvContext;

typedef enum csvCursorType {
    CSV_MAPPED,
    CSV_BUFFERED,
    CSV_CALLBACK,
    CSV_MAX,
} csvCursorType;

typedef struct csvOptions {
    char delimiter;
    char quote;
    char escape;
    char yield;
} csvOptions;

typedef struct csvCell {
    const char *ptr;
    size_t size;
    bool escaped;
} csvCell;

typedef struct csvRow {
    int columns;
    csvCell *column;
} csvRow;

csvContext *csv_open_file(const char *filename, csvCursorType type, csvOptions *options);
csvContext *csv_open_stream(ssize_t (*cb)(void*,void*,size_t), void *privdata, csvOptions *options);
csvOptions csv_init_defaults();
bool csv_read_row(csvContext *context, csvRow *row);
const char *csv_error(csvContext *context);
void csv_free(csvContext *context);

#ifdef __cplusplus
}
#endif
