# Run separately after reviewing the optimized checkpoint and its reports.
# Usage: -source route-board.tcl -tclargs <isolated-board-directory> [--resume-placed]
# Routing completion does not establish timing sign-off. No bitstream is made.
if {$argc < 1 || $argc > 2} {error "Expected board directory and optional --resume-placed"}
set resume [expr {$argc == 2}]
if {$resume && [lindex $argv 1] ne "--resume-placed"} {error "Unknown resume option"}
cd [file normalize [lindex $argv 0]]
if {[catch {
 set_param general.maxThreads 2
 if {[file exists completion-routed.dcp] || (!$resume && [file exists completion-placed.dcp])} {
  error "Placement/routing output already exists"
 }
 if {$resume} {
  open_checkpoint completion-placed.dcp
 } else {
  open_checkpoint completion-optimized.dcp
 }
 if {[llength [get_cells -hierarchical -filter {IS_BLACKBOX == 1}]]} {
  error "Unimplemented cells in optimized checkpoint"
 }
 if {!$resume} {
  place_design
  report_timing_summary -file completion-placed-timing.rpt
  write_checkpoint completion-placed.dcp
 }
 phys_opt_design
 route_design
 report_route_status -file completion-routed-status.rpt
 report_timing_summary -report_unconstrained -check_timing_verbose -file completion-routed-timing.rpt
 report_utilization -file completion-routed-utilization.rpt
 report_drc -file completion-routed-drc.rpt
 report_methodology -file completion-routed-methodology.rpt
 report_cdc -file completion-routed-cdc.rpt
 report_bus_skew -file completion-routed-bus-skew.rpt
 write_checkpoint completion-routed.dcp
 puts GRX_BOARD_ROUTED_REQUIRES_TIMING_REVIEW_NOT_INSTALLED
} message options]} {
 puts stderr $message
 puts stderr $options
 exit 1
}
exit 0
