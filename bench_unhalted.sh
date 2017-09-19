#! /usr/local/bin/bash

EVENT=unhalted-cycles
KERN_VER=before
TEST_NAME=empty
PROFILES_DIR=/root/CPU_CLK_UNHALTED_CORE
RAW_PMC_PROF=unhalted.pmc
RESULTS_DIR=/root/pmcstat_profs/$TEST_NAME
TOTAL_ITERS=5
TRIALS_PER_ITER=5000

rm -rf $RESULTS_DIR
mkdir $RESULTS_DIR
ITER=0
while [ $ITER -lt $TOTAL_ITERS ]; do
	rm -rf $PROFILES_DIR
	pmcstat -S $EVENT -O /tmp/$RAW_PMC_PROF &
	TRIAL=0
	while [ $TRIAL -lt $TRIALS_PER_ITER ]; do
		/root/empty.sh
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

