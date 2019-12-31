#!/bin/bash


STRONGER="0.70"
WEAKER="0.75"

mkdir -p out/$STRONGER-$WEAKER/white
mkdir -p out/$STRONGER-$WEAKER/black

for j in {1..25}
do
    for i in {1..8}
    do
        tmp1=$(mktemp -p out/$STRONGER-$WEAKER/white)
        tmp2=$(mktemp -p out/$STRONGER-$WEAKER/black)
        ./chessarbiter --verbose \
            --white-exec "./engine_diluter -p $STRONGER ./stockfish_10_x64_modern" --white-max-depth 10 --white-input /dev/null --white-move-time 100000 \
            --black-exec "./engine_diluter -p $WEAKER  ./stockfish_10_x64_modern" --black-max-depth 10 --black-input /dev/null --black-move-time 100000  > $tmp1 &

        ./chessarbiter --verbose \
            --black-exec "./engine_diluter -p $STRONGER ./stockfish_10_x64_modern" --black-max-depth 10 --black-input /dev/null --black-move-time 100000 \
            --white-exec "./engine_diluter -p $WEAKER  ./stockfish_10_x64_modern" --white-max-depth 10 --white-input /dev/null --white-move-time 100000  > $tmp2 &
    done
    wait
    echo "BATCH $j"
    echo -e $(grep 1-0 out/$STRONGER-$WEAKER/white/* | wc -l) $(grep 0-1 out/$STRONGER-$WEAKER/white/* | wc -l) $(grep 1/2-1/2 out/$STRONGER-$WEAKER/white/* | wc -l)
    echo -e $(grep 0-1 out/$STRONGER-$WEAKER/black/* | wc -l) $(grep 1-0 out/$STRONGER-$WEAKER/black/* | wc -l) $(grep 1/2-1/2 out/$STRONGER-$WEAKER/black/* | wc -l)

done