import os
import json
import subprocess
import boto3
import random
import logging

from typing import List

BUCKET = 'leonkacowicz'
QUEUE_URL = "https://sqs.us-west-2.amazonaws.com/284217563291/games.fifo"
RESULTS_QUEUE_URL = "https://sqs.us-west-2.amazonaws.com/284217563291/game_results"


def generate_games(num_games: int, num_moves: int) -> List[str]:
    if num_moves == 0:
        return [""] * num_games
    else:
        return subprocess.check_output(["./gengames", str(num_games), str(num_moves)]).decode('utf-8').split('\n')


def upload_weights_file(filename: str, remote_filename: str):
    s3 = boto3.client('s3')
    s3.upload_file(filename, 'leonkacowicz', 'chess/players/ampdist/' + remote_filename)
    pass


def send_game_request(weights_file: str, initial_pos: str, outputdir: str, game_id: str):
    if len(initial_pos) > 0:
        initial_pos = "startpos moves" + initial_pos
    else:
        initial_pos = "startpos"

    message = {
        "bucket": BUCKET,
        "outputdir": outputdir,
        "queue": RESULTS_QUEUE_URL,
        "white": {
            "exec": "chessengine",
            "movetime": 200,
            "weights_file": "chess/players/ampdist/" + weights_file
        },
        "black": {
            "exec": "stockfish10",
            "movetime": 200
        },
        "game_id": game_id,
        "initial_pos": initial_pos
    }

    sqs = boto3.client('sqs')
    sqs.send_message(QueueUrl=QUEUE_URL, MessageBody=json.dumps(message), MessageGroupId='1',
                     MessageDeduplicationId="%.12f" % random.random())
    pass


def get_next_game_result():
    sqs_client = boto3.client('sqs')
    while True:
        response = sqs_client.receive_message(QueueUrl=RESULTS_QUEUE_URL,
                                              VisibilityTimeout=30,
                                              WaitTimeSeconds=20,
                                              MaxNumberOfMessages=1)
        logging.debug('Got response: ' + str(response))
        if 'Messages' in response and len(response['Messages']) > 0:
            message = response['Messages'][0]
            sqs_client.delete_message(QueueUrl=RESULTS_QUEUE_URL, ReceiptHandle=message['ReceiptHandle'])
            logging.info("Received message: " + message['Body'])
            try:
                body = json.loads(message['Body'])
                return body['log_file']
            except Exception as e:
                logging.warning("Message body is not valid game result: ", message.body, e)
    pass


def download_game_log(remote_log_file: str) -> str:
    s3 = boto3.client('s3')
    s3.download_file(BUCKET, remote_log_file, 'game.log.gz')
    os.system("gunzip -f game.log.gz")
    return 'game.log'
    pass


def add_game_log_to_training_set(log_file: str, trainset_file: str):
    os.system("./parselog < %s >> %s" % (log_file, trainset_file))
    pass


def main():
    # initial weights in weights.txt
    num_games = 48
    num_iter = 1000
    for k in range(num_iter):
        # generate random initial positions
        initial_positions = generate_games(num_games, 0)

        # upload weights to S3
        weights_file = "weights_%d.txt" % (k,)
        upload_weights_file("weights.txt", weights_file)

        # send game requests
        for g in range(num_games):
            send_game_request(weights_file, initial_positions[g], "chess/ampdist/%03d" % (k,), "%d_%d" % (k, g))

        if os.path.exists("train.csv"):
            os.remove("train.csv")
        for g in range(num_games):
            try:
                remote_log_file = get_next_game_result()
                local_log_file = download_game_log(remote_log_file)
                # parse game results -> new training set
                add_game_log_to_training_set(local_log_file, "train.csv")
            except Exception as e:
                logging.error(e, exc_info=e)

        # train model starting on weights.txt to fit created training set
        os.system("python3 train.py")
        os.system("cp weights.new.txt weights.txt")
    pass


if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO, format='%(asctime)-15s %(message)s')
    main()
