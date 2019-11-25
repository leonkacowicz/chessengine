#!/bin/bash

BUCKET="leonkacowicz"
QUEUE_URL="https://sqs.us-west-2.amazonaws.com/284217563291/games.fifo"
WHITE="3f0c559dfa7954d375835967edb155587c0091dcb62c2b5de44b194a25934268"
BLACK="bccc97094406a7dd19fa5f785301f1fd34bf2b7d9aafd2a91942981e74bc7d48"
aws s3 cp "./${WHITE}.txt" "s3://${BUCKET}/chess/players/${WHITE}.txt"
aws s3 cp "./${BLACK}.txt" "s3://${BUCKET}/chess/players/${BLACK}.txt"

MSG="{\"bucket\": \"leonkacowicz\", \"outputdir\": \"chess/generations/\", \"white\": \"${WHITE}\", \"black\": \"${BLACK}\", \"movetime\": \"100\"}"
echo "$MSG" | jq
DEDUP=$(echo "${MSG}" | sha256sum | head -c 32)
DEDUP=$(date | sha256sum | head -c 32)
aws sqs send-message --region us-west-2 --queue-url "${QUEUE_URL}" --message-body "$MSG" --message-group-id 1 --message-deduplication-id "${DEDUP}"