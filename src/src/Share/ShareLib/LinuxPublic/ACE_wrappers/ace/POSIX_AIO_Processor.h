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
 *
 *  @file    POSIX_AIO_Processor.h
 *
 *  $Id: $
 *
 *  @author Alexander Libman <libman@terabit.com.au>
 */
//=============================================================================

#ifndef ACE_POSIX_AIO_PROCESSOR_H 
#define ACE_POSIX_AIO_PROCESSOR_H 

#include "ace/POSIX_AIO_FD_Info.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */


#if defined (ACE_HAS_AIO_CALLS) || defined(ACE_HAS_AIO_EMULATION)

ACE_BEGIN_VERSIONED_NAMESPACE_DECL

//Forward declaration
class ACE_POSIX_AIO_Dispatcher;
class ACE_POSIX_AIO_Config;
class ACE_POSIX_AIO_Provider;

#include "ace/Task.h"


/**
 * @class ACE_POSIX_AIO_Processor
 *
 * @brief POSIX implementation .
 */
class ACE_Export ACE_POSIX_AIO_Processor
{
public:
    typedef ACE_POSIX_AIO_FD_Repository::Mutex         Mutex;
    typedef ACE_POSIX_AIO_FD_Repository::Condition     Condition;
    typedef ACE_POSIX_AIO_FD_Repository::Monitor       Monitor;
    typedef ACE_POSIX_AIO_FD_Repository::Guard_Monitor Guard_Monitor;
    typedef ACE_POSIX_AIO_FD_Repository::Save_Guard    Save_Guard;

    /// Constructor.
    ACE_POSIX_AIO_Processor (ACE_POSIX_AIO_Dispatcher & completion_dispatcher,
                             const ACE_POSIX_AIO_Config & config);
    /// Virtual destructor.
    virtual ~ACE_POSIX_AIO_Processor (void);

    const ACE_POSIX_AIO_Config & config (void) const;
    ACE_POSIX_AIO_Dispatcher &   completion_dispatcher () const;

    ACE_POSIX_AIO_Provider *     provider () const;
    Monitor &                    leader_monitor ();

    virtual bool is_dedicated () const;
    virtual int  start ();
    virtual int  stop  ();
    virtual int  interrupt (void);
    virtual int  get_completed_aio (ACE_Time_Value * wait_time,
                 ACE_POSIX_Asynch_Result_Queue & completed_queue);
 

    int  get_supported_operations_mask () const;
    bool is_emulation () const;
    int  init ();
    int  fini ();
    
    int register_handle (ACE_HANDLE  handle,
                         const void *completion_key,
                         int         operations);

    int  unregister_handle (ACE_HANDLE handle);

    
    int  start_aio  (ACE_POSIX_Asynch_Result * result);
    int  cancel_aio (ACE_HANDLE h);
    int  cancel_aio (ACE_Handler * handler);
    int  cancel_all ();


private:
    const ACE_POSIX_AIO_Config & config_;
    ACE_POSIX_AIO_Dispatcher &   completion_dispatcher_;
    Monitor                      leader_monitor_;
    ACE_POSIX_AIO_Provider *     provider_;
    int                          supported_operations_;
};

inline const ACE_POSIX_AIO_Config &
ACE_POSIX_AIO_Processor::config(void) const
{
    return this->config_;
}

inline ACE_POSIX_AIO_Dispatcher &   
ACE_POSIX_AIO_Processor::completion_dispatcher() const
{
    return this->completion_dispatcher_;
}

inline ACE_POSIX_AIO_Provider *
ACE_POSIX_AIO_Processor::provider(void) const
{
    return this->provider_;
}
inline ACE_POSIX_AIO_Processor::Monitor &
ACE_POSIX_AIO_Processor::leader_monitor()
{
    return this->leader_monitor_;
}

/**
 * @class ACE_POSIX_AIO_Dedicated_Processor
 *
 * @brief POSIX implementation .
 */
class ACE_Export ACE_POSIX_AIO_Dedicated_Processor
    : public ACE_POSIX_AIO_Processor
    , public ACE_Task_Base
{
public:

    /// Constructor.
    ACE_POSIX_AIO_Dedicated_Processor 
        (ACE_POSIX_AIO_Dispatcher & completion_dispatcher,
         const ACE_POSIX_AIO_Config & config);

    /// Virtual destructor.
    virtual ~ACE_POSIX_AIO_Dedicated_Processor (void);

    virtual int  svc ();

    virtual bool is_dedicated () const;
    virtual int  start ();
    virtual int  stop  ();
    virtual int  interrupt (void);

    virtual int get_completed_aio (ACE_Time_Value * wait_time,
                           ACE_POSIX_Asynch_Result_Queue & completed_queue);
 
private:
    Monitor     task_monitor_;
    bool        flg_stop_;
};

ACE_END_VERSIONED_NAMESPACE_DECL

#endif /* ACE_HAS_AIO_CALLS */

#endif //ACE_POSIX_AIO_PROCESSOR_H 
