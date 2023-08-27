/***********************************************
 *      SDL2 + OpenGL graphics driver          *
 * KeyJ 2023                                   *
 ***********************************************/

#include <stdlib.h>

#include "main.h"

#ifdef USE_SDL

#include "mem_map.h"
#include "driver.h"

extern MEM_ZONE_METHODS SDLdrv_Methods;

EXTERN int FullScreen = TRUE;
static const char* WindowTitle = "Bomb - State of Mind";

/********************************************************/

#ifdef _WIN32
   #define GLAPIENTRY __stdcall
#else
   #define GLAPIENTRY
#endif
#define GLAPIENTRYP GLAPIENTRY *
#define GLAPI static

typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef void GLvoid;
typedef   signed char  GLbyte;
typedef unsigned char  GLubyte;
typedef   signed short GLshort;
typedef unsigned short GLushort;
typedef   signed int   GLint;
typedef unsigned int   GLuint;
typedef int GLsizei;
typedef float GLfloat;
typedef float GLclampf;
typedef unsigned int GLhandleARB;

#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_TEXTURE_2D 0x0DE1
#define GL_RGB 0x1907
#define GL_NEAREST 0x2600
#define GL_LINEAR 0x2601
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_UNSIGNED_SHORT_5_6_5 0x8363
#define GL_UNSIGNED_SHORT_5_6_5_REV 0x8364

typedef void (GLAPIENTRYP PFNGLCLEARPROC)(GLbitfield mask);
GLAPI PFNGLCLEARPROC glClear;

typedef void (GLAPIENTRYP PFNGLENABLEPROC)(GLenum cap);
GLAPI PFNGLENABLEPROC glEnable;

typedef void (GLAPIENTRYP PFNGLGENTEXTURESPROC)(GLsizei n, GLuint *textures);
GLAPI PFNGLGENTEXTURESPROC glGenTextures;

typedef void (GLAPIENTRYP PFNGLBINDTEXTUREPROC)(GLenum target, GLuint texture);
GLAPI PFNGLBINDTEXTUREPROC glBindTexture;

typedef void (GLAPIENTRYP PFNGLTEXPARAMETERIPROC)(GLenum target, GLenum pname, GLint param);
GLAPI PFNGLTEXPARAMETERIPROC glTexParameteri;

typedef void (GLAPIENTRYP PFNGLTEXIMAGE2DPROC)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
GLAPI PFNGLTEXIMAGE2DPROC glTexImage2D;

typedef void (GLAPIENTRYP PFNGLGETINTEGERVPROC)(GLenum pname, GLint *data);
GLAPI PFNGLGETINTEGERVPROC glGetIntegerv;



typedef void (GLAPIENTRYP PFNGLBEGINPROC)(GLenum mode);
GLAPI PFNGLBEGINPROC glBegin;
typedef void (GLAPIENTRYP PFNGLENDPROC)(void);
GLAPI PFNGLENDPROC glEnd;
typedef void (GLAPIENTRYP PFNGLVERTEX2FPROC)(GLfloat x, GLfloat y);
GLAPI PFNGLVERTEX2FPROC glVertex2f;
typedef void (GLAPIENTRYP PFNGLTEXCOORD2FPROC)(GLfloat s, GLfloat t);
GLAPI PFNGLTEXCOORD2FPROC glTexCoord2f;


static int LoadGL(void)
{
   #define LOAD_PROC(type, name) \
      name = (type) SDL_GL_GetProcAddress(#name); \
      if (!name) { Out_Error("can not load OpenGL function '%s'", #name); return 0; }
   LOAD_PROC(PFNGLCLEARPROC,         glClear);
   LOAD_PROC(PFNGLENABLEPROC,        glEnable);
   LOAD_PROC(PFNGLGENTEXTURESPROC,   glGenTextures);
   LOAD_PROC(PFNGLBINDTEXTUREPROC,   glBindTexture);
   LOAD_PROC(PFNGLTEXPARAMETERIPROC, glTexParameteri);
   LOAD_PROC(PFNGLTEXIMAGE2DPROC,    glTexImage2D);
   LOAD_PROC(PFNGLGETINTEGERVPROC,   glGetIntegerv);

LOAD_PROC(PFNGLBEGINPROC, glBegin);
LOAD_PROC(PFNGLENDPROC, glEnd);
LOAD_PROC(PFNGLVERTEX2FPROC, glVertex2f);
LOAD_PROC(PFNGLTEXCOORD2FPROC, glTexCoord2f);

   return 1;
}

/********************************************************/

static INT SDLdrv_Startup(MEM_ZONE_SDL *drv, INT width, INT height)
{
   if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)) { return 0; }

   drv->The_Window = SDL_CreateWindow(WindowTitle,
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      1024, 768,
      (FullScreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0) | SDL_WINDOW_OPENGL);
   if (!drv->The_Window) { return 0; }

   if (FullScreen) { SDL_ShowCursor(SDL_DISABLE); }

   SDL_GL_SetAttribute(SDL_GL_RED_SIZE,     8);
   SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,   8);
   SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,    8);
   SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,   8);
   SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,   0);
   SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
   SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, SDL_TRUE);
//   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
//   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
//   SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,  SDL_GL_CONTEXT_PROFILE_CORE);

   drv->The_Context = SDL_GL_CreateContext(drv->The_Window);
   if (!drv->The_Context) { return 0; }

   SDL_GL_MakeCurrent(drv->The_Window, drv->The_Context);
   SDL_GL_SetSwapInterval(1);

   if (!LoadGL()) { return 0; }

   glGenTextures(1, &drv->The_Texture);
   glBindTexture(GL_TEXTURE_2D, drv->The_Texture);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   return 1;
}

static void SDLdrv_ShowFrame(MEM_ZONE_SDL *drv)
{
   glClear(GL_COLOR_BUFFER_BIT);

   glBindTexture(GL_TEXTURE_2D, drv->The_Texture);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, drv->Display_Width, drv->Display_Height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, (const GLvoid*)drv->Screen_Buffer);
glEnable(GL_TEXTURE_2D);
glBegin(7 /*GL_QUADS*/);
glTexCoord2f(0.f, 0.f);  glVertex2f(-1.f, +1.f);
glTexCoord2f(0.f, 1.f);  glVertex2f(-1.f, -1.f);
glTexCoord2f(1.f, 1.f);  glVertex2f(+1.f, -1.f);
glTexCoord2f(1.f, 0.f);  glVertex2f(+1.f, +1.f);
glEnd();

   SDL_GL_SwapWindow(drv->The_Window);
}

static EVENT_TYPE SDLdrv_ProcessEvents(MEM_ZONE_SDL *drv)
{
   SDL_Event ev;
   while (SDL_PollEvent(&ev))
   {
      switch (ev.type)
      {
         case SDL_KEYDOWN:
            drv->Event.Key = ev.key.keysym.sym;
            return DRV_KEY_PRESS;
         case SDL_QUIT:
            drv->Event.Key = 'q';
            return DRV_KEY_PRESS;
         default:
            break;
      }
   }
   return DRV_NULL_EVENT;
}

static void SDLdrv_Shutdown(MEM_ZONE_SDL *drv)
{
   SDL_GL_MakeCurrent(NULL, NULL);
   SDL_GL_DeleteContext(drv->The_Context);
   SDL_DestroyWindow(drv->The_Window);
   SDL_Quit();
}

/********************************************************/

static INT SDLdrv_Propagate( MEM_ZONE *M, INT X, INT Y, INT Width, INT Height )
{
   Out_Message("STUB! SDLdrv_Propagate(%d,%d, %dx%d)", X, Y, Width, Height);
   return TRUE;
}

static INT SDLdrv_Flush( MEM_ZONE *M )
{
   SDLdrv_ShowFrame( (MEM_ZONE_SDL *)M );
   return TRUE;
}

static void SDLdrv_Destroy( MEM_ZONE *M )
{
   SDLdrv_Shutdown( (MEM_ZONE_SDL *)M );
   free(((MEM_ZONE_SDL *)M)->Screen_Buffer);
}

static PIXEL *SDLdrv_Get_Scanline( MEM_ZONE *M, INT Y )
{
   MEM_ZONE_SDL *SDL = (MEM_ZONE_SDL *)M;
   return (PIXEL*) &SDL->Screen_Buffer[Y * SDL->Display_Width];
}

static ZONE_CLIP_TYPE SDLdrv_Set_Position( MEM_ZONE *M, INT X, INT Y, INT Backup )
{
   Out_Message("STUB! SDLdrv_Set_Position(%d,%d, %d)", X, Y, Backup);
   return ZONE_NO_CLIP;
}

static MEM_ZONE_DRIVER *SDLdrv_Change_Size( MEM_ZONE_DRIVER *M )
{
   MEM_ZONE_SDL *SDL = (MEM_ZONE_SDL *)M;
   if ( SDL==NULL ) return( NULL );
   return( (MEM_ZONE_DRIVER *)SDL );
}

EXTERN EVENT_TYPE SDLdrv_Get_Event( MEM_ZONE_DRIVER *M )
{
   if ( M==NULL || !(M->Type & MEM_IS_DISPLAY) ) return( DRV_NULL_EVENT );
   MEM_ZONE_SDL *SDL = (MEM_ZONE_SDL *)M;
   SDL->Event.Modifier = DRV_NO_MODIFIER;
   SDL->Event.Key = 0;
   SDL->Event.Event = SDLdrv_ProcessEvents( SDL );
   return SDL->Event.Event;
}

static MEM_ZONE_DRIVER *SDLdrv_Setup( _G_DRIVER_ *Drv, void *Dummy )
{
   DRIVER_ARG *Args = (DRIVER_ARG *)Dummy;

   MEM_ZONE *M = (MEM_ZONE *)New_MEM_Zone( MEM_ZONE_SDL, &SDLdrv_Methods );
   if ( M==NULL ) Exit_Upon_Mem_Error( "driver MEM_ZONE", sizeof( MEM_ZONE_SDL ) );
   MEM_ZONE_SDL *SDL = (MEM_ZONE_SDL *)M;
   M->Type = (ZONE_TYPE)( M->Type | MEM_IS_DISPLAY );
   SDL->Refresh = _DIRECT_X_;
   return( (MEM_ZONE_DRIVER *)SDL );
}

static void SDLdrv_Print_Info( MEM_ZONE_DRIVER *M )
{
   Out_Message("STUB! SDLdrv_Print_Info()");
}

static INT SDLdrv_Adapt( MEM_ZONE_DRIVER *M, void *Dummy )
{
   DRIVER_ARG *Args = (DRIVER_ARG *)Dummy;
   MEM_ZONE_SDL *SDL = (MEM_ZONE_SDL *)M;
   if ( SDL==NULL ) return( 0 );
   INT i;
   for( i=0; i<Args->Nb_Modes; ++i )
   {
      MEM_IMG *Req = &Args->Modes[i];
      if (IMG_Format(Req) != FMT_565) { return i-1; }
   }
   return i;
}

static MEM_ZONE *SDLdrv_Open( MEM_ZONE_DRIVER *M, INT Nb )
{
   if ( M==NULL || !(M->Type&MEM_IS_DISPLAY) ) return( NULL );
   MEM_ZONE_SDL *SDL = (MEM_ZONE_SDL *)M;

   if ( Nb<1 ) // Clean all
   {
      ZONE_DESTROY( M );
      return( (MEM_ZONE *)M );
   }

   if ( Nb>SDL->Nb_Req_Modes )
   {
      SET_DRV_ERR2( "Invalid mode #%d request", (STRING)(INT)Nb );
      return( NULL );
   }

   MEM_IMG *Req = &SDL->Req_Modes[Nb-1];
   INT Width      = IMG_Width (Req);
   INT Height     = IMG_Height(Req);
   FORMAT Format  = IMG_Format(Req);
   FORMAT Format2 = (FORMAT)Req->Pad;
   if (Format != FMT_565)
   {
      SET_DRV_ERR2( "Invalid format #0x04%X request", (STRING)(INT)Format );
      return( NULL );
   }

   MEM_Width(M) = Width;
   MEM_Height(M) = Height;
   MEM_Pad(M) = 0;
   MEM_BpS( M ) = 2 * Width;
   MEM_Base_Ptr( M ) = NULL;
   MEM_Quantum(M) = Format_Depth( Format2 );
   MEM_Format(M) = Format2;
   M->Type = (ZONE_TYPE)( M->Type | MEM_IS_DISPLAY);

   SDL->Display_Width  = Width;
   SDL->Display_Height = Height;
   SDL->Screen_Buffer  = malloc(Width * Height * 2);
   if (!SDL->Screen_Buffer)
   {
      SET_DRV_ERR( "Out of memory" );
      return( NULL );
   }

   if (!SDLdrv_Startup(SDL, Width, Height))
   {
      SET_DRV_ERR2( "SDLdrv_Startup failed: %s", SDL_GetError() );
      return( NULL );
   }

   return (MEM_ZONE *)M;
}

EXTERN INT SDLdrv_Change_CMap( MEM_ZONE_DRIVER *M, INT Nb, COLOR_ENTRY *CMap )
{
   Out_Message("STUB! SDLdrv_Change_CMap(%d)", Nb);
   return 0;
}

/********************************************************/

static MEM_ZONE_METHODS SDLdrv_Methods = 
{
   SDLdrv_Propagate,
   SDLdrv_Flush,
   SDLdrv_Destroy,
   SDLdrv_Get_Scanline,
   SDLdrv_Set_Position,
};

EXTERN _G_DRIVER_ __G_SDL_DRIVER_ = 
{
   SDLdrv_Change_Size,
   SDLdrv_Get_Event,
   SDLdrv_Setup,
   SDLdrv_Print_Info,
   SDLdrv_Adapt,
   SDLdrv_Open,
   SDLdrv_Change_CMap
};

/********************************************************/

#endif   // USE_SDL

