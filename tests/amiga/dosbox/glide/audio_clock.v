// Digital sampling check for the unmodified upstream audio_clock module.
// Explicit zero initialization is a test precondition, not a startup test.
// This simulation cannot model metastability or establish hardware safety.
`timescale 1ps/1ps
module audio_clock_test;
  parameter integer HALF_PS = 40000;
  parameter integer PHASE_PS = 1000;
  reg sample_clock = 0, external_clock = 0;
  wire sampled_clock, divided_clock;
  audio_clock dut(external_clock, sample_clock, divided_clock, sampled_clock);
  always #5000 sample_clock = ~sample_clock;
  time rises[0:199];
  time falls[0:199];
  integer inputs = 0, fall_count = 0, outputs = 0, divisions = 0;
  time delay_ps, period_ps, last_sampled = 0;
  time min_delay = 1000000, max_delay = 0;
  time min_period = 1000000, max_period = 0;
  initial begin
    dut.clkbuf = 0;
    dut.clkgen = 0;
    #(PHASE_PS);
    repeat (200) begin
      external_clock = 1;
      #(HALF_PS);
      external_clock = 0;
      #(HALF_PS);
    end
    #100000;
    if (outputs != inputs || divisions != 25)
      $fatal(1, "Lost edges: input=%0d output=%0d divided=%0d", inputs, outputs, divisions);
    $display("PASS half_ps=%0d phase_ps=%0d rises=%0d latency_ps=%0d:%0d period_ps=%0d:%0d divided_rises=%0d",
      HALF_PS, PHASE_PS, outputs, min_delay, max_delay, min_period, max_period, divisions);
    $finish;
  end
  always @(posedge external_clock) begin
    rises[inputs] = $time;
    inputs = inputs + 1;
  end
  always @(negedge external_clock) if (inputs > 0) begin
    falls[fall_count] = $time;
    fall_count = fall_count + 1;
  end
  always @(posedge sampled_clock) begin
    if (outputs >= inputs) $fatal(1, "Unexpected sampled edge");
    delay_ps = $time - rises[outputs];
    if (delay_ps < 20000 || delay_ps > 30000)
      $fatal(1, "Unexpected sampling latency %0d", delay_ps);
    if (delay_ps < min_delay) min_delay = delay_ps;
    if (delay_ps > max_delay) max_delay = delay_ps;
    if (outputs > 0) begin
      period_ps = $time - last_sampled;
      if (period_ps < 2*HALF_PS-10000 || period_ps > 2*HALF_PS+10000)
        $fatal(1, "Unexpected sampled period %0d", period_ps);
      if (period_ps < min_period) min_period = period_ps;
      if (period_ps > max_period) max_period = period_ps;
    end
    last_sampled = $time;
    outputs = outputs + 1;
  end
  always @(posedge divided_clock) begin
    if (fall_count != 4 + 8*divisions)
      $fatal(1, "Divider edge on fall %0d, expected %0d", fall_count, 4+8*divisions);
    if ($time - falls[fall_count-1] < 20000 || $time - falls[fall_count-1] > 30000)
      $fatal(1, "Divider edge does not follow sampled falling input");
    divisions = divisions + 1;
  end
endmodule
