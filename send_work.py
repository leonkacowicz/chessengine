import random
import boto3
import json
from boto3_type_annotations import sqs

BUCKET = "leonkacowicz"
QUEUE_URL = "https://sqs.us-west-2.amazonaws.com/284217563291/games.fifo"
WHITE = "001cd75088ca4d64a2e6a57e0f10015aa1f9ca905aefe34b08f4b8b2ce17bc97.txt"
BLACK = "001cd75088ca4d64a2e6a57e0f10015aa1f9ca905aefe34b08f4b8b2ce17bc97.txt"
# #aws s3 cp "./${WHITE}.txt" "s3://${BUCKET}/chess/players/${WHITE}.txt"
# #aws s3 cp "./${BLACK}.txt" "s3://${BUCKET}/chess/players/${BLACK}.txt"
#

message = {
    "bucket": BUCKET,
    "outputdir": "chess/generations/",
    "queue": "https://sqs.us-west-2.amazonaws.com/284217563291/game_results",
    "white": {
        "exec": "chessengine",
        "movetime": 100,
        "weights_file": WHITE
    },
    "black": {
        "exec": "chessengine",
        "movetime": 100,
        "weights_file": BLACK
    },
    "game_id": "test_game_id",
    "extra_field": 42,
    "initial_pos": "startpos moves e2e4"
}

client: sqs.Client = boto3.client('sqs')
client.send_message(QueueUrl=QUEUE_URL, MessageBody=json.dumps(message), MessageGroupId='1', MessageDeduplicationId="%.12f" % random.random())
