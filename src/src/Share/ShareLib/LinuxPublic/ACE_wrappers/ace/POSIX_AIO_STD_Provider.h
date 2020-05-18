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
 *  @file    POSIX_AIO_STD_Provider.h
 *
 *  $Id:  $
 *
 *  @author Alexander Libman <libman@terabit.com.au>
 */
//=============================================================================

#ifndef ACE_POSIX_AIO_STD_PROVIDER_H 
#define ACE_POSIX_AIO_STD_PROVIDER_H 

#include "ace/POSIX_AIO_Provider.h"

#if defined (ACE_HAS_AIO_CALLS) 

#include "ace/Synch_Traits.h"
#include "ace/Thread_Semaphore.h"
#include "ace/Null_Semaphore.h"

ACE_BEGIN_VERSIONED_NAMESPACE_DECL


/**
 *
 * @class ACE_POSIX_AIO_STD_Provider
 *
 */

class ACE_Export ACE_POSIX_AIO_STD_Provider :
    public ACE_POSIX_AIO_Provider
{
public:
    ACE_POSIX_AIO_STD_Provider (const ACE_POSIX_AIO_Config & config);
    virtual ~ACE_POSIX_AIO_STD_Provider();


    //virtual int  init ();
    //virtual int  fini ();
    //virtual int  interrupt ();
    //virtual int  start_aio (ACE_POSIX_Asynch_Result * result,
    //                ACE_POSIX_Asynch_Result_Queue & completed_queue);
    //virtual int  get_completed_aio (ACE_Time_Value * wait_time,
    //                Guard_Monitor &  leader_guard,
    //                ACE_POSIX_Asynch_Result_Queue & completed_queue);

    virtual int  get_supported_operations_mask () const;

    virtual int register_handle (ACE_HANDLE  handle,
                                const void *completion_key,
                                int         operations);

    virtual int  unregister_handle (ACE_HANDLE handle);

    

    virtual bool begin_aio_wait (ACE_Time_Value * wait_time,
                    Guard_Monitor &  leader_guard,
                    ACE_POSIX_Asynch_Result_Queue & completed_queue);

    virtual int  finish_aio_completions (
                    ACE_POSIX_Asynch_Result_Queue & completed_queue);


    virtual int  cancel_aio (ACE_POSIX_Asynch_Result * result,
                    ACE_POSIX_Asynch_Result_Queue & completed_queue);

    virtual int  cancel_aio (ACE_HANDLE handle,
                    ACE_POSIX_Asynch_Result_Queue & completed_queue);

    virtual int  cancel_aio (ACE_Handler * handler,
                    ACE_POSIX_Asynch_Result_Queue & completed_queue);

    virtual int  cancel_all (
                    ACE_POSIX_Asynch_Result_Queue & completed_queue);

protected:
    int   start_aio_impl (ACE_POSIX_Asynch_Result * result);

    bool  test_result_status (ACE_POSIX_Asynch_Result *result);

    // return number completed operations
    int   check_wait_queue (
                ACE_POSIX_Asynch_Result_Queue & completed_queue);

    // return  > 0  number of started deferred operations
    //           0  no started operations
    //         < 0  some completions due start errors 
    int   start_deferred_queue (
                ACE_POSIX_Asynch_Result_Queue & completed_queue);

    bool  cancel_deferred_queue (ACE_Handler * handler,
                ACE_POSIX_Asynch_Result_Queue & completed_queue);

    bool  cancel_active_queue (ACE_Handler * handler,
                ACE_POSIX_Asynch_Result_Queue & active_queue,
                ACE_POSIX_Asynch_Result_Queue & completed_queue);


protected:

    Monitor                       mon_provider_;
    ACE_POSIX_Asynch_Result_Queue started_queue_;
    ACE_POSIX_Asynch_Result_Queue wait_queue_;
    ACE_POSIX_Asynch_Result_Queue deferred_queue_;

};

/**
 *
 * @class ACE_POSIX_AIO_AIOCB_Provider
 *
 */

class ACE_Export ACE_POSIX_AIO_AIOCB_Provider :
    public ACE_POSIX_AIO_STD_Provider
{
public:
    ACE_POSIX_AIO_AIOCB_Provider (const ACE_POSIX_AIO_Config & config);
    virtual ~ACE_POSIX_AIO_AIOCB_Provider();

    virtual int  init ();
    virtual int  fini ();
    virtual int  interrupt ();

    virtual int  start_aio (ACE_POSIX_Asynch_Result * result,
                    ACE_POSIX_Asynch_Result_Queue & completed_queue);

    virtual int  get_completed_aio (ACE_Time_Value * wait_time,
                    Guard_Monitor &  leader_guard,
                    ACE_POSIX_Asynch_Result_Queue & completed_queue);

private:
    int   build_wait_queue ();
    bool  check_interrupter_result ();
    bool  wait_for_aio(ACE_Time_Value * wait_time, int count);

    bool                          need_interrupt_;
    Notification_Pipe             interrupter_;
    Notification_Result           interrupt_result_;

    aiocb*                         aiocb_array_ [ACE_AIO_MAX_SIZE];
};

#if !defined(__Lynx__) && !defined (__FreeBSD__)

#if defined(AIX) || defined(sun) || defined(__APPLE__) || defined (ACE_VXWORKS)
typedef union sigval sigval_t;
#endif
/**
 *
 * @class ACE_POSIX_AIO_CB_Provider
 *
 */

class ACE_Export ACE_POSIX_AIO_CB_Provider :
    public ACE_POSIX_AIO_STD_Provider
{
public:
    ACE_POSIX_AIO_CB_Provider (const ACE_POSIX_AIO_Config & config);
    virtual ~ACE_POSIX_AIO_CB_Provider();

    virtual int  init ();
    virtual int  fini ();
    virtual int  interrupt ();

    virtual int  start_aio (ACE_POSIX_Asynch_Result * result,
                    ACE_POSIX_Asynch_Result_Queue & completed_queue);

    virtual int  get_completed_aio (ACE_Time_Value * wait_time,
                    Guard_Monitor &  leader_guard,
                    ACE_POSIX_Asynch_Result_Queue & completed_queue);

    // This only public so the "extern C" completion function can see it
    // when needed.
    static void aio_completion_func (sigval_t cb_data);

private:
    bool  wait_for_aio(ACE_Time_Value * wait_time);

    /// semaphore variable to notify
    /// used to wait the first AIO start
    ACE_SYNCH_SEMAPHORE sema_;
};

#endif /* !defined(__Lynx__) && !defined (__FreeBSD__) */

ACE_END_VERSIONED_NAMESPACE_DECL

#endif /* ACE_HAS_AIO_CALLS */
#endif //ACE_POSIX_AIO_STD_PROVIDER_H 
