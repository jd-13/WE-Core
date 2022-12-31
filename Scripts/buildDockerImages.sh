#!/bin/bash

set -e

echo "=== Building clang image"
cd ../DockerFiles/clang10
docker buildx build --platform linux/amd64,linux/arm64 --push --tag jackd13/audioplugins:clang10 .

echo "=== Building gcc image"
cd ../gcc10
docker buildx build --platform linux/amd64,linux/arm64 --push --tag jackd13/audioplugins:gcc10 .
