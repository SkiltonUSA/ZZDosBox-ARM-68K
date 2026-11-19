# Test 2: isolated post-route physical optimization, unchanged source RTL.
# Candidate DCP is diagnostic only. Never overwrite the baseline or write a bitstream.
cd [file dirname [info script]]
set_param general.maxThreads 2
proc slack {kind} {return [get_property SLACK [get_timing_paths -delay_type $kind -max_paths 1]]}
if {[catch {
  open_checkpoint baseline-routed.dcp
  set pll [get_cells zz9000_ps_i/clk_wiz_0/inst/CLK_CORE_DRP_I/clk_inst/plle2_adv_inst]
  if {[llength $pll] != 1} {error "Expected one video PLL"}
  set original [list CLKFBOUT_MULT [get_property CLKFBOUT_MULT $pll] DIVCLK_DIVIDE [get_property DIVCLK_DIVIDE $pll] CLKOUT0_DIVIDE [get_property CLKOUT0_DIVIDE $pll]]
  set_property -dict {CLKFBOUT_MULT 15 DIVCLK_DIVIDE 1 CLKOUT0_DIVIDE 10} $pll
  update_timing
  set before_max [slack max]
  set before_min [slack min]
  if {abs($before_max+1.621)>0.00001 || abs($before_min-0.010)>0.00001} {error "150 MHz control differs from recorded baseline"}
  puts "PHYSOPT_BEFORE WNS=$before_max WHS=$before_min"
  report_timing_summary -report_unconstrained -file video-physopt-before-timing.rpt
  phys_opt_design -directive AggressiveExplore
  report_timing_summary -report_unconstrained -file video-physopt-intermediate-timing.rpt
  route_design -directive Explore
  puts "PHYSOPT_AFTER WNS=[slack max] WHS=[slack min]"
  report_timing_summary -report_unconstrained -file video-physopt-after-timing.rpt
  report_timing -to [get_clocks clk_out1_zz9000_ps_clk_wiz_0_0] -max_paths 5 -file video-physopt-after-paths.rpt
  report_route_status -file video-physopt-route.rpt
  report_utilization -file video-physopt-utilization.rpt
  set_property -dict $original $pll
  update_timing
  puts "PHYSOPT_RESTORED_CLOCK WNS=[slack max] WHS=[slack min]"
  report_timing_summary -report_unconstrained -file video-physopt-restored-timing.rpt
  report_drc -file video-physopt-drc.rpt
  write_checkpoint -force video-physopt-candidate.dcp
  puts VIDEO_PHYSOPT_COMPLETED_CANDIDATE_ONLY_NO_BITSTREAM
} message options]} {puts "VIDEO_PHYSOPT_ERROR $message"; puts $options; exit 1}
exit 0
