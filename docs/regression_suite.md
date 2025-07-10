# Automated Regression Suite

The project uses GoogleTest for unit and integration tests. When `BUILD_TESTS` is
enabled CMake builds several test executables under `tests/`. The regression
suite runs these binaries via `ctest` and can be integrated into CI.

## Running Locally

```bash
cmake -S . -B build -DBUILD_TESTS=ON
cmake --build build
cd build && ctest --output-on-failure
```

## Continuous Integration

CI jobs should configure the project with `BUILD_TESTS=ON` and execute `ctest`.
Additional scripts like `tests/end_to_end.py` can be invoked to simulate user
flows. Failing tests will mark the job failed preventing regressions from
entering `main`.
