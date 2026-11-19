// Consumer-side test of unchanged upstream video_formatter.v.
// Known post-startup state; source beam counter is replaced by controlled stimuli.
// This is not scanout, startup, metastability, or complete CDC verification.
`timescale 1ps/1ps
module video_fetch_test;
  parameter integer SCALE = 0;
  parameter integer SKEW = 0;
  parameter integer PHASE = 1000;
  reg clk = 0;
  always #5000 clk = ~clk;
  reg [11:0] request_line = 1;
  reg frame_sync = 0;
  reg valid = 0, last = 0;
  reg [31:0] data = 0;
  wire ready;
  video_formatter dut(
    .m_axis_vid_tdata(data), .m_axis_vid_tlast(last),
    .m_axis_vid_tready(ready), .m_axis_vid_tuser(1'b0),
    .m_axis_vid_tvalid(valid), .m_axis_vid_aclk(clk), .aresetn(1'b1),
    .dvi_clk(1'b0), .control_data(32'b0), .control_op(8'b0),
    .control_interlace(1'b0), .scanline_intensity(8'b0),
    .scanline_width(2'b0), .scanline_parity(1'b0), .scanline_intensity2(8'b0));
  integer cycle = 0, word_index = 0, completed = 0, accepted = 0;
  integer line, expected, before_count, waited, i;
  reg [11:0] previous = 0;
  reg consumed = 0;
  // Stall every fifth beat. Stable payload whenever VALID && !READY.
  always @(negedge clk) begin
    cycle = cycle + 1;
    if (!valid || consumed) begin
      valid = (cycle % 5 != 0);
      last = (word_index == 15);
      data = {16'(completed+1), 16'(word_index)};
    end
  end
  always @(posedge clk) begin
    consumed = valid && ready;
    if (consumed) begin
      accepted = accepted + 1;
      if (word_index == 15) begin
        word_index = 0;
        completed = completed + 1;
      end else word_index = word_index + 1;
    end
  end
  initial begin
    force dut.need_line_fetch = request_line;
    force dut.need_frame_sync = frame_sync;
    dut.next_input_state = 4;
    dut.ready_for_vdma = 0;
    dut.inptr = 0;
    dut.last_line_fetch = 0;
    dut.need_line_fetch_reg = 0;
    dut.need_line_fetch_reg2 = 0;
    dut.need_frame_sync_reg = 0;
    dut.scale_y = SCALE;
    dut.scale_y_effective = SCALE;
    dut.control_interlace_in = 0;
    dut.vsync_request = 0;
    // Verify that the fetch side waits for frame synchronization.
    repeat (12) @(negedge clk);
    if (ready || completed) $fatal(1, "Fetch before frame synchronization");
    frame_sync = 1;
    repeat (4) @(negedge clk);
    frame_sync = 0;
    for (line=1; line<=512; line=line+1) begin
      @(negedge clk); #(PHASE);
      before_count = completed;
      // Stress either rising bits first or falling bits first. This intentionally
      // holds an inconsistent sample for SKEW AXI periods, not a physical model.
      if (SKEW > 0 && (line & (line-1)) == 0 && line > 1) begin
        request_line = (line & 2) ? (previous & 12'(line)) : (previous | 12'(line));
        #(SKEW*10000);
      end
      request_line = line;
      previous = line;
      expected = line >> SCALE;
      waited = 0;
      while (completed < expected && waited < 60) begin
        @(negedge clk); waited = waited + 1;
      end
      if (completed != expected) $fatal(1, "Missing/extra line at %0d: got %0d expected %0d",line,completed,expected);
      // Check complete line payload rather than only internal state transitions.
      if (completed > before_count) begin
        for (i=0; i<16; i=i+1)
          if (dut.line_buffer[i] !== {16'(completed),16'(i)})
            $fatal(1, "Payload mismatch line %0d word %0d got %h", completed,i,dut.line_buffer[i]);
      end
      repeat (40) @(negedge clk);
      if (completed != expected || ready || word_index != 0)
        $fatal(1, "Unrequested fetch at %0d: completed=%0d ready=%0d word=%0d",line,completed,ready,word_index);
    end
    if (accepted != (512 >> SCALE)*16) $fatal(1, "Unexpected accepted beat count %0d",accepted);
    $display("PASS scale=%0d skew_cycles=%0d phase_ps=%0d lines=%0d beats=%0d",SCALE,SKEW,PHASE,completed,accepted);
    $finish;
  end
  initial begin #1000000000; $fatal(1, "Timeout"); end
endmodule
