/***********************************************
 *      SDL2 + OpenGL graphics driver          *
 * KeyJ 2023                                   *
 ***********************************************/

#include <stdlib.h>

#include "main.h"

#ifdef USE_SDL

#include "mem_map.h"
#include "driver.h"

#ifdef NDEBUG
   #define GL_DEBUG 0
#else
   #define GL_DEBUG 1
#endif

const float ASPECT_RATIO = 320.f / 200.f;
#define DEFAULT_WINDOW_SIZE 1280,800

extern MEM_ZONE_METHODS SDLdrv_Methods;

EXTERN int FullScreen = TRUE;
static const char* WindowTitle = "Bomb - State of Mind";

#ifndef NDEBUG
static Uint32 StartTime  = 0;
static Uint32 FrameCount = 0;
#endif

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
typedef char GLchar;
typedef int GLsizei;
typedef float GLfloat;
typedef float GLclampf;
typedef unsigned int GLhandleARB;

#define GL_TRIANGLE_STRIP 5
#define GL_QUADS 7
#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_VIEWPORT 0x0BA2
#define GL_TEXTURE_2D 0x0DE1
#define GL_RGB 0x1907
#define GL_NEAREST 0x2600
#define GL_LINEAR 0x2601
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_MAJOR_VERSION 0x821B
#define GL_MINOR_VERSION 0x821C
#define GL_UNSIGNED_SHORT_5_6_5 0x8363
#define GL_UNSIGNED_SHORT_5_6_5_REV 0x8364
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31
#define GL_DEBUG_OUTPUT 0x92E0

typedef void (GLAPIENTRYP PFNGLCLEARPROC)(GLbitfield mask);
GLAPI PFNGLCLEARPROC glClear;
typedef void (GLAPIENTRYP PFNGLENABLEPROC)(GLenum cap);
GLAPI PFNGLENABLEPROC glEnable;
typedef void (GLAPIENTRYP PFNGLVIEWPORTPROC)(GLint x, GLint y, GLsizei width, GLsizei height);
GLAPI PFNGLVIEWPORTPROC glViewport;
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
typedef GLuint (GLAPIENTRYP PFNGLCREATEPROGRAMPROC)(void);
GLAPI PFNGLCREATEPROGRAMPROC glCreateProgram;
typedef GLuint (GLAPIENTRYP PFNGLCREATESHADERPROC)(GLenum type);
GLAPI PFNGLCREATESHADERPROC glCreateShader;
typedef void (GLAPIENTRYP PFNGLSHADERSOURCEPROC)(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
GLAPI PFNGLSHADERSOURCEPROC glShaderSource;
typedef void (GLAPIENTRYP PFNGLCOMPILESHADERPROC)(GLuint shader);
GLAPI PFNGLCOMPILESHADERPROC glCompileShader;
typedef void (GLAPIENTRYP PFNGLATTACHSHADERPROC)(GLuint program, GLuint shader);
GLAPI PFNGLATTACHSHADERPROC glAttachShader;
typedef void (GLAPIENTRYP PFNGLLINKPROGRAMPROC)(GLuint program);
GLAPI PFNGLLINKPROGRAMPROC glLinkProgram;
typedef void (GLAPIENTRYP PFNGLUSEPROGRAMPROC)(GLuint program);
GLAPI PFNGLUSEPROGRAMPROC glUseProgram;
typedef GLint (GLAPIENTRYP PFNGLGETUNIFORMLOCATIONPROC)(GLuint program, const GLchar *name);
GLAPI PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
typedef void (GLAPIENTRYP PFNGLUNIFORM2FPROC)(GLint location, GLfloat v0, GLfloat v1);
GLAPI PFNGLUNIFORM2FPROC glUniform2f;
typedef void (GLAPIENTRYP PFNGLGENVERTEXARRAYSPROC)(GLsizei n, GLuint *arrays);
GLAPI PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
typedef void (GLAPIENTRYP PFNGLBINDVERTEXARRAYPROC)(GLuint array);
GLAPI PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
typedef void (GLAPIENTRYP PFNGLDRAWARRAYSPROC)(GLenum mode, GLint first, GLsizei count);
GLAPI PFNGLDRAWARRAYSPROC glDrawArrays;
typedef void (GLAPIENTRYP PFNGLBEGINPROC)(GLenum mode);
GLAPI PFNGLBEGINPROC glBegin;
typedef void (GLAPIENTRYP PFNGLENDPROC)(void);
GLAPI PFNGLENDPROC glEnd;
typedef void (GLAPIENTRYP PFNGLVERTEX2FPROC)(GLfloat x, GLfloat y);
GLAPI PFNGLVERTEX2FPROC glVertex2f;
typedef void (GLAPIENTRYP PFNGLTEXCOORD2FPROC)(GLfloat s, GLfloat t);
GLAPI PFNGLTEXCOORD2FPROC glTexCoord2f;

#if GL_DEBUG
typedef void (GLAPIENTRYP PFNGLGETSHADERINFOLOGPROC)(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
GLAPI PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
typedef void (GLAPIENTRYP PFNGLGETPROGRAMINFOLOGPROC)(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
GLAPI PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
typedef GLenum (GLAPIENTRYP PFNGLGETERRORPROC)(void);
GLAPI PFNGLGETERRORPROC glGetError;
typedef void (GLAPIENTRY *GLDEBUGPROC)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);
typedef void (GLAPIENTRYP PFNGLDEBUGMESSAGECALLBACKPROC)(GLDEBUGPROC callback, void* userParam);
GLAPI PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback;
#endif

static int LoadGL(void)
{
   #define LOAD_PROC(req, type, name) \
      name = (type) SDL_GL_GetProcAddress(#name); \
      if (!name && req) { Out_Error("can not load OpenGL function '%s'", #name); return 0; }
   LOAD_PROC(1, PFNGLCLEARPROC,                glClear);
   LOAD_PROC(1, PFNGLENABLEPROC,               glEnable);
   LOAD_PROC(1, PFNGLVIEWPORTPROC,             glViewport);
   LOAD_PROC(1, PFNGLGENTEXTURESPROC,          glGenTextures);
   LOAD_PROC(1, PFNGLBINDTEXTUREPROC,          glBindTexture);
   LOAD_PROC(1, PFNGLTEXPARAMETERIPROC,        glTexParameteri);
   LOAD_PROC(1, PFNGLTEXIMAGE2DPROC,           glTexImage2D);
   LOAD_PROC(1, PFNGLGETINTEGERVPROC,          glGetIntegerv);
   LOAD_PROC(0, PFNGLCREATEPROGRAMPROC,        glCreateProgram);
   LOAD_PROC(0, PFNGLCREATESHADERPROC,         glCreateShader);
   LOAD_PROC(0, PFNGLSHADERSOURCEPROC,         glShaderSource);
   LOAD_PROC(0, PFNGLCOMPILESHADERPROC,        glCompileShader);
   LOAD_PROC(0, PFNGLATTACHSHADERPROC,         glAttachShader);
   LOAD_PROC(0, PFNGLLINKPROGRAMPROC,          glLinkProgram);
   LOAD_PROC(0, PFNGLUSEPROGRAMPROC,           glUseProgram);
   LOAD_PROC(0, PFNGLGETUNIFORMLOCATIONPROC,   glGetUniformLocation);
   LOAD_PROC(0, PFNGLUNIFORM2FPROC,            glUniform2f);
   LOAD_PROC(0, PFNGLGENVERTEXARRAYSPROC,      glGenVertexArrays);
   LOAD_PROC(0, PFNGLBINDVERTEXARRAYPROC,      glBindVertexArray);
   LOAD_PROC(0, PFNGLDRAWARRAYSPROC,           glDrawArrays);
   LOAD_PROC(0, PFNGLBEGINPROC,                glBegin);
   LOAD_PROC(0, PFNGLENDPROC,                  glEnd);
   LOAD_PROC(0, PFNGLVERTEX2FPROC,             glVertex2f);
   LOAD_PROC(0, PFNGLTEXCOORD2FPROC,           glTexCoord2f);
#if GL_DEBUG
   LOAD_PROC(0, PFNGLGETSHADERINFOLOGPROC,     glGetShaderInfoLog);
   LOAD_PROC(0, PFNGLGETPROGRAMINFOLOGPROC,    glGetProgramInfoLog);
   LOAD_PROC(0, PFNGLGETERRORPROC,             glGetError);
   LOAD_PROC(0, PFNGLDEBUGMESSAGECALLBACKPROC, glDebugMessageCallback);
#endif
   return 1;
}

/********************************************************/

static void SDLdrv_UpdateViewport(MEM_ZONE_SDL *drv, INT vpWidth, INT vpHeight)
{
   DEBUG(Out_Message("viewport size: %dx%d", vpWidth, vpHeight));
   float w = vpHeight * ASPECT_RATIO, h = vpHeight;
   if (w > vpWidth) {
      w = vpWidth;  h = vpWidth / ASPECT_RATIO;
   }
   drv->VP_Width  =  w / vpWidth;
   drv->VP_Height = -h / vpHeight;
   if (drv->GL_Version >= 33) {
      glUniform2f(drv->Loc_uSize, drv->VP_Width, drv->VP_Height);
   }
}

#if GL_DEBUG
void GLAPIENTRY myGLdebugProc(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam) {
   DEBUG(Out_Message("[GL] src=0x%04X type=0x%04X id=0x%04X sev=0x%04X %s", source, type, id, severity, message));
}
#endif

static INT SDLdrv_Startup(MEM_ZONE_SDL *drv, INT width, INT height)
{
   if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)) {
      DEBUG(Out_Message("SDL_Init() failed: %s", SDL_GetError()));
      return 0;
   }

   drv->The_Window = SDL_CreateWindow(WindowTitle,
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, DEFAULT_WINDOW_SIZE,
      (FullScreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_RESIZABLE) | SDL_WINDOW_OPENGL);
   if (!drv->The_Window) {
      DEBUG(Out_Message("SDL_CreateWindow() failed: %s", SDL_GetError()));
      return 0;
   }

   drv->The_Context = SDL_GL_CreateContext(drv->The_Window);
   if (!drv->The_Context) {
      DEBUG(Out_Message("SDL_GL_CreateContext() failed: %s", SDL_GetError()));
      return 0;
   }

   if (FullScreen) { SDL_ShowCursor(SDL_DISABLE); }
   SDL_GL_MakeCurrent(drv->The_Window, drv->The_Context);
   SDL_GL_SetSwapInterval(1);

   if (!LoadGL()) { return 0; }

   GLint v;
   v = 0;  glGetIntegerv(GL_MAJOR_VERSION, &v);
   drv->GL_Version = v * 10;
   v = 0;  glGetIntegerv(GL_MINOR_VERSION, &v);
   drv->GL_Version += (v >= 10) ? 9 : v;
   DEBUG(Out_Message("OpenGL version seems to be %d.%d", drv->GL_Version / 10, drv->GL_Version % 10));

#if GL_DEBUG
   // ignore GL_INVALID_ENUM errors resulting from querying the OpenGL version
   // on OpenGL < 3.0
   while (glGetError());

   // activate GL_DEBUG_OUTPUT on OpenGL >= 4.2 (this is the first version
   // where this is guaranteed to be valid)
   if (drv->GL_Version >= 42) {
      glDebugMessageCallback(myGLdebugProc, NULL);
      glEnable(GL_DEBUG_OUTPUT);
   }
#endif

   glGenTextures(1, &drv->The_Texture);
   glBindTexture(GL_TEXTURE_2D, drv->The_Texture);
   GLuint filter = (drv->GL_Version >= 33) ? GL_LINEAR : GL_NEAREST;
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   if (drv->GL_Version >= 33) {
      DEBUG(Out_Message("using OpenGL 3.3+ shader-based pipeline"));

      GLuint vao;  // it's nonsense, but we *have* to do it; some drivers insist
      glGenVertexArrays(1, &vao);
      glBindVertexArray(vao);

      drv->The_Shader = glCreateProgram();

      GLuint vs = glCreateShader(GL_VERTEX_SHADER);
      static const char* vsSrc =
            "#version 330"
      "\n"  "uniform vec2 uSize;"
      "\n"  "out vec2 vTexCoord;"
      "\n"  "void main() {"
      "\n"  "  vec2 p = vec2(((gl_VertexID & 2) != 0) ? 1. : 0., ((gl_VertexID & 1) != 0) ? 1. : 0.);"
      "\n"  "  gl_Position = vec4((p * vec2(2.) - vec2(1.)) * uSize, 0., 1.);"
      "\n"  "  vTexCoord = p;"
      "\n"  "}";
      glShaderSource(vs, 1, &vsSrc, NULL);
      glCompileShader(vs);
      glAttachShader(drv->The_Shader, vs);
#if GL_DEBUG
      char log[1024];
      glGetShaderInfoLog(vs, 1024, NULL, log);
      DEBUG(Out_Message("VS compile log:\n%s", log));
#endif

      GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
      static const char* fsSrc =
            "#version 330"
      "\n"  "in vec2 vTexCoord;"
      "\n"  "uniform vec2 uTexSize;"
      "\n"  "uniform sampler2D uTex;"
      "\n"  "layout(location=0) out vec4 oColor;"
      "\n"  "void main() {"
      "\n"  "  vec2 p = vTexCoord * uTexSize;"
      "\n"  "  vec2 i = floor(p + vec2(.5));"
      "\n"  "  p = clamp((p - i) / fwidth(p), -.5, .5) + i;"
      "\n"  "  oColor = texture(uTex, p / uTexSize);"
      "\n"  "}";
      glShaderSource(fs, 1, &fsSrc, NULL);
      glCompileShader(fs);
      glAttachShader(drv->The_Shader, fs);
#if GL_DEBUG
      glGetShaderInfoLog(fs, 1024, NULL, log);
      DEBUG(Out_Message("FS compile log:\n%s", log));
#endif

      glLinkProgram(drv->The_Shader);
#if GL_DEBUG
      glGetProgramInfoLog(drv->The_Shader, 1024, NULL, log);
      DEBUG(Out_Message("program link log:\n%s", log));
#endif
      glUseProgram(drv->The_Shader);

      drv->Loc_uSize = glGetUniformLocation(drv->The_Shader, "uSize");
      glUniform2f(glGetUniformLocation(drv->The_Shader, "uTexSize"), drv->Display_Width, drv->Display_Height);
   }
   else
   {
      DEBUG(Out_Message("using OpenGL 1.1 fixed-function pipeline"));
   }

   GLint vp[4];
   glGetIntegerv(GL_VIEWPORT, vp);
   SDLdrv_UpdateViewport(drv, vp[2], vp[3]);

#if GL_DEBUG
   GLenum err; do { err = glGetError(); if (err) { DEBUG(Out_Message("[Init] GL error 0x%04X", err)); } } while (err);
#endif
#ifndef NDEBUG
   StartTime = SDL_GetTicks();
#endif
   return 1;
}

static void SDLdrv_ShowFrame(MEM_ZONE_SDL *drv)
{
   glClear(GL_COLOR_BUFFER_BIT);

   glBindTexture(GL_TEXTURE_2D, drv->The_Texture);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, drv->Display_Width, drv->Display_Height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, (const GLvoid*)drv->Screen_Buffer);

   if (drv->GL_Version >= 33)
   {
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
   }
   else
   {
      glEnable(GL_TEXTURE_2D);
      glBegin(GL_QUADS);
         glTexCoord2f(0.f, 0.f);  glVertex2f(-drv->VP_Width, -drv->VP_Height);
         glTexCoord2f(0.f, 1.f);  glVertex2f(-drv->VP_Width, +drv->VP_Height);
         glTexCoord2f(1.f, 1.f);  glVertex2f(+drv->VP_Width, +drv->VP_Height);
         glTexCoord2f(1.f, 0.f);  glVertex2f(+drv->VP_Width, -drv->VP_Height);
      glEnd();
   }

#if GL_DEBUG
   GLenum err; do { err = glGetError(); if (err) { DEBUG(Out_Message("[Draw] GL error 0x%04X", err)); } } while (err);
#endif
#ifndef NDEBUG
   FrameCount++;
#endif
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
         case SDL_WINDOWEVENT:
            if (ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
            {
               glViewport(0, 0, ev.window.data1, ev.window.data2);
               SDLdrv_UpdateViewport(drv, ev.window.data1, ev.window.data2);
            }
            break;
         default:
            break;
      }
   }
   return DRV_NULL_EVENT;
}

static void SDLdrv_Shutdown(MEM_ZONE_SDL *drv)
{
#ifndef NDEBUG
   Uint32 dt = SDL_GetTicks() - StartTime;
   DEBUG(Out_Message("%d frames in %d ms -> %.2f fps", FrameCount, dt, FrameCount / ((dt ? dt : 1) * 0.001)));
#endif
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

MEM_ZONE_METHODS SDLdrv_Methods = 
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

