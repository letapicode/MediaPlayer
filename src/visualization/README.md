# Visualization Module

This module contains plug-in visualizers that can be attached to the core media player.

`BasicVisualizer` demonstrates how to implement the `Visualizer` interface. It performs a simple FFT on incoming PCM data using the utility `simpleFFT` and stores the latest frequency spectrum.
