#include "../csvmonkey.hpp"
#include "csvmonkey.h"

using namespace csvmonkey;

/* Make sure csvmonkey's CsvCell is plain data (no virtuals) */
static_assert(std::is_pod<CsvCell>::value, "CsvCell must be plain data for us to pun it");

/* Default delimiter, quote char, etc */
static csvOptions __csv_defaults { ',', '"', 0, 0 };

/* Basic csv context class */
struct csvContext {
    StreamCursor *cursor;
    CsvReader *reader;

    int error;
    char errstr[255];

    csvContext(StreamCursor *cursor, csvOptions *opt) {
        this->cursor = cursor;
        this->reader = new CsvReader(*cursor, opt->delimiter, opt->quote, opt->escape,
                                     opt->yield);
    }

    virtual ~csvContext() {
        delete this->cursor;
        delete this->reader;
    }
};

/* Very simple wrapper to the csv context that captures the file descriptor so
 * we can clean it up when we're done */
struct BufferedCsvContext: public csvContext {
    int fd;

    BufferedCsvContext(FdStreamCursor *cur, csvOptions *opt, int fd): csvContext(cur, opt) {
        this->fd = fd;
    }

    ~BufferedCsvContext() {
        close(this->fd);
    }
};

extern "C" {
    /* Convenience method to get default options for delimiter, escape character
     * quote character, etc */
    csvOptions csv_init_defaults(void) {
        return __csv_defaults;
    }

    static csvContext *get_mapped_context(const char *filename, csvOptions *opt) {
        MappedFileCursor *cursor;

        try {
            cursor = new MappedFileCursor();
            cursor->open(filename);
        } catch(csvmonkey::Error &e) {
            delete cursor;
            return NULL;
        }

        return new csvContext(cursor, opt ? opt : &__csv_defaults);
    }

    static csvContext *get_buffered_context(const char *filename, csvOptions *opt) {
        /* Make sure we can open the file */
        int fd = open(filename, O_RDONLY);
        if (fd == -1) {
            return NULL;
        }

        return new BufferedCsvContext(new FdStreamCursor(fd), opt ? opt : &__csv_defaults, fd);
    }

    csvContext *csv_open_file(const char *filename, csvCursorType type, csvOptions *opt) {
        switch (type) {
            case CSV_MAPPED:
                return get_mapped_context(filename, opt);
            case CSV_BUFFERED:
                return get_buffered_context(filename, opt);
            default:
                return NULL;
        }
    }

    csvContext *csv_open_stream(ssize_t (*cb)(void*,void*,size_t), void *privdata, csvOptions *options) {
        CallbackStreamCursor *cursor = new CallbackStreamCursor(cb, privdata);
        return new csvContext(cursor, options ? options : &__csv_defaults);
    }

    bool csv_read_row(csvContext *context, csvRow *row) {
        try {
            if (context->reader->read_row()) {
                CsvCursor &cmrow = context->reader->row();
                row->column = (csvCell*)cmrow.cells.data();
                row->columns = cmrow.count;
                return true;
            }
        } catch(csvmonkey::Error &e) {
            strncpy(context->errstr, e.what(), sizeof(context->errstr));
            context->error = 1;
        } 

        return false; 
    }

    const char *csv_error(csvContext *context) {
        return context->error ? context->errstr : NULL;
    } 

    void csv_free(csvContext *context) {
        if (context) {
            delete context;
        }
    }
}
