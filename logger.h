#ifndef SRC_LOGGER_H
#define SRC_LOGGER_H

#include <stdbool.h>
#include <stdlib.h>


enum LogCode
{
    LOG_CODE_SUCCES  = 0,
    LOG_CODE_FAILURE = 1
};

enum LogLevelDetails
{
    LOG_LEVEL_DETAILS_ZERO  = 0b000u,
    LOG_LEVEL_DETAILS_INFO  = 0b001u,
    LOG_LEVEL_DETAILS_ERROR = 0b010u,
    LOG_LEVEL_DETAILS_ALL   = 0b011u,
};


enum LogCode init_logger();
enum LogCode destroy_logger();

enum LogCode set_level_details(const unsigned level_details);
enum LogCode set_logout_file(const char * const filename);


enum LogCode internal_func_log(const char* const func_name, const int line_num, 
                               const char* const filename,
                               enum LogLevelDetails level_details,
                               const bool check, ...);

#define logg(log_level_details, ...) \
            internal_func_log(__func__, __LINE__, __FILE__, log_level_details, false, __VA_ARGS__)


#ifndef NDEBUG

#define INTERNAL_MACRO_FIRST_HELPER(first, ...) first
#define INTERNAL_MACRO_FIRST(...) INTERNAL_MACRO_FIRST_HELPER(__VA_ARGS__, throwaway)


#define lassert(...) \
            do \
            { \
                if(!(INTERNAL_MACRO_FIRST(__VA_ARGS__))) \
                { \
                    internal_func_log(__func__, __LINE__, __FILE__, LOG_LEVEL_DETAILS_ERROR, \
                                      __VA_ARGS__); \
                    exit(EXIT_FAILURE); \
                } \
            } while(0)

#else /*NDEBUG*/

#define lassert(check, format, ...) do {} while(0)

#endif /*NDEBUG*/


#endif /*SRC_LOGGER_H*/