# Run only after reviewing the routed reports. This writes a local bitstream;
# it does not program hardware or close inherited timing-coverage gaps.
if {$argc != 1} {error "Expected isolated board directory"}
cd [file normalize [lindex $argv 0]]
if {[catch {
 set_param general.maxThreads 2
 if {[file exists completion.bit]} {error "Bitstream already exists"}
 open_checkpoint completion-routed.dcp
 if {[get_property PART [current_design]] ne "xc7z020clg400-1"} {error "Wrong device"}
 if {![llength [get_cells -hier -filter {NAME =~ *grx_completion_0*}]]} {
  error "Missing completion endpoint"
 }
 if {[llength [get_cells -hier -filter {IS_BLACKBOX == 1}]]} {error "Unimplemented cells"}
 foreach delay {max min} {
  set paths [get_timing_paths -delay_type $delay -max_paths 1]
  if {![llength $paths]} {error "Missing $delay timing path"}
  if {[get_property SLACK [lindex $paths 0]] < 0} {error "Failed $delay timing"}
 }
 report_timing_summary -report_unconstrained -check_timing_verbose -file completion-bit-timing.rpt
 report_drc -file completion-bit-drc.rpt
 write_bitstream completion.bit
 puts GRX_BITSTREAM_WRITTEN_NOT_INSTALLED
} message options]} {
 puts stderr $message
 puts stderr $options
 exit 1
}
exit 0
