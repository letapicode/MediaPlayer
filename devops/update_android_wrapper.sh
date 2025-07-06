#!/bin/bash
# Regenerate gradle-wrapper.jar for the Android project.
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="${SCRIPT_DIR}/.."
cd "${PROJECT_ROOT}/src/android"
# Use the version defined in gradle-wrapper.properties
gradle wrapper --gradle-version 8.1.1 --distribution-type bin
