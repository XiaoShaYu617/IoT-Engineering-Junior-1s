`timescale 1ns / 1ps
//*************************************************************************
//   > 文件名: data_mem.v
//   > 描述  ：异步数据存储器模块，采用寄存器搭建而成，类似寄存器堆
//   >         同步写，异步读
//   > 作者  : LOONGSON
//   > 日期  : 2016-04-14
//*************************************************************************
module data_ram(
    input         clk,         // 时钟
    input  [3:0]  wen,         // 字节写使能
    input  [7:0] addr,        // 地址
    input  [31:0] wdata,       // 写数据
    output reg [31:0] rdata,       // 读数据
    
    //调试端口，用于读出数据显示
    input  [7:0] test_addr,
    output reg [31:0] test_data
);
    reg [31:0] DM[255:0];  //数据存储器，字节地址7'b000_0000~7'b111_1111

    //写数据
    // posedge clk
    always @(*)    // 当写控制信号为1，数据写入内存
    begin
        if (wen[3])
        begin
            DM[addr][31:24] <= wdata[31:24];
        end
    end
    always @(*)
    begin
        if (wen[2])
        begin
            DM[addr][23:16] <= wdata[23:16];
        end
    end
    always @(*)
    begin
        if (wen[1])
        begin
            DM[addr][15: 8] <= wdata[15: 8];
        end
    end
    always @(*)
    begin
        if (wen[0])
        begin
            DM[addr][7 : 0] <= wdata[7 : 0];
        end
    end
    
    //读数据,取4字节
    always @(*)
    begin
        case (addr)
            8'd0 : rdata <= DM[0];
            8'd1 : rdata <= DM[1];
            8'd2 : rdata <= DM[2];
            8'd3 : rdata <= DM[3];
            8'd4 : rdata <= DM[4];
            8'd5 : rdata <= DM[5];
            8'd6 : rdata <= DM[6];
            8'd7 : rdata <= DM[7];
            8'd8 : rdata <= DM[8];
            8'd9 : rdata <= DM[9];
            8'd10 : rdata <= DM[10];
            8'd11 : rdata <= DM[11];
            8'd12 : rdata <= DM[12];
            8'd13 : rdata <= DM[13];
            8'd14 : rdata <= DM[14];
            8'd15 : rdata <= DM[15];
            8'd16 : rdata <= DM[16];
            8'd17 : rdata <= DM[17];
            8'd18 : rdata <= DM[18];
            8'd19 : rdata <= DM[19];
            8'd20 : rdata <= DM[20];
            8'd21 : rdata <= DM[21];
            8'd22 : rdata <= DM[22];
            8'd23 : rdata <= DM[23];
            8'd24 : rdata <= DM[24];
            8'd25 : rdata <= DM[25];
            8'd26 : rdata <= DM[26];
            8'd27 : rdata <= DM[27];
            8'd28 : rdata <= DM[28];
            8'd29 : rdata <= DM[29];
            8'd30 : rdata <= DM[30];
            8'd31 : rdata <= DM[31];
            8'd32 : rdata <= DM[32];
            8'd33 : rdata <= DM[33];
            8'd34 : rdata <= DM[34];
            8'd35 : rdata <= DM[35];
            8'd36 : rdata <= DM[36];
            8'd37 : rdata <= DM[37];
            8'd38 : rdata <= DM[38];
            8'd39 : rdata <= DM[39];
            8'd40 : rdata <= DM[40];
            8'd41 : rdata <= DM[41];
            8'd42 : rdata <= DM[42];
            8'd43 : rdata <= DM[43];
            8'd44 : rdata <= DM[44];
            8'd45 : rdata <= DM[45];
            8'd46 : rdata <= DM[46];
            8'd47 : rdata <= DM[47];
            8'd48 : rdata <= DM[48];
            8'd49 : rdata <= DM[49];
            8'd50 : rdata <= DM[50];
            8'd51 : rdata <= DM[51];
            8'd52 : rdata <= DM[52];
            8'd53 : rdata <= DM[53];
            8'd54 : rdata <= DM[54];
            8'd55 : rdata <= DM[55];
            8'd56 : rdata <= DM[56];
            8'd57 : rdata <= DM[57];
            8'd58 : rdata <= DM[58];
            8'd59 : rdata <= DM[59];
            8'd60 : rdata <= DM[60];
            8'd61 : rdata <= DM[61];
            8'd62 : rdata <= DM[62];
            8'd63 : rdata <= DM[63];
            8'd64 : rdata <= DM[64];
            8'd65 : rdata <= DM[65];
            8'd66 : rdata <= DM[66];
            8'd67 : rdata <= DM[67];
            8'd68 : rdata <= DM[68];
            8'd69 : rdata <= DM[69];
            8'd70 : rdata <= DM[70];
            8'd71 : rdata <= DM[71];
            8'd72 : rdata <= DM[72];
            8'd73 : rdata <= DM[73];
            8'd74 : rdata <= DM[74];
            8'd75 : rdata <= DM[75];
            8'd76 : rdata <= DM[76];
            8'd77 : rdata <= DM[77];
            8'd78 : rdata <= DM[78];
            8'd79 : rdata <= DM[79];
            8'd80 : rdata <= DM[80];
            8'd81 : rdata <= DM[81];
            8'd82 : rdata <= DM[82];
            8'd83 : rdata <= DM[83];
            8'd84 : rdata <= DM[84];
            8'd85 : rdata <= DM[85];
            8'd86 : rdata <= DM[86];
            8'd87 : rdata <= DM[87];
            8'd88 : rdata <= DM[88];
            8'd89 : rdata <= DM[89];
            8'd90 : rdata <= DM[90];
            8'd91 : rdata <= DM[91];
            8'd92 : rdata <= DM[92];
            8'd93 : rdata <= DM[93];
            8'd94 : rdata <= DM[94];
            8'd95 : rdata <= DM[95];
            8'd96 : rdata <= DM[96];
            8'd97 : rdata <= DM[97];
            8'd98 : rdata <= DM[98];
            8'd99 : rdata <= DM[99];
            8'd100 : rdata <= DM[100];
            8'd101 : rdata <= DM[101];
            8'd102 : rdata <= DM[102];
            8'd103 : rdata <= DM[103];
            8'd104 : rdata <= DM[104];
            8'd105 : rdata <= DM[105];
            8'd106 : rdata <= DM[106];
            8'd107 : rdata <= DM[107];
            8'd108 : rdata <= DM[108];
            8'd109 : rdata <= DM[109];
            8'd110 : rdata <= DM[110];
            8'd111 : rdata <= DM[111];
            8'd112 : rdata <= DM[112];
            8'd113 : rdata <= DM[113];
            8'd114 : rdata <= DM[114];
            8'd115 : rdata <= DM[115];
            8'd116 : rdata <= DM[116];
            8'd117 : rdata <= DM[117];
            8'd118 : rdata <= DM[118];
            8'd119 : rdata <= DM[119];
            8'd120 : rdata <= DM[120];
            8'd121 : rdata <= DM[121];
            8'd122 : rdata <= DM[122];
            8'd123 : rdata <= DM[123];
            8'd124 : rdata <= DM[124];
            8'd125 : rdata <= DM[125];
            8'd126 : rdata <= DM[126];
            8'd127 : rdata <= DM[127];
            8'd128 : rdata <= DM[128];
            8'd129 : rdata <= DM[129];
            8'd130 : rdata <= DM[130];
            8'd131 : rdata <= DM[131];
            8'd132 : rdata <= DM[132];
            8'd133 : rdata <= DM[133];
            8'd134 : rdata <= DM[134];
            8'd135 : rdata <= DM[135];
            8'd136 : rdata <= DM[136];
            8'd137 : rdata <= DM[137];
            8'd138 : rdata <= DM[138];
            8'd139 : rdata <= DM[139];
            8'd140 : rdata <= DM[140];
            8'd141 : rdata <= DM[141];
            8'd142 : rdata <= DM[142];
            8'd143 : rdata <= DM[143];
            8'd144 : rdata <= DM[144];
            8'd145 : rdata <= DM[145];
            8'd146 : rdata <= DM[146];
            8'd147 : rdata <= DM[147];
            8'd148 : rdata <= DM[148];
            8'd149 : rdata <= DM[149];
            8'd150 : rdata <= DM[150];
            8'd151 : rdata <= DM[151];
            8'd152 : rdata <= DM[152];
            8'd153 : rdata <= DM[153];
            8'd154 : rdata <= DM[154];
            8'd155 : rdata <= DM[155];
            8'd156 : rdata <= DM[156];
            8'd157 : rdata <= DM[157];
            8'd158 : rdata <= DM[158];
            8'd159 : rdata <= DM[159];
            8'd160 : rdata <= DM[160];
            8'd161 : rdata <= DM[161];
            8'd162 : rdata <= DM[162];
            8'd163 : rdata <= DM[163];
            8'd164 : rdata <= DM[164];
            8'd165 : rdata <= DM[165];
            8'd166 : rdata <= DM[166];
            8'd167 : rdata <= DM[167];
            8'd168 : rdata <= DM[168];
            8'd169 : rdata <= DM[169];
            8'd170 : rdata <= DM[170];
            8'd171 : rdata <= DM[171];
            8'd172 : rdata <= DM[172];
            8'd173 : rdata <= DM[173];
            8'd174 : rdata <= DM[174];
            8'd175 : rdata <= DM[175];
            8'd176 : rdata <= DM[176];
            8'd177 : rdata <= DM[177];
            8'd178 : rdata <= DM[178];
            8'd179 : rdata <= DM[179];
            8'd180 : rdata <= DM[180];
            8'd181 : rdata <= DM[181];
            8'd182 : rdata <= DM[182];
            8'd183 : rdata <= DM[183];
            8'd184 : rdata <= DM[184];
            8'd185 : rdata <= DM[185];
            8'd186 : rdata <= DM[186];
            8'd187 : rdata <= DM[187];
            8'd188 : rdata <= DM[188];
            8'd189 : rdata <= DM[189];
            8'd190 : rdata <= DM[190];
            8'd191 : rdata <= DM[191];
            8'd192 : rdata <= DM[192];
            8'd193 : rdata <= DM[193];
            8'd194 : rdata <= DM[194];
            8'd195 : rdata <= DM[195];
            8'd196 : rdata <= DM[196];
            8'd197 : rdata <= DM[197];
            8'd198 : rdata <= DM[198];
            8'd199 : rdata <= DM[199];
            8'd200 : rdata <= DM[200];
            8'd201 : rdata <= DM[201];
            8'd202 : rdata <= DM[202];
            8'd203 : rdata <= DM[203];
            8'd204 : rdata <= DM[204];
            8'd205 : rdata <= DM[205];
            8'd206 : rdata <= DM[206];
            8'd207 : rdata <= DM[207];
            8'd208 : rdata <= DM[208];
            8'd209 : rdata <= DM[209];
            8'd210 : rdata <= DM[210];
            8'd211 : rdata <= DM[211];
            8'd212 : rdata <= DM[212];
            8'd213 : rdata <= DM[213];
            8'd214 : rdata <= DM[214];
            8'd215 : rdata <= DM[215];
            8'd216 : rdata <= DM[216];
            8'd217 : rdata <= DM[217];
            8'd218 : rdata <= DM[218];
            8'd219 : rdata <= DM[219];
            8'd220 : rdata <= DM[220];
            8'd221 : rdata <= DM[221];
            8'd222 : rdata <= DM[222];
            8'd223 : rdata <= DM[223];
            8'd224 : rdata <= DM[224];
            8'd225 : rdata <= DM[225];
            8'd226 : rdata <= DM[226];
            8'd227 : rdata <= DM[227];
            8'd228 : rdata <= DM[228];
            8'd229 : rdata <= DM[229];
            8'd230 : rdata <= DM[230];
            8'd231 : rdata <= DM[231];
            8'd232 : rdata <= DM[232];
            8'd233 : rdata <= DM[233];
            8'd234 : rdata <= DM[234];
            8'd235 : rdata <= DM[235];
            8'd236 : rdata <= DM[236];
            8'd237 : rdata <= DM[237];
            8'd238 : rdata <= DM[238];
            8'd239 : rdata <= DM[239];
            8'd240 : rdata <= DM[240];
            8'd241 : rdata <= DM[241];
            8'd242 : rdata <= DM[242];
            8'd243 : rdata <= DM[243];
            8'd244 : rdata <= DM[244];
            8'd245 : rdata <= DM[245];
            8'd246 : rdata <= DM[246];
            8'd247 : rdata <= DM[247];
            8'd248 : rdata <= DM[248];
            8'd249 : rdata <= DM[249];
            8'd250 : rdata <= DM[250];
            8'd251 : rdata <= DM[251];
            8'd252 : rdata <= DM[252];
            8'd253 : rdata <= DM[253];
            8'd254 : rdata <= DM[254];
            8'd255 : rdata <= DM[255];
        endcase
    end
    //调试端口，读出特定内存的数据
    always @(*)
    begin
        case (test_addr)
            8'd0 : test_data <= DM[0];
            8'd1 : test_data <= DM[1];
            8'd2 : test_data <= DM[2];
            8'd3 : test_data <= DM[3];
            8'd4 : test_data <= DM[4];
            8'd5 : test_data <= DM[5];
            8'd6 : test_data <= DM[6];
            8'd7 : test_data <= DM[7];
            8'd8 : test_data <= DM[8];
            8'd9 : test_data <= DM[9];
            8'd10 : test_data <= DM[10];
            8'd11 : test_data <= DM[11];
            8'd12 : test_data <= DM[12];
            8'd13 : test_data <= DM[13];
            8'd14 : test_data <= DM[14];
            8'd15 : test_data <= DM[15];
            8'd16 : test_data <= DM[16];
            8'd17 : test_data <= DM[17];
            8'd18 : test_data <= DM[18];
            8'd19 : test_data <= DM[19];
            8'd20 : test_data <= DM[20];
            8'd21 : test_data <= DM[21];
            8'd22 : test_data <= DM[22];
            8'd23 : test_data <= DM[23];
            8'd24 : test_data <= DM[24];
            8'd25 : test_data <= DM[25];
            8'd26 : test_data <= DM[26];
            8'd27 : test_data <= DM[27];
            8'd28 : test_data <= DM[28];
            8'd29 : test_data <= DM[29];
            8'd30 : test_data <= DM[30];
            8'd31 : test_data <= DM[31];
            8'd32 : test_data <= DM[32];
            8'd33 : test_data <= DM[33];
            8'd34 : test_data <= DM[34];
            8'd35 : test_data <= DM[35];
            8'd36 : test_data <= DM[36];
            8'd37 : test_data <= DM[37];
            8'd38 : test_data <= DM[38];
            8'd39 : test_data <= DM[39];
            8'd40 : test_data <= DM[40];
            8'd41 : test_data <= DM[41];
            8'd42 : test_data <= DM[42];
            8'd43 : test_data <= DM[43];
            8'd44 : test_data <= DM[44];
            8'd45 : test_data <= DM[45];
            8'd46 : test_data <= DM[46];
            8'd47 : test_data <= DM[47];
            8'd48 : test_data <= DM[48];
            8'd49 : test_data <= DM[49];
            8'd50 : test_data <= DM[50];
            8'd51 : test_data <= DM[51];
            8'd52 : test_data <= DM[52];
            8'd53 : test_data <= DM[53];
            8'd54 : test_data <= DM[54];
            8'd55 : test_data <= DM[55];
            8'd56 : test_data <= DM[56];
            8'd57 : test_data <= DM[57];
            8'd58 : test_data <= DM[58];
            8'd59 : test_data <= DM[59];
            8'd60 : test_data <= DM[60];
            8'd61 : test_data <= DM[61];
            8'd62 : test_data <= DM[62];
            8'd63 : test_data <= DM[63];
            8'd64 : test_data <= DM[64];
            8'd65 : test_data <= DM[65];
            8'd66 : test_data <= DM[66];
            8'd67 : test_data <= DM[67];
            8'd68 : test_data <= DM[68];
            8'd69 : test_data <= DM[69];
            8'd70 : test_data <= DM[70];
            8'd71 : test_data <= DM[71];
            8'd72 : test_data <= DM[72];
            8'd73 : test_data <= DM[73];
            8'd74 : test_data <= DM[74];
            8'd75 : test_data <= DM[75];
            8'd76 : test_data <= DM[76];
            8'd77 : test_data <= DM[77];
            8'd78 : test_data <= DM[78];
            8'd79 : test_data <= DM[79];
            8'd80 : test_data <= DM[80];
            8'd81 : test_data <= DM[81];
            8'd82 : test_data <= DM[82];
            8'd83 : test_data <= DM[83];
            8'd84 : test_data <= DM[84];
            8'd85 : test_data <= DM[85];
            8'd86 : test_data <= DM[86];
            8'd87 : test_data <= DM[87];
            8'd88 : test_data <= DM[88];
            8'd89 : test_data <= DM[89];
            8'd90 : test_data <= DM[90];
            8'd91 : test_data <= DM[91];
            8'd92 : test_data <= DM[92];
            8'd93 : test_data <= DM[93];
            8'd94 : test_data <= DM[94];
            8'd95 : test_data <= DM[95];
            8'd96 : test_data <= DM[96];
            8'd97 : test_data <= DM[97];
            8'd98 : test_data <= DM[98];
            8'd99 : test_data <= DM[99];
            8'd100 : test_data <= DM[100];
            8'd101 : test_data <= DM[101];
            8'd102 : test_data <= DM[102];
            8'd103 : test_data <= DM[103];
            8'd104 : test_data <= DM[104];
            8'd105 : test_data <= DM[105];
            8'd106 : test_data <= DM[106];
            8'd107 : test_data <= DM[107];
            8'd108 : test_data <= DM[108];
            8'd109 : test_data <= DM[109];
            8'd110 : test_data <= DM[110];
            8'd111 : test_data <= DM[111];
            8'd112 : test_data <= DM[112];
            8'd113 : test_data <= DM[113];
            8'd114 : test_data <= DM[114];
            8'd115 : test_data <= DM[115];
            8'd116 : test_data <= DM[116];
            8'd117 : test_data <= DM[117];
            8'd118 : test_data <= DM[118];
            8'd119 : test_data <= DM[119];
            8'd120 : test_data <= DM[120];
            8'd121 : test_data <= DM[121];
            8'd122 : test_data <= DM[122];
            8'd123 : test_data <= DM[123];
            8'd124 : test_data <= DM[124];
            8'd125 : test_data <= DM[125];
            8'd126 : test_data <= DM[126];
            8'd127 : test_data <= DM[127];
            8'd128 : test_data <= DM[128];
            8'd129 : test_data <= DM[129];
            8'd130 : test_data <= DM[130];
            8'd131 : test_data <= DM[131];
            8'd132 : test_data <= DM[132];
            8'd133 : test_data <= DM[133];
            8'd134 : test_data <= DM[134];
            8'd135 : test_data <= DM[135];
            8'd136 : test_data <= DM[136];
            8'd137 : test_data <= DM[137];
            8'd138 : test_data <= DM[138];
            8'd139 : test_data <= DM[139];
            8'd140 : test_data <= DM[140];
            8'd141 : test_data <= DM[141];
            8'd142 : test_data <= DM[142];
            8'd143 : test_data <= DM[143];
            8'd144 : test_data <= DM[144];
            8'd145 : test_data <= DM[145];
            8'd146 : test_data <= DM[146];
            8'd147 : test_data <= DM[147];
            8'd148 : test_data <= DM[148];
            8'd149 : test_data <= DM[149];
            8'd150 : test_data <= DM[150];
            8'd151 : test_data <= DM[151];
            8'd152 : test_data <= DM[152];
            8'd153 : test_data <= DM[153];
            8'd154 : test_data <= DM[154];
            8'd155 : test_data <= DM[155];
            8'd156 : test_data <= DM[156];
            8'd157 : test_data <= DM[157];
            8'd158 : test_data <= DM[158];
            8'd159 : test_data <= DM[159];
            8'd160 : test_data <= DM[160];
            8'd161 : test_data <= DM[161];
            8'd162 : test_data <= DM[162];
            8'd163 : test_data <= DM[163];
            8'd164 : test_data <= DM[164];
            8'd165 : test_data <= DM[165];
            8'd166 : test_data <= DM[166];
            8'd167 : test_data <= DM[167];
            8'd168 : test_data <= DM[168];
            8'd169 : test_data <= DM[169];
            8'd170 : test_data <= DM[170];
            8'd171 : test_data <= DM[171];
            8'd172 : test_data <= DM[172];
            8'd173 : test_data <= DM[173];
            8'd174 : test_data <= DM[174];
            8'd175 : test_data <= DM[175];
            8'd176 : test_data <= DM[176];
            8'd177 : test_data <= DM[177];
            8'd178 : test_data <= DM[178];
            8'd179 : test_data <= DM[179];
            8'd180 : test_data <= DM[180];
            8'd181 : test_data <= DM[181];
            8'd182 : test_data <= DM[182];
            8'd183 : test_data <= DM[183];
            8'd184 : test_data <= DM[184];
            8'd185 : test_data <= DM[185];
            8'd186 : test_data <= DM[186];
            8'd187 : test_data <= DM[187];
            8'd188 : test_data <= DM[188];
            8'd189 : test_data <= DM[189];
            8'd190 : test_data <= DM[190];
            8'd191 : test_data <= DM[191];
            8'd192 : test_data <= DM[192];
            8'd193 : test_data <= DM[193];
            8'd194 : test_data <= DM[194];
            8'd195 : test_data <= DM[195];
            8'd196 : test_data <= DM[196];
            8'd197 : test_data <= DM[197];
            8'd198 : test_data <= DM[198];
            8'd199 : test_data <= DM[199];
            8'd200 : test_data <= DM[200];
            8'd201 : test_data <= DM[201];
            8'd202 : test_data <= DM[202];
            8'd203 : test_data <= DM[203];
            8'd204 : test_data <= DM[204];
            8'd205 : test_data <= DM[205];
            8'd206 : test_data <= DM[206];
            8'd207 : test_data <= DM[207];
            8'd208 : test_data <= DM[208];
            8'd209 : test_data <= DM[209];
            8'd210 : test_data <= DM[210];
            8'd211 : test_data <= DM[211];
            8'd212 : test_data <= DM[212];
            8'd213 : test_data <= DM[213];
            8'd214 : test_data <= DM[214];
            8'd215 : test_data <= DM[215];
            8'd216 : test_data <= DM[216];
            8'd217 : test_data <= DM[217];
            8'd218 : test_data <= DM[218];
            8'd219 : test_data <= DM[219];
            8'd220 : test_data <= DM[220];
            8'd221 : test_data <= DM[221];
            8'd222 : test_data <= DM[222];
            8'd223 : test_data <= DM[223];
            8'd224 : test_data <= DM[224];
            8'd225 : test_data <= DM[225];
            8'd226 : test_data <= DM[226];
            8'd227 : test_data <= DM[227];
            8'd228 : test_data <= DM[228];
            8'd229 : test_data <= DM[229];
            8'd230 : test_data <= DM[230];
            8'd231 : test_data <= DM[231];
            8'd232 : test_data <= DM[232];
            8'd233 : test_data <= DM[233];
            8'd234 : test_data <= DM[234];
            8'd235 : test_data <= DM[235];
            8'd236 : test_data <= DM[236];
            8'd237 : test_data <= DM[237];
            8'd238 : test_data <= DM[238];
            8'd239 : test_data <= DM[239];
            8'd240 : test_data <= DM[240];
            8'd241 : test_data <= DM[241];
            8'd242 : test_data <= DM[242];
            8'd243 : test_data <= DM[243];
            8'd244 : test_data <= DM[244];
            8'd245 : test_data <= DM[245];
            8'd246 : test_data <= DM[246];
            8'd247 : test_data <= DM[247];
            8'd248 : test_data <= DM[248];
            8'd249 : test_data <= DM[249];
            8'd250 : test_data <= DM[250];
            8'd251 : test_data <= DM[251];
            8'd252 : test_data <= DM[252];
            8'd253 : test_data <= DM[253];
            8'd254 : test_data <= DM[254];
            8'd255 : test_data <= DM[255];
        endcase
    end
endmodule