import os
import sys
import json
import subprocess
import boto3
import random
import logging

from typing import List, Dict

BUCKET = 'leonkacowicz'
QUEUE_URL = "https://sqs.us-west-2.amazonaws.com/284217563291/games.fifo"
RESULTS_QUEUE_URL = "https://sqs.us-west-2.amazonaws.com/284217563291/game_results"


def send_game_request(outputdir: str, game_id: str, white_dilution: int, black_dilution: int):
    initial_pos = "startpos"
    message = {
        "bucket": BUCKET,
        "outputdir": outputdir,
        "queue": RESULTS_QUEUE_URL,
        "white": {
            "exec": "./engine_diluter -p %.2f -e ./stockfish10" % (float(white_dilution) / 100,),
            "depth": 4
        },
        "black": {
            "exec": "./engine_diluter -p %.2f -e ./stockfish10" % (float(black_dilution) / 100,),
            "depth": 4
        },
        "game_id": game_id,
        "initial_pos": initial_pos,
        "white_dilution": white_dilution,
        "black_dilution": black_dilution
    }

    sqs = boto3.client('sqs')
    sqs.send_message(QueueUrl=QUEUE_URL, MessageBody=json.dumps(message), MessageGroupId='1',
                     MessageDeduplicationId="%.12f" % random.random())
    pass


def drain_queue(queue_url: str):
    sqs_client = boto3.client('sqs')
    while True:
        response = sqs_client.receive_message(QueueUrl=queue_url,
                                              VisibilityTimeout=30,
                                              WaitTimeSeconds=1,
                                              MaxNumberOfMessages=1)
        if 'Messages' in response and len(response['Messages']) > 0:
            logging.info("Found %d messages in queue: %s" % (len(response['Messages']), queue_url))
            for i in range(len(response['Messages'])):
                message = response['Messages'][i]
                sqs_client.delete_message(QueueUrl=RESULTS_QUEUE_URL, ReceiptHandle=message['ReceiptHandle'])
        else:
            logging.info("Queue drained: %s" % queue_url)
            return


def get_next_game_result() -> Dict:
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
                return json.loads(message['Body'])
            except Exception as e:
                logging.warning("Message body is not valid game result: ", message.body, e)


def download_game_log(remote_log_file: str) -> str:
    s3 = boto3.client('s3')
    logging.info("Downloading file s3://%s/%s" % (BUCKET, remote_log_file))
    s3.download_file(BUCKET, remote_log_file, 'game.log.gz')
    logging.info("unzipping file")
    os.system("gunzip -f game.log.gz")
    os.system("grep 'lost on time' game.log")
    return 'game.log'


def add_game_log_to_training_set(log_file: str, trainset_file: str):
    if os.system("./parselog < %s >> %s" % (log_file, trainset_file)) != 0:
        sys.exit(1)


def main():
    # initial weights in weights.txt
    drain_queue(QUEUE_URL)
    drain_queue(RESULTS_QUEUE_URL)

    dilutions = [x for x in range(0, 105, 5)]

    stronger_depth = 4
    weaker_depth = 4
    num_games = 100

    for weaker in range(len(dilutions) - 1):
        stronger = weaker + 1
        # send game requests
        for g in range(num_games):
            send_game_request(outputdir="chess/elo_calibration/sf10_depth_%02d_p%03d_vs_sf10_depth_%02d_p%03d" %
                              (stronger_depth, dilutions[stronger], weaker_depth, dilutions[weaker]),
                              game_id=("%08x" % random.randint(0, 256 ** 4)),
                              white_dilution=dilutions[stronger],
                              black_dilution=dilutions[weaker])

            send_game_request(outputdir="chess/elo_calibration/sf10_depth_%02d_p%03d_vs_sf10_depth_%02d_p%03d" %
                                        (weaker_depth, dilutions[weaker], stronger_depth, dilutions[stronger]),
                              game_id=("%08x" % random.randint(0, 256 ** 4)),
                              white_dilution=dilutions[stronger],
                              black_dilution=dilutions[weaker])

        for g in range(2 * num_games):
            try:
                message = get_next_game_result()
                print(message)
            except Exception as e:
                logging.error(e, exc_info=e)
    pass


if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO, format='%(asctime)-15s %(message)s')
    main()
