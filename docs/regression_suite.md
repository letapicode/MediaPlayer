# Automated Regression Suite

The project uses GoogleTest for unit and integration tests. When `BUILD_TESTS` is
enabled CMake builds several test executables under `tests/`. The regression
suite runs these binaries via `ctest` and can be integrated into CI.

## Running Locally

The helper script `tests/run_regression_suite.sh` builds the project with
tests enabled and executes the full suite of checks.

```bash
./tests/run_regression_suite.sh
```

## Continuous Integration

CI jobs should run `tests/run_regression_suite.sh` to build the project with
tests enabled and execute all checks. Failing tests will mark the job failed
preventing regressions from entering `main`.

### UI Responsiveness

The Qt test harness `tests/ui_responsiveness.qml` simulates a large library scan
while checking that the main window continues to process events. If
`qmltestrunner` is installed, `tests/run_regression_suite.sh` will automatically
execute the QML test. Otherwise it can be run manually with the same command.
A short guide is provided in `docs/ui_responsiveness_test.md`.
