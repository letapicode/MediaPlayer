#Format Conversion

This module provides utilities to convert audio and video files using FFmpeg
libraries. The `AudioConverter` class exposes a simple `convert()` method which
takes an input file path and an output file path. The output format is inferred
from the extension of the output file.

The `VideoConverter` class offers similar functionality for video files. It can
transcode between formats and optionally resize or set a target bitrate or
constant rate factor (CRF) for encoders that support it.

`FormatConverter` wraps these helpers to run conversions asynchronously. Its
`convertAudioAsync` and `convertVideoAsync` methods execute on a background
thread and provide progress callbacks (0-1 range) and a completion callback for
UI integration.

### Example CLI Usage

```
#include "mediaplayer/FormatConverter.h"
#include <iostream>

int main(int argc, char** argv) {
  if (argc < 3) {
    std::cout << "usage: convert <in> <out>" << std::endl;
    return 0;
  }
  mediaplayer::FormatConverter conv;
  mediaplayer::AudioEncodeOptions opts;
  opts.bitrate = 128000; // customize encoding settings
  conv.convertAudioAsync(
      argv[1], argv[2], opts, [](float p) { std::cout << "progress " << p * 100 << "%\n"; },
      [](bool ok) { std::cout << (ok ? "done" : "error") << std::endl; });
  conv.wait();
}
```

This converts `argv[1]` to the format specified by `argv[2]` while printing
progress updates.

### mediaconvert CLI

A standalone command line tool `mediaconvert` is built in this directory. It
provides easy access to the conversion functionality.

```
mediaconvert <audio|video> <input> <output> [options]
```

Options allow setting bitrate or `--crf` quality level, codec and for video the
output size. Progress is printed to the console.

### Cancellation API

`FormatConverter` now supports cancelling a running job via `cancel()`. The
`isCancelled()` method reports whether a cancellation was requested. The
conversion loops in `AudioConverter` and `VideoConverter` check this flag so a
conversion can be aborted gracefully.

### Qt Integration

For Qt applications the `mediaplayer_desktop` library provides a `FormatConverterQt`
class that emits progress and completion signals. This wrapper internally uses
the same conversion engine and can be used directly from QML.
