# Performance Tests

This directory holds utilities for measuring MediaPlayer's performance. The `memory_cpu_profile.sh` script runs the desktop application under Valgrind's Massif and Callgrind tools to capture memory and CPU usage.

Usage:
```bash
./memory_cpu_profile.sh <media_file>
```
The resulting `massif.txt` and `callgrind.out` reports can be inspected manually.
