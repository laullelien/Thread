#include <SDL2/SDL.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "oggstream.h"
#include "stream_common.h"

int main(int argc, char *argv[])
{
  int res;

  if (argc != 2)
  {
    fprintf(stderr, "Usage: %s FILE", argv[0]);
    exit(EXIT_FAILURE);
  }
  assert(argc == 2);

  // Initialisation de la SDL
  res = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS);
  atexit(SDL_Quit);
  assert(res == 0);

  // Your code HERE
  // start the two stream readers (theoraStreamReader and vorbisStreamReader)
  // each in a thread
  pthread_t video_id;
  pthread_t sound_id;

  if (pthread_create(&video_id, NULL, theoraStreamReader, argv[1]))
  {
    perror("Video thread creation failed");
  }

  if (pthread_create(&sound_id, NULL, vorbisStreamReader, argv[1]))
  {
    perror("Sound thread creation failed");
  }

  // wait for vorbis thread
  pthread_join(sound_id, NULL);

  // 1 seconde of sound in advance, thus wait 1 seconde
  // before leaving
  sleep(1);

  // Wait for theora and theora2sdl threads
  pthread_cancel(video_id);
  pthread_cancel(image_id);

  // TODO
  /* liberer des choses ? */

  exit(EXIT_SUCCESS);
}
