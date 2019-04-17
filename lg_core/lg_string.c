
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include "lg_string.h"

char *lg_string_strtok(const char *src, char *dest, char **save_ptr, int ch)
{

    *dest = 0;
    if (save_ptr && *save_ptr) 
	src = *save_ptr;

    char *p = dest;
    char *s = (char *)src;
    for (; s && *s; s++) {

	if (*s == ch) {

	    *p = 0;
	    *save_ptr = s + 1;
	    return dest;
	}

	*p++ = *s;
    }

    if (src || *save_ptr) {

	*p = 0;
	*save_ptr = NULL;
	return dest;
    }

    return NULL;
}

char *lg_string_strstr(const char *str, const char *needle)
{
    if (!str || !needle)
	return NULL;

    size_t len = strlen(needle);
    if(len == 0) return NULL;

    while(*str) {

	if(strncasecmp(str, needle, len) == 0)
	    return (char *)str;

	++str;
    }

    return NULL;
}

char *lg_string_strspn(char *buffer, char ch)
{

    char *str = NULL;
    for (str = buffer; *str; str++)
	if (*str == ch)
	    return str;

    return NULL;
}

char *lg_string_cutting(char *buffer, char ch)
{
    if (!buffer) return NULL;

    char *str = lg_string_strspn(buffer, ch);
    if (!str) return NULL;

    *str = 0;
    return (str + 1);
}

int lg_string_cmp(const char *s1, int s1_len, const char *s2, int s2_len)
{

    int len = s1_len <= s2_len ? s1_len : s2_len;
    int ret = strncmp(s1, s2, len);
    if (ret == 0) {

	if (s1_len == s2_len)
	    return 0;

	if (s1_len < s2_len)
	    return -1;
	else
	    return 1;
    }

    return ret;
}

int lg_string_ch_count(const char *str, char ch)
{

    int count = 0;
    for (; *str; str++) 
	if (*str == ch) count++;

    return count;
}

size_t lg_string_url_encode(char *dest, char *src, size_t size) 
{

    size_t n;
    size_t dest_size = 0;
    char hextable[] = "0123456789ABCDEF";
    unsigned char *str = (unsigned char *)src;

    if (src == NULL) return 0;

    for (n = 0; n < size; n++) {

	if (!isalnum(str[n])) {

	    *dest++ = '%';
	    *dest++ = hextable[str[n] >> 4];
	    *dest++ = hextable[str[n] & 0x0F];
	    dest_size += 3;

	} else {

	    *dest++ = str[n];
	    dest_size++;
	}
    }

    *dest = 0;
    return dest_size;
}

size_t lg_string_url_decode(char *dest, char *src, size_t size)
{

    char h, l;
    size_t vh, vl, n;
    size_t dest_size = 0;
    if (src == NULL) return 0;

    for (n = 0; n < size; n++) {

	if (src[n] == '%') {

	    h = toupper(src[n + 1]);
	    l = toupper(src[n + 2]);
	    vh = isalpha(h) ? (10 + (h - 'A')) : (h - '0');
	    vl = isalpha(l) ? (10 + (l - 'A')) : (l - '0');

	    *dest++ = ((vh << 4) + vl);
	    dest_size++;
	    n++;
	    n++;

	} else if (src[n] == '+') {

	    *dest++ = ' ';
	    dest_size++;

	} else {

	    *dest++ = src[n];
	    dest_size++;
	}
    }

    *dest = 0;
    return dest_size;
}

char *lg_string_rand_number(char *buffer, int len)
{

    int i;
    char *str = buffer;
    for (i = 0; i < len; i++)
	*str ++ = rand() % 9 + 48;

    *str = 0;
    return buffer;
}


char *lg_string_rand_keyword(char *buffer, int len)
{

    int i;
    char *str = buffer;
    for (i = 0; i < len; i++)
	*str++ = rand() % 26 + 97;

    *str = 0;
    return buffer;
}

char *lg_string_url_to_domain(char *url)
{
    if (!url) return NULL;

    struct in_addr addr;
    if (inet_pton(AF_INET, url, &addr))
	return url;

    if (lg_string_ch_count(url, '.') == 1)
	return url;

    char *p = NULL;
    for (p = url; p && *p; p++)
	if (*p == '.')return ++p;

    return NULL;
}

int lg_string_to_number(char *str)
{
    if (!str) return 0;
    return atoi(str);
}







