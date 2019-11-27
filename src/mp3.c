#include "mp3.h"

/**
 * Returns the audio quality depending on which mode we are
 * using and the quality parameter.
 */
int32_t mp3_get_output_quality(int32_t mode, int32_t quality) {
  if (mode == MP3_VBR_MODE) {
    return quality;
  } else {
    switch (quality) {
      default: return 320;
      case 0: return 32;
      case 1: return 48;
      case 2: return 64;
      case 3: return 96;
      case 4: return 128;
      case 5: return 160;
      case 6: return 192;
      case 7: return 224;
      case 8: return 256;
      case 9: return 320;
    }
  }
}

/**
 * Creates a structure used to keep lame and mp3_t data.
 */
EMSCRIPTEN_KEEPALIVE
mp3_t* mp3_create() {
  return malloc(sizeof(mp3_t));
}

/**
 * Frees all the used memory.
 */
EMSCRIPTEN_KEEPALIVE
int32_t mp3_destroy(mp3_t* e) {
  lame_close(e->gfp);
  free(e->input_buffer_left);
  free(e->input_buffer_right);
  free(e->output_buffer);
  free(e);
  return MP3_NO_ERROR;
}

/**
 * Initializes lame and all the buffers.
 */
EMSCRIPTEN_KEEPALIVE
int32_t mp3_init(mp3_t* e, int32_t sample_rate, int32_t num_channels, int32_t quality, int32_t samples, int32_t mode) {
  if (sample_rate < 11025 || sample_rate > 48000) {
    return MP3_ERROR_CONFIG_SAMPLE_RATE;
  }

  if (num_channels < 1 || num_channels > 2) {
    return MP3_ERROR_CONFIG_NUM_CHANNELS;
  }

  if (quality < 0 || quality > 9) {
    return MP3_ERROR_CONFIG_QUALITY;
  }

  if (samples < 2048 || samples > 16384) {
    return MP3_ERROR_CONFIG_SAMPLES;
  }

  // Inicializamos la estructura de LAME.
  e->gfp = lame_init();
  if (!e->gfp) {
    return MP3_ERROR_LAME_INIT;
  }

  // Reservamos la memoria necesaria para copiar las muestras que van
  // a entrar a través del micrófono.
  e->input_buffer_left = malloc(samples * sizeof(float));
  e->input_buffer_right = malloc(samples * sizeof(float));

  // Muestras de entrada.
  e->input_samples = samples;
  e->input_samplerate = sample_rate;
  e->input_channels = num_channels;

  e->output_samplerate = MP3_DEFAULT_OUTPUT_SAMPLE_RATE;
  e->output_quality = mp3_get_output_quality(mode, quality);

  // Reservamos 1MB para ir tirando. Aquí se irá almacenando
  // todo lo que necesitemos a la hora de codificar el MP3.
  e->output_buffer_max_size = samples * 1.25 + 7200;
  e->output_buffer_size = 0;
  e->output_buffer = malloc(e->output_buffer_max_size);

  // Establecemos la configuración de LAME.
  lame_set_in_samplerate(e->gfp, e->input_samplerate);
  lame_set_out_samplerate(e->gfp, e->output_samplerate);
  lame_set_num_channels(e->gfp, e->input_channels);
  if (mode == MP3_VBR_MODE) {
    lame_set_VBR(e->gfp, vbr_default);
    lame_set_VBR_q(e->gfp, e->output_quality);
  } else {
    lame_set_VBR(e->gfp, vbr_off);
    lame_set_brate(e->gfp, e->output_quality);
  }

  // Reservamos la memoria necesaria para los settings
  // que hemos establecido.
  if (lame_init_params(e->gfp) < 0) {
    return MP3_ERROR_LAME_INIT_PARAMS;
  }

  return MP3_NO_ERROR;
}

/**
 * Encodes MP3 frames.
 */
EMSCRIPTEN_KEEPALIVE
int32_t mp3_encode(mp3_t* e, int32_t num_samples) {
  // If 0 samples are passed we consider that we have already finished
  // to encode the mp3 so let's flush what we have in
  // the internal buffer.
  if (num_samples == 0) {
    return mp3_flush(e);
  }

  // If negative samples are passed this returns an error.
  if (num_samples < 0) {
    return MP3_ERROR_NUM_SAMPLES;
  }

  // If more samples than allowed are passed, then
  // we return an error.
  if (num_samples > e->input_samples) {
    return MP3_ERROR_TOO_MUCH_SAMPLES;
  }

  // Encodes the buffer and returns the number of bytes
  // that you have encoded.
  float* right = e->input_channels == 2 ? e->input_buffer_right : NULL;
  int32_t num_bytes = lame_encode_buffer_ieee_float(
    e->gfp,
    e->input_buffer_left,
    right,
    num_samples,
    e->output_buffer,
    e->output_buffer_max_size
  );

  if (num_bytes < 0) {
    return num_bytes - 100;
  }
  e->output_buffer_size = num_bytes;
  return MP3_NO_ERROR;
}

/**
 * Prevents losing half encoded frames.
 */
int32_t mp3_flush(mp3_t* e) {
  int32_t num_bytes = lame_encode_flush(e->gfp, e->output_buffer, e->output_buffer_max_size);
  if (num_bytes < 0) {
    return MP3_ERROR_LAME_ENCODE_FLUSH;
  }
  e->output_buffer_size = num_bytes;
  return MP3_NO_ERROR;
}
