`timescale 1ns / 1ps
//*************************************************************************
//   > 文件名: inst_rom.v
//   > 描述  ：异步指令存储器模块，采用寄存器搭建而成，类似寄存器堆
//   >         内嵌好指令，只读，异步读
//   > 作者  : LOONGSON
//   > 日期  : 2016-04-14
//*************************************************************************
module inst_rom(
    input             clk,  // 时钟
    input      [7 :0] addr, // 指令地址
    output reg [31:0] inst  // 指令
    );

    wire [31:0] inst_rom[109:0];  // 指令存储器，字节地址7'b000_0000~7'b111_1111
    //------------- 指令编码 ---------|指令地址|--- 汇编指令 -----|- 指令结果 -----//
    assign inst_rom[0] = 32'hAC010000; //00H
    assign inst_rom[1] = 32'hAC020004; //04H
    assign inst_rom[2] = 32'hAC030008; //08H
    assign inst_rom[3] = 32'hAC04000C; //0CH
    assign inst_rom[4] = 32'hAC050010; //10H
    assign inst_rom[5] = 32'hAC060018; //14H
    assign inst_rom[6] = 32'hAC070070; //18H
    assign inst_rom[7] = 32'hAC190074; //1CH
    assign inst_rom[8] = 32'hAC0D0078; //20H
    assign inst_rom[9] = 32'h40017000; //24H
    assign inst_rom[10] = 32'h24210004; //28H
    assign inst_rom[11] = 32'h40817000; //2CH
    assign inst_rom[12] = 32'h42000018; //30H
    assign inst_rom[13] = 32'h24010001; //34H  addi $1, $0, 1     $1=1
    assign inst_rom[14] = 32'h00011100; //38H  sll $2, $1, 4      $2=16
    assign inst_rom[15] = 32'h00411821; //3CH  addu $3, $2, $1    $3=17
    assign inst_rom[16] = 32'h00022082; //40H  srl $4, $2, 2      $4=4
    assign inst_rom[17] = 32'h28990005; //44H  slti $25, $4, 5    $25=1
    assign inst_rom[18] = 32'hFFFFFFFF; //48H  异常指令
    assign inst_rom[19] = 32'h00642823; //4CH
   
    assign inst_rom[20] = 32'hAC050014; //50H
    assign inst_rom[21] = 32'h00A23027; //54H
    assign inst_rom[22] = 32'h00C33825; //58H
    assign inst_rom[23] = 32'h00E64026; //5CH
    //assign inst_rom[24] = 32'h15030002; //60H
    assign inst_rom[24] = 32'h11030002; //60H
    assign inst_rom[25] = 32'hAC08001C; //64H
    assign inst_rom[26] = 32'h0022482A; //68H
    assign inst_rom[27] = 32'h8C0A001C; //6CH
    assign inst_rom[28] = 32'h15450002; //70H
    assign inst_rom[29] = 32'h00415824; //74H
    assign inst_rom[30] = 32'hAC0B001C; //78H
    assign inst_rom[31] = 32'h0C000026; //7CH
    assign inst_rom[32] = 32'hAC040010; //80H
    assign inst_rom[33] = 32'h3C0C000C; //84H
    assign inst_rom[34] = 32'h004CD007; //88H
    assign inst_rom[35] = 32'h275B0044; //8CH
    //assign inst_rom[35] = 32'h275B0040; //8CH
    assign inst_rom[36] = 32'h0360F809; //90H
    assign inst_rom[37] = 32'h24010008; //94H
    assign inst_rom[38] = 32'hA07A0005; //98H
    assign inst_rom[39] = 32'h0143682B; //9CH
    assign inst_rom[40] = 32'h1DA00002; //A0H
    assign inst_rom[41] = 32'h00867004; //A4H
    assign inst_rom[42] = 32'h000E7883; //A8H
    assign inst_rom[43] = 32'h002F8006; //ACH
    assign inst_rom[44] = 32'h1A000007; //B0H
    assign inst_rom[45] = 32'h002F8007; //B4H
    assign inst_rom[46] = 32'h06000006; //B8H
    assign inst_rom[47] = 32'h001A5900; //BCH
    assign inst_rom[48] = 32'h8D5C0003; //C0H
    assign inst_rom[49] = 32'h179D0007; //C4H
    assign inst_rom[50] = 32'hA0AF0008; //C8H
    assign inst_rom[51] = 32'h80B20008; //CCH
    assign inst_rom[52] = 32'h90B30008; //D0H
    assign inst_rom[53] = 32'h2DF8FFFF; //D4H
    assign inst_rom[54] = 32'h0185E825; //D8H
    assign inst_rom[55] = 32'h01600008; //DCH
    assign inst_rom[56] = 32'h31F4FFFF; //E0H
    assign inst_rom[57] = 32'h35F5FFFF; //E4H
    assign inst_rom[58] = 32'h39F6FFFF; //E8H
    assign inst_rom[59] = 32'h019D0018; //ECH
    assign inst_rom[60] = 32'h0000B812; //F0H
    assign inst_rom[61] = 32'h0000F010; //F4H
    assign inst_rom[62] = 32'h03400013; //F8H
    assign inst_rom[63] = 32'h03600011; //FCH
    assign inst_rom[64] = 32'h40807000; //100H
    assign inst_rom[65] = 32'h0000000C; //104H
    assign inst_rom[66] = 32'h40027000; //108H
    assign inst_rom[67] = 32'h40036800; //10CH
    assign inst_rom[68] = 32'h40046000; //110H
    assign inst_rom[69] = 32'h24010020; //114H
    assign inst_rom[70] = 32'h01EE882A; //118H
    assign inst_rom[71] = 32'h3C111234; //11CH
    assign inst_rom[72] = 32'h26315678; //120H
    assign inst_rom[73] = 32'hAC310000; //124H
    assign inst_rom[74] = 32'h00118900; //128H
    assign inst_rom[75] = 32'h1E20FFFD; //12CH
    assign inst_rom[76] = 32'h24210004; //130H
    assign inst_rom[77] = 32'h2402003C; //134H
    assign inst_rom[78] = 32'h8C31FFE4; //138H
    assign inst_rom[79] = 32'h00118902; //13CH
    assign inst_rom[80] = 32'hAC510000; //140H
    assign inst_rom[81] = 32'h1620FFFD; //144H
    assign inst_rom[82] = 32'h24420004; //148H
    assign inst_rom[83] = 32'h24060044; //14CH
    assign inst_rom[84] = 32'h24070064; //150H
    assign inst_rom[85] = 32'h8C23FFE4; //154H
    assign inst_rom[86] = 32'h8C44FFFC; //158H
    assign inst_rom[87] = 32'h00642825; //15CH
    assign inst_rom[88] = 32'hA0E50000; //160H
    assign inst_rom[89] = 32'h24E70001; //164H
    assign inst_rom[90] = 32'h24210004; //168H
    assign inst_rom[91] = 32'h1446FFF9; //16CH
    assign inst_rom[92] = 32'h2442FFFC; //170H
    assign inst_rom[93] = 32'h24090064; //174H
    assign inst_rom[94] = 32'h91290003; //178H
    assign inst_rom[95] = 32'h240D0068; //17CH
    assign inst_rom[96] = 32'h8DAD0000; //180H
    assign inst_rom[97] = 32'h00094E00; //184H
    assign inst_rom[98] = 32'h39AD0009; //188H
    assign inst_rom[99] = 32'hACED0001; //18CH
    assign inst_rom[100] = 32'h8C010000; //190H
    assign inst_rom[101] = 32'h8C020004; //194H
    assign inst_rom[102] = 32'h8C030008; //198H
    assign inst_rom[103] = 32'h8C04000C; //19CH
    assign inst_rom[104] = 32'h8C050010; //1A0H
    assign inst_rom[105] = 32'h8C060018; //1A4H
    assign inst_rom[106] = 32'h8C070070; //1A8H
    assign inst_rom[107] = 32'h8C190074; //1ACH
    assign inst_rom[108] = 32'h8C0D0078; //1B0H
    assign inst_rom[109] = 32'h0800000D; //1B4H

    //读指令,取4字节
    // always @(posedge clk)
    always @(*)
    begin
        case (addr)
            8'd0:   inst <= inst_rom[0];
            8'd1:   inst <= inst_rom[1];
            8'd2:   inst <= inst_rom[2];
            8'd3:   inst <= inst_rom[3];
            8'd4:   inst <= inst_rom[4];
            8'd5:   inst <= inst_rom[5];
            8'd6:   inst <= inst_rom[6];
            8'd7:   inst <= inst_rom[7];
            8'd8:   inst <= inst_rom[8];
            8'd9:   inst <= inst_rom[9];
            8'd10:  inst <= inst_rom[10];
            8'd11:  inst <= inst_rom[11];
            8'd12:  inst <= inst_rom[12];
            8'd13:  inst <= inst_rom[13];
            8'd14:  inst <= inst_rom[14];
            8'd15:  inst <= inst_rom[15];
            8'd16:  inst <= inst_rom[16];
            8'd17:  inst <= inst_rom[17];
            8'd18:  inst <= inst_rom[18];
            8'd19:  inst <= inst_rom[19];
            8'd20:  inst <= inst_rom[20];
            8'd21:  inst <= inst_rom[21];
            8'd22:  inst <= inst_rom[22];
            8'd23:  inst <= inst_rom[23];
            8'd24:  inst <= inst_rom[24];
            8'd25:  inst <= inst_rom[25];
            8'd26:  inst <= inst_rom[26];
            8'd27:  inst <= inst_rom[27];
            8'd28:  inst <= inst_rom[28];
            8'd29:  inst <= inst_rom[29];
            8'd30:  inst <= inst_rom[30];
            8'd31:  inst <= inst_rom[31];
            8'd32:  inst <= inst_rom[32];
            8'd33:  inst <= inst_rom[33];
            8'd34:  inst <= inst_rom[34];
            8'd35:  inst <= inst_rom[35];
            8'd36:  inst <= inst_rom[36];
            8'd37:  inst <= inst_rom[37];
            8'd38:  inst <= inst_rom[38];
            8'd39:  inst <= inst_rom[39];
            8'd40:  inst <= inst_rom[40];
            8'd41:  inst <= inst_rom[41];
            8'd42:  inst <= inst_rom[42];
            8'd43:  inst <= inst_rom[43];
            8'd44:  inst <= inst_rom[44];
            8'd45:  inst <= inst_rom[45];
            8'd46:  inst <= inst_rom[46];
            8'd47:  inst <= inst_rom[47];
            8'd48:  inst <= inst_rom[48];
            8'd49:  inst <= inst_rom[49];
            8'd50:  inst <= inst_rom[50];
            8'd51:  inst <= inst_rom[51];
            8'd52:  inst <= inst_rom[52];
            8'd53:  inst <= inst_rom[53];
            8'd54:  inst <= inst_rom[54];
            8'd55:  inst <= inst_rom[55];
            8'd56:  inst <= inst_rom[56];
            8'd57:  inst <= inst_rom[57];
            8'd58:  inst <= inst_rom[58];
            8'd59:  inst <= inst_rom[59];
            8'd60:  inst <= inst_rom[60];
            8'd61:  inst <= inst_rom[61];
            8'd62:  inst <= inst_rom[62];
            8'd63:  inst <= inst_rom[63];
            8'd64:  inst <= inst_rom[64];
            8'd65:  inst <= inst_rom[65];
            8'd66:  inst <= inst_rom[66];
            8'd67:  inst <= inst_rom[67];
            8'd68:  inst <= inst_rom[68];
            8'd69:  inst <= inst_rom[69];
            8'd70:  inst <= inst_rom[70];
            8'd71:  inst <= inst_rom[71];
            8'd72:  inst <= inst_rom[72];
            8'd73:  inst <= inst_rom[73];
            8'd74:  inst <= inst_rom[74];
            8'd75:  inst <= inst_rom[75];
            8'd76:  inst <= inst_rom[76];
            8'd77:  inst <= inst_rom[77];
            8'd78:  inst <= inst_rom[78];
            8'd79:  inst <= inst_rom[79];
            8'd80:  inst <= inst_rom[80];
            8'd81:  inst <= inst_rom[81];
            8'd82:  inst <= inst_rom[82];
            8'd83:  inst <= inst_rom[83];
            8'd84:  inst <= inst_rom[84];
            8'd85:  inst <= inst_rom[85];
            8'd86:  inst <= inst_rom[86];
            8'd87:  inst <= inst_rom[87];
            8'd88:  inst <= inst_rom[88];
            8'd89:  inst <= inst_rom[89];
            8'd90:  inst <= inst_rom[90];
            8'd91:  inst <= inst_rom[91];
            8'd92:  inst <= inst_rom[92];
            8'd93:  inst <= inst_rom[93];
            8'd94:  inst <= inst_rom[94];
            8'd95:  inst <= inst_rom[95];
            8'd96:  inst <= inst_rom[96];
            8'd97:  inst <= inst_rom[97];
            8'd98:  inst <= inst_rom[98];
            8'd99:  inst <= inst_rom[99];
            8'd100: inst <= inst_rom[100];
            8'd101: inst <= inst_rom[101];
            8'd102: inst <= inst_rom[102];
            8'd103: inst <= inst_rom[103];
            8'd104: inst <= inst_rom[104];
            8'd105: inst <= inst_rom[105];
            8'd106: inst <= inst_rom[106];
            8'd107: inst <= inst_rom[107];
            8'd108: inst <= inst_rom[108];
            8'd109: inst <= inst_rom[109];
            default: inst <= 32'd0;
        endcase
    end
endmodule