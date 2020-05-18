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
 *  @file    POSIX_AIO_Dispatcher.h
 *
 *  $Id: $
 *
 *  @author Alexander Libman <libman@terabit.com.au>
 */
//=============================================================================

#ifndef ACE_POSIX_AIO_DISPATCHER_H 
#define ACE_POSIX_AIO_DISPATCHER_H 

#include "ace/POSIX_Asynch_IO.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */


#if defined (ACE_HAS_AIO_CALLS) || defined(ACE_HAS_AIO_EMULATION)

#include "ace/Thread_Manager.h"
#include <vector>

ACE_BEGIN_VERSIONED_NAMESPACE_DECL

//Forward declaration
class ACE_POSIX_AIO_Processor;
class ACE_POSIX_AIO_Config;

/**
 * @class ACE_POSIX_AIO_Dispatcher
 *
 * @brief POSIX implementation .
 */
class ACE_Export ACE_POSIX_AIO_Dispatcher
{
    // friend class ACE_POSIX_AIO_Processor;

public:
    typedef ACE_SYNCH_MUTEX            Mutex;
    typedef ACE_SYNCH_CONDITION        Condition;
    typedef Monitor_T<Mutex,Condition> Monitor;
    typedef Guard_Monitor_T<Monitor>   Guard_Monitor;
    typedef Guard_Monitor::Save_Guard  Save_Guard;


    /// Constructor.
    ACE_POSIX_AIO_Dispatcher (const ACE_POSIX_AIO_Config & config1,
                              const ACE_POSIX_AIO_Config & config2);


    /// Virtual destructor.
    virtual ~ACE_POSIX_AIO_Dispatcher (void);

    void add_completions (ACE_POSIX_Asynch_Result_Queue & completed_queue);

    void add_completion (ACE_POSIX_Asynch_Result *result);

    void add_main_completions (ACE_POSIX_Asynch_Result_Queue & completed_queue);

    void add_main_completion (ACE_POSIX_Asynch_Result *result);

    void wakeup ();
    
    
    int  handle_events (ACE_Time_Value * wait_time);

    int  add_processor (ACE_POSIX_AIO_Processor * processor);
    
    int  close ();
  
    ACE_Thread_Manager & thread_manager();

private:
    int   handle_events_d (ACE_Time_Value * wait_time);
    int   handle_events_s (ACE_Time_Value * wait_time);

    void  poll_dedicated_processors (ACE_POSIX_Asynch_Result_Queue& tmp_queue);

    int   process_completion_queue (ACE_POSIX_Asynch_Result_Queue& tmp_queue);

    int   clear_main_queue (Guard_Monitor & guard);

    int   process_main_queue (Guard_Monitor & guard);
    
    void  interrupt_shared_processor ();



    const ACE_POSIX_AIO_Config &    config1_ ;
    const ACE_POSIX_AIO_Config &    config2_ ;

    /// Thread manager for AIO_Dedicated_Processor's threads
    ACE_Thread_Manager              thr_manager_;

    Monitor                         completion_monitor_;
    ACE_POSIX_Asynch_Result_Queue   main_completion_queue_;

    // can be only one
    ACE_POSIX_AIO_Processor *       shared_processor_;

    typedef std::vector<ACE_POSIX_AIO_Processor *> ProcessorVector;
    ProcessorVector                 dedicated_processors_;

    int                             num_waiting_threads_;
};

inline ACE_Thread_Manager &
ACE_POSIX_AIO_Dispatcher::thread_manager ()
{
    return this->thr_manager_;
}

ACE_END_VERSIONED_NAMESPACE_DECL

#endif /* ACE_HAS_AIO_CALLS */

#endif //TPROACTOR_POSIX_AIO_DISPATCHER_H 


