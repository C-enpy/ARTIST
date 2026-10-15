#!/bin/bash

set -euo pipefail
build_dir="${1:-build}"
lcov --directory "$build_dir" --capture --output-file "$build_dir/coverage.info"
lcov --remove "$build_dir/coverage.info" '/usr/*' '*conan*' '*/_deps/*' '*test*' \
    --output-file "$build_dir/coverage.info"
lcov --list "$build_dir/coverage.info"
