# Improve the recorded routed candidate without changing RTL or constraints.
# A negative setup/hold/skew result remains a failed candidate, never a waiver.
if {$argc != 1} {error "Expected isolated board directory"}
cd [file normalize [lindex $argv 0]]
if {[catch {
 set_param general.maxThreads 2
 if {[file exists completion-refined.dcp]} {error "Refined checkpoint already exists"}
 open_checkpoint completion-routed.dcp
 phys_opt_design -directive AggressiveExplore
 route_design -directive Explore
 report_route_status -file completion-refined-status.rpt
 report_timing_summary -report_unconstrained -check_timing_verbose -file completion-refined-timing.rpt
 report_drc -file completion-refined-drc.rpt
 report_bus_skew -file completion-refined-bus-skew.rpt
 report_timing -max_paths 60 -nworst 4 -slack_lesser_than 0 -file completion-refined-failing.rpt
 write_checkpoint completion-refined.dcp
 puts GRX_REFINED_REQUIRES_TIMING_REVIEW_NOT_INSTALLED
} message options]} {
 puts stderr $message
 puts stderr $options
 exit 1
}
exit 0
