#! /usr/local/bin/bash

# Run sysctl kern.sched.topology_spec to check if CPU topology is sane.

EVENT=ITLB_MISSES.WALK_DURATION
KERN_VER=after_share_main
TEST_NAME=clang
PROFILES_DIR=/root/ITLB_MISSES.WALK_DURATION
RAW_PMC_PROF=walk.pmc
RESULTS_DIR=/root/pmcstat_profs/$TEST_NAME
TOTAL_ITERS=3

rm -rf $RESULTS_DIR
mkdir $RESULTS_DIR
ITER=0
while [ $ITER -lt $TOTAL_ITERS ]; do
	rm -rf $PROFILES_DIR
	pmcstat -S $EVENT -O /tmp/$RAW_PMC_PROF &
	pmcpid=$!
	cpuset -c -l 0 ./test_clang.sh &
	cpid1=$!
	cpuset -c -l 1 ./test_clang.sh &
	cpid2=$!
	wait $cpid1
	wait $cpid2
	kill $pmcpid
	wait $pmcpid
	pmcstat -R /tmp/$RAW_PMC_PROF -g > /root/log.txt
	gprof /boot/kernel/kernel $PROFILES_DIR/kernel.gmon > $PROFILES_DIR/kernel.report
	mv /root/log.txt $RESULTS_DIR/$TEST_NAME.$EVENT.$KERN_VER.log$ITER.txt
	mv $PROFILES_DIR/kernel.report $RESULTS_DIR/$TEST_NAME.$EVENT.$KERN_VER.report$ITER.txt

	let ITER+=1
done

