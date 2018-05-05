#ifndef NET_SVC_HANDLER_H
#define NET_SVC_HANDLER_H
#include <ace/SOCK_Stream.h>
#include <ace/Svc_Handler.h>
#include <ace/INET_Addr.h>

#include "authority_handler.h"

class Net_SVC_Handler : public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_MT_SYNCH>
{
    typedef ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_MT_SYNCH> Super;
public:
    Net_SVC_Handler()
    {
        ACE_OS::memset(sremote_addr_, 0 , sizeof(sremote_addr_));
        ACE_OS::memset(remote_ip_, 0 , sizeof(remote_ip_));
        ACE_OS::memset(recv_buf_, 0 , sizeof(recv_buf_));

        nremote_ip_ = 0;
    }

    int initialise()
    {
        ACE_DEBUG ((LM_INFO,ACE_TEXT ("(New Connect)\r\n")));

        return 0;
    }

    virtual ~Net_SVC_Handler()
    {
        ACE_DEBUG ((LM_INFO,ACE_TEXT ("ACE_SYN_Handler\t[%T]::~ACE_SYN_Handler() [%s] [fd:%d]\r\n"), sremote_addr_,this->get_handle()));
    }

    /// Called when input events occur (e.g., connection or data).
    virtual int handle_input (ACE_HANDLE fd = ACE_INVALID_HANDLE)
    {
        get_remote_peer_info();
        (void)(fd);
        ///////////////////////////////////////////////////////// new test
        static const int MSG_BLOCK_LEN = 256;
        ACE_Message_Block *message_block = new ACE_Message_Block(MSG_BLOCK_LEN);
        /////////////////////////////////////////////////////////

        int rc = ACE_OS::recv(this->get_handle(), message_block->wr_ptr(), MSG_BLOCK_LEN);
        if (rc <= 0)
        {
            if (errno == ETIME || errno == ETIMEDOUT)
            {
                ACE_DEBUG ((LM_INFO, ACE_TEXT("recv from [fd:%d,remote_addr:%s,sremote_ip:%s,nremote_ip:%d] -> timeout\r\n"),
                            this->get_handle(),
                            sremote_addr_,
                            remote_ip_,
                            nremote_ip_));
            }
            else
            {
                this->close();
                this->shutdown();
            }
        }
        else
        {
//            ACE_DEBUG ((LM_INFO, ACE_TEXT("recv from [fd:%d,remote_addr:%s,sremote_ip:%s,nremote_ip:%d] -> %s\r\n"),
//                        this->get_handle(),
//                        sremote_addr_,
//                        remote_ip_,
//                        nremote_ip_,
//                        recv_buf_));
            ///todo:
            Authority_Handler::instance()->put_msg(message_block);
        }
        return 0;
    }


    virtual int handle_signal(int signum, siginfo_t *siginfo = 0, ucontext_t *p = 0)
    {
        if(signum == SIGPIPE)
        {
            ACE_DEBUG ((LM_ERROR, ACE_TEXT("ACE_SYN_Handler\t[%T] handle_signal SIGPIPE: %d \n"),SIGPIPE));
            return 0;
        }
        else
        {
            ACE_DEBUG ((LM_WARNING, ACE_TEXT("ACE_SYN_Handler\t[%T] handle_signal num: %d \n"),signum));
            return Super::handle_signal(signum , siginfo , p);
        }
    }

    int get_remote_peer_info(void)
    {
        if (!is_parse_remote_addr_)
        {
            ACE_INET_Addr remote_addr;
            int rc = this->peer().get_remote_addr(remote_addr);
            if(-1 == rc)
            {
                ACE_DEBUG ((LM_ERROR, ACE_TEXT("ACE_SYN_Handler\t[%T] get_remote_addr error: %d \n"), ACE_OS::last_error()));
                return -1;
            }

            rc = remote_addr.addr_to_string(sremote_addr_, sizeof(sremote_addr_));
            if(-1 == rc)
            {
                ACE_DEBUG ((LM_ERROR, ACE_TEXT("ACE_SYN_Handler\t[%T] remote_addr.addr_to_string[may be buffer is small !!!] error: %d \n"), ACE_OS::last_error()));
                return -1;
            }

            nremote_ip_ = ACE_HTONL(remote_addr.get_ip_address());
            const char *premote_ip = (char *)ACE_OS::inet_ntoa(*(struct in_addr *)(int *)&(nremote_ip_));
            ACE_OS::strncpy(remote_ip_, premote_ip, sizeof(remote_ip_)-1);

            ACE_DEBUG ((LM_INFO, ACE_TEXT("ACE_SYN_Handler\t[%T] get_remote_peer_info: fd:%d,remote_addr:%s,sremote_ip:%s,nremote_ip:%d\n"),
                        this->get_handle(),
                        sremote_addr_,
                        remote_ip_,
                        nremote_ip_));
        }

        return 0;
    }

private:
    bool is_parse_remote_addr_;
    char sremote_addr_[64];
    char remote_ip_[64];
    int  nremote_ip_;
    char recv_buf_[1024];
};

#endif // NET_SVC_HANDLER_H
