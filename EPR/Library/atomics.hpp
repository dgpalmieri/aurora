/*      File: atomics.hpp                                                  */


/* all system-specific changes to accommodate architectural differences    */
/* should be made here                                                     */

#ifndef _ATOMICS_H

#define _ATOMICS_H

typedef unsigned long int uLong;

/* set these to the appropriate atomic integer on the system */
#if defined(MTA)

  typedef unsigned char      uByte1; /* 0..255 */
  typedef unsigned short     uByte4; /* 0..4,294,967,296 */
  typedef unsigned int       uByte8; /* 0..billions&billions */

#elif defined(SUN)

  typedef unsigned char      uByte1; /* 0..255 */
  typedef unsigned int       uByte4; /* 0..4,294,967,296 */
  typedef unsigned long long uByte8; /* 0..billions&billions */

#elif defined(SP2)

  typedef unsigned char      uByte1; /* 0..255 */
  typedef unsigned int       uByte4; /* 0..4,294,967,296 */
  typedef unsigned long long uByte8; /* 0..billions&billions */

/* also for x64 */
#elif defined(SGI32)

  typedef unsigned char      uByte1; /* 0..255 */
  typedef unsigned int       uByte4; /* 0..4,294,967,296 */
  typedef unsigned long long uByte8; /* 0..billions&billions */

#elif defined(SGI64)

  typedef unsigned char      uByte1; /* 0..255 */
  typedef unsigned int       uByte4; /* 0..4,294,967,296 */
  typedef unsigned long      uByte8; /* 0..billions&billions */

#elif defined(OSX)

  typedef unsigned char      uByte1; /* 0..255 */
  typedef unsigned int       uByte4; /* 0..4,294,967,296 */
  typedef unsigned long long uByte8; /* 0..billions&billions */

#endif

/* set how many bits of resolution to store an intensity    */
/* need to have room for 2 mults without overflow in uByte4 */

/* I wasn't sure if we wanted to change these to "C++ style" */
/* so I did it and we can undo it if needed */
const int INTEN_NUM = 32768;
const int INTEN_BIT = 15;

/* a voxel (RGB,alpha,beta) is stored in 8 bytes */
/* 4 Bytes: RRRRRRRR|RRGGGGGG|GGGGGGBB|BBBBBBBB  */
/* 4 Bytes: aaaaaaaa|aaaaaaaa|bbbbbbbb|bbbbbbbb  */
/* Dr. Genetti: 10 bits for red, 12 green, 10 blue, 12 for alpha beta. */
/* Dr. Genetti: These are packed into 2 4 byte numbers. */
/* Dr. Genetti: Was not portable access to 8 byte unsigned ints on platforms at the time */

/* Dr. Genetti: I do not think the following values are used for aurora rendering */

const uByte8 RED_MASK = 0xffc00000;
const uByte8 GREEN_MASK = 0x003ffc00;
const uByte8 BLUE_MASK = 0x000003ff;

const int RED_SHIFT = 22;
const int GREEN_SHIFT = 10;
const int BLUE_SHIFT = 0;

const uByte8 ALPHA_MASK = 0xffff0000;
const uByte8 BETA_MASK = 0x0000ffff;

const int ALPHA_SHIFT = 16;
const int BETA_SHIFT = 0;

#endif
