/***********************************************
 *            SDL2 audio driver                *
 * KeyJ 2023                                   *
 ***********************************************/

#include "main.h"
#include "audio.h"

#if defined( USE_SDL )

#include <SDL.h>

static SDL_AudioDeviceID deviceID = 0;

/********************************************************************/

static INT Ping( )
{
   if (SDL_Init(SDL_INIT_AUDIO)) { return -1; }
   return (SDL_GetNumAudioDevices(SDL_FALSE) > 0) ? 0 : -1;
}

static INT Init( INT Mode, INT Mix )
{
   SDL_AudioSpec spec;
   bzero(&spec, sizeof(spec));
   spec.freq = Mix;
   spec.format = (Mode & _AUDIO_16BITS_) ? AUDIO_S16 : AUDIO_U8;
   spec.channels = (Mode & _AUDIO_STEREO_) ? 2 : 1;
   spec.samples = 8192;
   Audio_Out_Mode = Mode;

   deviceID = SDL_OpenAudioDevice(NULL, SDL_FALSE, &spec, NULL, 0);
   if (!deviceID)
   {
      Out_Error("failed to open audio device: %s", SDL_GetError());
      return -1;
   }

   SDL_PauseAudioDevice(deviceID, 0);
   return 0;
}

static void Flush_Force( BYTE *Buf, INT Len )
{
   SDL_QueueAudio(deviceID, Buf, Len);
}

static INT Flush( BYTE *Buf, INT Len )
{
   Flush_Force(Buf, Len);
   return Len;
}

static void Reset( )
{
   SDL_ClearQueuedAudio(deviceID);
   Reset_Audio_Buffer();
}

static void Close( )
{
   SDL_PauseAudioDevice(deviceID, 1);
   SDL_CloseAudioDevice(deviceID);
}

/********************************************************************/

EXTERN AUDIO_DRIVER _SDL_Audio_Drv_ = {
   "SDL2 audio driver",
   Ping, Init, Reset, Close, Flush, Flush_Force,
   _AUDIO_FLAGS_
};

/********************************************************************/

#endif      // USE_SDL
