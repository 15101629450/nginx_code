
#include "lg_mysql.h"
#include <stdio.h>
#include <string.h>
#include <mysql.h>

MYSQL *lg_mysql_conn(char *host, char *base, char *user, char *pass)
{

    MYSQL *conn = mysql_init(NULL);
    if (!conn) return NULL;

    if(!mysql_real_connect(conn, host, user, pass, base, 0, 0, 0)) {
	mysql_close(conn);
	return NULL;
    }

    if (mysql_set_character_set( conn, "utf8")) {
	mysql_close(conn);
	return NULL;
    }   

    return conn;
}

int lg_mysql_code(MYSQL *conn, int code)
{
    mysql_close(conn);
    return code;
}

int lg_mysql_select(char *host, char *base, char *user, char *pass, char *sql, lg_mysql_callback *fun, void *p)
{

    MYSQL *conn = lg_mysql_conn(host, base, user, pass);
    if (!conn) return -1;

    int ret = mysql_real_query(conn, sql, strlen(sql));
    if (ret != 0) return lg_mysql_code(conn, -1);

    MYSQL_RES *res = mysql_store_result(conn);
    if (!res) return lg_mysql_code(conn, -1);

    // int row_len = mysql_num_rows(res);
    int col_len = mysql_num_fields(res);

    int i;
    int col_type[col_len];
    char *col_name[col_len];
    MYSQL_FIELD *fields = mysql_fetch_fields(res);
    for (i = 0; i < col_len; i++) {
	col_type[i] = fields[i].type;
	col_name[i] = fields[i].name;
    }

    MYSQL_ROW data;
    while ((data = mysql_fetch_row(res))) {
	fun(col_len, col_name, col_type, data, p);
    }

    mysql_free_result(res);
    return lg_mysql_code(conn, 0);
}

void lg_mysql_type_print(void)
{
    fprintf(stdout, "MYSQL_TYPE_DECIMAL=%d\n",		MYSQL_TYPE_DECIMAL);
    fprintf(stdout, "MYSQL_TYPE_NEWDECIMAL=%d\n",           MYSQL_TYPE_NEWDECIMAL);
    fprintf(stdout, "MYSQL_TYPE_TINY=%d\n",                 MYSQL_TYPE_TINY);
    fprintf(stdout, "MYSQL_TYPE_SHORT=%d\n",                MYSQL_TYPE_SHORT);
    fprintf(stdout, "MYSQL_TYPE_LONG=%d\n",                 MYSQL_TYPE_LONG);
    fprintf(stdout, "MYSQL_TYPE_FLOAT=%d\n",                MYSQL_TYPE_FLOAT);
    fprintf(stdout, "MYSQL_TYPE_DOUBLE=%d\n",               MYSQL_TYPE_DOUBLE);
    fprintf(stdout, "MYSQL_TYPE_NULL=%d\n",                 MYSQL_TYPE_NULL);
    fprintf(stdout, "MYSQL_TYPE_TIMESTAMP=%d\n",            MYSQL_TYPE_TIMESTAMP);
    fprintf(stdout, "MYSQL_TYPE_LONGLONG=%d\n",             MYSQL_TYPE_LONGLONG);
    fprintf(stdout, "MYSQL_TYPE_INT24=%d\n",                MYSQL_TYPE_INT24);
    fprintf(stdout, "MYSQL_TYPE_DATE=%d\n",                 MYSQL_TYPE_DATE);
    fprintf(stdout, "MYSQL_TYPE_TIME=%d\n",                 MYSQL_TYPE_TIME);
    fprintf(stdout, "MYSQL_TYPE_DATETIME=%d\n",             MYSQL_TYPE_DATETIME);
    fprintf(stdout, "MYSQL_TYPE_YEAR=%d\n",                 MYSQL_TYPE_YEAR);
    fprintf(stdout, "MYSQL_TYPE_NEWDATE=%d\n",              MYSQL_TYPE_NEWDATE);
    fprintf(stdout, "MYSQL_TYPE_ENUM=%d\n",                 MYSQL_TYPE_ENUM);
    fprintf(stdout, "MYSQL_TYPE_SET=%d\n",                  MYSQL_TYPE_SET);
    fprintf(stdout, "MYSQL_TYPE_TINY_BLOB=%d\n",            MYSQL_TYPE_TINY_BLOB);
    fprintf(stdout, "MYSQL_TYPE_MEDIUM_BLOB=%d\n",          MYSQL_TYPE_MEDIUM_BLOB);
    fprintf(stdout, "MYSQL_TYPE_LONG_BLOB=%d\n",            MYSQL_TYPE_LONG_BLOB);
    fprintf(stdout, "MYSQL_TYPE_BLOB=%d\n",                 MYSQL_TYPE_BLOB);
    fprintf(stdout, "MYSQL_TYPE_VAR_STRING=%d\n",           MYSQL_TYPE_VAR_STRING);
    fprintf(stdout, "MYSQL_TYPE_STRING=%d\n",               MYSQL_TYPE_STRING);
    fprintf(stdout, "MYSQL_TYPE_TINY=%d\n",                 MYSQL_TYPE_TINY);
    fprintf(stdout, "MYSQL_TYPE_ENUM=%d\n",                 MYSQL_TYPE_ENUM);
    fprintf(stdout, "MYSQL_TYPE_GEOMETRY=%d\n",             MYSQL_TYPE_GEOMETRY);
    fprintf(stdout, "MYSQL_TYPE_BIT=%d\n",                  MYSQL_TYPE_BIT);
}


