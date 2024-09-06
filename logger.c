#include <stdarg.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <time.h>

#include "logger.h"

#define START_LOGGING_LINE "========START LOGGING======="
#define END_LOGGING_LINE   "=========END LOGGING========"

enum LogCode log_info(const char* const log_name_str, const char* const format, 
                      const char* const func_name, const char* const filename, const int line_num,
                      va_list* const args);
enum LogCode log_lassert(const char* const format, const char* const func_name,
                         const char* const filename, const int line_num, va_list* const args);


static struct
{
    unsigned output_flags;
    const char* logout_name;
    FILE* logout;
} LOGGER = {.output_flags = LOG_LEVEL_DETAILS_ZERO, .logout_name = NULL, .logout = NULL};



enum LogCode init_logger()
{
    if (LOGGER.logout_name || LOGGER.logout)
    {
        perror("Logger already init\n");
        return LOG_CODE_FAILURE;
    }
    LOGGER.logout_name = "./logout.log";
    if (!(LOGGER.logout = fopen(LOGGER.logout_name, "ab")))
    {
        perror("Can't open file\n");
        return LOG_CODE_FAILURE;
    }

    LOGGER.output_flags = LOG_LEVEL_DETAILS_INFO;


    fprintf(LOGGER.logout, "\n" START_LOGGING_LINE "\n\n");

    return LOG_CODE_SUCCES;
}

enum LogCode destroy_logger()
{
    fprintf(LOGGER.logout, "\n" END_LOGGING_LINE "\n");


    LOGGER.output_flags = LOG_LEVEL_DETAILS_ZERO;

    LOGGER.logout_name = NULL;

    if (LOGGER.logout && fclose(LOGGER.logout))
    {
        perror("Can't close file\n");
        return LOG_CODE_FAILURE;
    }
    LOGGER.logout = NULL;

    return LOG_CODE_SUCCES;
}


enum LogCode set_level_details(const unsigned level_details)
{
    if (LOG_LEVEL_DETAILS_ALL > level_details)
    {  
        perror("Incorrect level details flag\n");
        return LOG_CODE_FAILURE;
    }

    LOGGER.output_flags = level_details;

    return LOG_CODE_SUCCES;
}

enum LogCode set_logout_file(const char* const filename)
{
    assert(filename);

    if (LOGGER.logout)
        fprintf(LOGGER.logout, "\n" END_LOGGING_LINE "\n");

    LOGGER.logout_name = filename;

    if (LOGGER.logout && fclose(LOGGER.logout))
    {  
        perror("Can't close file\n");
        return LOG_CODE_FAILURE;
    }
    
    if (!(LOGGER.logout = fopen(filename, "ab"))){
        perror("Can't open file\n");
        return LOG_CODE_FAILURE;
    }
    
    fprintf(LOGGER.logout, "\n" START_LOGGING_LINE "\n\n");
    
    return LOG_CODE_SUCCES;
}



enum LogCode internal_func_log(const char* const func_name, const int line_num, 
                               const char* const filename,
                               enum LogLevelDetails level_details,
                               const bool check, ...)
{
    assert(func_name);
    assert(filename);

    va_list args;
    va_start(args, check);

    const char* format = va_arg(args, const char*);
    if (!format)
    {
        format = "";
    }

    if (LOGGER.output_flags & LOG_LEVEL_DETAILS_INFO & level_details)
    {
        if (log_info("LOG_INFO", format, func_name, filename, line_num, &args) == LOG_CODE_FAILURE)
        {  
            perror("log_info error\n");
            return LOG_CODE_FAILURE;
        }
    }
    else if(LOGGER.output_flags & LOG_LEVEL_DETAILS_ERROR & level_details)
    {
        if (log_info("LOG_ERROR", format, func_name, filename, line_num, &args) == LOG_CODE_FAILURE)
        {  
            perror("log_info error\n");
            return LOG_CODE_FAILURE;
        }
        if (log_lassert(format, func_name, filename, line_num, &args) == LOG_CODE_FAILURE)
        {
            perror("log_lassert error\n");
            return LOG_CODE_FAILURE;
        }
    }

    va_end(args);
    return LOG_CODE_SUCCES;
}


#define MAX_TIME_STR_LEN_ 64
enum LogCode log_info(const char* const log_name_str, const char* const format, 
                      const char* const func_name, const char* const filename, const int line_num,
                      va_list* const args)
{
    assert(log_name_str);
    assert(format);
    assert(args);
    assert(func_name);
    assert(filename);

    const time_t current_time = time(NULL);
    const struct tm * const current_local_time = localtime(&current_time);
    char current_time_str[MAX_TIME_STR_LEN_];
    if (strftime(current_time_str, MAX_TIME_STR_LEN_, "%Y %b %d %X", current_local_time) <= 0)
    {
        perror("strftime format error\n");
        return LOG_CODE_FAILURE;
    }

    if (fprintf(LOGGER.logout, "%-12sLOGGER.logout, %s. Func - %s() in %s:%d:  ",
                log_name_str, current_time_str, func_name, filename, line_num) <= 0)
    {  
        perror("fprintf error\n");
        return LOG_CODE_FAILURE;
    }
 
    if (vfprintf(LOGGER.logout, format, *args) < 0)
    {  
        perror("vprintf error\n");
        return LOG_CODE_FAILURE;
    }
    
    fprintf(LOGGER.logout, "\n");

    return LOG_CODE_SUCCES;
}
#undef MAX_TIME_STR_LEN_


enum LogCode log_lassert(const char* const format, const char* const func_name,
                         const char* const filename, const int line_num, va_list* const args)
{
    assert(format);
    assert(args);
    assert(func_name);
    assert(filename);

    if (fprintf(stderr, "\nLASSERT ERROR. Func - %s() in %s:%d:  ",
                func_name, filename, line_num) <= 0)
    {  
        perror("fprintf error\n");
        return LOG_CODE_FAILURE;
    }
 
    if (vfprintf(stderr, format, *args) < 0)
    {  
        perror("vprintf error\n");
        return LOG_CODE_FAILURE;
    }
    
    fprintf(LOGGER.logout, "\n" END_LOGGING_LINE "\n");

    return LOG_CODE_SUCCES;
}