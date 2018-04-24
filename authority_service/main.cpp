#include <iostream>
#include <ace/OS.h>
#include "net_svc_handler.h"
#include "net_server.h"
#include "creactor_task.h"

int ACE_TMAIN(int /*argc*/, char **/*argv*/)
{
    /////////////////////////////////////////////////////////////////////////////
    /// \brief select_reactor
    /// todo:accept select
    CReactor_Task *select_reactor = new CReactor_Task();//ACE_Reactor::instance();
    select_reactor->minit("my_select_reactor", TYPE_ACE_SELECT_REACTOR);
    select_reactor->start();

    Net_Server<Net_SVC_Handler> net_server;
    net_server.initialise();

    ACE_INET_Addr local_addr;
    local_addr.set_port_number(8090);

    if (0 == net_server.open(local_addr, select_reactor->get_reactor()))
    {
        ACE_DEBUG((LM_INFO, ACE_TEXT("Server Listen Succsseful...\r\n")));
    }
    else
    {
        ACE_DEBUG((LM_INFO, ACE_TEXT("Server Listen fail...\r\n")));
    }

    while (ACE_Reactor::instance()->reactor_event_loop_done() == 0)
    {
        ACE_Reactor::instance()->run_reactor_event_loop();
    }

    return 0;
}
