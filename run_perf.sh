#!/bin/bash

set -e

PERF_FILE="data/perf.data"

cargo build --release --bin perf
sudo perf record -g --call-graph=dwarf -o $PERF_FILE ./target/release/perf
sudo perf report -i $PERF_FILE
