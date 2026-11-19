# Test 3: representative display-mode regression on the isolated physical candidate.
cd [file dirname [info script]]
set_param general.maxThreads 2
proc slack {kind} {return [get_property SLACK [get_timing_paths -delay_type $kind -max_paths 1]]}
if {[catch {
  open_checkpoint video-physopt-candidate.dcp
  set pll [get_cells zz9000_ps_i/clk_wiz_0/inst/CLK_CORE_DRP_I/clk_inst/plle2_adv_inst]
  if {[llength $pll] != 1} {error "Expected one video PLL"}
  set original [list CLKFBOUT_MULT [get_property CLKFBOUT_MULT $pll] DIVCLK_DIVIDE [get_property DIVCLK_DIVIDE $pll] CLKOUT0_DIVIDE [get_property CLKOUT0_DIVIDE $pll]]
  if {$original ne {CLKFBOUT_MULT 33 DIVCLK_DIVIDE 4 CLKOUT0_DIVIDE 11}} {error "Candidate startup PLL differs from baseline"}
  set before_max [slack max]
  set before_min [slack min]
  set rows [open video-candidate-regression.tsv w]
  puts $rows "mode\tmult\tdiv\toutdiv\tperiod_ns\twns_ns\twhs_ns"
  foreach mode {{control75 33 4 11} {640x480 15 1 60} {800x600 14 1 35} {1280x1024 54 5 10} {1920x1080 15 1 10} {2560x1440_legal 41 4 7}} {
    lassign $mode name mult div outdiv
    set_property -dict [list CLKFBOUT_MULT $mult DIVCLK_DIVIDE $div CLKOUT0_DIVIDE $outdiv] $pll
    update_timing
    set clock [get_clocks clk_out1_zz9000_ps_clk_wiz_0_0]
    set period [get_property PERIOD $clock]
    if {abs($period-10.0*$div*$outdiv/$mult)>0.002} {error "Clock period mismatch for $name"}
    set wns [slack max]
    set whs [slack min]
    if {$name eq "control75" && (abs($wns-$before_max)>0.00001 || abs($whs-$before_min)>0.00001)} {error "Unchanged candidate control mismatch"}
    puts $rows "$name\t$mult\t$div\t$outdiv\t$period\t$wns\t$whs"
    flush $rows
    puts "CANDIDATE_MODE_RESULT $name period=$period WNS=$wns WHS=$whs"
    report_timing_summary -report_unconstrained -file video-candidate-$name-timing.rpt
    report_timing -to $clock -max_paths 3 -file video-candidate-$name-paths.rpt
    report_drc -checks [get_drc_checks PDRC-*] -file video-candidate-$name-pll-drc.rpt
  }
  close $rows
  set_property -dict $original $pll
  update_timing
  if {abs([slack max]-$before_max)>0.00001 || abs([slack min]-$before_min)>0.00001} {error "Restored candidate control mismatch"}
  report_route_status -file video-candidate-route.rpt
  report_bus_skew -file video-candidate-bus-skew.rpt
  report_cdc -file video-candidate-cdc.rpt
  report_methodology -file video-candidate-methodology.rpt
  puts VIDEO_CANDIDATE_REGRESSION_COMPLETED_ORIGINAL_CLOCK_RESTORED
} message options]} {puts "VIDEO_CANDIDATE_REGRESSION_ERROR $message"; puts $options; exit 1}
exit 0
