#! /usr/local/bin/bash

TOTAL_ITERS=3
TRIALS_PER_ITER=50000

TRIAL=0
while [ $TRIAL -lt $TRIALS_PER_ITER ]; do
	clang -c /root/empty.c
	let TRIAL+=1
done

