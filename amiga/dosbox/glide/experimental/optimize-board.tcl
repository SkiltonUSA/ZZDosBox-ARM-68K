# Open the completed synthesis checkpoint in a fresh Vivado process.
# Usage: -source optimize-board.tcl -tclargs <isolated-board-directory>
# No bitstream generation or hardware programming.
if {$argc != 1} {error "Expected one isolated board directory"}
cd [file normalize [lindex $argv 0]]
if {[catch {
 set_param general.maxThreads 2
 if {[file exists completion-optimized.dcp]} {error "Optimization output already exists"}
 open_checkpoint completion-synth.dcp
 puts "GRX_E7M_DEDICATED_ROUTE [get_property CLOCK_DEDICATED_ROUTE [get_nets ZORRO_E7M_IBUF]]"
 puts "GRX_MMCM_OUT1_CLOCKS [get_clocks -quiet -of_objects [get_pins zz9000_ps_i/MNTZorro_v0_1_S00_AXI_0/inst/mmcm_adv_inst/CLKOUT1]]"
 opt_design
 set boxes [get_cells -hierarchical -filter {IS_BLACKBOX == 1}]
 puts "GRX_OPT_BLACKBOX_COUNT [llength $boxes]"
 if {[llength $boxes]} {error "Unimplemented cells remain: $boxes"}
 report_utilization -file completion-opt-utilization.rpt
 report_drc -file completion-opt-drc.rpt
 report_clocks -file completion-opt-clocks.rpt
 report_cdc -file completion-opt-cdc.rpt
 report_timing_summary -report_unconstrained -check_timing_verbose -file completion-opt-timing.rpt
 write_checkpoint completion-optimized.dcp
 puts GRX_BOARD_OPTIMIZED_NOT_ROUTED_OR_INSTALLED
} message options]} {
 puts stderr $message
 puts stderr $options
 exit 1
}
exit 0
