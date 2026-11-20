# Run after integrate-board.tcl in an isolated staged project. No bitstream/flash.
cd [file dirname [file dirname [file normalize [info script]]]]
if {[catch {
 set_param general.maxThreads 2
 if {![file exists completion-integrated.xsa]} {error "Integrate and validate the board first"}
 open_project fpga/XACPGPU_G4C/ZZ9000_proto/ZZ9000_proto.xpr
 set bd [get_files */zz9000_ps.bd]
 set_property SYNTH_CHECKPOINT_MODE NONE $bd
 open_bd_design $bd
 if {[llength [get_bd_cells -quiet grx_completion_0]] != 1} {error "Completion endpoint missing"}
 validate_bd_design
 update_compile_order -fileset sources_1
 synth_design -top zz9000_ps_wrapper -part xc7z020clg400-1
 report_utilization -file completion-synth-utilization.rpt
 report_drc -file completion-synth-drc.rpt
 write_checkpoint -force completion-synth.dcp
 set f [open completion-synth-blackboxes.txt w]
 puts $f [get_cells -hier -filter {IS_BLACKBOX == 1}]
 close $f
 puts "GRX_BOARD_SYNTHESIZED_NOT_ROUTED_OR_INSTALLED"
 close_project
} message options]} {
 puts stderr $message
 puts stderr $options
 exit 1
}
exit 0
