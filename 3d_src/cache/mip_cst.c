/***********************************************
 *           Useful mip constants              *
 * Skal 98                                     *
 ***********************************************/

#include "main3d.h"

/******************************************************************/

EXTERN USHORT Mip_Width[MAX_MIP] = { 1, 2, 4, 8, 16, 32, 64, 128, 256 };
EXTERN UINT Mip_Size[MAX_MIP] = { 1*1, 2*2, 4*4, 8*8, 16*16, 32*32, 64*64, 128*128, 256*256 };
EXTERN UINT Mip_Masks[9] = { 0x0000, 0x0101, 0x0303, 0x0707, 0x0f0f, 0x1f1f, 0x3f3f, 0x7f7f, 0xffff };
EXTERN UINT Mip_Mask;

/******************************************************************/
      // this ones are prototyped in mat.h...
EXTERN FLT Mip_Scale;
EXTERN INT Cur_Mip;
EXTERN INT Cur_Mip_Slot;
EXTERN FLT Uo_Mip, Vo_Mip;

/******************************************************************/

EXTERN FLT Mip_Uo[3][MAX_MIP] = 
{
   { 1*65536.0, 2*65536.0, 4*65536.0, 8*65536.0, 16*65536.0, 32*65536.0, 64*65536.0, 128*65536.0, 256*65536.0 },
   { 0, 0, 0, 0,  0, 0, 0, 0, 0 },
   { 1*65536.0, 2*65536.0, 4*65536.0, 8*65536.0, 16*65536.0, 32*65536.0, 64*65536.0, 128*65536.0, 256*65536.0 }
};
EXTERN FLT Mip_Vo[3][MAX_MIP] = 
{
   { 0, 0, 0, 0,  0, 0, 0, 0, 0 },
   { 1*65536.0, 2*65536.0, 4*65536.0, 8*65536.0, 16*65536.0, 32*65536.0, 64*65536.0, 128*65536.0, 256*65536.0 },
   { 1*65536.0, 2*65536.0, 4*65536.0, 8*65536.0, 16*65536.0, 32*65536.0, 64*65536.0, 128*65536.0, 256*65536.0 }
};
EXTERN FLT Mip_Scales[MAX_MIP] =
{
   256.0*256.0, 512.0*256.0, 1024.0*256.0, 2048.0*256.0,
   4096.0*256.0, 8192.0*256.0, 16384.0*256.0, 32768.0*256.0,
   65536.0*256.0
};

EXTERN INT Mip_Slot0_Offsets[3] = { 1, 1*256, 1+1*256 };

/******************************************************************/
/******************************************************************/
