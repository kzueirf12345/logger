#ifndef SRC_LOGGER_H
#define SRC_LOGGER_H

#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

typedef struct place_in_code_t
{
    const char* file;
    const char* func;
    int line;
} place_in_code_t;

#define CODE_LINE_POISON -0xB00B


enum LoggError
{
    LOGG_ERROR_SUCCESS = 0,
    LOGG_ERROR_FAILURE = 1
};
static_assert(LOGG_ERROR_SUCCESS == 0);

const char* logg_strerror(const enum LoggError);

#define logg_error_handle(call_func, ...)                                                           \
    do {                                                                                            \
        logg_error_handler = call_func;                                                             \
        if (logg_error_handler)                                                                     \
        {                                                                                           \
            fprintf(stderr, "Can't " #call_func". Logg error: %s\n",                                \
                            input_strerror(logg_error_handler));                                    \
            __VA_ARGS__                                                                             \
            return logg_error_handler;                                                              \
        }                                                                                           \
    } while(0)


enum LogLevelDetails
{
    LOG_LEVEL_DETAILS_ZERO    = 0b0000u,
    LOG_LEVEL_DETAILS_INFO    = 0b0001u,
    LOG_LEVEL_DETAILS_ERROR   = 0b0010u,
    LOG_LEVEL_DETAILS_LASSERT = 0b0100u,
    LOG_LEVEL_DETAILS_DUMB    = 0b1000u,
    LOG_LEVEL_DETAILS_ALL     = 0b1111u,
};
static_assert(LOG_LEVEL_DETAILS_ZERO == 0);


enum LoggError logger_ctor(void);
enum LoggError logger_dtor(void);

enum LoggError logger_set_level_details(const unsigned level_details);
enum LoggError logger_set_logout_file(const char* const filename);


enum LoggError internal_func_log_(const place_in_code_t place_in_code, 
                                enum LogLevelDetails level_details, // NOTE - NOT RENAME THIS
                                const char* const check_str,
                                const char* const format, ...);

#define logg(log_level_details, format, ...)                                                        \
        do                                                                                          \
        {                                                                                           \
            if (internal_func_log_((place_in_code_t)                                                \
                                  { .file = __FILE__, .func = __func__, .line = __LINE__ },         \
                                  log_level_details, NULL, format, ##__VA_ARGS__)                   \
                != LOGG_ERROR_SUCCESS)                                                                \
                fprintf(stderr, "Can't log smth\n");                                                \
        } while (0)


#ifndef NDEBUG

#define LASSERT_INTERNAL_(check, log_level_details, format, ...)                                    \
        do                                                                                          \
        {                                                                                           \
            if(!(check))                                                                            \
            {                                                                                       \
                if (internal_func_log_((place_in_code_t)                                            \
                                      { .file = __FILE__, .func = __func__, .line = __LINE__ },     \
                                      LOG_LEVEL_DETAILS_LASSERT | log_level_details,                \
                                      #check, format, ##__VA_ARGS__) != LOGG_ERROR_SUCCESS)           \
                    fprintf(stderr, "Can't log smth\n");                                            \
                if (logger_dtor())                                                                  \
                    fprintf(stderr, "Can't destroy logger\n");                                      \
                assert(0);                                                                          \
            }                                                                                       \
        } while(0)

#define  lassert(check, format, ...)                                                                \
        LASSERT_INTERNAL_(check, LOG_LEVEL_DETAILS_ERROR, format, ##__VA_ARGS__) 

#define nlassert(check, format, ...)                                                                \
        LASSERT_INTERNAL_(check, LOG_LEVEL_DETAILS_ZERO,  format, ##__VA_ARGS__) 

#else /*NDEBUG*/

#define  lassert(check, format, ...) do {} while(0)
#define nlassert(check, format, ...) do {} while(0)

#endif /*NDEBUG*/


#endif /*SRC_LOGGER_H*/