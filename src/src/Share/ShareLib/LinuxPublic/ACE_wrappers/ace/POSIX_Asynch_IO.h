/* -*- C++ -*- */
//=============================================================================
/**
 *  @file    POSIX_Asynch_IO.h
 *
 *  POSIX_Asynch_IO.h,v 4.33 2002/11/09 23:51:29 shuston Exp
 *
 *  The implementation classes for POSIX implementation of Asynch
 *  Operations are defined here in this file.
 *
 *  @author Irfan Pyarali <irfan@cs.wustl.edu>
 *  @author Tim Harrison <harrison@cs.wustl.edu>
 *  @author Alexander Babu Arulanthu <alex@cs.wustl.edu>
 *  @author Roger Tragin <r.tragin@computer.org>
 *  @author Alexander Libman <alibman@baltimore.com>
 */
//=============================================================================

#ifndef ACE_POSIX_ASYNCH_IO_H 
#define ACE_POSIX_ASYNCH_IO_H 

#include "ace/pre.h"

#include "ace/ACE_export.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#if defined (ACE_HAS_AIO_CALLS) || defined(ACE_HAS_AIO_EMULATION)

#include "ace/Asynch_IO_Impl.h"

#if defined (ACE_HAS_AIO_EMULATION)
#  include "ace/POSIX_AIO_Emulation.h"
#endif

#if defined(ACE_HAS_LINUX_LIBAIO)
#  include  <libaio.h>    // Linux native aio calls
#endif //ACE_HAS_LINUX_LIBAIO


#if !defined (ACE_HAS_AIO_EMULATION)
#  include "ace/os_include/os_aio.h"
#endif /* ACE_HAS_AIO_EMULATION */

#include "ace/Asynch_File_Transmitter.h"
#include "ace/Single_List_T.h"
#include "ace/Double_List_T.h"
#include "ace/Monitor_T.h"
#include "ace/Synch_T.h"

ACE_BEGIN_VERSIONED_NAMESPACE_DECL

class ACE_POSIX_Proactor;


inline int
translate_errno ()
{
    int rc = ACE_OS::last_error ();
    
    // on some systems EAGAIN and EWOULDBLOCK 
    // are different
    
    if (rc == EAGAIN)
    {
        return EWOULDBLOCK;
    }
    return rc;
}

/////////////////////////////////////////////////////////////
/**
 * @class ACE_POSIX_Asynch_Result
 *
 * This class provides concrete implementation for ACE_Asynch_Result
 * for POSIX4 platforms. This class extends @c aiocb and makes it more
 * useful.
 */
class ACE_Export ACE_POSIX_Asynch_Result :
    public ACE_Asynch_Result_Impl
{
public:

  LinkS_T <ACE_POSIX_Asynch_Result>  * get_link () const;

  // This method will be called by Emulation Providers
  // to execute operation.
  // Return codes:
  //  2 - AIO finished, next operation possible on this fd
  //  1 - AIO finished, next operation may be possible on this fd
  //  0 - not finished
  virtual int execute (void) = 0;

  struct aiocb *get_aiocb(void) { return &this->aio_.aiocb_; }

#if defined(ACE_HAS_LINUX_LIBAIO)
  struct iocb  *get_iocb(void) { return &this->aio_.iocb_; }
#endif

  // fill the embedded aiocb structure based on 
  // original ACE_Asynch_Result
  void build_aiocb (void);

  ACE_POSIX_Proactor      *posix_proactor ()
    {
      return this->posix_proactor_;
    }


protected:

  /// Constructor.
  ACE_POSIX_Asynch_Result (
       ACE_Allocator            *allocator,
       void                     *area,
       const ACE_Asynch_Result&  original,
       ACE_POSIX_Proactor       *posix_proactor);
       
  virtual ~ACE_POSIX_Asynch_Result  ();

  /// ACE_Asynch_Result_Impl methods

  // start implementation must be overriden for each Result
  virtual int start_impl ();
  
  // post implementation is common for all Results
  virtual int post_impl ();

private:
  // prohibit copy and assignment
  ACE_POSIX_Asynch_Result (const ACE_POSIX_Asynch_Result& other);
  ACE_POSIX_Asynch_Result & operator = (const ACE_POSIX_Asynch_Result& other);

  /// link element
  mutable LinkS_T <ACE_POSIX_Asynch_Result> link_;

  
  // Platform specific data
  // aiocb is required for POSIX AIO
  // iocb is required for LINUX AIO
  // both of them are never used by provider
  // BTW, Emulatuion Providers do need these fields at all
  union 
  {
     aiocb aiocb_;

#if defined(ACE_HAS_LINUX_LIBAIO)
     iocb  iocb_;
#endif // ACE_HAS_LINUX_LIBAIO

  } aio_;

  ACE_POSIX_Proactor      *posix_proactor_;
};

/**
 * @class LinkS_Functor_T <ACE_POSIX_Asynch_Result>
 * functor-converter:
 *   input:  address of ACE_POSIX_Asynch_Result
 *   output: address of link field
 */
template <> 
class LinkS_Functor_T <ACE_POSIX_Asynch_Result> : 
    public std::unary_function <ACE_POSIX_Asynch_Result, 
                                LinkS_T <ACE_POSIX_Asynch_Result> > 
{
public :
    LinkS_T <ACE_POSIX_Asynch_Result> * 
    operator () (const  ACE_POSIX_Asynch_Result * x)  const 
    {
        return x->get_link ();
    }
};

class Result2Link 
{
public :
    LinkS_T <ACE_POSIX_Asynch_Result> * 
    operator () (const  ACE_POSIX_Asynch_Result * x)  const 
    {
        return x->get_link ();
    }
};

/**
 * @class ACE_POSIX_Asynch_Result_List
 * @class ACE_POSIX_Asynch_Result_Queue
 */
typedef 
Single_List_T  <ACE_POSIX_Asynch_Result, Result2Link> 
ACE_POSIX_Asynch_Result_List;

typedef 
Single_Queue_T <ACE_POSIX_Asynch_Result, Result2Link> 
ACE_POSIX_Asynch_Result_Queue;



/**
 * @class ACE_POSIX_Asynch_Read_Stream_Result
 *
 * @brief Abstract base class for all the concrete implementation
 * classes that provide different implementations for the
 * ACE_Asynch_Read_Stream::Result class.
 *
 */

class ACE_POSIX_Asynch_Read_Stream_Result :
    public ACE_POSIX_Asynch_Result
{
public:
  typedef ACE_Asynch_Read_Stream_Result User_Result;

  ACE_POSIX_Asynch_Read_Stream_Result (
                             ACE_Allocator  *allocator,
                             void * area,
                             const ACE_Asynch_Read_Stream_Result&  original,
                             ACE_POSIX_Proactor *posix_proactor);
                           
  
  /// Destructor.
  virtual ~ACE_POSIX_Asynch_Read_Stream_Result (void);

  /// Proactor will call this method to execute asynch operation
  virtual int execute ();

  virtual ACE_Asynch_Result & get_original_result ();

private:
  // prohibit copy and assignment
  ACE_POSIX_Asynch_Read_Stream_Result (const ACE_POSIX_Asynch_Read_Stream_Result& other);
  ACE_POSIX_Asynch_Read_Stream_Result & operator = (const ACE_POSIX_Asynch_Read_Stream_Result& other);

  ACE_Asynch_Read_Stream_Result  original_;
};



/**
 * @class ACE_POSIX_Asynch_Write_Stream_Result
 *
 * @brief Abstract base class for all the concrete implementation
 * classes that provide different implementations for the
 * ACE_Asynch_Write_Stream::Result class.
 *
 */

class ACE_POSIX_Asynch_Write_Stream_Result :
  public ACE_POSIX_Asynch_Result
{
public:

  typedef ACE_Asynch_Write_Stream_Result User_Result;

  ACE_POSIX_Asynch_Write_Stream_Result (
                             ACE_Allocator  *allocator,
                             void * area,
                             const ACE_Asynch_Write_Stream_Result&  original,
                             ACE_POSIX_Proactor *posix_proactor);
                           

  /// Destructor.
  virtual ~ACE_POSIX_Asynch_Write_Stream_Result (void);

  /// Proactor will call this method to execute asynch operation
  virtual int execute ();

  virtual ACE_Asynch_Result & get_original_result ();
  
private:
  // prohibit copy and assignment
  ACE_POSIX_Asynch_Write_Stream_Result (const ACE_POSIX_Asynch_Write_Stream_Result& other);
  ACE_POSIX_Asynch_Write_Stream_Result & operator = (const ACE_POSIX_Asynch_Write_Stream_Result& other);

  ACE_Asynch_Write_Stream_Result original_;
};

/**
 * @class ACE_POSIX_Asynch_Read_Dgram_Result
 *
 * @brief Abstract base class for all the concrete implementation
 * classes that provide different implementations for the
 * ACE_Asynch_Read_Dgram::Result class.
 *
 */

class ACE_POSIX_Asynch_Read_Dgram_Result :
    public ACE_POSIX_Asynch_Result
{
public:
  typedef ACE_Asynch_Read_Dgram_Result User_Result;

  ACE_POSIX_Asynch_Read_Dgram_Result (
                             ACE_Allocator  *allocator,
                             void * area,
                             const ACE_Asynch_Read_Dgram_Result&  original,
                             ACE_POSIX_Proactor *posix_proactor);
                         

  /// Destructor.
  virtual ~ACE_POSIX_Asynch_Read_Dgram_Result (void);

  /// Proactor will call this method to execute asynch operation
  virtual int execute ();

  virtual ACE_Asynch_Result & get_original_result ();

private:
  // prohibit copy and assignment
  ACE_POSIX_Asynch_Read_Dgram_Result (const ACE_POSIX_Asynch_Read_Dgram_Result& other);
  ACE_POSIX_Asynch_Read_Dgram_Result & operator = (const ACE_POSIX_Asynch_Read_Dgram_Result& other);
  
  ACE_Asynch_Read_Dgram_Result  original_;
};

/**
 * @class ACE_POSIX_Asynch_Write_Dgram_Result
 *
 * @brief Abstract base class for all the concrete implementation
 * classes that provide different implementations for the
 * ACE_Asynch_Write_Dgram::Result class.
 *
 */

class ACE_POSIX_Asynch_Write_Dgram_Result :
    public ACE_POSIX_Asynch_Result
{
public:
  typedef ACE_Asynch_Write_Dgram_Result User_Result;

  ACE_POSIX_Asynch_Write_Dgram_Result (
                             ACE_Allocator  *allocator,
                             void * area,
                             const ACE_Asynch_Write_Dgram_Result&  original,
                             ACE_POSIX_Proactor *posix_proactor);

                           
  /// Destructor.
  virtual ~ACE_POSIX_Asynch_Write_Dgram_Result (void);

  /// Proactor will call this method to execute asynch operation
  virtual int execute ();

  virtual ACE_Asynch_Result & get_original_result ();

private:
  // prohibit copy and assignment
  ACE_POSIX_Asynch_Write_Dgram_Result (const ACE_POSIX_Asynch_Write_Dgram_Result& other);
  ACE_POSIX_Asynch_Write_Dgram_Result & operator = (const ACE_POSIX_Asynch_Write_Dgram_Result& other);
  
  ACE_Asynch_Write_Dgram_Result original_;
};

/**
 * @class ACE_POSIX_Asynch_Connect_Result
 *
 * @brief Abstract base class for all the concrete implementation
 * classes that provide different implementations for the
 * ACE_Asynch_Connect::Result class.
 *
 */

class ACE_POSIX_Asynch_Connect_Result :
    public ACE_POSIX_Asynch_Result
{
public:
  typedef ACE_Asynch_Connect_Result User_Result;

  ACE_POSIX_Asynch_Connect_Result (
                             ACE_Allocator  *allocator,
                             void * area,
                             const ACE_Asynch_Connect_Result&  original,
                             ACE_POSIX_Proactor *posix_proactor);
                           

  /// Destructor.
  virtual ~ACE_POSIX_Asynch_Connect_Result (void);

  /// Proactor will call this method to execute asynch operation
  virtual int execute ();
  
  virtual ACE_Asynch_Result & get_original_result ();
  
  /// Connect operation specific method.
  /// Initiates connect in non-bloking mode,
  /// therefore, we can wait for completion
  /// return:
  /// 0  - connect started, will complete later
  /// -1 - errors, connect has not been started
  virtual int start_impl ();

private:
  // prohibit copy and assignment
  ACE_POSIX_Asynch_Connect_Result (const ACE_POSIX_Asynch_Connect_Result& other);
  ACE_POSIX_Asynch_Connect_Result & operator = (const ACE_POSIX_Asynch_Connect_Result& other);
  
  ACE_Asynch_Connect_Result original_;
};

/**
 * @class ACE_POSIX_Asynch_Accept_Result
 *
 * @brief Abstract base class for all the concrete implementation
 * classes that provide different implementations for the
 * ACE_Asynch_Accept::Result class.
 *
 */

class ACE_POSIX_Asynch_Accept_Result :
    public ACE_POSIX_Asynch_Result
{
public:
  typedef ACE_Asynch_Accept_Result User_Result;

  
  ACE_POSIX_Asynch_Accept_Result (
                             ACE_Allocator  *allocator,
                             void * area,
                             const ACE_Asynch_Accept_Result&  original,
                             ACE_POSIX_Proactor *posix_proactor);
                           
  /// Destructor.
  virtual ~ACE_POSIX_Asynch_Accept_Result (void);

  /// Proactor will call this method to execute asynch operation
  virtual int execute ();

  virtual ACE_Asynch_Result & get_original_result ();

private:
  // prohibit copy and assignment
  ACE_POSIX_Asynch_Accept_Result (const ACE_POSIX_Asynch_Accept_Result& other);
  ACE_POSIX_Asynch_Accept_Result & operator = (const ACE_POSIX_Asynch_Accept_Result& other);

  ACE_Asynch_Accept_Result original_;
};

/**
 * @class ACE_POSIX_Asynch_Transmit_File_Result
 *
 * @brief Abstract base class for all the concrete implementation
 * classes that provide different implementations for the
 * ACE_Asynch_Transmit_File::Result class.
 *
 */

class ACE_POSIX_Asynch_Transmit_File_Result :
   public ACE_POSIX_Asynch_Result,
   public ACE_Asynch_File_Transmitter::Callback
{
public:
  typedef ACE_Asynch_Transmit_File_Result User_Result;

  ACE_POSIX_Asynch_Transmit_File_Result (
                             ACE_Allocator  *allocator,
                             void * area,
                             const ACE_Asynch_Transmit_File_Result&  original,
                             ACE_POSIX_Proactor *posix_proactor);
                         
  /// Destructor.
  virtual ~ACE_POSIX_Asynch_Transmit_File_Result (void);

  /// Proactor will call this method to execute asynch operation
  virtual int execute ();

  virtual ACE_Asynch_Result & get_original_result ();

  /// Transmit-file operation specific method.
  /// Initiates transmission in non-bloking mode,
  /// therefore, we can wait for completion
  /// return:
  /// 0  - transmit started, will complete later
  /// -1 - errors, transmit has not been started
  virtual int start_impl ();

  /// ACE_Asynch_File_Transmitter::Callback interface 
  virtual void on_complete (ACE_Asynch_File_Transmitter& transmitter);

private:
  // prohibit copy and assignment
  ACE_POSIX_Asynch_Transmit_File_Result (const ACE_POSIX_Asynch_Transmit_File_Result& other);
  ACE_POSIX_Asynch_Transmit_File_Result & operator = (const ACE_POSIX_Asynch_Transmit_File_Result& other);

  ACE_Asynch_File_Transmitter transmitter_;
};

//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
template <class RESULT>
class ACE_POSIX_Asynch_Result_T :
  public ACE_POSIX_Asynch_Result
{
public:
  typedef RESULT User_Result;
  
  ACE_POSIX_Asynch_Result_T (ACE_Allocator  *allocator,
                             void           *area,
                             const RESULT &  original,
                             ACE_POSIX_Proactor *posix_proactor)
  : ACE_POSIX_Asynch_Result (allocator, 
                             area,
                             original,
                             posix_proactor)
  , original_               (original)
    {
    }
                             
  /// Destructor.
  virtual ~ACE_POSIX_Asynch_Result_T (void)
    {
    }

  /// Proactor will call this method to execute asynch operation
  virtual int execute ()
    {
       return -1;
    }
  virtual ACE_Asynch_Result & get_original_result ()
    {
      return this->original_;
    }

private:
  // prohibit copy and assignment
  ACE_POSIX_Asynch_Result_T (const ACE_POSIX_Asynch_Result_T<RESULT> & other);
  ACE_POSIX_Asynch_Result_T & operator = (const ACE_POSIX_Asynch_Result_T<RESULT> & other);

  RESULT            original_;
};

typedef 
  ACE_POSIX_Asynch_Result_T<ACE_Asynch_Timer>
  ACE_POSIX_Asynch_Timer;


typedef 
  ACE_POSIX_Asynch_Result_T<ACE_Wakeup_Completion>
  ACE_POSIX_Wakeup_Completion;

typedef 
  ACE_POSIX_Asynch_Result_T<ACE_Asynch_User_Result>
  ACE_POSIX_Asynch_User_Result;

ACE_END_VERSIONED_NAMESPACE_DECL

#endif /* ACE_HAS_AIO_CALLS */

#include "ace/post.h"

#endif //ACE_POSIX_ASYNCH_IO_H 
