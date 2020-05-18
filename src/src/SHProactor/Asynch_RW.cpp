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

// ============================================================================
/**
 *  @file TRB_Asynch_RW.cpp
 *
 *  TRB_Asynch_RW.cpp
 *
 *
 *  @author Alexander Libman <libman@terabit.com.au>
 */
// ============================================================================
#ifndef ASYNCH_RW_C
#define ASYNCH_RW_C

#include "Asynch_RW.h"

//----------------------------------------------------------
#include "ace/Version.h"

#if ((ACE_MAJOR_VERSION == 5) && (ACE_MINOR_VERSION ==3))

#include "ace/OS.h"

#else // Assume ACE 5.4.x

#include "ace/OS_NS_sys_socket.h"

#endif // ((ACE_MAJOR_VERSION == 5) && (ACE_MINOR_VERSION ==3))
//----------------------------------------------------------


template <class READER, class WRITER>
TRB_Asynch_RW_T<READER, WRITER>::TRB_Asynch_RW_T (void)
  : handle_ (ACE_INVALID_HANDLE),
    completion_key_(0),
    rs_(),
    ws_(),
    m_isOK(false)
{
     ;
}

template <class READER, class WRITER>
TRB_Asynch_RW_T<READER, WRITER>::~TRB_Asynch_RW_T (void)
{
}

template <class READER, class WRITER> int
TRB_Asynch_RW_T<READER, WRITER>::close(void)
{
  rs_.close();
  ws_.close();
  //ACE_OS::closesocket (this->handle_);
  return 0;
}

template <class READER, class WRITER> int
TRB_Asynch_RW_T<READER, WRITER>::cancel(void)
{
  this->rs_.cancel ();
  this->ws_.cancel ();

  return 0;
}
template <class READER, class WRITER> int
TRB_Asynch_RW_T<READER, WRITER>::cancel_and_close(void)
{
    GetLocker()->Lock();
    if ( m_isOK )
    {
       this->cancel();
       this->close();
       m_isOK = false;
    }
    GetLocker()->UnLock();
    return 0;
}

template <class READER, class WRITER> int
TRB_Asynch_RW_T<READER, WRITER>::open 
         (ACE_Handler &handler,
          ACE_HANDLE handle, 
          const void *completion_key,
          ACE_Proactor *proactor,
          int pass_ownership)
{

  GetLocker()->Lock();
  if ( this->IsOk() ){
       GetLocker()->UnLock();
       return -1;
  }
  this->handle_ = handle;
  this->completion_key_ = completion_key;
  //this->flg_owner_ = pass_ownership;

  if (this->rs_.open(handler,
                     handle,
                     completion_key,
                     proactor) < 0)
  {
      GetLocker()->UnLock();
      return -1;
  }

  if (this->ws_.open(handler,
                     handle,
                     completion_key,
                     proactor) < 0)
  {
     GetLocker()->UnLock();
     return -1;
  }
  m_isOK = true;
  GetLocker()->UnLock();
  return 0;
}

// ****************************************************************

template <class READER, class WRITER> int
TRB_Asynch_RW_Stream_T<READER, WRITER>::read (const Buffer_Info& buf_info,
                            size_t num_bytes_to_read,
                            const void *act,
                            int priority,
                            int signal_number)
{
   this->GetLocker()->Lock();
   if ( !this->IsOk() ){
      this->GetLocker()->UnLock();
      return -1;
   }
   int ret = this->reader().read (buf_info,
                              num_bytes_to_read,
                              act,
                              priority,
                              signal_number);
   this->GetLocker()->UnLock(); 
  return ret;
}
 
template <class READER, class WRITER> int
TRB_Asynch_RW_Stream_T<READER, WRITER>::write (const Buffer_Info& buf_info,
                             size_t num_bytes_to_write,
                             const void *act,
                             int priority,
                             int signal_number)
{
   this->GetLocker()->Lock();
   if ( !this->IsOk() ){
      this->GetLocker()->UnLock();
      return -2;
   }
   int ret = this->writer().write (buf_info,
                               num_bytes_to_write,
                               act,
                               priority,
                               signal_number);
     this->GetLocker()->UnLock();
  return ret;

}

// ****************************************************************

template <class READER, class WRITER> ssize_t
TRB_Asynch_RW_Dgram_T<READER, WRITER>::recv (const Buffer_Info& buf_info,  
                 size_t &             number_of_bytes_recvd,  
                 int                  flags,  
                 int                  protocol_family,
                 const void *         act,
                 int                  priority,
                 int                  signal_number)
{
  if (this->handle_ == ACE_INVALID_HANDLE ||  this->flg_cancel_ != 0)
    return -1;

  return this->reader().recv (buf_info,
                              number_of_bytes_recvd,  
                              flags,  
                              protocol_family,
                              act,
                              priority,
                              signal_number);
}
 

template <class READER, class WRITER> ssize_t
TRB_Asynch_RW_Dgram_T<READER, WRITER>::send  (const Buffer_Info& buf_info,
                 size_t &            number_of_bytes_sent,  
                 int                 flags,  
                 const ACE_Addr &    remote_addr,  
                 const void *        act,  
                 int                 priority,  
                 int                 signal_number)
{
  if (this->handle_ == ACE_INVALID_HANDLE ||  this->flg_cancel_ != 0)
    return -1;

  return this->writer().send  (buf_info,
                               number_of_bytes_sent,  
                               flags,  
                               remote_addr,  
                               act,  
                               priority,  
                               signal_number);
}


#endif //ASYNCH_RW_C


