/*******************************************************************************
**                                                                            **
**                 Information Security One(China)Ltd.                        **
**                                                                            **
*******************************************************************************/

/***************************COPYRIGHT INFOMATION********************************
**                                                                            **
** This program contains proprietary information which is a trade secret of   **
** Information Security One(China)Ltd. and also is protected as an            **
** unpublished program in confidence and is not permitted to use or make      **
** copies thereof other than as permitted in a written agreement with         **
** Information Security One(China)Ltd.                                        **
*******************************************************************************/

/*****************************FILE INFOMATION***********************************
**
** Project       : OCTOPUS 
** File Name     : crypt_md5.cpp
** Description   : SIM系统md5摘要时实现文件
** Contact       : wangxy@cn.is-one.net
** Creation date : 2005-09-01
**
*******************************************************************************/

////////////////////////////////////////////////////////////////////////////////
/// COMPILER OPTIONS


////////////////////////////////////////////////////////////////////////////////
/// INCLUDES

#ifndef MD
#define MD 5
#endif

#include <stdio.h>
#include <time.h>
#include <string.h>
#if MD == 2
#include "md2.h"
#endif
#if MD == 4
#include "md4.h"
#endif
#if MD == 5
#include "md5.h"
#endif

////////////////////////////////////////////////////////////////////////////////
/// DEFINES, MACROS 
/**
 * Length of test block, number of test blocks.
 */
#define TEST_BLOCK_LEN 1000
#define TEST_BLOCK_COUNT 1000


////////////////////////////////////////////////////////////////////////////////
/// EXTERN CONSTANT AND EXTERN VARIABLE DECLARATIONS


////////////////////////////////////////////////////////////////////////////////
/// TYPE DEFINITIONS 


////////////////////////////////////////////////////////////////////////////////
/// LOCAL CONSTANT DEFINITIONS 


////////////////////////////////////////////////////////////////////////////////
/// GLOBAL VARIABLES DEFINITIONS


////////////////////////////////////////////////////////////////////////////////
/// INLINED FUNCTIONS


////////////////////////////////////////////////////////////////////////////////
/// CLASS STATIC MEMBER VARIABLE DEFINITIONS


///#############################################################################
/// CLASS Name  :

////////////////////////////////////////////////////////////////////////////////
/// CLASS INTERFACE MEMBER FUNCTIONS

////////////////////////////////////////////////////////////////////////////////
/// CLASS INTERNAL MEMBER FUNCTIONS


///#############################################################################
/// CLASS Name  :

////////////////////////////////////////////////////////////////////////////////
/// CLASS INTERFACE MEMBER FUNCTIONS

////////////////////////////////////////////////////////////////////////////////
/// CLASS INTERNAL MEMBER FUNCTIONS


///#############################################################################
/// CLASS Name  :

////////////////////////////////////////////////////////////////////////////////
/// CLASS INTERFACE MEMBER FUNCTIONS

////////////////////////////////////////////////////////////////////////////////
/// CLASS INTERNAL MEMBER FUNCTIONS



///#############################################################################
/// END



static void MDString PROTO_LIST((char *));
static void MDTimeTrial PROTO_LIST((void));
static void MDTestSuite PROTO_LIST((void));
static void MDFile PROTO_LIST((char *));
static void MDFilter PROTO_LIST((void));
static void MDPrint PROTO_LIST((unsigned char [16]));

#if MD == 2
#define MD5_CTX MD2_CTX
#define MDInit MD2Init
#define MDUpdate MD2Update
#define MDFinal MD2Final
#endif
#if MD == 4
#define MD5_CTX MD4_CTX
#define MDInit MD4Init
#define MDUpdate MD4Update
#define MDFinal MD4Final
#endif
#if MD == 5
#define MD5_CTX MD5_CTX
#define MDInit MD5Init
#define MDUpdate MD5Update
#define MDFinal MD5Final
#endif

/**
  Main driver.

Arguments (may be any combination):
  -sstring - digests string
  -t       - runs time trial
  -x       - runs test script
  filename - digests file
  (none)   - digests standard input
 */
/***
int main(int argc, char* argv[])
{
  int i;

  if (argc > 1)
 for (i = 1; i < argc; i++)
   if (argv[i][0] == '-' && argv[i][1] == 's')
     MDString (argv[i] + 2);
   else if (strcmp (argv[i], "-t") == 0)
     MDTimeTrial ();
   else if (strcmp (argv[i], "-x") == 0)
     MDTestSuite ();
   else
     MDFile (argv[i]);
  else
 MDFilter ();

  return (0);
}
*/

/**
  * Digests a string and prints the result.
  */
static void MDString (char *string)
{
  MD5_CTX context;
  unsigned char digest[16];
  unsigned int len = static_cast<unsigned int>(strlen (string));

  MDInit (&context);
  MDUpdate (&context, (unsigned char*)string, len);
  MDFinal (digest, &context);

 /// printf ("MD%d (\"%s\") = ", MD, string);
  MDPrint (digest);
 //// printf ("\n");
}

/** 
  * Measures the time to digest TEST_BLOCK_COUNT TEST_BLOCK_LEN-byte  blocks.
  */
static void MDTimeTrial ()
{
  MD5_CTX context;
  time_t endTime, startTime;
  unsigned char block[TEST_BLOCK_LEN], digest[16];
  unsigned int i;


 /// printf ("MD%d time trial. Digesting %d %d-byte blocks ...", MD,  TEST_BLOCK_LEN, TEST_BLOCK_COUNT);

  /** Initialize block */
  for (i = 0; i < TEST_BLOCK_LEN; i++)
 block[i] = (unsigned char)(i & 0xff);

  /** Start timer */
  time (&startTime);

  /** Digest blocks */
  MDInit (&context);
  for (i = 0; i < TEST_BLOCK_COUNT; i++)
 MDUpdate (&context, block, TEST_BLOCK_LEN);
  MDFinal (digest, &context);

  /** Stop timer */
  time (&endTime);

 // printf (" done\n");
 // printf ("Digest = ");
  MDPrint (digest);
 // printf ("\nTime = %ld seconds\n", (long)(endTime-startTime));
 /// printf ("Speed = %ld bytes/second\n",  (long)TEST_BLOCK_LEN * (long)TEST_BLOCK_COUNT/(endTime-startTime));
}

/**
  * Digests a reference suite of strings and prints the results.
  */
static void MDTestSuite ()
{
  ///printf ("MD%d test suite:\n", MD);

  MDString ("");
  MDString ("a");
  MDString ("abc");
  MDString ("message digest");
  MDString ("abcdefghijklmnopqrstuvwxyz");
  MDString ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
  MDString ("1234567890123456789012345678901234567890\1234567890123456789012345678901234567890");
}

/**
  * Digests a file and prints the result.
  */
static void MDFile (char *filename)
{
  FILE *file;
  MD5_CTX context;
  int len;
  unsigned char buffer[1024], digest[16];

  if ((file = fopen (filename, "rb")) == NULL)
 printf ("%s can't be opened\n", filename);

  else {
 MDInit (&context);
 while (len = static_cast<int>(fread(buffer, 1, 1024, file)))
   MDUpdate (&context, buffer, len);
 MDFinal (digest, &context);

 fclose (file);

 printf ("MD%d (%s) = ", MD, filename);
 MDPrint (digest);
 printf ("\n");
  }
}

/**
  * Digests the standard input and prints the result.
  */
static void MDFilter ()
{
  MD5_CTX context;
  int len;
  unsigned char buffer[16], digest[16];

  MDInit (&context);
  while (len = static_cast<int>(fread(buffer, 1, 16, stdin)))
 MDUpdate (&context, buffer, len);
  MDFinal (digest, &context);

  MDPrint (digest);
  printf ("\n");
}

/**
  * Prints a message digest in hexadecimal.
  */
static void MDPrint (unsigned char digest[16])
//unsigned char digest[16];
{

  unsigned int i;

  for (i = 0; i < 16; i++)
 printf ("%02x", digest[i]);
}

