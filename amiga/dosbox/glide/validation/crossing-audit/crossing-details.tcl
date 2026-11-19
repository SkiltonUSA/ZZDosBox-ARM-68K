cd [file dirname [info script]]
set_param general.maxThreads 2
if {[catch {
    open_checkpoint baseline-routed.dcp
    report_cdc -details -file crossing-details.rpt
    foreach cell [get_cells {zz9000_ps_i/audio_clock_0/inst/clkbuf_reg[*]}] {
        puts "AUDIO_CELL $cell INIT=[get_property INIT $cell] ASYNC_REG=[get_property ASYNC_REG $cell] LOC=[get_property LOC $cell]"
    }
    puts CROSSING_DETAILS_COMPLETED
} message options]} {
    puts "CROSSING_DETAILS_ERROR $message"
    puts $options
    exit 1
}
exit 0
