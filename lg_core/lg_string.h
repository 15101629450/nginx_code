
#ifndef __LG_STRING_H__
#define	__LG_STRING_H__

#include <time.h>

#ifdef __cplusplus
extern "C" { 
#endif

    extern char *lg_string_strtok(const char *src, char *dest, char **save_ptr, int ch);
    extern char *lg_string_strstr(const char *haystack, const char *needle);
    extern char *lg_string_strspn(char *buffer, char ch);
    extern char *lg_string_cutting(char *buffer, char ch);
    extern int lg_string_cmp(const char *s1, int s1_len, const char *s2, int s2_len);

    extern size_t lg_string_url_encode(char *dest, char *src, size_t size);
    extern size_t lg_string_url_decode(char *dest, char *src, size_t size);

    extern char *lg_string_rand_number(char *buffer, int len);
    extern char *lg_string_rand_keyword(char *buffer, int len);
    extern char *lg_string_url_to_domain(char *url);

    extern int lg_string_to_number(char *str);

#ifdef __cplusplus
}
#endif
#endif//__LG_STRING_H__

