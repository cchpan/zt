#ifndef AUTHORITY_HANDLER_IMPL_H
#define AUTHORITY_HANDLER_IMPL_H
#include <ace/Task.h>
#include <assert.h>
#include <string.h>
#include "authority_db_interface.h"

class Authority_Handler_Impl : public ACE_Task<ACE_MT_SYNCH>
{
public:
    Authority_Handler_Impl() {}
    virtual ~Authority_Handler_Impl() {}

    bool initialise()
    {
        /////////////////////
        ///todo: DB
        if (!_sql_db_interface.initialise()) return false;

        if (0 != _sql_db_interface.query_all_ahthority_key(_authority_keys)) return false;

        return true;
    }

    void release()
    {
        /////////////////////
        ///todo: thread exit???
        _sql_db_interface.release();
    }

    void run()
    {
        this->activate();
    }

    int put_msg(ACE_Message_Block *message_block)
    {
        return this->putq(message_block);
    }

    /// Run by a daemon thread to handle deferred processing.
    virtual int svc (void)
    {
        ACE_Message_Block * msg_block = nullptr;
        while (1)
        {
            if (this->getq(msg_block) == -1)
            {
                if (errno == ETIMEDOUT || errno == ETIME) continue;
                else
                {
                    ACE_DEBUG((LM_ERROR, "get queue error!!!\r\n"));
                    break;
                }
            }
            if (msg_block)
            {
                /// todo: call md5 api and compera user,passwd
                ACE_DEBUG((LM_INFO, "debug: msg:%s", msg_block->rd_ptr()));
                bool is_exists_key = false;
                for (auto & fkey : _authority_keys)
                {
                    if (memcmp(fkey.c_str(), msg_block->rd_ptr(), fkey.length()/*!!!!!!!!!msg_block.length()*/) == 0)
                    {
                        /// todo: response successful ACK.
                        ///
                        is_exists_key = true;
                    }

                }
                if (!is_exists_key)
                {
                    if (0 == _sql_db_interface.insert_authrotiy_key(msg_block->rd_ptr()))
                    {
                        _authority_keys.push_back(std::string(msg_block->rd_ptr()));
                        /// todo: response successful ACK.
                    }
                    else
                    {
                        /// todo: response error ACK.

                    }
                }
                delete msg_block;
            }
        }
        return 0;
    }

private:
    Authority_DB_Interface _sql_db_interface;
    std::list<std::string> _authority_keys;
};

#endif // AUTHORITY_HANDLER_IMPL_H
