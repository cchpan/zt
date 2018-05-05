#ifndef DB_MYSQL_OPERATION_H
#define DB_MYSQL_OPERATION_H
#include <mysql/mysql.h>

class DB_Mysql_Operation
{
#define SV_DATA_LEN 128
public:
    DB_Mysql_Operation();
    ~DB_Mysql_Operation();


    bool initialise(const char * const host,
                    const char * const user,
                    const char * const passwd,
                    unsigned int port = 0,
                    const char * const unix_socket = 0,
                    unsigned long clientflag = 0
            );

    void release();

    void close_sql_db();

    int connect_sql();

    int select_sql_db(const char * const ptr_db);

    bool is_exists_sql_db(const char * const ptr_db_name);

    int create_sql_database(const char * const ptr_db_cmd);

    bool is_exists_sql_table(const char * const ptr_db_table_name);

    int create_sql_table(const char * const ptr_db_table_cmd);

    int select_sql(MYSQL_RES** res_result, const char * const ptr_cmd);

    int insert_sql(const char * const ptr_cmd);

private:
    MYSQL   *_mysql;
    char _host[SV_DATA_LEN];
    char _user[SV_DATA_LEN];
    char _passwd[SV_DATA_LEN];
    char _db[SV_DATA_LEN];
    unsigned int _port;
    char _unix_socket[SV_DATA_LEN];
    unsigned long _clientflag;
};

#endif // DB_MYSQL_OPERATION_H
