#ifndef __MP3_H__
#define __MP3_H__

#include <emscripten.h>
#include <stdint.h>
#include <stdlib.h>
#include <lame/lame.h>

/**
 * Constants.
 */
#define MP3_DEFAULT_OUTPUT_SAMPLE_RATE 44100

/**
 * Error list.
 */
#define MP3_NO_ERROR 0

#define MP3_CBR_MODE 0
#define MP3_VBR_MODE 1

#define MP3_ERROR_CONFIG_SAMPLE_RATE -1
#define MP3_ERROR_CONFIG_NUM_CHANNELS -2
#define MP3_ERROR_CONFIG_QUALITY -3
#define MP3_ERROR_LAME_INIT -4
#define MP3_ERROR_LAME_INIT_PARAMS -5
#define MP3_ERROR_REALLOC_BUFFER_SIZE -6
#define MP3_ERROR_TOO_MUCH_SAMPLES -7
#define MP3_ERROR_LAME_ENCODE_IEEE_FLOAT -8
#define MP3_ERROR_LAME_ENCODE_FLUSH -9
#define MP3_ERROR_NUM_SAMPLES -10
#define MP3_ERROR_CONFIG_SAMPLES -11

typedef struct {
  // Output configuration.
  int32_t output_buffer_size;
  int32_t output_buffer_max_size;
  int32_t output_samplerate;
  int32_t output_quality;

  // Input configuration.
  int32_t input_samples;
  int32_t input_samplerate;
  int32_t input_channels;

  // Pointer to input samples.
  float* input_buffer_left;
  float* input_buffer_right;

  // Pointer to output frames.
  uint8_t* output_buffer;

  // LAME structure
  lame_global_flags* gfp;
} mp3_t;

// Creates the mp3_t structure and returns a pointer to it.
mp3_t* mp3_create();

// Initializes the mp3_t structure reserving space for
// buffers and configuring LAME.
int32_t mp3_init(mp3_t* e, int32_t sample_rate, int32_t num_channels, int32_t quality, int32_t samples, int32_t mode);

// Encodes MP3 frames.
int32_t mp3_encode(mp3_t* e, int32_t num_samples);

// Flushes half-encoded frames.
int32_t mp3_flush(mp3_t* e);

// Destroys MP3 structure and frees memory.
int32_t mp3_destroy(mp3_t* e);

#endif
