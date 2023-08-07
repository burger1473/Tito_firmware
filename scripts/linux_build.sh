#!/bin/bash

readonly PROJECT_DIRECTORY="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && cd .. && pwd)"
#readonly IMAGE_NAME='zephyr-environment'
readonly IMAGE_NAME='ghcr.io/spacebee-technologies/zephyr-environment'

docker run \
  --privileged \
  --rm \
  -t \
  -i \
  -v "${PROJECT_DIRECTORY}:/workspace" \
  "${IMAGE_NAME}" \
  bash -c 'cd /workspace; rm -r -f build; west build -b sam_v71_xult'
