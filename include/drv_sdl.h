/***********************************************
 *      SDL2 + OpenGL graphics driver          *
 * KeyJ 2023                                   *
 ***********************************************/

#ifndef _DRV_SDL_H_
#define _DRV_SDL_H_

#include <stdarg.h>

/********************************************************/
/********************************************************/

#ifdef USE_SDL

/* transf.h uses a few reserved #defines which clash
 * with immintrin.h, which in turn is included by SDL.h */
#undef _X
#undef _Y
#undef _Z
#undef _W
#undef _D
#undef _U
#undef _V

#include <SDL.h>

extern int FullScreen;

typedef struct {
   MEM_ZONE_DRIVER_FIELDS

   /* generic */
   USHORT*       Screen_Buffer;
   INT           Display_Width;
   INT           Display_Height;

   /* SDL2 specific */
   SDL_Window*   The_Window;
   SDL_GLContext The_Context;

   /* OpenGL specific */
   UINT          The_Texture;
} MEM_ZONE_SDL;

extern _G_DRIVER_ __G_SDL_DRIVER_;
#define _G_SDL_DRIVER_ (&__G_SDL_DRIVER_)

#endif      // USE_SDL

/********************************************************/
/********************************************************/

#endif   // _DRV_SDL_H_

