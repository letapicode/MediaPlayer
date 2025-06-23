#Format Conversion

This module provides utilities to convert audio and video files using FFmpeg
libraries. The `AudioConverter` class exposes a simple `convert()` method which
takes an input file path and an output file path. The output format is inferred
from the extension of the output file.

The `VideoConverter` class offers similar functionality for video files. It can
transcode between formats and optionally resize or set a target bitrate.
