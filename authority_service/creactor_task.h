#ifndef CREACTOR_TASK_H
#define CREACTOR_TASK_H
#include <assert.h>
#include <ace/Log_Msg.h>
#include <ace/Reactor.h>
#include <ace/Select_Reactor.h>
#include <ace/Dev_Poll_Reactor.h>
#include <ace/Thread_Semaphore.h>
#include <ace/TP_Reactor.h>
#include <ace/Task.h>
#include <ace/Thread_Manager.h>


#define TYPE_ACE_REACTOR 0x00
#define TYPE_ACE_SELECT_REACTOR 0x01
#define TYPE_ACE_DEV_POLL_REACTOR 0x02

class CReactor_Task : public ACE_Task<ACE_NULL_SYNCH>
{
public:
    CReactor_Task (bool bset_affinity = false):thr_id_(0), pReactor_(0)
    {
        ACE_OS::memset(info_,0,sizeof(info_));
        bset_affinity_ = bset_affinity;

        dev_poll_size_ = ACE_DEFAULT_SELECT_REACTOR_SIZE;
    }

    virtual ~CReactor_Task (void)
    {
        if(pReactor_)
        {
            delete pReactor_;
            pReactor_ = 0;
        }
    }

    void set_dev_poll_reactor_size(int size)
    {
        dev_poll_size_ = size;
        dev_poll_size_ = dev_poll_size_ < ACE_DEFAULT_SELECT_REACTOR_SIZE ? ACE_DEFAULT_SELECT_REACTOR_SIZE: dev_poll_size_;
    }

public:
    virtual int handle_signal (int signum, siginfo_t * = 0, ucontext_t * = 0)
    {
        if(signum == SIGPIPE)
        {
            ACE_DEBUG((LM_ERROR, "REACTOR_TASK [ %s:%ld ] handle_signal SIGPIPE: %d\r\n",info_, thr_id_ , SIGPIPE));
            return 0;
        }
        else
        {
            ACE_DEBUG((LM_ERROR, "REACTOR_TASK [ %s:%ld ] other handle_signal_num : %d\r\n",info_, thr_id_, signum));
            return -1;
        }
    }

    int minit(const char *pinfo = "" , const int ntype = TYPE_ACE_REACTOR)
    {
        ACE_OS::strncpy(info_, pinfo,sizeof(info_)-1);

        if(TYPE_ACE_SELECT_REACTOR == ntype)
        {
            ACE_Reactor_Impl *impl = new ACE_Select_Reactor();
            pReactor_ = new ACE_Reactor(impl,true);

        }
        else if(TYPE_ACE_DEV_POLL_REACTOR == ntype)
        {
            ACE_Reactor_Impl *impl = new ACE_Dev_Poll_Reactor(dev_poll_size_);
            pReactor_ = new ACE_Reactor(impl,true);
        }
        else
        {
            pReactor_ = new ACE_Reactor();
        }

        pReactor_->register_handler(SIGPIPE,this);
        sig_set_.sig_add (SIGPIPE);

        return 0;
    }

    virtual int svc()
    {
        thr_id_ = this->thr_mgr()->thr_self(); assert(thr_id_ == ACE_Thread::self());
        if(pReactor_)
        {
            pReactor_->owner(ACE_Thread::self());

            while(pReactor_->reactor_event_loop_done() == 0)
            {
                ACE_DEBUG((LM_ERROR, "REACTOR_TASK [%s] THREAD ID:%l staring\r\n",info_, thr_id_));

                pReactor_->restart(1);
                pReactor_->run_reactor_event_loop();
            }
        }
        ACE_DEBUG((LM_ERROR, "REACTOR_TASK [%s] REACTOR ENDED,THREAD ID:%l\r\n",info_, thr_id_));

        return 0;
    }

    void stop()
    {
        this->resume();

        if(pReactor_)
        {
            pReactor_->remove_handler(sig_set_);
            pReactor_->end_reactor_event_loop();
        }

        this->wait();
    }

    int start()
    {
        int rc = this->activate();
        if(-1 == rc)
        {
            ACE_DEBUG((LM_ERROR, "REACTOR_TASK %s REACTOR STAR FAIL\r\n",info_));
        }
        return rc;
    }

    ACE_Reactor *get_reactor(void)
    {
        return pReactor_;
    }

    ACE_thread_t getthr_id_(void)
    {
        return thr_id_;
    }

private:
    char info_[128];
    bool bset_affinity_;
    ACE_thread_t thr_id_;
    ACE_Reactor *pReactor_;
    ACE_Sig_Set  sig_set_;
    int dev_poll_size_;
};

#endif // CREACTOR_TASK_H
