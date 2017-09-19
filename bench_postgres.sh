#! /usr/local/bin/bash

EVENT=ITLB_MISSES.WALK_DURATION
KERN_VER=before
TEST_NAME=postgres
PROFILES_DIR=/root/ITLB_MISSES.WALK_DURATION
RAW_PMC_PROF=walk.pmc
RESULTS_DIR=/root/pmcstat_profs/$TEST_NAME
TOTAL_ITERS=3

rm -rf $RESULTS_DIR
mkdir $RESULTS_DIR
ITER=0
while [ $ITER -lt $TOTAL_ITERS ]; do
	rm -rf $PROFILES_DIR
	createdb pgbench
	pgbench -i -s 150 pgbench
	pmcstat -S $EVENT -O /tmp/$RAW_PMC_PROF &
	pmcpid=$!
	pgbench -j 6 -c 6 -T 300 -S pgbench
	kill $pmcpid
	wait $pmcpid
	dropdb pgbench
	pmcstat -R /tmp/$RAW_PMC_PROF -g > /root/log.txt
	gprof /boot/kernel/kernel $PROFILES_DIR/kernel.gmon > $PROFILES_DIR/kernel.report
	mv /root/log.txt $RESULTS_DIR/$TEST_NAME.$EVENT.$KERN_VER.log$ITER.txt
	mv $PROFILES_DIR/kernel.report $RESULTS_DIR/$TEST_NAME.$EVENT.$KERN_VER.report$ITER.txt

	let ITER+=1
done

