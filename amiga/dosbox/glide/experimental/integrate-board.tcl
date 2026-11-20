# Experimental completion endpoint integration. Run only in a staged project copy.
# Does not write a bitstream, alter clocks or program hardware.
cd [file dirname [file dirname [file normalize [info script]]]]
proc one {objects description} {
 if {[llength $objects] != 1} {error "Expected one $description; got $objects"}
 return [lindex $objects 0]
}
proc attach {source destination} {
 set src [one [get_bd_pins $source] $source]
 set dst [one [get_bd_pins $destination] $destination]
 set net [get_bd_nets -quiet -of_objects $src]
 if {[llength $net]} {connect_bd_net -net [one $net $source] $dst} else {connect_bd_net $src $dst}
}
if {[catch {
 set_param general.maxThreads 2
 open_project fpga/XACPGPU_G4C/ZZ9000_proto/ZZ9000_proto.xpr
 add_files [glob completion/*.v]
 # Parameter-list localparams in the renderer require SystemVerilog parsing.
 set_property FILE_TYPE SystemVerilog [get_files */RasterIX_G1/RasterIX/rtl/*/*.v]
 # Vivado module references require a Verilog top even with SV child modules.
 set_property FILE_TYPE Verilog [get_files */RasterIX_G1/RasterIX/rtl/RasterIX/RasterIX.v]
 update_compile_order -fileset sources_1
 set bd [one [get_files */zz9000_ps.bd] zz9000_ps.bd]
 set_property SYNTH_CHECKPOINT_MODE NONE $bd
 open_bd_design $bd
 update_module_reference [one [get_ips *RasterIX_0*] "RasterIX module reference"]
 # The published GP1 peripheral interconnect already uses M08 at the GPU clock.
 set interconnect [get_bd_cells ps7_0_axi_periph]
 if {[get_property CONFIG.NUM_MI $interconnect] != 9} {error "Unexpected peripheral topology"}
 set_property CONFIG.NUM_MI 10 $interconnect
 create_bd_cell -type module -reference grx_completion_endpoint grx_completion_0
 attach processing_system7_0/FCLK_CLK0 grx_completion_0/aclk
 attach proc_sys_reset_0/peripheral_aresetn grx_completion_0/aresetn
 attach processing_system7_0/FCLK_CLK0 ps7_0_axi_periph/M09_ACLK
 attach proc_sys_reset_0/peripheral_aresetn ps7_0_axi_periph/M09_ARESETN
 connect_bd_intf_net [get_bd_intf_pins ps7_0_axi_periph/M09_AXI] [get_bd_intf_pins grx_completion_0/S_AXI]
 set ready [get_bd_pins RasterIX_0/m_cmd_resp_axis_tready]
 foreach net [get_bd_nets -quiet -of_objects $ready] {disconnect_bd_net $net $ready}
 connect_bd_intf_net [get_bd_intf_pins RasterIX_0/m_cmd_resp_axis] [get_bd_intf_pins grx_completion_0/S_RESP_AXIS]
 attach RasterIX_0/gpu_idle grx_completion_0/gpu_idle
 attach RasterIX_0/gpu_fault grx_completion_0/gpu_fault
 set cpu [get_bd_addr_spaces processing_system7_0/Data]
 foreach seg [get_bd_addr_segs -of_objects $cpu] {
  set start [get_property OFFSET $seg]
  set size [get_property RANGE $seg]
  if {($start < 0x83c90000) && (($start+$size) > 0x83c80000)} {error "Experimental address overlaps $seg"}
 }
 set reg [one [get_bd_addr_segs grx_completion_0/S_AXI/*] "completion address block"]
 assign_bd_address -offset 0x83c80000 -range 0x10000 -target_address_space $cpu $reg
 validate_bd_design
 save_bd_design
 generate_target all $bd
 update_compile_order -fileset sources_1
 write_bd_tcl -force completion-integrated-bd.tcl
 write_hw_platform -fixed -force completion-integrated.xsa
 report_ip_status -file completion-ip-status.rpt
 set f [open completion-topology.txt w]
 puts $f "Candidate only; no released XACP ABI or installed firmware"
 puts $f "Completion register block: $reg"
 foreach name {grx_completion_0 ps7_0_axi_periph/m09_couplers/auto_cc axi_interconnect_4} {
  set cell [get_bd_cells -quiet $name]
  if {[llength $cell]} {puts $f "$name [get_property VLNV $cell]"}
 }
 close $f
 puts "GRX_BOARD_INTEGRATION_VALIDATED_NO_BITSTREAM"
 close_project
} message options]} {
 puts stderr $message
 puts stderr $options
 exit 1
}
exit 0
