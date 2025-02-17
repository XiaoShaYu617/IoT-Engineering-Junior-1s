module divider(
    input              clk,         // 时钟信号
    input              div_begin,   // 除法开始信号
    input      [31:0]  div_op1,     // 被除数
    input      [31:0]  div_op2,     // 除数
    output reg [31:0]  quotient,    // 商
    output reg [31:0]  remainder,   // 余数
    output reg         div_end      // 除法结束信号
);

    // 状态机状态定义
    reg [1:0] state;
    parameter IDLE  = 2'b00, // 空闲状态
              WORK  = 2'b01, // 计算状态
              DONE  = 2'b10; // 完成状态

    reg [31:0] dividend;  // 当前被除数
    reg [31:0] divisor;   // 当前除数
    reg [31:0] temp_quotient; // 中间商
    reg [31:0] temp_remainder; // 中间余数
    reg [5:0]  bit_index;  // 位数计数器

    always @(posedge clk) begin
        case (state)
            IDLE: begin
                div_end <= 0;  // 清除结束信号
                if (div_begin) begin
                    // 初始化计算
                    dividend       <= div_op1;
                    divisor        <= div_op2;
                    temp_quotient  <= 0;
                    temp_remainder <= 0;
                    bit_index      <= 31;
                    state          <= WORK;
                end
            end
            WORK: begin
                if (bit_index >= 0) begin
                    temp_remainder = {temp_remainder[30:0], dividend[31]};
                    dividend = dividend << 1;

                    if (temp_remainder >= divisor) begin
                        temp_remainder = temp_remainder - divisor;
                        temp_quotient = (temp_quotient << 1) | 1;
                    end else begin
                        temp_quotient = temp_quotient << 1;
                    end

                    // 更新位计数
                    if (bit_index == 0) begin
                        state <= DONE;
                    end else begin
                        bit_index <= bit_index - 1;
                    end
                end
            end
            DONE: begin
                // 计算完成，输出结果
                quotient <= temp_quotient;
                remainder <= temp_remainder;
                div_end <= 1;  // 置高结束信号
                state <= IDLE; // 返回空闲状态
            end
            default: state <= IDLE; // 安全防护
        endcase
    end
endmodule
