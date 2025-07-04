#Visualization Module

This module contains plug-in visualizers that can be attached to the core media player.

`BasicVisualizer` demonstrates how to implement the `Visualizer` interface. It performs a simple FFT on incoming PCM data using the utility `simpleFFT`, which now uses a fast Cooley--Tukey algorithm, and stores the latest frequency spectrum.

`ProjectMVisualizer` wraps the [projectM](https://github.com/projectM-visualizer/projectm) library. It renders classic MilkDrop-style presets using OpenGL and outputs
to an off-screen framebuffer. The resulting texture ID can be retrieved with
`texture()` for display in the UI. The constructor accepts a configuration where
you can reduce `meshX`, `meshY` or `fps` to lower quality on constrained devices
such as mobile phones.

Additional configuration options include `presetPath`, `titleFont` and
`menuFont` to point projectM to custom preset directories or font files. You can
change these at runtime using `setPresetPath()` or `setFonts()` which will
reinitialize the projectM context with the new settings.
