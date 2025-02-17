`timescale 1ns / 1ps

////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:
//
// Create Date:   11:57:16 04/23/2016
// Design Name:   pipeline_cpu
// Module Name:   F:/new_lab/8_pipeline_cpu/tb.v
// Project Name:  pipeline_cpu
// Target Device:  
// Tool versions:  
// Description: 
//
// Verilog Test Fixture created by ISE for module: pipeline_cpu
//
// Dependencies:
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
////////////////////////////////////////////////////////////////////////////////

module tb;

    // Inputs
    reg clk;
    reg resetn;
    reg [4:0] rf_addr;
    reg [31:0] mem_addr;

    // Outputs
    wire [31:0] rf_data;
    wire [31:0] mem_data;
    wire [31:0] IF_pc;
    wire [31:0] IF_inst;
    wire [31:0] ID_pc;
    wire [31:0] EXE_pc;
    wire [31:0] MEM_pc;
    wire [31:0] WB_pc;
    wire [31:0] cpu_5_valid;
    wire [31:0] print_rf_wdata;
    wire [31:0] print_dm_wdata;
    wire [31:0] print_prior_seq_pc;
    wire print_jbr_taken;
    wire prior_predict_jbr_taken;
    wire [32:0]print_jbr_bus;
    wire [31:0]print_exe_result;
    wire [31:0]print_rs_value;
    wire [31:0]print_rt_value;
    wire [168:0] print_ID_EXE_bus;
    wire  print_modply;
    wire [31:0]print_quotient;
    wire    [31:0]print_alu_operand1;
    wire     [31:0]print_alu_operand2;
    // Instantiate the Unit Under Test (UUT)
    pipeline_cpu uut (
        .clk(clk), 
        .resetn(resetn), 
        .rf_addr(rf_addr), 
        .mem_addr(mem_addr), 
        .rf_data(rf_data), 
        .mem_data(mem_data), 
        .IF_pc(IF_pc), 
        .IF_inst(IF_inst), 
        .ID_pc(ID_pc), 
        .EXE_pc(EXE_pc), 
        .MEM_pc(MEM_pc), 
        .WB_pc(WB_pc), 
        .cpu_5_valid(cpu_5_valid),
        .print_rf_wdata(print_rf_wdata),
        .print_dm_wdata(print_dm_wdata),
        .print_prior_seq_pc(print_prior_seq_pc),
        .print_jbr_taken(print_jbr_taken),
        .prior_predict_jbr_taken(prior_predict_jbr_taken),
        .print_jbr_bus(print_jbr_bus),
        .print_exe_result(print_exe_result),
        .print_rs_value(print_rs_value),
        .print_rt_value(print_rt_value),
        .print_ID_EXE_bus(print_ID_EXE_bus),
        . print_modply( print_modply),
        .print_quotient(print_quotient),
        .print_alu_operand1(print_alu_operand1),
        .print_alu_operand2(print_alu_operand2)
    );

    initial begin
        // Initialize Inputs
        clk = 0;
        resetn = 0;
        rf_addr = 4'b1111;
        mem_addr = 0;

        // Wait 100 ns for global reset to finish
        #100;
      resetn = 1;
        // Add stimulus here
    end
   always #5 clk=~clk;
endmodule
