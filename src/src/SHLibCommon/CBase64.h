#pragma once

#include <stdio.h>

#define B64_DEF_LINE_SIZE   72
#define B64_MIN_LINE_SIZE    4

class Base64  
{
public:
	Base64();
	virtual ~Base64();	

public:

	/**
	  * 
	  * @param 
	  * @param 
	  * @return 0 
	  * @exception 无
	  * @see 
      */
	void InitTransTable(unsigned char *chs);
   
	/**
	  * 
	  * @param 
	  * @param 
	  * @return 0 
	  * @exception 无
	  * @see 
      */
	int operate_file( int opt, char *infilename, char *outfilename );
    
	/**
	  * 
	  * @param 
	  * @param 
	  * @return 0 
	  * @exception 无
	  * @see 
	      */
	unsigned char* operate_buf( int opt, unsigned char *inbuf, unsigned int inbufsize, unsigned int& outbufsize);

protected:
	char cb64[64];
	char cd64[128];
	int linesize;
	bool beInit;

private:
	/**
	  * 
	  * @param 
	  * @param 
	  * @return 0 
	  * @exception 无
	  * @see 
	  */
	  unsigned int encodebuf( unsigned char* inbuf, unsigned int inbufsize, unsigned char* outbuf );

    /**
	  * 
	  * @param 
	  * @param 
	  * @return 0 
	  * @exception 无
	  * @see 
	  */
	  unsigned int decodebuf( unsigned char* inbuf, unsigned int inbufsize, unsigned char* outbuf );

    /**
	  * 
	  * @param 
	  * @param 
	  * @return 0 
	  * @exception 无
	  * @see 
	  */
	  void encodeblock( unsigned char in[3], unsigned char out[4], int len );

	/**
	  * 
	  * @param 
	  * @param 
	  * @return 0 
	  * @exception 无
	  * @see 
	  */
	  void encodefile( FILE *infile, FILE *outfile );

	/**
	  * 
	  * @param 
	  * @param 
	  * @return 0 
	  * @exception 无
	  * @see 
	  */
	  void decodeblock( unsigned char in[4], unsigned char out[3] );

	/**
	  * 
	  * @param 
	  * @param 
	  * @return 0 
	  * @exception 无
	  * @see 
	  */
	  void decodefile( FILE *infile, FILE *outfile );

	/**
	  * 
	  * @param 
	  * @param 
	  * @return 0 
	  * @exception 无
	  * @see 
	  */
	  char* b64_message( int errcode );

    /**
	  * 
	  * @param 
	  * @param 
	  * @return 0 
	  * @exception 无
	  * @see 
	  */
	int b64( int opt, char *infilename, char *outfilename );

};

