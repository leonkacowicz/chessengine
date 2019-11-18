#!/bin/bash

WORKDIR=${1%/} # removes trailing slash
CHESSARBITER="./cmake-build-release/src/arbiter/chessarbiter"
CHESSENGINE="./cmake-build-release/src/engine/chessengine"

function wait_for_files_to_exist() {
    until ls "$WORKDIR" | grep "white_.*\.txt" > /dev/null && ls "$WORKDIR" | grep "black_.*\.txt" > /dev/null; do
        sleep 0.5
    done
}

function wait_for_files_to_vanish() {
    for file in "$@"; do
        while [ -f "$file" ]; do
            sleep 0.1
        done
    done
}

while true; do
  echo "Waiting for files..."
  wait_for_files_to_exist
  WHITE=$(ls "$WORKDIR" | grep "white_.*\.txt" | head -n 1)
  BLACK=$(ls "$WORKDIR" | grep "black_.*\.txt" | head -n 1)

  WHITE_OPTIONS=$(mktemp)
  BLACK_OPTIONS=$(mktemp)

  echo "setoption name evaluator value $WHITE" > "$WHITE_OPTIONS"
  echo "setoption name evaluator value $BLACK" > "$BLACK_OPTIONS"

  echo "WHITE=$WHITE"
  echo "BLACK=$BLACK"
  "$CHESSARBITER" --verbose --white-exec "$CHESSENGINE" \
      --white-input "$WHITE_OPTIONS" \
      --white-move-time 100 \
      --black-exec "$CHESSENGINE" \
      --black-input "$BLACK_OPTIONS" \
      --black-move-time 100 \
      --output "$WORKDIR/${WHITE%%.*}_${BLACK%%.*}.pgn"| tee "$WORKDIR/output.txt"

  rm "$WORKDIR/$WHITE" "$WORKDIR/$BLACK" "$WHITE_OPTIONS" "$BLACK_OPTIONS"
  wait_for_files_to_vanish "$WORKDIR/$WHITE" "$WORKDIR/$BLACK" "$WHITE_OPTIONS" "$BLACK_OPTIONS"
done