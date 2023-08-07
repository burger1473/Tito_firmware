#!/bin/bash

readonly PROJECT_DIRECTORY="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && cd .. && pwd)"
readonly IMAGE_NAME='ghcr.io/spacebee-technologies/zephyr-environment'

docker run \
  --privileged \
  --rm \
  --network=host \
  -t \
  -i \
  -v "${PROJECT_DIRECTORY}:/workspace" \
  "${IMAGE_NAME}" \
  bash -c 'cd /workspace/build; west debug'
