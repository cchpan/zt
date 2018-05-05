#include "db_mysql_operation.h"
#include <string.h>
#include <ace/Log_Msg.h>

DB_Mysql_Operation::DB_Mysql_Operation()
{

}

DB_Mysql_Operation::~DB_Mysql_Operation()
{
    close_sql_db();
}

bool DB_Mysql_Operation::initialise(const char * const host,
                                    const char * const user,
                                    const char * const passwd,
                                    unsigned int port,
                                    const char *const unix_socket,
                                    unsigned long clientflag)
{
    // init mysqlclient
    if (_mysql) return true;

    _mysql = mysql_init(0);
    if (!_mysql)
    {
        ACE_DEBUG((LM_ERROR, "failed to init to database: Error: [null].\n"));
        return false;
    }

    // assignment member variables
    memset(_host, 0, SV_DATA_LEN);
    memset(_user, 0, SV_DATA_LEN);
    memset(_passwd, 0, SV_DATA_LEN);
    memset(_db, 0, SV_DATA_LEN);
    memset(_unix_socket, 0, SV_DATA_LEN);
    if (host) memcpy(_host, host, strlen(host));
    if (user) memcpy(_user, user, strlen(user));
    if (passwd) memcpy(_passwd, passwd, strlen(passwd));
    if (unix_socket) memcpy(_unix_socket, unix_socket, strlen(unix_socket));
    _port = port;
    _clientflag = clientflag;

    return true;
}

void DB_Mysql_Operation::release()
{
    close_sql_db();
}

void DB_Mysql_Operation::close_sql_db()
{
    if (_mysql)
    {
        mysql_close(_mysql);
        _mysql = NULL;
        ACE_DEBUG((LM_DEBUG, "close database %s.\n", _db));
    }
}

int DB_Mysql_Operation::connect_sql()
{
    if (!mysql_real_connect(_mysql, _host, _user, _passwd, NULL, _port, NULL, _clientflag))
    {
        ACE_DEBUG((LM_ERROR, "error connect to sql:[%s]\n", const_cast<char*>(mysql_error(_mysql))));
        return -1;
    }
    ACE_DEBUG((LM_DEBUG, "success to Connect to mysql.\n"));

    return 0;
}

int DB_Mysql_Operation::select_sql_db(const char * const ptr_db)
{
    memcpy(_db, ptr_db, strlen(ptr_db));
    int rc = mysql_select_db(_mysql, _db);
    if (rc != 0)
    {
        ACE_DEBUG((LM_ERROR, "error select sql db: [%s] [%s].\n",  _db, mysql_error(_mysql)));
        close_sql_db();
        return -1;
    }
    ACE_DEBUG((LM_DEBUG, "success to select to database: [%s].\n", _db));

    return 0;
}

int DB_Mysql_Operation::create_sql_database(const char * const ptr_db_cmd)
{
    if (mysql_real_query(_mysql, ptr_db_cmd, strlen(ptr_db_cmd)) == 0)
    {
        ACE_DEBUG((LM_ERROR, "success create sql database.\n"));
        return 0;
    }
    else
    {
        ACE_DEBUG((LM_ERROR, "error(%s) create sql database.\n", mysql_error(_mysql)));
        return -1;
    }
}

bool DB_Mysql_Operation::is_exists_sql_table(const char * const ptr_db_table_name)
{
    MYSQL_RES *res = mysql_list_tables(_mysql, ptr_db_table_name);
    if (res)
    {
        if (res->row_count == 0) return false;
        return true;
    }
    return false;
}

int DB_Mysql_Operation::create_sql_table(const char * const ptr_db_table_cmd)
{
    if (mysql_real_query(_mysql, ptr_db_table_cmd, strlen(ptr_db_table_cmd)) == 0)
    {
        ACE_DEBUG((LM_ERROR, "success create sql table.\n"));
        return 0;
    }
    else
    {
        ACE_DEBUG((LM_ERROR, "error(%s) create sql table.\n", mysql_error(_mysql)));
        return -1;
    }
}

int DB_Mysql_Operation::select_sql(MYSQL_RES **res_result, const char * const ptr_cmd)
{
    int rc = mysql_real_query(_mysql, ptr_cmd, strlen(ptr_cmd));
    if (rc)
    {
        ACE_DEBUG((LM_ERROR, "select_sql()->cmd:[%s] Error:[%s %d].\r\n",
                   ptr_cmd,
                   mysql_error(_mysql),
                   mysql_errno(_mysql)));
    }
    else
    {
        *res_result = mysql_store_result(_mysql);
        if (NULL != res_result)
        {
            ACE_DEBUG((LM_ERROR, "select_sql() ->cmd:[%s] Success.\r\n", ptr_cmd));
        }
    }

    return rc;
}

int DB_Mysql_Operation::insert_sql(const char * const ptr_cmd)
{
    int rc = mysql_real_query(_mysql, ptr_cmd, strlen(ptr_cmd));
    if (rc)
    {
        ACE_DEBUG((LM_ERROR, "insert_sql()->cmd:[%s] Error:[%s %d].\r\n",
                   ptr_cmd,
                   mysql_error(_mysql),
                   mysql_errno(_mysql)));
    }
    else
    {
        ACE_DEBUG((LM_ERROR, "insert_sql() ->cmd:[%s] Success.\r\n", ptr_cmd));
    }

    return rc;
}

bool DB_Mysql_Operation::is_exists_sql_db(const char * const ptr_db_name)
{
    MYSQL_RES * res = mysql_list_dbs(_mysql, ptr_db_name);
    if (res)
    {
        if (res->row_count == 0) return false;
        return true;
    }
    return false;
}


