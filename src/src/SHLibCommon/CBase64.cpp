#include <string.h>
#include "CBase64.h"

Base64::Base64()
{
	linesize = B64_DEF_LINE_SIZE;
	beInit = false;
}

Base64::~Base64()
{
}

void Base64::InitTransTable(unsigned char *chs)//64bytes
{
	int i;

	for (i=0; i <= 63; i++)
	{
		cb64[i] = chs[i];
	}

	for (i=0; i <= 127; i++)
	{
		cd64[i] = '$';
	}
	
	for (i=0; i <= 63; i++)
	{
		cd64[cb64[i]-43] = (char)(i + 62);
	}

	beInit = true;
}
/**
  * encodeblock
  *
  * encode 3 8-bit binary bytes as 4 '6-bit' characters
  */
void Base64::encodeblock( unsigned char in[3], unsigned char out[4], int len )
{
    out[0] = cb64[ in[0] >> 2 ];
    out[1] = cb64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
    out[2] = (unsigned char) (len > 1 ? cb64[ ((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ] : '=');
    out[3] = (unsigned char) (len > 2 ? cb64[ in[2] & 0x3f ] : '=');
}

/**
  * encode
  *
  * base64 encode a stream adding padding and line breaks as per spec.
  */
void Base64::encodefile( FILE *infile, FILE *outfile)
{
    unsigned char in[3], out[4];
    int i, len, blocksout = 0;

    while( !feof( infile ) ) 
	{
        len = 0;
        for( i = 0; i < 3; i++ ) 
		{
            in[i] = (unsigned char) getc( infile );
            if( !feof( infile ) ) 
			{
                len++;
            }
            else 
			{
                in[i] = 0;
            }
        }
        if( len ) 
		{
            encodeblock( in, out, len );
            for( i = 0; i < 4; i++ ) 
			{
                putc( out[i], outfile );
            }
            blocksout++;
        }
        if( blocksout >= (linesize/4) || feof( infile ) ) 
		{
            if( blocksout ) 
			{
                fprintf( outfile, "\r\n" );
            }
            blocksout = 0;
        }
    }
}

unsigned int Base64::encodebuf(unsigned char *inbuf, unsigned int inbufsize, unsigned char *outbuf)
{
	unsigned char in[3], out[4];
	unsigned int inIndex = 0, outIndex = 0;
    int i, len, blocksout = 0;
	
    while( inIndex<inbufsize ) 
	{
        len = 0;
        for( i = 0; i < 3; i++ ) 
		{
            in[i] = inbuf[inIndex];
            if( inIndex<inbufsize ) 
			{
                len++;
            }
            else 
			{
                in[i] = 0;
            }
			inIndex++;
        }
		
        if( len ) 
		{
            encodeblock( in, out, len );
            for( i = 0; i < 4; i++ ) 
			{
                outbuf[outIndex++] = out[i];
            }
            blocksout++;
        }
        if( blocksout >= (linesize/4) || inIndex>=inbufsize ) 
		{
            if( blocksout ) 
			{
                outbuf[outIndex++] = '\r';
				outbuf[outIndex++] = '\n';
            }
            blocksout = 0;
        }
    }
	
	return outIndex;
}

/**
  * decodeblock
  *
  * decode 4 '6-bit' characters into 3 8-bit binary bytes
  */
void Base64::decodeblock( unsigned char in[4], unsigned char out[3] )
{   
    out[ 0 ] = (unsigned char ) (in[0] << 2 | in[1] >> 4);
    out[ 1 ] = (unsigned char ) (in[1] << 4 | in[2] >> 2);
    out[ 2 ] = (unsigned char ) (((in[2] << 6) & 0xc0) | in[3]);
}

/**
  * decode
  *
  * decode a base64 encoded stream discarding padding, line breaks and noise
  */
void Base64::decodefile( FILE *infile, FILE *outfile )
{
    unsigned char in[4], out[3], v;
    int i, len;

    while( !feof( infile ) ) 
	{
        for( len = 0, i = 0; i < 4 && !feof( infile ); i++ ) 
		{
            v = 0;
            while( !feof( infile ) && v == 0 ) 
			{
                v = (unsigned char) getc( infile );
                v = (unsigned char) ((v < 43 || v > 122) ? 0 : cd64[ v - 43 ]);
                if( v ) 
				{
                    v = (unsigned char) ((v == '$') ? 0 : v - 61);
                }
            }

            if( !feof( infile ) ) 
			{
                len++;
                if( v ) 
				{
                    in[ i ] = (unsigned char) (v - 1);
                }
            }
            else 
			{
                in[i] = 0;
            }
        }

        if( len ) 
		{
            decodeblock( in, out );
            for( i = 0; i < len - 1; i++ ) 
			{
                putc( out[i], outfile );
            }
        }
    }
}

unsigned int Base64::decodebuf(unsigned char *inbuf, unsigned int inbufsize, unsigned char *outbuf)
{
    unsigned char in[4], out[3], v;
	unsigned int inIndex = 0, outIndex = 0;
    int i, len;
	
    while( inIndex<inbufsize ) 
	{
        for( len = 0, i = 0; i < 4 && inIndex<inbufsize; i++ ) 
		{
            v = 0;
            while( inIndex<inbufsize && v == 0 ) 
			{
                v = inbuf[inIndex];
                v = (unsigned char) ((v < 43 || v > 122) ? 0 : cd64[ v - 43 ]);
                if( v ) 
				{
                    v = (unsigned char) ((v == '$') ? 0 : v - 61);
                }
				inIndex++;
            }
			
            if( inIndex<inbufsize ) 
			{
                len++;
                if( v ) 
				{
                    in[ i ] = (unsigned char) (v - 1);
                }
            }
            else 
			{
                in[i] = 0;
            }
        }
		
        if( len ) 
		{
            decodeblock( in, out );
            for( i = 0; i < len - 1; i++ ) 
			{
				outbuf[outIndex++] = out[i];
            }
        }
    }
	
	return outIndex;
}

/**
  * returnable errors
  * Error codes returned to the operating system.
  *
  */
#define B64_SYNTAX_ERROR        1
#define B64_FILE_ERROR          2
#define B64_FILE_IO_ERROR       3
#define B64_ERROR_OUT_CLOSE     4
#define B64_LINE_SIZE_TO_MIN    5

/**
  * b64_message
  *
  * Gather text messages in one place.
  *
  */
char* Base64::b64_message( int errcode )
{
    #define B64_MAX_MESSAGES 6
    char *msgs[ B64_MAX_MESSAGES ] = 
	{
            "b64:000:Invalid Message Code.",
            "b64:001:Syntax Error -- check help for usage.",
            "b64:002:File Error Opening/Creating Files.",
            "b64:003:File I/O Error -- Note: output file not removed.",
            "b64:004:Error on output file close.",
            "b64:004:linesize set to minimum."
    };
    char *msg = msgs[ 0 ];

    if( errcode > 0 && errcode < B64_MAX_MESSAGES ) 
	{
        msg = msgs[ errcode ];
    }

    return( msg );
}

/**
  * b64
  * 'engine' that opens streams and calls encode/decode
  */

int Base64::b64( int opt, char *infilename, char *outfilename )
{
    FILE *infile;
    int retcode = B64_FILE_ERROR;

    if( !infilename ) 
	{
        infile = stdin;
    }
    else 
	{
        infile = fopen( infilename, "rb" );
    }

    if( !infile ) 
	{
        perror( infilename );
    }
    else 
	{
        FILE *outfile;
        if( !outfilename ) 
		{
            outfile = stdout;
        }
        else 
		{
            outfile = fopen( outfilename, "wb" );
        }

        if( !outfile ) 
		{
            perror( outfilename );
        }
        else 
		{
            if( opt == 'e' ) 
			{
                encodefile( infile, outfile );
            }
            else 
			{
                decodefile( infile, outfile );
            }

            if (ferror( infile ) || ferror( outfile )) 
			{
                retcode = B64_FILE_IO_ERROR;
            }
            else 
			{
                 retcode = 0;
            }
            if( outfile != stdout ) 
			{
                if( fclose( outfile ) != 0 ) 
				{
                    perror( b64_message( B64_ERROR_OUT_CLOSE ) );
                    retcode = B64_FILE_IO_ERROR;
                }
            }
        }
        if( infile != stdin ) 
		{
            fclose( infile );
        }
    }

    return( retcode );
}

/**
  * main
  *
  * parse and validate arguments and call b64 engine or help
  */
int Base64::operate_file( int opt, char *infilename, char *outfilename )
{
	if(beInit==false)
	{
		unsigned char chs[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
		InitTransTable(chs);
	}
	
    int retcode = 0;
	
    switch( opt ) 
	{
        case 'e':
        case 'd':
				retcode = b64( opt, infilename, outfilename );
				break;
         default:
            retcode = B64_SYNTAX_ERROR;
    }

    return( retcode );
}



unsigned char* Base64::operate_buf(int opt, unsigned char *inbuf, unsigned int inbufsize, unsigned int &outbufsize)
{
	if(beInit==false)
	{
		unsigned char chs[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
		InitTransTable(chs);
	}
	
	unsigned int outbufsize0 = 0;
	unsigned char* outbuf = NULL;

    switch( opt ) 
	{
	case 'e':
		{
			if(inbufsize==1)
				outbufsize0 = 10;
			else
				outbufsize0 = ((inbufsize-1)/3)*4 + 6;
			outbufsize0 = inbufsize*6;
			outbuf = new unsigned char[outbufsize0];
			memset( outbuf,0,outbufsize0 );
			outbufsize = encodebuf( inbuf, inbufsize, outbuf );

			outbufsize = outbufsize0;
			break;
		}
	case 'd':
		{
			outbufsize0 = inbufsize;//((inbufsize-6)/4)*3 + 3;
			outbuf = new unsigned char[outbufsize0];
			memset( outbuf,0,outbufsize0 );
			outbufsize = decodebuf( inbuf, inbufsize, outbuf );

		//	outbufsize = outbufsize0;
			break;
		}
	default:
		{
			outbufsize = 0;
		}
    }

	return outbuf;
}
