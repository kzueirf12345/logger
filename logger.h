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

#define CODE_LINE_POISON -228666


enum LogCode
{
    LOG_CODE_SUCCESS = 0,
    LOG_CODE_FAILURE = 1
};
static_assert(LOG_CODE_SUCCESS == 0);

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


enum LogCode logger_ctor(void);
enum LogCode logger_dtor(void);

enum LogCode logger_set_level_details(const unsigned level_details);
enum LogCode logger_set_logout_file(const char* const filename);


enum LogCode internal_func_log_(const place_in_code_t place_in_code, 
                                enum LogLevelDetails level_details, // NOTE - NOT RENAME THIS
                                const char* const check_str,
                                const char* const format, ...);

#define logg(log_level_details, format, ...)                                                        \
        do                                                                                          \
        {                                                                                           \
            if (internal_func_log_((place_in_code_t)                                                \
                                  { .file = __FILE__, .func = __func__, .line = __LINE__ },         \
                                  log_level_details, NULL, format, ##__VA_ARGS__)                   \
                != LOG_CODE_SUCCESS)                                                                \
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
                                      #check, format, ##__VA_ARGS__) != LOG_CODE_SUCCESS)           \
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