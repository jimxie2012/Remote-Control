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

//=============================================================================
/**
 *  @file    ACE_POSIX_AIO_SUN_Port_Strategy.h
 *
 *  $Id:  $
 *
 *  @author Alexander Libman <libman@terabit.com.au>
 */
//=============================================================================

#ifndef ACE_POSIX_AIO_SUN_PORT_PROVIDER_H 
#define ACE_POSIX_AIO_SUN_PORT_PROVIDER_H 

#include "ace/POSIX_AIO_Emulation_Provider.h"

#if defined (ACE_HAS_AIO_CALLS) || defined(ACE_HAS_AIO_EMULATION)
#if defined (ACE_HAS_SUN_COMPLETION_PORTS) && defined (sun)

#include /**/ <port.h>

ACE_BEGIN_VERSIONED_NAMESPACE_DECL

/**
 * @class ACE_POSIX_AIO_SUN_Port_Provider
 *
 */

class ACE_Export ACE_POSIX_AIO_SUN_Port_Provider : 
  public ACE_POSIX_AIO_Emulation_Provider
{
    enum { POLL_MAX_EVENTS = 256 };

public:

    ACE_POSIX_AIO_SUN_Port_Provider (const ACE_POSIX_AIO_Config & config);
    virtual ~ACE_POSIX_AIO_SUN_Port_Provider();

    virtual int get_specific_flags () const;

    virtual int init ();
    virtual int fini ();
    virtual int declare_interest (ACE_POSIX_AIO_FD_Info * info,
                                  int interest,
                                  int remain_mask);

    virtual int clear_interest (ACE_POSIX_AIO_FD_Info * info);

    virtual int get_ready_fd (ACE_Time_Value * wait_time,
                              Guard_Monitor &  leader_guard,
                              int & ready_mask,
                              int & remain_mask);

    virtual int poll_ready_fd (Guard_Monitor &  leader_guard,
                              int & ready_mask,
                              int & remain_mask);

    virtual int interrupt ();
  

private:
    ACE_HANDLE find_next_ready_fd (int & ready_mask,
                                   int & remain_mask);
    int        wait_for_ready_fd (ACE_Time_Value * wait_time);

 
  ACE_HANDLE     fd_poller_;

  port_event_t   events_ [POLL_MAX_EVENTS];

  uint_t            max_num_events_;
  uint_t            num_remain_;
  uint_t            cur_index_;
};

ACE_END_VERSIONED_NAMESPACE_DECL

#endif /* ACE_HAS_SUN_COMPLETION_PORTS */
#endif /* ACE_HAS_AIO_CALLS */
#endif //ACE_POSIX_AIO_SUN_PORT_PROVIDER_H 
