# Etercast MP3 Encoder

![Etercast](https://etercast.com/images/og_landscape.jpg)

This repository contains part of the [Etercast](https://etercast.com) MP3 encoding library. It uses [LAME](https://lame.sourceforge.io/) and C/C++ -> WebAssembly compiler [Emscripten](https://emscripten.org).

## How to build it

If you have [Emscripten](https://emscripten.org) installed locally then you can run:

```sh
make
```

Otherwise you can use the [Docker](https://www.docker.com/) image by [trcezi](https://hub.docker.com/r/trzeci/emscripten) to build the MP3 encoder by executing:

```sh
npm run build:emscripten
```

## How to use it

```javascript
import instantiate from '@etercast/mp3'

// instantiate() function fetches and instantiates the WASM module
// and returns a Encoder class.
// If you upload the mp3.wasm to a CDN you can specify the
// WASM url by passing it to instantiate.
// const Encoder = await instantiate('https://mycdn.com/mp3.wasm')
const Encoder = await instantiate()

// You can also use Encoder.create()
const encoder = new Encoder({
  sampleRate: audioContext.sampleRate,
  samples: 2048,
  numChannels: 1
})

// leftChannelData must be a Float32Array with 2048 (the
// specified in the encoder samples option) samples.
// rightChannelData is optional (can be null or undefined).
const encodedMP3Frames = encoder.encode(leftChannelData, rightChannelData)

// Remaining MP3 encoded frames. It flushes available MP3
// frames not returned in the previous call.
const remainingEncodedMP3Frames = encoder.encode()

const blob = new Blob([
  encodedMP3Frames,
  remainingEncodedMP3Frames
], {
  type: 'audio/mp3'
})
const url = URL.createObjectURL(blob)
const a = document.createElement('a')
a.href = url
a.download = 'audio.mp3'
a.dispatchEvent(new MouseEvent('click'))
```

## How to build documentation

Calling `npm run build` will build this documentation but if you want to build documentation specifically you can run:

```sh
npm run build:docs
```

## Demo

If you want to see a live demo running, run:

```sh
npm run serve:examples
```

Made with :heart: by [ROJO 2](https://rojo2.com)
