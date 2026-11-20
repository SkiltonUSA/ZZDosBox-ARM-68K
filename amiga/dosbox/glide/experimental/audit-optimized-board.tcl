# Independently reopen an optimized checkpoint when the original process exit
# was not retained. This does not replace routing or timing review.
if {$argc != 1} {error "Expected one isolated board directory"}
cd [file normalize [lindex $argv 0]]
if {[catch {
 set_param general.maxThreads 2
 open_checkpoint completion-optimized.dcp
 if {[get_property PART [current_design]] ne "xc7z020clg400-1"} {error "Wrong device"}
 if {![llength [get_cells -hier -filter {NAME =~ *grx_completion_0*}]]} {
  error "Missing completion endpoint"
 }
 if {[llength [get_cells -hier -filter {IS_BLACKBOX == 1}]]} {
  error "Unimplemented cells remain"
 }
 report_utilization -file completion-opt-audit-utilization.rpt
 report_drc -file completion-opt-audit-drc.rpt
 report_clocks -file completion-opt-audit-clocks.rpt
 report_cdc -file completion-opt-audit-cdc.rpt
 report_timing_summary -report_unconstrained -check_timing_verbose -file completion-opt-audit-timing.rpt
 puts GRX_OPT_CHECKPOINT_REOPENED_NOT_ROUTED_OR_INSTALLED
} message options]} {
 puts stderr $message
 puts stderr $options
 exit 1
}
exit 0
