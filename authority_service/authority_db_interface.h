#ifndef AUTHORITY_DB_INTERFACE_H
#define AUTHORITY_DB_INTERFACE_H
#include <string>
#include <list>
/*
 * Database Api
 * */
class DB_Mysql_Operation;
class Authority_DB_Interface
{
public:
    Authority_DB_Interface();
    ~Authority_DB_Interface();

    bool initialise();

    void release();

    int query_all_ahthority_key(std::list<std::string> &authoriy_keys);

    int insert_authrotiy_key(const char * const constauthority_key);

private:
    DB_Mysql_Operation *_db_operation;
};

#endif // AUTHORITY_DB_INTERFACE_H
