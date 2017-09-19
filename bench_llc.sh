#! /usr/local/bin/bash

EVENT=llc-misses
KERN_VER=after_share_main
TEST_NAME=clang
PROFILES_DIR=/root/llc-misses
RAW_PMC_PROF=llc.pmc
RESULTS_DIR=/root/pmcstat_profs/$TEST_NAME
TOTAL_ITERS=3
TRIALS_PER_ITER=50000

rm -rf $RESULTS_DIR
mkdir $RESULTS_DIR
ITER=0
while [ $ITER -lt $TOTAL_ITERS ]; do
	rm -rf $PROFILES_DIR
	pmcstat -S $EVENT -O /tmp/$RAW_PMC_PROF &
	TRIAL=0
	while [ $TRIAL -lt $TRIALS_PER_ITER ]; do
		clang -c /root/empty.c
		let TRIAL+=1
	done
	kill $!
	wait $!
	pmcstat -R /tmp/$RAW_PMC_PROF -g > /root/log.txt
	gprof /boot/kernel/kernel $PROFILES_DIR/kernel.gmon > $PROFILES_DIR/kernel.report
	mv /root/log.txt $RESULTS_DIR/$TEST_NAME.$EVENT.$KERN_VER.log$ITER.txt
	mv $PROFILES_DIR/kernel.report $RESULTS_DIR/$TEST_NAME.$EVENT.$KERN_VER.report$ITER.txt

	let ITER+=1
done

