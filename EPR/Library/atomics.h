/*      File: atomics.h                                                    */


/* all system specific changes should be made here */

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
#define INTEN_NUM 32768
#define INTEN_BIT    15

/* a voxel (RGB,alpha,beta) is stored in 8 bytes */
/* 4 Bytes: RRRRRRRR|RRGGGGGG|GGGGGGBB|BBBBBBBB  */
/* 4 Bytes: aaaaaaaa|aaaaaaaa|bbbbbbbb|bbbbbbbb  */
#define RED_MASK    0xffc00000
#define GREEN_MASK  0x003ffc00
#define BLUE_MASK   0x000003ff
#define RED_SHIFT           22
#define GREEN_SHIFT         10
#define BLUE_SHIFT           0
#define ALPHA_MASK  0xffff0000
#define BETA_MASK   0x0000ffff
#define ALPHA_SHIFT         16
#define BETA_SHIFT           0

#endif

