
#include "web_site_module.h"

char domain_map_check_ch(char ch)
{
    switch (ch) {
	case '0' : return '0';
	case '1' : return '1';
	case '2' : return '2';
	case '3' : return '3';
	case '4' : return '4';
	case '5' : return '5';
	case '6' : return '6';
	case '7' : return '7';
	case '8' : return '8';
	case '9' : return '9';
    }
    return 0;
}

char domain_map_check_index(int index)
{
    switch (index) {
	case 0 : return '0';
	case 1 : return '1';
	case 2 : return '2';
	case 3 : return '3';
	case 4 : return '4';
	case 5 : return '5';
	case 6 : return '6';
	case 7 : return '7';
	case 8 : return '8';
	case 9 : return '9';
    }
    return 0;
}

char domain_map_db(lg_db_t *db, char *key, int index)
{
    lg_db_t *node = lg_db_find(db, lg_db_data_string(key));
    if (node && node->value.string) return *node->value.string;
    return domain_map_check_index(index);
}

/**************/
/*** encode ***/
/**************/
void domain_map_encode_r(lg_db_t *db, char *src, char *dest)
{
    char *str = NULL;
    for (str = src; *str; str++) {

	switch (*str) {

	    case '0':
		*dest++ = domain_map_db(db, "domain_map_0", 0);
		break;
	    case '1':
		*dest++ = domain_map_db(db, "domain_map_1", 1);
		break;
	    case '2':
		*dest++ = domain_map_db(db, "domain_map_2", 2);
		break;
	    case '3':
		*dest++ = domain_map_db(db, "domain_map_3", 3);
		break;
	    case '4':
		*dest++ = domain_map_db(db, "domain_map_4", 4);
		break;
	    case '5':
		*dest++ = domain_map_db(db, "domain_map_5", 5);
		break;
	    case '6':
		*dest++ = domain_map_db(db, "domain_map_6", 6);
		break;
	    case '7':
		*dest++ = domain_map_db(db, "domain_map_7", 7);
		break;
	    case '8':
		*dest++ = domain_map_db(db, "domain_map_8", 8);
		break;
	    case '9':
		*dest++ = domain_map_db(db, "domain_map_9", 9);
		break;
	}
    }

    *dest = 0;
}

void domain_map_encode(lg_db_t *db, int src, char *dest)
{
    *dest = 0;
    if (!src) return;

    char _src[1024];
    sprintf(_src, "%03d", src);
    domain_map_encode_r(db, _src, dest);
}

/**************/
/*** decode ***/
/**************/
int domain_map_decode_check(char *map, int ch)
{
    int i;
    for (i = 0; i < 10; i++)
	if (map[i] == ch) return domain_map_check_index(i);

    return domain_map_check_ch(ch);
}

int domain_map_decode_r(lg_db_t *db, char *src, char *dest)
{
    if (!src) {
	*dest = 0;
	return 0;
    }

    char map[10];
    map[0] = domain_map_db(db, "domain_map_0", 0);
    map[1] = domain_map_db(db, "domain_map_1", 1);
    map[2] = domain_map_db(db, "domain_map_2", 2);
    map[3] = domain_map_db(db, "domain_map_3", 3);
    map[4] = domain_map_db(db, "domain_map_4", 4);
    map[5] = domain_map_db(db, "domain_map_5", 5);
    map[6] = domain_map_db(db, "domain_map_6", 6);
    map[7] = domain_map_db(db, "domain_map_7", 7);
    map[8] = domain_map_db(db, "domain_map_8", 8);
    map[9] = domain_map_db(db, "domain_map_9", 9);

    char *str = NULL;
    for (str = src; *str; str++) {

	char key = domain_map_decode_check(map, *str);
	if (key) *dest++ = key;
    }

    *dest = 0;
    return 0;
}

int domain_map_decode(lg_db_t *db, char *src)
{
    if (!src) return 0;

    char dest[1024] = {0};
    domain_map_decode_r(db, src, dest);
    return lg_string_to_number(dest);
}




