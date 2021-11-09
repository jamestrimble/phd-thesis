#!/bin/bash

set -euo pipefail

./run.sh mcsplaininstances
./run.sh mcs33vedinstances
./run.sh randomplaininstances2
./run.sh randomplaininstances3

./summarise.sh mcsplaininstances
./summarise.sh mcs33vedinstances
./summarise2.sh randomplaininstances2
./summarise2.sh randomplaininstances3

