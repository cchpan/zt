#include "authority_db_interface.h"
#include <sstream>
#include "db_mysql_operation.h"
#include <ace/Log_Msg.h>

Authority_DB_Interface::Authority_DB_Interface()
{

}

Authority_DB_Interface::~Authority_DB_Interface()
{
    release();
}

bool Authority_DB_Interface::initialise()
{
    int rc = 0;
    if (!_db_operation)
    {
        _db_operation = new DB_Mysql_Operation();
    }

    rc = _db_operation->initialise("localhost", "root", "123") ? 0 : -1;

    if (rc == 0) rc = _db_operation->connect_sql();

    if (rc == 0 && !_db_operation->is_exists_sql_db("authority"))
    {
        rc = _db_operation->create_sql_database("CREATE DATABASE IF NOT EXISTS `authority` default charset utf8 COLLATE utf8_general_ci;");
    }

    if (rc == 0) rc = _db_operation->select_sql_db("authority");

    if (rc == 0 && !_db_operation->is_exists_sql_table("authority_terminal"))
    {
        rc = _db_operation->create_sql_table("CREATE TABLE IF NOT EXISTS `authority_terminal`("
                                             " `index` INT UNSIGNED AUTO_INCREMENT,"
                                             " PRIMARY KEY(`index`),"
                                             " `key` VARCHAR(128) NOT NULL,"
                                             " `status` INT NOT NULL DEFAULT '1')ENGINE=InnoDB DEFAULT CHARSET=utf8;"
                                             );
    }
    if (rc == 0)
    {
        ACE_DEBUG((LM_ERROR, "DB initialise Successful.\r\n"));
    }
    else
    {
        ACE_DEBUG((LM_ERROR, "DB initialise fail.\r\n"));
    }

    return rc == 0 ? true : false;
}

void Authority_DB_Interface::release()
{
    if (_db_operation)
    {
        _db_operation->release();
        delete _db_operation;
        _db_operation = nullptr;
    }
}

int Authority_DB_Interface::query_all_ahthority_key(std::list<std::string> &authoriy_keys)
{
    if (_db_operation)
    {
        MYSQL_RES *res = nullptr;
        int rc = _db_operation->select_sql(&res, "SELECT `key` from `authority_terminal`;");
        if (rc != 0 || res == nullptr) return rc;

        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)))
        {
            if (row[0])
                authoriy_keys.push_back(row[0]);
        }
        ////////////////////////
        ///test:log
        for (auto &a : authoriy_keys)
        {
            ACE_DEBUG((LM_DEBUG, "%s\r\n", a.c_str()));
        }
        return 0;
    }
    return -1;
}

int Authority_DB_Interface::insert_authrotiy_key(const char * const constauthority_key)
{
    if (_db_operation)
    {
        std::stringstream ss;
        ss << "INSERT INTO `authority_terminal`(`key`) VALUE (\"" << constauthority_key << "\");";
        return _db_operation->insert_sql(ss.str().c_str());
    }
    return -1;
}
