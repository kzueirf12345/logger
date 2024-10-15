#include <stdarg.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <time.h>

#include "logger.h"


static struct
{
    unsigned output_flags;
    const char* logout_name;
    FILE* logout;
    bool is_used;
} LOGGER = {.output_flags = LOG_LEVEL_DETAILS_ZERO, .logout_name = NULL, .logout = NULL,
            .is_used = false};

static void LOGGER_is_init_asserts_(void)
{
    assert(LOGGER.logout && "LOGGER is not init");
    assert(LOGGER.logout_name && "LOGGER is not init");
}


enum LogCode logger_ctor(void)
{
    assert(!LOGGER.logout_name || !LOGGER.logout);

    LOGGER.logout_name = "./log/logout.log";
    if (!(LOGGER.logout = fopen(LOGGER.logout_name, "ab")))
    {
        perror("Can't open file");
        return LOG_CODE_FAILURE;
    }

    LOGGER.output_flags = LOG_LEVEL_DETAILS_INFO;

    return LOG_CODE_SUCCESS;
}

enum LogCode logger_dtor(void)
{
    if (LOGGER.is_used)
        fprintf(LOGGER.logout, "\n");


    LOGGER.output_flags = LOG_LEVEL_DETAILS_ZERO;

    LOGGER.logout_name = NULL;

    if (LOGGER.logout && fclose(LOGGER.logout))
    {
        perror("Can't close file");
        return LOG_CODE_FAILURE;
    }
    LOGGER.logout = NULL;

    return LOG_CODE_SUCCESS;
}


enum LogCode logger_set_level_details(const unsigned level_details)
{
    LOGGER_is_init_asserts_();
    assert((level_details <= LOG_LEVEL_DETAILS_ALL) && "Incorrect level details flag");

    LOGGER.output_flags = level_details;

    return LOG_CODE_SUCCESS;
}

enum LogCode logger_set_logout_file(const char* const filename)
{
    LOGGER_is_init_asserts_();
    assert(filename);

    if (LOGGER.logout)
        fprintf(LOGGER.logout,  "\n");

    LOGGER.logout_name = filename;

    if (LOGGER.logout && fclose(LOGGER.logout))
    {  
        perror("Can't close file");
        return LOG_CODE_FAILURE;
    }
    
    if (!(LOGGER.logout = fopen(filename, "ab"))){
        perror("Can't open file");
        return LOG_CODE_FAILURE;
    }
    
    
    return LOG_CODE_SUCCESS;
}



static enum LogCode log_write_(const char* const log_name_str,
                               const place_in_code_t* const place_in_code,
                               const char* const check_str,
                               const char* const format, va_list* const args);

static enum LogCode log_lassert_(const place_in_code_t place_in_code, const char* const check_str,
                                 const char* const format, va_list* const args);

// ahah, sorry, idk how do it otherwise. I want sleep
#define LOG_WRITE_WITH_MODS_(level_details, log_mode_str, place_in_code_ptr)                        \
        if (active_levels & level_details)                                                          \
        {                                                                                           \
            if (log_write_(log_mode_str, place_in_code_ptr, check_str, format, &args)               \
                == LOG_CODE_FAILURE)                                                                \
            {                                                                                       \
                fprintf(stderr, log_mode_str " error\n");                                           \
                return LOG_CODE_FAILURE;                                                            \
            }                                                                                       \
            break;                                                                                  \
        } do {} while(0)

enum LogCode internal_func_log_(const place_in_code_t place_in_code,
                                enum LogLevelDetails level_details, // NOTE - NOT RENAME THIS
                                const char* const check_str,
                                const char* const format, ...)
{
    LOGGER_is_init_asserts_();
    assert(place_in_code.file);
    assert(place_in_code.func);
    assert(place_in_code.line);

    va_list args = {};
    va_start(args, format);

    // NOTE - NOT RENAME THIS
    const unsigned active_levels = LOGGER.output_flags & level_details;

    do
    {
        LOG_WRITE_WITH_MODS_(LOG_LEVEL_DETAILS_INFO , "LOG_INFO" , &place_in_code);
        LOG_WRITE_WITH_MODS_(LOG_LEVEL_DETAILS_ERROR, "LOG_ERROR", &place_in_code);
        LOG_WRITE_WITH_MODS_(LOG_LEVEL_DETAILS_DUMB , "LOG_DUMB" , NULL);
    } while (0);
    

    if(LOGGER.output_flags & LOG_LEVEL_DETAILS_LASSERT & level_details)
    {
        va_start(args, format);

        if (log_lassert_(place_in_code, check_str, format, &args) == LOG_CODE_FAILURE)
        {
            perror("log_lassert error");
            return LOG_CODE_FAILURE;
        }
    }

    va_end(args);
    return LOG_CODE_SUCCESS;
}
#undef LOG_WRITE_WITH_MODS_


static enum LogCode log_additional_topic_(const char* const log_name_str);

static enum LogCode log_write_(const char* const log_name_str,
                               const place_in_code_t* const place_in_code,
                               const char* const check_str,
                               const char* const format, va_list* const args)
{
    LOGGER_is_init_asserts_();
    assert(log_name_str);
    assert(format);
    assert(args);

    LOGGER.is_used = true;

    if (log_additional_topic_(log_name_str) != LOG_CODE_SUCCESS)
    {
        fprintf(stderr, "Can't logging addintional topic\n");
        return LOG_CODE_FAILURE;
    }

    if (place_in_code)
    {
        assert(place_in_code->file);
        assert(place_in_code->func);
        assert(place_in_code->line);
        if (fprintf(LOGGER.logout, "%s:%-4d (%s()):  ",
                    place_in_code->file, place_in_code->line, place_in_code->func) <= 0)
        {  
            perror("fprintf error");
            return LOG_CODE_FAILURE;
        }
    }

    if (check_str)
    {
        if (fprintf(LOGGER.logout, "lassert(%s);  ", check_str) <= 0)
        {  
            perror("fprintf error");
            return LOG_CODE_FAILURE;
        }
    }
 
    if (vfprintf(LOGGER.logout, format, *args) < 0)
    {  
        perror("vprintf error");
        return LOG_CODE_FAILURE;
    }
    
    fprintf(LOGGER.logout, "\n");

    return LOG_CODE_SUCCESS;
}


static enum LogCode log_lassert_(const place_in_code_t place_in_code, const char* const check_str,
                                 const char* const format, va_list* const args)
{
    LOGGER_is_init_asserts_();
    assert(format);
    assert(args);
    assert(place_in_code.file);
    assert(place_in_code.func);
    assert(place_in_code.line);

    if (fprintf(stderr, "LASSERT ERROR. %s:%-4d (%s()):\n",
                place_in_code.file, place_in_code.line, place_in_code.func) <= 0)
    {  
        perror("fprintf error");
        return LOG_CODE_FAILURE;
    }

    if (check_str)
    {
        if (fprintf(stderr, "lassert(%s);  ", check_str) <= 0)
        {  
            perror("fprintf error");
            return LOG_CODE_FAILURE;
        }
    }

    if (vfprintf(stderr, format, *args) < 0)
    {  
        perror("vprintf error");
        return LOG_CODE_FAILURE;
    }
    fprintf(stderr, "\n\n");

    return LOG_CODE_SUCCESS;
}


#define MAX_TIME_STR_SIZE_ 64
static enum LogCode log_additional_topic_(const char* const log_name_str)
{
    LOGGER_is_init_asserts_();
    assert(log_name_str);

    const time_t current_time = time(NULL);
    const struct tm * const current_local_time = localtime(&current_time);
    char current_time_str[MAX_TIME_STR_SIZE_] = {};
    if (strftime(current_time_str, MAX_TIME_STR_SIZE_, "%Y %b %d %X", current_local_time) <= 0)
    {
        perror("strftime format error");
        return LOG_CODE_FAILURE;
    }

    if (fprintf(LOGGER.logout, "%-12s, %s. ", log_name_str, current_time_str) <= 0)
    {  
        perror("fprintf error");
        return LOG_CODE_FAILURE;
    }

    return LOG_CODE_SUCCESS;
}
#undef MAX_TIME_STR_SIZE_