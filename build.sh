#!/bin/bash
set -e

IMAGE="asm-x86-dev"

echo "=== Build de l'image Linux x86-64 ==="
docker build --platform linux/amd64 -t "$IMAGE" .

echo "=== Démarrage de l'environnement ==="
docker run --rm -it \
    --platform linux/amd64 \
    -v "$PWD:/work" \
    -w /work \
    "$IMAGE"
