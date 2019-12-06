#!/bin/bash
#set -x
WORKDIR=${1%/} # removes trailing slash
mkdir -p "$WORKDIR"
aws s3 cp "s3://leonkacowicz/chess/chessarbiter" "$WORKDIR/"
aws s3 cp "s3://leonkacowicz/chess/chessengine" "$WORKDIR/"
CHESSARBITER="$WORKDIR/chessarbiter"
CHESSENGINE="$WORKDIR/chessengine"
chmod +x "${CHESSARBITER}"
chmod +x "${CHESSENGINE}"

QUEUE_URL="https://sqs.us-west-2.amazonaws.com/284217563291/games.fifo"
RESPONSE_QUEUE_URL="https://sqs.us-west-2.amazonaws.com/284217563291/game_results"

while true; do
    echo "Waiting for work..."

    MSG=""
    while [ "$MSG" == "" ]; do
        MSG=$(aws sqs receive-message --region us-west-2 --queue-url "$QUEUE_URL" --visibility-timeout 30 --wait-time-seconds 10 --max-number-of-messages 1)
    done
    echo "Got message: $MSG"
    MSG=$(echo "$MSG" | jq '.Messages | .[]')
    MSG_RECEIPT=$(echo "$MSG" | jq -r '.ReceiptHandle')
    aws sqs delete-message --queue-url "$QUEUE_URL" --region us-west-2 --receipt-handle "$MSG_RECEIPT"

    MSG=$(echo "$MSG" | jq -r '.Body')
    BUCKET=$(echo "$MSG" | jq -r '.bucket')
    GENERATION=$(echo "$MSG" | jq -r '.generation')
    OUTPUTDIR=$(echo "$MSG" | jq -r '.outputdir')
    WHITE=$(echo "$MSG" | jq -r '.white')
    BLACK=$(echo "$MSG" | jq -r '.black')
    MOVETIME=$(echo "$MSG" | jq -r '.movetime')

    WHITE_OPTIONS=$(mktemp)
    BLACK_OPTIONS=$(mktemp)

    aws s3 cp "s3://${BUCKET}/chess/players/${WHITE}.txt" "$WORKDIR/players/${WHITE}.txt"
    aws s3 cp "s3://${BUCKET}/chess/players/${BLACK}.txt" "$WORKDIR/players/${BLACK}.txt"

    echo "setoption name evaluator value $WORKDIR/players/${WHITE}.txt" > "${WHITE_OPTIONS}"
    echo "setoption name evaluator value $WORKDIR/players/${BLACK}.txt" > "${BLACK_OPTIONS}"

    GAME="${WHITE}_${BLACK}"

    echo "WHITE=$WHITE"
    echo "BLACK=$BLACK"
    mkdir -p "${WORKDIR}/log"
    mkdir -p "${WORKDIR}/game"
    "$CHESSARBITER" --verbose \
        --white-exec "${CHESSENGINE}" --white-input "${WHITE_OPTIONS}" --white-move-time "${MOVETIME}" \
        --black-exec "${CHESSENGINE}" --black-input "${BLACK_OPTIONS}" --black-move-time "${MOVETIME}" \
        --output "$WORKDIR/game/${GAME}.pgn" | tee "$WORKDIR/log/${GAME}.log"
    gzip --best -f "$WORKDIR/log/${GAME}.log"

    RESULT=$(tail -n 1 "$WORKDIR/game/${GAME}.pgn")
    MSG=$(echo "$MSG" | jq ". + {result: \"$RESULT\"}")
    aws s3 cp "$WORKDIR/game/${GAME}.pgn" "s3://${BUCKET}/${OUTPUTDIR%/}/${GENERATION}/"
    aws s3 cp "$WORKDIR/log/${GAME}.log.gz" "s3://${BUCKET}/${OUTPUTDIR%/}/${GENERATION}/"
    echo "${MSG}"
    aws sqs send-message --region us-west-2 --queue-url "${RESPONSE_QUEUE_URL}" --message-body "$MSG"
    rm "$WHITE_OPTIONS" "$BLACK_OPTIONS"
done
