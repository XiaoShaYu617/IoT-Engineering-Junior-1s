`timescale 1ns / 1ps
//*************************************************************************
//   > 文件名: fetch.v
//   > 描述  :五级流水CPU的取指模块
//   > 作者  : LOONGSON
//   > 日期  : 2016-04-14
//*************************************************************************
`define STARTADDR 32'H00000034   // 程序起始地址为34H
module fetch(                    // 取指级
    input             clk,       // 时钟
    input             resetn,    // 复位信号，低电平有效
    input             IF_valid,  // 取指级有效信号
    input             next_fetch,// 取下一条指令，用来锁存PC值
    input      [31:0] inst,      // inst_rom取出的指令
    input      [32:0] jbr_bus,   // 跳转总线
    output     [31:0] inst_addr, // 发往inst_rom的取指地址
    output         IF_over,   // IF模块执行完成
    output     [63:0] IF_ID_bus, // IF->ID总线
    
    //5级流水新增接口
    input      [32:0] exc_bus,   // Exception pc总线
        
    //展示PC和取出的指令
    output     [31:0] IF_pc,
    output     [31:0] IF_inst,
    output     [31:0] print_prior_seq_pc,
    output            print_jbr_taken,
    output            prior_predict_jbr_taken
);

//-----{程序计数器PC}begin
    wire [31:0] next_pc;
    wire [31:0] seq_pc;
    reg  [31:0] pc;
    
    //跳转pc
    wire        jbr_taken;
    wire [31:0] jbr_target;
    assign {jbr_taken, jbr_target} = jbr_bus;  // 跳转总线传是否跳转和目标地址
    assign print_jbr_taken=jbr_taken;
    //Exception PC
    wire        exc_valid;
    wire [31:0] exc_pc;
    assign {exc_valid,exc_pc} = exc_bus;
    
    //pc+4
    assign seq_pc[31:2]    = pc[31:2] + 1'b1;  // 下一指令地址：PC=PC+4
    assign seq_pc[1:0]     = pc[1:0];

//-----{分支预测}begin
//分支预测: 所有除寄存器跳转指令之外的分支跳转指令
wire [5:0] op;     
wire [4:0] rs; 
wire [4:0] rt;   
wire [4:0] rd;       
wire [4:0] sa;      
wire [5:0] funct;    
wire op_zero;  // 操作码全0
wire sa_zero;  // sa域全0
wire inst_J, inst_JAL, inst_JALR, inst_JR;
wire inst_BEQ, inst_BGEZ, inst_BGTZ, inst_BLEZ, inst_BLTZ, inst_BNE;
    
assign op     = inst[31:26];  // 操作码
assign rs     = inst[25:21];  // 源操作数1
assign rt     = inst[20:16];  // 源操作数2
assign rd     = inst[15:11];  // 目标操作数
assign sa     = inst[10:6];   // 特殊域，可能存放偏移量
assign funct  = inst[5:0];    // 功能码
assign op_zero = ~(|op);
assign sa_zero = ~(|sa);
assign inst_J     = (op == 6'b000010);             //跳转
assign inst_JAL   = (op == 6'b000011);             //跳转和链接
assign inst_JALR  = op_zero & (rt==5'd0) & (rd==5'd31)
                  & sa_zero & (funct == 6'b001001);         //跳转寄存器并链接
assign inst_JR    = op_zero & (rt==5'd0) & (rd==5'd0 )
                  & sa_zero & (funct == 6'b001000);             //跳转寄存器
assign inst_BEQ   = ~op_zero & (op == 6'b000100);             //判断相等跳转
assign inst_BGEZ  = ~op_zero & (op == 6'b000001) & (rt==5'd1);//大于等于0跳转
assign inst_BGTZ  = ~op_zero & (op == 6'b000111) & (rt==5'd0);//大于0跳转
assign inst_BLEZ  = ~op_zero & (op == 6'b000110) & (rt==5'd0);//小于等于0跳转
assign inst_BLTZ  = ~op_zero & (op == 6'b000001) & (rt==5'd0);//小于0跳转
assign inst_BNE   = ~op_zero & (op == 6'b000101);             //判断不等跳转

wire inst_jr;    //寄存器跳转指令
assign inst_jr     = inst_JALR | inst_JR;
wire [31:0] j_target;
wire [31:0] bd_pc;
wire j_taken;
wire br_taken;
assign bd_pc = inst_addr + 3'b100;
assign j_target = {bd_pc[31:28],inst[25:0],2'b00};
assign j_taken = inst_J | inst_JAL;
assign br_taken = inst_BEQ | inst_BNE | inst_BGEZ | inst_BGTZ | inst_BLEZ | inst_BLTZ;
wire [15:0] offset;
assign offset = inst[15:0];
wire [31:0] br_target;
wire predict_jbr_taken;
wire [31:0] predict_jbr_target;
assign br_target[31:2] = bd_pc[31:2] + {{14{offset[15]}}, offset};  
assign br_target[1:0]  = bd_pc[1:0];
assign predict_jbr_taken = j_taken | br_taken; 
assign predict_jbr_target = j_taken ? j_target : br_target;

reg prior_predict_jbr_taken;
reg prior_is_inst_jr;
reg [31:0] prior_seq_pc;
always @(posedge clk)   
begin
    prior_predict_jbr_taken <= predict_jbr_taken;
    prior_is_inst_jr <= inst_jr;
    prior_seq_pc <= seq_pc;
end
assign  print_prior_seq_pc = prior_seq_pc ;

assign  print_prior_predict_jbr_taken=prior_predict_jbr_taken;
//-----{分支预测}end

    // 新指令：若有Exception,则PC为Exceptio入口地址
    //         若指令跳转，则PC为跳转地址；否则为pc+4
//    assign next_pc = exc_valid ? exc_pc : 
//                     jbr_taken ? jbr_target : seq_pc;
assign next_pc = exc_valid ? exc_pc : 
                 (prior_predict_jbr_taken & ~jbr_taken) ? prior_seq_pc :
                 (prior_is_inst_jr & jbr_taken) ? jbr_target : 
                 predict_jbr_taken ? predict_jbr_target : seq_pc;

    always @(posedge clk)    // PC程序计数器
    begin
        if (!resetn)
        begin
            pc <= `STARTADDR; // 复位，取程序起始地址
        end
        else if (next_fetch)
        begin
            pc <= next_pc;    // 不复位，取新指令
        end
    end
//-----{程序计数器PC}end

//-----{发往inst_rom的取指地址}begin
    assign inst_addr = pc;
//-----{发往inst_rom的取指地址}end

//-----{IF执行完成}begin
    //由于指令rom为同步读写的,
    //取数据时，有一拍延时
    //即发地址的下一拍时钟才能得到对应的指令
    //故取指模块需要两拍时间
    //故每次PC刷新，IF_over都要置0
    //然后将IF_valid锁存一拍即是IF_over信号
    assign IF_over = IF_valid;
//    always @(*) begin
//        IF_over = IF_valid;
//    end
//    always @(posedge clk)
//    begin
//        if (!resetn || next_fetch)
//        begin
//            IF_over <= 1'b0;
//        end
//        else
//        begin
//            IF_over <= IF_valid;
//        end
//    end
    //如果指令rom为异步读的，则IF_valid即是IF_over信号，
    //即取指一拍完成
//-----{IF执行完成}end

//-----{IF->ID总线}begin
    assign IF_ID_bus = {pc, inst};  // 取指级有效时，锁存PC和指令
   //assign IF_ID_bus = (~jbr_taken & prior_predict_jbr_taken) | (prior_is_inst_jr & jbr_taken) ? {64'h0} : {pc, inst};
//-----{IF->ID总线}end

//-----{展示IF模块的PC值和指令}begin
    assign IF_pc   = pc;
    assign IF_inst = inst;
//-----{展示IF模块的PC值和指令}end


endmodule