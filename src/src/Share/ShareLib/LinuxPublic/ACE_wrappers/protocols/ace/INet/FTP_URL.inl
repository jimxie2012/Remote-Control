// -*- C++ -*-
//
// $Id: FTP_URL.inl 90891 2010-06-28 09:55:39Z mcorino $

ACE_BEGIN_VERSIONED_NAMESPACE_DECL

namespace ACE
{
  namespace FTP
  {

    ACE_INLINE
    const ACE_CString& URL::get_scheme () const
      {
        return PROTOCOL;
      }

    ACE_INLINE
    u_short URL::default_port () const
      {
        return FTP_PORT;
      }

  }
}

ACE_END_VERSIONED_NAMESPACE_DECL
