# Diagnostic steady-state PLL configurations on unchanged routed logic.
# No bitstream or checkpoint is saved; runtime reconfiguration is not simulated.
cd [file dirname [info script]]
set_param general.maxThreads 2
proc slack {kind} {
  return [get_property SLACK [get_timing_paths -delay_type $kind -max_paths 1]]
}
if {[catch {
  open_checkpoint baseline-routed.dcp
  set pll [get_cells zz9000_ps_i/clk_wiz_0/inst/CLK_CORE_DRP_I/clk_inst/plle2_adv_inst]
  if {[llength $pll] != 1} {error "Expected one video PLL"}
  set original [list CLKFBOUT_MULT [get_property CLKFBOUT_MULT $pll] DIVCLK_DIVIDE [get_property DIVCLK_DIVIDE $pll] CLKOUT0_DIVIDE [get_property CLKOUT0_DIVIDE $pll]]
  set before_max [slack max]
  set before_min [slack min]
  set clock_name clk_out1_zz9000_ps_clk_wiz_0_0
  set rows [open video-mode-timing.tsv w]
  puts $rows "mode\tmult\tdiv\toutdiv\tperiod_ns\twns_ns\twhs_ns"
  foreach mode {
    {baseline 33 4 11}
    {640x480 15 1 60}
    {800x600 14 1 35}
    {1024x768 13 1 20}
    {1280x720 15 1 20}
    {1280x1024 54 5 10}
    {1920x1080 15 1 10}
    {1600x1200 21 1 13}
    {2560x1440 41 2 14}
  } {
    lassign $mode name mult div outdiv
    set_property -dict [list CLKFBOUT_MULT $mult DIVCLK_DIVIDE $div CLKOUT0_DIVIDE $outdiv] $pll
    update_timing
    set clock [get_clocks $clock_name]
    set period [get_property PERIOD $clock]
    set expected [expr {10.0*$div*$outdiv/$mult}]
    if {abs($period-$expected)>0.002} {error "Clock did not update for $name: $period expected $expected"}
    set wns [slack max]
    set whs [slack min]
    if {$name eq "baseline" && (abs($wns-$before_max)>0.00001 || abs($whs-$before_min)>0.00001)} {
      error "Unchanged PLL control changed timing"
    }
    puts $rows "$name\t$mult\t$div\t$outdiv\t$period\t$wns\t$whs"
    flush $rows
    puts "VIDEO_MODE_RESULT $name period=$period WNS=$wns WHS=$whs"
    report_timing_summary -report_unconstrained -file video-mode-$name-timing.rpt
    report_timing -to $clock -max_paths 3 -file video-mode-$name-paths.rpt
    report_drc -checks [get_drc_checks PDRC-*] -file video-mode-$name-pll-drc.rpt
  }
  close $rows
  set_property -dict $original $pll
  update_timing
  if {abs([slack max]-$before_max)>0.00001 || abs([slack min]-$before_min)>0.00001} {
    error "Restored PLL timing does not match original"
  }
  puts VIDEO_MODE_TIMING_COMPLETED_ORIGINAL_RESTORED_NO_CHECKPOINT_SAVED
} message options]} {puts "VIDEO_MODE_TIMING_ERROR $message"; puts $options; exit 1}
exit 0
