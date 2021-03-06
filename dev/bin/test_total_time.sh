#!/bin/bash

echo "Launching time tests for SAT without whatched literals"
test_time/time_set_test_sat_nowl.sh > results/test_time/result_sat_nowl

echo "Launching time tests for SAT with whatched literals"
test_time/time_set_test_sat_wl.sh > results/test_time/result_sat_wl

echo "Launching time tests for SAT with clause learning and without whatched literals"
test_time/time_set_test_sat_cl.sh > results/test_time/result_sat_cl

echo "Launching time tests for SAT with clause learning and whatched literals"
test_time/time_set_test_sat_clwl.sh > results/test_time/result_sat_clwl


echo "Launching time tests for UNSAT without whatched literals"
test_time/time_set_test_unsat_nowl.sh > results/test_time/result_unsat_nowl

echo "Launching time tests for UNSAT with whatched literals"
test_time/time_set_test_unsat_wl.sh > results/test_time/result_unsat_wl

echo "Launching time tests for UNSAT with clause learning and without whatched literals"
test_time/time_set_test_unsat_cl.sh > results/test_time/result_unsat_cl

echo "Launching time tests for UNSAT with clause learning and whatched literals"
test_time/time_set_test_unsat_clwl.sh > results/test_time/result_unsat_clwl
