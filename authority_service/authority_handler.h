#ifndef AUTHORITY_HANDLER_H
#define AUTHORITY_HANDLER_H

class ACE_Message_Block;
class Authority_Handler_Impl;

class Authority_Handler
{
public:
    Authority_Handler();
    ~Authority_Handler();

    static Authority_Handler* instance();

    bool initialise();

    void run();

    int put_msg(ACE_Message_Block *message_block);

private:
    Authority_Handler_Impl *impl_;
};

#endif // AUTHORITY_HANDLER_H
