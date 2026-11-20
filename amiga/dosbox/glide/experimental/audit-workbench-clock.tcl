# Steady-state check of the observed Workbench 1920x800 firmware preset.
# Changes the in-memory timing model only. No checkpoint or bitstream is saved.
if {$argc != 1} {error "Expected isolated board directory"}
cd [file normalize [lindex $argv 0]]
set_param general.maxThreads 2
proc worst {kind} {return [get_property SLACK [get_timing_paths -delay_type $kind -max_paths 1]]}
if {[catch {
 open_checkpoint completion-routed.dcp
 set pll [get_cells zz9000_ps_i/clk_wiz_0/inst/CLK_CORE_DRP_I/clk_inst/plle2_adv_inst]
 if {[llength $pll] != 1} {error "Expected one video PLL"}
 set original [list CLKFBOUT_MULT [get_property CLKFBOUT_MULT $pll] DIVCLK_DIVIDE [get_property DIVCLK_DIVIDE $pll] CLKOUT0_DIVIDE [get_property CLKOUT0_DIVIDE $pll]]
 set before_max [worst max]
 set before_min [worst min]
 # Recorded video.c preset: 100 MHz * 15 / 1 / 12 = 125 MHz.
 set_property -dict {CLKFBOUT_MULT 15 DIVCLK_DIVIDE 1 CLKOUT0_DIVIDE 12} $pll
 update_timing
 set period [get_property PERIOD [get_clocks clk_out1_zz9000_ps_clk_wiz_0_0]]
 if {abs($period-8.0)>0.002} {error "125 MHz clock did not propagate"}
 puts "GRX_WORKBENCH_125 WNS=[worst max] WHS=[worst min]"
 report_timing_summary -report_unconstrained -check_timing_verbose -file completion-workbench125-timing.rpt
 report_timing -max_paths 10 -file completion-workbench125-paths.rpt
 report_drc -checks [get_drc_checks PDRC-*] -file completion-workbench125-pll.rpt
 report_bus_skew -file completion-workbench125-bus-skew.rpt
 set_property -dict $original $pll
 update_timing
 if {abs([worst max]-$before_max)>0.00001 || abs([worst min]-$before_min)>0.00001} {
  error "Restored timing differs from original"
 }
 puts GRX_WORKBENCH_CLOCK_AUDIT_COMPLETE_REVIEW_REPORTS_NO_HARDWARE_CHANGE
} message options]} {
 puts stderr $message
 puts stderr $options
 exit 1
}
exit 0
