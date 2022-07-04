#ifndef HE_COMMON_HEADER
#define HE_COMMON_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERR_COL "\033[31m"
#define WAR_COL "\033[33m"
#define MSG_COL "\033[32m"
#define DEF_COL "\033[0m"

#define MESSAGE "[\033[1;32mmessage\033[0m]"
#define ERROR "[\033[1;31merror\033[0m]"
#define WARNING "[\033[1;33mwarning\033[0m]"

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