#!/bin/bash

WORKDIR=${1%/} # removes trailing slash
CHESSARBITER="./cmake-build-release/src/arbiter/chessarbiter"
CHESSENGINE="./cmake-build-release/src/engine/chessengine"

QUEUE_URL="https://sqs.us-west-2.amazonaws.com/284217563291/games.fifo"

while true; do
    echo "Waiting for work..."

    MSG=""
    while [ "$MSG" == "" ]; do
        MSG=$(aws sqs receive-message --region us-west-2 --queue-url "$QUEUE_URL" --visibility-timeout 30 --wait-time-seconds 10)
    done
    MSG_RECEIPT=$(echo "$MSG" | jq '.Messages | .[] | .ReceiptHandle')
    aws sqs delete-message --queue-url "$QUEUE_URL" --region us-west-2 --receipt-handle "$MSG_RECEIPT"

    WHITE=$(echo "$MSG" | jq '.white')
    BLACK=$(echo "$MSG" | jq '.black')

    WHITE_OPTIONS=$(mktemp)
    BLACK_OPTIONS=$(mktemp)

    echo "setoption name evaluator value $WORKDIR/players/${WHITE}.txt" >"$WHITE_OPTIONS"
    echo "setoption name evaluator value $WORKDIR/players/${BLACK}.txt" >"$BLACK_OPTIONS"

    GAME="${WHITE}_${BLACK}"

    echo "WHITE=$WHITE"
    echo "BLACK=$BLACK"
    "$CHESSARBITER" --verbose --white-exec "$CHESSENGINE" \
        --white-input "$WHITE_OPTIONS" \
        --white-move-time 100 \
        --black-exec "$CHESSENGINE" \
        --black-input "$BLACK_OPTIONS" \
        --black-move-time 100 \
        --output "$WORKDIR/game/${GAME}.pgn" | gzip -c >"$WORKDIR/log/${GAME}.log.gz"

    rm "$WHITE_OPTIONS" "$BLACK_OPTIONS"
done
