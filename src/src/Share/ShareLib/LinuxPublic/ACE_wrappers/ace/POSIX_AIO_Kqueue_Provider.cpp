/*********************************************************************
** Copyright (C) 2003 Terabit Pty Ltd.  All rights reserved.
**
** This file is part of the POSIX-Proactor module.
**
**
**
**
**
**
**
**
**********************************************************************/

/* -*- C++ -*- */
// $Id:  $

#include "ace/POSIX_AIO_Kqueue_Provider.h"

#if defined (ACE_HAS_AIO_CALLS) || defined(ACE_HAS_AIO_EMULATION)
#if defined (ACE_HAS_KQUEUE)


#include "ace/Log_Msg.h"


ACE_BEGIN_VERSIONED_NAMESPACE_DECL

/**
 *****************************************************************
 *  BSD and Mac Kqueue  Provider
 *
 *****************************************************************
 */
ACE_POSIX_AIO_Kqueue_Provider::~ACE_POSIX_AIO_Kqueue_Provider()
{
    fini ();
}

ACE_POSIX_AIO_Kqueue_Provider::ACE_POSIX_AIO_Kqueue_Provider
  (const ACE_POSIX_AIO_Config & config)
: ACE_POSIX_AIO_Emulation_Provider (config)
, max_num_events_ (POLL_MAX_EVENTS)
, num_remain_     (0)
, cur_index_      (0)
, fd_kqueue_      (ACE_INVALID_HANDLE)
, interrupter_    (true)  // non blocking read side
{
    init ();
}

int 
ACE_POSIX_AIO_Kqueue_Provider::get_specific_flags ()const
{
    // return 0;
    return (ACE_POSIX_AIO_FD_Info::KEEP_READ_READY |
            ACE_POSIX_AIO_FD_Info::KEEP_WRITE_READY );
}


int
ACE_POSIX_AIO_Kqueue_Provider::init ()
{
    if (this->fd_kqueue_ != ACE_INVALID_HANDLE)
    {
        return 0;
    }

    this->fd_kqueue_ = ::kqueue ();
    
    if (this->fd_kqueue_ == ACE_INVALID_HANDLE)
    {
        return -1;
    }

    return this->declare_interest_i (this->interrupter_.read_handle (),
                                     ACE_POSIX_AIO_FD_Info::READ);
}

int
ACE_POSIX_AIO_Kqueue_Provider::fini ()
{
    if (this->fd_kqueue_ != ACE_INVALID_HANDLE)
    {
        ACE_OS::close (this->fd_kqueue_);
    }

    this->fd_kqueue_ = ACE_INVALID_HANDLE;
    return 0;
}

int
ACE_POSIX_AIO_Kqueue_Provider::interrupt ()
{
    return this->interrupter_.notify();
}


// *****************************************************************
//  Declare interest
//  Return code :
//    1 - leader was interrupted
//    0 - OK, we do not have to interrupt leader
//   -1 - errors
// *****************************************************************

int
ACE_POSIX_AIO_Kqueue_Provider::declare_interest (ACE_POSIX_AIO_FD_Info * info,
                                                 int interest,
                                                 int remain_mask)
{
    ACE_UNUSED_ARG (interest);
    if (remain_mask != 0)
    {
        return 0;
    }
    int fd = info->get_handle();
    return declare_interest_i (fd, interest);

}

int
ACE_POSIX_AIO_Kqueue_Provider::clear_interest (ACE_POSIX_AIO_FD_Info * info)
{
    int fd = info->get_handle();
    return this->clear_interest_i (fd);
}

// *****************************************************************
//
// *****************************************************************
int
ACE_POSIX_AIO_Kqueue_Provider::declare_interest_i (int fd, 
                                                   int interest)
{
    struct kevent events[2];

    int num = 0;

    if (ACE_BIT_ENABLED (interest, ACE_POSIX_AIO_FD_Info::READ))
    {
        EV_SET(&events[num],
           fd,                         // ident = file descriptor
           EVFILT_READ,                // filter
           EV_ADD | EV_CLEAR,          // action flags (edge-triggered) 
           0,                          // filter flags  
           0,                          // data
           0);                         // udata
        ++num;
    }

    if (ACE_BIT_ENABLED (interest, ACE_POSIX_AIO_FD_Info::WRITE))
    {
        EV_SET(&events[num],
           fd,                         // ident = file descriptor
           EVFILT_WRITE,               // filter
           EV_ADD | EV_CLEAR,          // action flags (edge-triggered) 
           0,                          // filter flags  
           0,                          // data
           0);                         // udata
        ++num;
    }

    if (num == 0)
    {
        return 0;
    }

    // Change the events associated with the given file descriptor.
    int rc = ::kevent(fd_kqueue_, events, num, 0, 0, 0);
    if (rc < 0)
    {
        ACE_ERROR_RETURN ((LM_ERROR,
            ACE_LIB_TEXT("(%t) ACE_POSIX_AIO_Kqueue_Strategy::declare_interest : %p\n"),
            ACE_LIB_TEXT("kevent failed")),
            -1);
    }
    return 0;
}

int
ACE_POSIX_AIO_Kqueue_Provider::clear_interest_i (int fd)
{
    struct kevent events[2];

    EV_SET(&events[0],
           fd,                         // ident = file descriptor
           EVFILT_READ,                // filter
           EV_DELETE,                  // action flags 
           0,                          // filter flags  
           0,                          // data
           0);                         // udata

    EV_SET(&events[1],
           fd,                         // ident = file descriptor
           EVFILT_WRITE,                // filter
           EV_DELETE,                  // action flags 
           0,                          // filter flags  
           0,                          // data
           0);                         // udata

    // Change the events associated with the given file descriptor.
    int rc = ::kevent(fd_kqueue_, events, 2, 0, 0, 0);
    if (rc < 0)
    {
        switch (errno)
        {
        case ENOENT:  // already removed, it is OK
        case EPERM:   // does not support
        case EBADF:   // invalid fd (ignore for cancell_all)
            break;

        default:
            ACE_ERROR_RETURN ((LM_ERROR,
                ACE_LIB_TEXT("(%t) ACE_POSIX_AIO_Kqueue_Strategy::clear_interest : %p\n"),
                ACE_LIB_TEXT("kevent failed")),
                -1);
        }
    }
    return 0;
}

int
ACE_POSIX_AIO_Kqueue_Provider::poll_ready_fd (Guard_Monitor &  leader_guard,
                                           int & ready_mask,
                                           int & remain_mask)
{
    // only one thread can take leadership
    Save_Guard lock_leader (leader_guard, Save_Guard::ACQUIRE);

    ACE_HANDLE fd = find_next_ready_fd (ready_mask, remain_mask);
    return fd;
}

int
ACE_POSIX_AIO_Kqueue_Provider::get_ready_fd (ACE_Time_Value * wait_time,
                                           Guard_Monitor &  leader_guard,
                                           int & ready_mask,
                                           int & remain_mask)
{
    // only one thread can take leadership
    Save_Guard lock_leader (leader_guard, Save_Guard::ACQUIRE);

    ACE_HANDLE fd = find_next_ready_fd (ready_mask, remain_mask);
    if (fd != ACE_INVALID_HANDLE)
    {
        return fd;
    }

    fd = this->wait_for_ready_fd (wait_time);
    if (fd <= 0)
    {
        return ACE_INVALID_HANDLE;
    }

    fd = find_next_ready_fd (ready_mask, remain_mask);
    return fd;
}

ACE_HANDLE
ACE_POSIX_AIO_Kqueue_Provider::find_next_ready_fd (int & ready_mask,
                                                   int & remain_mask)
{
    while (this->cur_index_ < this->num_remain_)
    {
        int index = cur_index_;

        ++this->cur_index_;

        struct kevent *pevent = & this->events_ [index];

        ready_mask = 0;

        switch (pevent->filter)
        {
        case EVFILT_READ:
            ACE_SET_BITS (ready_mask, ACE_POSIX_AIO_FD_Info::READ);
            break;

        case EVFILT_WRITE:
            ACE_SET_BITS (ready_mask, ACE_POSIX_AIO_FD_Info::WRITE);
            break;

        default:
            continue;
        }

        ACE_ASSERT (ready_mask != 0);

        int fd = pevent->ident;

        // do not report notification pipe
        if (this->interrupter_.read_handle () == fd)
        {
            this->interrupter_.read ();
            continue;
        }

        remain_mask = ACE_POSIX_AIO_FD_Info::READ
                    | ACE_POSIX_AIO_FD_Info::WRITE;


        return fd;
    }

    return ACE_INVALID_HANDLE;
}



int
ACE_POSIX_AIO_Kqueue_Provider::wait_for_ready_fd (ACE_Time_Value * wait_time)
{
    ACE_TRACE ("ACE_POSIX_AIO_Kqueue_Provider::wait_for_ready_fd");

    timespec_t   timeout;
    timespec_t * ptimeout = 0;

    if (wait_time != 0 )
    {
        timeout =  wait_time->operator timespec_t ();
        ptimeout = &timeout;
    }
    
    this->num_remain_ = ::kevent (fd_kqueue_,
                                  0,
                                  0,
                                  this->events_,
                                  this->max_num_events_,
                                  ptimeout);
    this->cur_index_ = 0;

    if (this->num_remain_ < 0)
    {
        return this->num_remain_ = 0;
    }

    return this->num_remain_;
}

ACE_END_VERSIONED_NAMESPACE_DECL

#endif /* ACE_HAS_KQUEUE */
#endif /* ACE_HAS_AIO_CALLS */
