/*      File: err_msg.h                                                    */


/* macros for common error messages */

#define ERROR_BAD_OPEN(filename) { \
            printf("Error opening %s\n", filename); exit(1); }

#define ERROR_BAD_READ(filename) { \
            printf("Error reading %s\n", filename); exit(2); }

#define ERROR_NUM_LESS_1(filename) { \
            printf("Number of elements <= 0 in %s\n", filename); exit(3); }

#define ERROR_BAD_MALLOC(filename) { \
            printf("Malloc error for %s\n", filename); exit(4); }

#define ERROR_MSG(msg) { \
            printf("%s\n", msg); exit(99); }

#define WARNING_MSG(msg) { \
            printf("%s\n", msg); }

