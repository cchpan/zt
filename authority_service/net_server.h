#ifndef NET_SERVER_H
#define NET_SERVER_H
#include <ace/SOCK_Acceptor.h>

#include <ace/Acceptor.h>

template <typename Net_SVC_Handler>
class Net_Server : public ACE_Acceptor<Net_SVC_Handler, ACE_SOCK_ACCEPTOR>
{
public:
    Net_Server(){}
    virtual ~Net_Server(){}
    typedef ACE_Acceptor<Net_SVC_Handler, ACE_SOCK_ACCEPTOR> Super;

    bool initialise()
    {

        return true;
    }

private:
    // = The following three methods define the Acceptor's strategies
    // for creating, accepting, and activating SVC_HANDLER's,
    // respectively.

    /**
     * Bridge method for creating a SVC_HANDLER.  The default is to
     * create a new {SVC_HANDLER} if {sh} == 0, else {sh} is unchanged.
     * However, subclasses can override this policy to perform
     * SVC_HANDLER creation in any way that they like (such as creating
     * subclass instances of SVC_HANDLER, using a singleton, dynamically
     * linking the handler, etc.).  Returns -1 on failure, else 0.
     */
    virtual int make_svc_handler (Net_SVC_Handler *&sh)
    {
        int rc = Super::make_svc_handler(sh);
        if(-1 == rc)
        {
            ACE_DEBUG((LM_ERROR,ACE_TEXT("ACE_SYN_Acceptor\t[%T]::make_svc_handler,error: %d \n"), ACE_OS::last_error()));
            return -1;
        }

        sh->initialise();

        return 0;
    }

    /**
     * Bridge method for accepting the new connection into the
     * @a svc_handler.  The default behavior delegates to the
     * PEER_ACCEPTOR::accept.
     */
    virtual int accept_svc_handler (Net_SVC_Handler *svc_handler)
    {
        int rc = Super::accept_svc_handler(svc_handler);
        if(-1 == rc)
        {
            if(svc_handler)
            {
                svc_handler->reactor(0);
                delete svc_handler;
                svc_handler = 0;
            }

            if(EMFILE == ACE_OS::last_error() || ENFILE == ACE_OS::last_error())
            {
                ACE_DEBUG((LM_WARNING,ACE_TEXT("ACE_SYN_Acceptor\t[%T] 连接句柄达到系统上限!!!\n")));
            }
            else
            {
                ACE_DEBUG((LM_ERROR,ACE_TEXT("ACE_SYN_Acceptor\t[%T] Unknown errno: %d \n") , ACE_OS::last_error()));
            }
        }

        return rc;
    }

private:

};

#endif // NET_SERVER_H
