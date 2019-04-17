
#ifndef	__LGMYSQL_H__
#define	__LGMYSQL_H__

#include <mysql.h>

typedef void (lg_mysql_callback)(int col_len, char **col_name, int *col_type, char **data, void *p);
extern int lg_mysql_select(char *host, char *base, char *user, char *pass, char *sql, lg_mysql_callback *fun, void *p);
extern void lg_mysql_type_print(void);

#endif


