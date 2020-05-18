#ifndef D3DES_H
#define D3DES_H
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
** File Name     : d3des.h
** Description   : SIM系统3des算法实现头文件
** Contact       : wangxy@cn.is-one.net
** Creation date : 2005-09-01
**
*******************************************************************************/

////////////////////////////////////////////////////////////////////////////////
/// INCLUDES (first system includes)


////////////////////////////////////////////////////////////////////////////////
/// DEFINES AND MACROS
#define D2_DES		/* include double-length support */
#define D3_DES		/* include triple-length support */

#ifdef D3_DES
#ifndef D2_DES
#define D2_DES		/* D2_DES is needed for D3_DES */
#endif
#endif

#define EN0	0	/* MODE == encrypt */
#define DE1	1	/* MODE == decrypt */

    
////////////////////////////////////////////////////////////////////////////////
/// TYPE DEFINITIONS (except classes, first type forward declarations)

/** A useful alias on 68000-ish machines, but NOT USED HERE. */

typedef union {
	unsigned long blok[2];
	unsigned short word[4];
	unsigned char byte[8];
	} M68K;
    
    
////////////////////////////////////////////////////////////////////////////////
/// CONSTANT DEFINITIONS 
    
    
////////////////////////////////////////////////////////////////////////////////
/// GLOBAL CONSTANT AND GLOBAL VARIABLE EXTERN DECLARATIONS
    
    
////////////////////////////////////////////////////////////////////////////////
/// INLINED FUNCTIONS
    
    
 ///#############################################################################
 /// CLASS DEFINITIONS
    
 ////////////////////////////////////////////////////////////////////////////////
 /// Class Name  : 
 /// Description : 
 /// See Also    : @see 

/**
  * hexkey[8]     MODE
  * Sets the internal key register according to the hexadecimal
  * key contained in the 8 bytes of hexkey, according to the DES,
  * for encryption or decryption according to MODE.
  */
  extern void deskey(unsigned char *, short);

/**
  * cookedkey[32]
  * Loads the internal key register with the data in cookedkey.
  */
  extern void usekey(unsigned long *);


/**
  * cookedkey[32]
  *  Copies the contents of the internal key register into the storage
  * located at &cookedkey[0].
  */
  extern void cpkey(unsigned long *);

/**
  * from[8]	      to[8]
  * Encrypts/Decrypts (according to the key currently loaded in the
  * internal key register) one block of eight bytes at address 'from'
  * into the block at address 'to'.  They can be the same.
  */
  extern void des(unsigned char *, unsigned char *);

#ifdef D2_DES

#define desDkey(a,b)	des2key((a),(b))
/**
  * hexkey[16]     MODE
  * Sets the internal key registerS according to the hexadecimal
  * keyS contained in the 16 bytes of hexkey, according to the DES,
  * for DOUBLE encryption or decryption according to MODE.
  * NOTE: this clobbers all three key registers!
  */
  extern void des2key(unsigned char *, short);

/**
  * from[8]	      to[8]
  * Encrypts/Decrypts (according to the keyS currently loaded in the
  * internal key registerS) one block of eight bytes at address 'from'
  * into the block at address 'to'.  They can be the same.
  */
  extern void Ddes(unsigned char *, unsigned char *);

/**
  * from[16]	      to[16]
  * Encrypts/Decrypts (according to the keyS currently loaded in the
  * internal key registerS) one block of SIXTEEN bytes at address 'from'
  * into the block at address 'to'.  They can be the same.
  */
  extern void D2des(unsigned char *, unsigned char *);

/**
  * password,	single-length key[8]
  * With a double-length default key, this routine hashes a NULL-terminated
  * string into an eight-byte random-looking key, suitable for use with the
  * deskey() routine.
  */
  extern void makekey(char *, unsigned char *);

#define makeDkey(a,b)	make2key((a),(b))
/**
  * password,	double-length key[16]
  * With a double-length default key, this routine hashes a NULL-terminated
  * string into a sixteen-byte random-looking key, suitable for use with the
  * des2key() routine.
  */
  extern void make2key(char *, unsigned char *);


#ifndef D3_DES	/* D2_DES only */

#define useDkey(a)	use2key((a))
#define cpDkey(a)	cp2key((a))

/**
  * cookedkey[64]
  * Loads the internal key registerS with the data in cookedkey.
  * NOTE: this clobbers all three key registers!
  */
  extern void use2key(unsigned long *);


/**
  * cookedkey[64]
  * Copies the contents of the internal key registerS into the storage
  * located at &cookedkey[0].
  */
  extern void cp2key(unsigned long *);

#else	/* D3_DES too */

#define useDkey(a)	use3key((a))
#define cpDkey(a)	cp3key((a))

/** 
  * hexkey[24]     MODE
  * Sets the internal key registerS according to the hexadecimal
  * keyS contained in the 24 bytes of hexkey, according to the DES,
  * for DOUBLE encryption or decryption according to MODE.
  */
  extern void des3key(unsigned char *, short);


/**
  * cookedkey[96]
  * Loads the 3 internal key registerS with the data in cookedkey.
  */
  extern void use3key(unsigned long *);

/**	cookedkey[96]
  * Copies the contents of the 3 internal key registerS into the storage
  * located at &cookedkey[0].
  */
  extern void cp3key(unsigned long *);

/**
  * password,	triple-length key[24]
  * With a triple-length default key, this routine hashes a NULL-terminated
  * string into a twenty-four-byte random-looking key, suitable for use with
  * the des3key() routine.
  */
  extern void make3key(char *, unsigned char *);

#endif	/* D3_DES */
#endif	/* D2_DES */

/* d3des.h V5.09 rwo 9208.04 15:06 Graven Imagery
 ********************************************************************/


#endif /// D3DES_H end 
