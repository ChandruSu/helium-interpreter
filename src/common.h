#ifndef HE_COMMON_HEADER
#define HE_COMMON_HEADER

#include <ctype.h>
#include <libgen.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#ifdef __MINGW32__
#define ERR_COL ""
#define WAR_COL ""
#define MSG_COL ""
#define DEF_COL ""
#define MESSAGE "[message]"
#define ERROR "[error]"
#define WARNING "[warning]"

#else
#define ERR_COL "\e[31m"
#define WAR_COL "\e[33m"
#define MSG_COL "\e[32m"
#define DEF_COL "\e[0m"

#define MESSAGE "[\e[1;32mmessage\033[0m]"
#define ERROR "[\e[1;31merror\033[0m]"
#define WARNING "[\e[1;33mwarning\033[0m]"
#endif

#define MAX_CALL_STACK 0xff
#define MAX_STACK_SIZE 0xff
#define MAX_HEAP_SIZE 0xfff
#define MAX_LOCAL_CONSTANTS 0xff
#define MAX_LOCAL_VARIABLES 0xff

// #define HE_DEBUG_MODE

#define streq(a, b) strcmp(a, b) == 0

typedef unsigned char boolean;
#define true 1
#define false 0

/**
 * @brief Prints file error and terminates program.
 * 
 * @param msg Error message
 * @param fname File path
 */
void file_error(const char* msg, const char* fname);

/**
 * @brief Reads file from path into character buffer and returns
 *      pointer.
 * 
 * @param filepath Path to file 
 * @return String buffer
 */
const char* read_file(const char* filepath);

/**
 * @brief Returns a substring beginning at a specified position
 *      till the end of the line (till newline character is reached).
 * 
 * @param source Super-string
 * @param start Beginning position
 * @return Substring
 */
const char* get_line(const char* source, int start);

/**
 * @brief Returns a string of a character repeated n times.
 * 
 * @param c Character to repeat
 * @param n Number of times to repeat
 * @return Padding string
 */
const char* paddchar(char c, int n);

/**
 * @brief Calculates a unique hashcode for the provided
 *      string.
 * 
 * @param str String to hash
 * @return hashcode
 */
size_t strhash(const char *str);

/**
 * @brief Prints error message to standard error and terminates the
 *      programmer.
 * 
 * @param msg Error message
 */
void failure(const char* msg);

#endif