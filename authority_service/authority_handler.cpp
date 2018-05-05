#include "authority_handler.h"
#include "authority_handler_impl.h"

Authority_Handler::Authority_Handler()
    : impl_ (nullptr)
{

}

Authority_Handler::~Authority_Handler()
{

}

Authority_Handler *Authority_Handler::instance()
{
    static Authority_Handler m;
    return &m;
}

bool Authority_Handler::initialise()
{
    if (!impl_)
    {
        impl_ = new Authority_Handler_Impl();
    }
    if (impl_)
        return impl_->initialise();
    return false;
}

void Authority_Handler::run()
{
    if (impl_)
        return impl_->run();
}

int Authority_Handler::put_msg(ACE_Message_Block *message_block)
{
    if (impl_)
        return impl_->put_msg(message_block);
    return -1;
}




