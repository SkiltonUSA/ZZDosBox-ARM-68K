# Test 1: same output frequency, legal VCO candidate. Reports only.
cd [file dirname [info script]]
set_param general.maxThreads 2
proc slack {kind} {return [get_property SLACK [get_timing_paths -delay_type $kind -max_paths 1]]}
if {[catch {
  open_checkpoint baseline-routed.dcp
  set pll [get_cells zz9000_ps_i/clk_wiz_0/inst/CLK_CORE_DRP_I/clk_inst/plle2_adv_inst]
  if {[llength $pll] != 1} {error "Expected one video PLL"}
  set original [list CLKFBOUT_MULT [get_property CLKFBOUT_MULT $pll] DIVCLK_DIVIDE [get_property DIVCLK_DIVIDE $pll] CLKOUT0_DIVIDE [get_property CLKOUT0_DIVIDE $pll]]
  set before_max [slack max]
  set before_min [slack min]
  if {abs($before_max-0.156)>0.00001 || abs($before_min-0.010)>0.00001} {error "Baseline does not match recorded timing"}
  set rows [open pll-ratio-check.tsv w]
  puts $rows "case\tmult\tdiv\toutdiv\tperiod_ns\twns_ns\twhs_ns"
  foreach mode {{original 41 2 14} {candidate 41 4 7}} {
    lassign $mode name mult div outdiv
    set_property -dict [list CLKFBOUT_MULT $mult DIVCLK_DIVIDE $div CLKOUT0_DIVIDE $outdiv] $pll
    update_timing
    set period [get_property PERIOD [get_clocks clk_out1_zz9000_ps_clk_wiz_0_0]]
    if {abs($period-6.82926829268)>0.002} {error "Output frequency changed for $name"}
    puts $rows "$name\t$mult\t$div\t$outdiv\t$period\t[slack max]\t[slack min]"
    flush $rows
    puts "PLL_RATIO_RESULT $name period=$period WNS=[slack max] WHS=[slack min]"
    report_timing_summary -report_unconstrained -file pll-ratio-$name-timing.rpt
    report_drc -checks [get_drc_checks PDRC-*] -file pll-ratio-$name-drc.rpt
  }
  close $rows
  set_property -dict $original $pll
  update_timing
  if {abs([slack max]-$before_max)>0.00001 || abs([slack min]-$before_min)>0.00001} {error "Restored control mismatch"}
  help phys_opt_design
  puts PLL_RATIO_CHECK_COMPLETED_ORIGINAL_RESTORED_NO_CHECKPOINT_SAVED
} message options]} {puts "PLL_RATIO_CHECK_ERROR $message"; puts $options; exit 1}
exit 0
