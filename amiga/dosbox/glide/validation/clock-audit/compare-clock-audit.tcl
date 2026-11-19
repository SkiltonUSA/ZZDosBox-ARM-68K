cd [file dirname [info script]]
set_param general.maxThreads 2
proc worst_slack {kind} {
    return [get_property SLACK [get_timing_paths -delay_type $kind -max_paths 1]]
}
if {[catch {
    open_checkpoint baseline-routed.dcp
    set before_max [worst_slack max]
    set before_min [worst_slack min]
    puts "CLOCK_CONTROL_BEFORE $before_max $before_min"
    reset_timing
    read_xdc clock-audit-original.xdc
    set after_max [worst_slack max]
    set after_min [worst_slack min]
    puts "CLOCK_CONTROL_AFTER $after_max $after_min"
    if {abs($before_max-$after_max)>0.00001 || abs($before_min-$after_min)>0.00001} {
        error "Unchanged constraint round-trip changed worst timing; stop candidate"
    }
    report_timing_summary -report_unconstrained -file clock-control-timing.rpt
    reset_timing
    read_xdc clock-audit-inherited.xdc
    report_clocks -file clock-inherited-clocks.rpt
    report_timing_summary -report_unconstrained -file clock-inherited-timing.rpt
    report_methodology -file clock-inherited-methodology.rpt
    report_cdc -file clock-inherited-cdc.rpt
    puts "CLOCK_INHERITED_SLACK [worst_slack max] [worst_slack min]"
    puts CLOCK_COMPARISON_COMPLETED_NO_DESIGN_SAVED
} message options]} {
    puts "CLOCK_COMPARISON_ERROR $message"
    puts $options
    exit 1
}
exit 0
