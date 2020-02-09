import sys
import os
import tempfile
import boto3
import json
import logging
import subprocess
from boto3_type_annotations import sqs, s3


class EngineConfiguration:
    exec: str
    options_file: str
    weights_file: str
    movetime: int
    depth: int
    initial_time: int
    time_increment: int

    def __init__(self, color: str, message: dict):
        self.color = color
        self.exec = message['exec']
        self.movetime = int(message.get('movetime', 0))
        self.depth = int(message.get('depth', 0))
        self.initial_time = int(message.get('initial_time', 0))
        self.time_increment = int(message.get('time_increment', 0))
        self.weights_file = message.get('weights_file', '')
        self.options_file = ''


class GameRequest:
    white_player: EngineConfiguration
    black_player: EngineConfiguration
    bucket: str
    initial_pos: str
    game_id: str
    outputdir: str
    queue: str
    original_message: dict

    def __init__(self, message: dict):
        self.white_player = EngineConfiguration("white", message['white'])
        self.black_player = EngineConfiguration("black", message['black'])
        self.players = {'white': self.white_player, 'black': self.black_player}
        self.bucket = message['bucket']
        self.initial_pos = message['initial_pos'] if 'initial_pos' in message else 'startpos'
        self.game_id = message['game_id']
        self.outputdir = message.get('outputdir', "/chess/games/")
        self.queue = message.get('queue', '')
        self.original_message = message
    pass


class GameResult:
    pgn_file: str
    log_file: str
    request: GameRequest
    result: str

    def __init__(self, pgn_file: str, log_file: str, request: GameRequest):
        self.pgn_file = pgn_file
        self.log_file = log_file
        self.request = request
        self.result = ''
    pass


def normal_path(p: str):
    return '/'.join([e for e in p.split('/') if e])


def get_request_from_queue(sqs_client: sqs.Client) -> GameRequest:
    while True:
        logging.info("Waiting for messages")
        response = sqs_client.receive_message(QueueUrl=os.getenv('REQUEST_QUEUE_URL'), VisibilityTimeout=30, WaitTimeSeconds=20,
                                              MaxNumberOfMessages=1)
        logging.debug('Got response: ' + str(response))
        if 'Messages' in response and len(response['Messages']) > 0:
            message = response['Messages'][0]
            sqs_client.delete_message(QueueUrl=os.getenv('REQUEST_QUEUE_URL'), ReceiptHandle=message['ReceiptHandle'])
            try:
                body = json.loads(message['Body'])
                if 'game_id' not in body:
                    body['game_id'] = message['MD5OfBody']
                return GameRequest(body)
            except Exception as e:
                logging.warning("Message body is not valid GameRequest: ", message.body, e)


def prepare_player(s3_client: s3.Client, game_request: GameRequest, player: EngineConfiguration):
    if player.weights_file:
        local_weights_file = "./players/" + player.color + ".txt"
        if not os.path.exists(local_weights_file):
            if not os.path.exists("./players"):
                os.mkdir("./players")
            s3_client.download_file(game_request.bucket, player.weights_file, local_weights_file)
        player.options_file = tempfile.mktemp()
        with open(player.options_file, 'w') as temp:
            temp.write("setoption name evaluator value " + local_weights_file)


def run_game(game_request: GameRequest) -> GameResult:
    cmd = "./chessarbiter --verbose "
    for player in (game_request.white_player, game_request.black_player):
        cmd += " --%s-exec '%s'" % (player.color, player.exec)
        if player.options_file:
            cmd += " --%s-input '%s'" % (player.color, player.options_file)
        if player.movetime:
            cmd += " --%s-move-time %d" % (player.color, player.movetime)
        if player.depth:
            cmd += " --%s-max-depth %d" % (player.color, player.depth)
        if player.initial_time:
            cmd += " --%s-init-time %d" % (player.color, player.initial_time)
        if player.time_increment:
            cmd += " --%s-time-increment %d" % (player.color, player.time_increment)

    if game_request.initial_pos:
        cmd += " --initial-pos '%s'" % game_request.initial_pos

    game_result = GameResult("./game/%s.pgn" % (game_request.game_id,),
                             "./game/%s.log" % (game_request.game_id,),
                             game_request)
    cmd += " --output '%s'" % (game_result.pgn_file,)
    cmd += " | tee '%s'" % (game_result.log_file,)
    if not os.path.exists("game"):
        os.mkdir("game")
    os.system(cmd)
    game_result.result = subprocess.check_output(['tail', '-1', game_result.pgn_file]).decode('utf-8').strip()
    return game_result


def upload_game_result(s3_client: s3.Client, game_result: GameResult):
    os.system("gzip -f " + game_result.log_file)
    game_result.log_file += ".gz"
    log_key = normal_path(game_result.request.outputdir + "/" + game_result.log_file.split('/')[-1])
    s3_client.upload_file(
        game_result.log_file,
        Bucket=game_result.request.bucket,
        Key=log_key)
    pgn_key = normal_path(game_result.request.outputdir + "/" + game_result.pgn_file.split('/')[-1])
    s3_client.upload_file(
        game_result.pgn_file,
        Bucket=game_result.request.bucket,
        Key=pgn_key)
    waiter = s3_client.get_waiter('object_exists')
    waiter.wait(Bucket=game_result.request.bucket, Key=log_key)
    waiter.wait(Bucket=game_result.request.bucket, Key=pgn_key)


def notify_result_queue(sqs_client: sqs.Client, game_result: GameResult):
    if game_result.request.queue:
        game_result.request.original_message['log_file'] = normal_path(game_result.request.outputdir + "/" + game_result.log_file.split('/')[-1])
        game_result.request.original_message['pgn_file'] = normal_path(game_result.request.outputdir + "/" + game_result.pgn_file.split('/')[-1])
        game_result.request.original_message['result'] = game_result.result
        sqs_client.send_message(QueueUrl=game_result.request.queue,
                                MessageBody=json.dumps(game_result.request.original_message))
    pass


def clean_up(game_result: GameResult):

    files = [game_result.pgn_file, game_result.log_file, game_result.request.white_player.options_file,
             game_result.request.black_player.options_file, "./players/white.txt", "./players/black.txt"]
    for file in files:
        if len(file) > 0:
            if os.path.exists(file):
                logging.info("deleting file: " + file)
                os.remove(file)
    pass


def main():
    session = boto3.Session(region_name='us-west-2')
    sqs_client = session.client('sqs')
    s3_client = session.client('s3')
    while True:
        try:
            request = get_request_from_queue(sqs_client)
            prepare_player(s3_client, request, request.white_player)
            prepare_player(s3_client, request, request.black_player)
            game_result = run_game(request)
            upload_game_result(s3_client, game_result)
            notify_result_queue(sqs_client, game_result)
            clean_up(game_result)
        except Exception as e:
            logging.error(e, exc_info=e)


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format='%(asctime)-15s %(message)s')
    if not os.getenv('REQUEST_QUEUE_URL'):
        logging.fatal('REQUEST_QUEUE_URL env variable not set')
        sys.exit(1)
    logger = logging.getLogger("main")
    logger.setLevel(logging.INFO)
    logger.info("Starting worker")
    main()
