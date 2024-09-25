#ifndef SRC_LOGGER_H
#define SRC_LOGGER_H

#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>


enum LogCode
{
    LOG_CODE_SUCCES  = 0,
    LOG_CODE_FAILURE = 1
};
static_assert(LOG_CODE_SUCCES == 0);

enum LogLevelDetails
{
    LOG_LEVEL_DETAILS_ZERO  = 0b000u,
    LOG_LEVEL_DETAILS_INFO  = 0b001u,
    LOG_LEVEL_DETAILS_ERROR = 0b010u,
    LOG_LEVEL_DETAILS_ALL   = 0b011u,
};


enum LogCode logger_ctor(void);
enum LogCode logger_dtor(void);

enum LogCode logger_set_level_details(const unsigned level_details);
enum LogCode logger_set_logout_file(const char * const filename);


enum LogCode internal_func_log(const char* const func_name, const int line_num, 
                               const char* const filename, enum LogLevelDetails level_details,
                               const char* const format, ...);

#define logg(log_level_details, format, ...)                                                       \
            internal_func_log(__func__, __LINE__, __FILE__, log_level_details, format,      \
                              ##__VA_ARGS__)


#ifndef NDEBUG

#define lassert(check, ...)                                                                        \
            do                                                                                     \
            {                                                                                      \
                if(!(check))                                                                       \
                {                                                                                  \
                    internal_func_log(__func__, __LINE__, __FILE__, LOG_LEVEL_DETAILS_ERROR,       \
                                      ##__VA_ARGS__);                                              \
                    if (logger_dtor())                                                             \
                        fprintf(stderr, "Can't destroy logger\n");                                 \
                    assert(0);                                                                     \
                }                                                                                  \
            } while(0)

#else /*NDEBUG*/

#define lassert(check, format, ...) do {} while(0)

#endif /*NDEBUG*/


#endif /*SRC_LOGGER_H*/