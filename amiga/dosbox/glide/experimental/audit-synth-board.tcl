# Independently reopen a synthesis checkpoint; no design changes or programming.
# Usage: -source audit-synth-board.tcl -tclargs <isolated-board-directory>
if {$argc != 1} {error "Expected one isolated board directory"}
cd [file normalize [lindex $argv 0]]
if {[catch {
 set_param general.maxThreads 2
 open_checkpoint completion-synth.dcp
 if {[get_property PART [current_design]] ne "xc7z020clg400-1"} {error "Wrong device"}
 if {![llength [get_cells -hier -filter {NAME =~ *grx_completion_0*}]]} {
  error "Missing completion endpoint"
 }
 set boxes [get_cells -hier -filter {IS_BLACKBOX == 1}]
 foreach box $boxes {
  if {$box ni {dbg_hub u_ila_0}} {error "Unexpected black box: $box"}
 }
 puts "GRX_SYNTH_AUDIT_DEBUG_PLACEHOLDERS $boxes"
 report_utilization -file completion-synth-audit-utilization.rpt
 report_drc -file completion-synth-audit-drc.rpt
 report_clocks -file completion-synth-audit-clocks.rpt
 report_timing_summary -report_unconstrained -check_timing_verbose -file completion-synth-audit-timing.rpt
 puts GRX_SYNTH_CHECKPOINT_REOPENED_NOT_TIMING_SIGNOFF
} message options]} {
 puts stderr $message
 puts stderr $options
 exit 1
}
exit 0
