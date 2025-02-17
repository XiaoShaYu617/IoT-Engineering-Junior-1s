#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 
#include<iostream>
#include<vector>
#include<fstream>
#include<time.h> 
#include<io.h>
#include<string>
#include<iomanip>
#include <Winsock2.h>

#pragma comment(lib, "ws2_32.lib")
using namespace std;

//总报文长度10028字节，首部28字节，数据10000字节
const int packetLen = 10028;
const int messageLen = 10000;
const int headerLen = packetLen-messageLen;
string serverIp = "127.0.0.2";
string clientIp = "127.0.0.1";
USHORT serverPort = 8888;
USHORT clientPort = 8888;
int length = 0;
int seq;
int ack;
vector<string> files;
string path("C:\\Users\\辛杰\\Desktop\\lab3-1发送端");

char sendBuffer[packetLen];
char recvBuffer[packetLen];
unsigned long long totalDataSent = 0;
vector<char> dataContent;
char timeRecord[20] = { 0 };


//加载套接字库
//WORD：两字节无符号整数。
WORD wVersionRequested;
WSADATA IpWSAData;
SOCKET sockSrv;
//要连接的服务器
SOCKADDR_IN  Server;


void calCheckSum()
{
    int sum = 0;
    //计算前26字节
    for (int i = 0; i < packetLen; i += 2)
    {
        if (i == 26)
            continue;
        //每次往上加16位
        sum += (sendBuffer[i] << 8) + sendBuffer[i + 1];
        //出现进位,移至最低位
        if (sum >= 0x10000)
        {
            sum -= 0x10000;
            sum += 1;
        }
    }
    //取反
    USHORT checkSum = ~(USHORT)sum;
    sendBuffer[26] = (char)(checkSum >> 8);
    sendBuffer[27] = (char)checkSum;

    // 打印校验和
    cout << "发送报文的校验和: 0x" << hex << (unsigned int)checkSum << dec << endl;
}

//校验码检查
bool checkCheckSum()
{
    int sum = 0;
    //计算除校验码位的全部字节
    for (int i = 0; i < packetLen; i += 2)
    {
        if (i == 26)
            continue;
        //每次往上加16位
        sum += (recvBuffer[i] << 8) + recvBuffer[i + 1];
        //出现进位,移至最低位
        if (sum >= 0x10000)
        {
            sum -= 0x10000;
            sum += 1;
        }
    }
    //checksum计算
    //取反
    USHORT checksum = (recvBuffer[26] << 8) + (unsigned char)recvBuffer[27];


    cout << "接收报文的校验和: 0x" << hex << (unsigned int)checksum << dec << endl;
    if (checksum + (USHORT)sum == 0xffff)
    {
        return true;
    }
    {
        cout << "校验失败，重新发送" << endl;
        return false;
    }
}
void getTime()
{
    time_t t = time(0);
    strftime(timeRecord, sizeof(timeRecord), "%H:%M:%S", localtime(&t));
}

void sendlog()
{
    getTime();
    cout << timeRecord << " [send] ";
    int seqtmp = (int)((unsigned char)sendBuffer[12] << 24) + (int)((unsigned char)sendBuffer[13] << 16) + (int)((unsigned char)sendBuffer[14] << 8) + (int)(unsigned char)sendBuffer[15];
    int acktmp = (int)((unsigned char)sendBuffer[16] << 24) + (int)((unsigned char)sendBuffer[17] << 16) + (int)((unsigned char)sendBuffer[18] << 8) + (int)(unsigned char)sendBuffer[19];
    int lengthtmp = (int)((unsigned char)sendBuffer[20] << 24) + (int)((unsigned char)sendBuffer[21] << 16) + (int)((unsigned char)sendBuffer[22] << 8) + (int)(unsigned char)sendBuffer[23];
    cout << "  Seq: " << setw(5) << setiosflags(ios::left) << seqtmp << "Ack: " << setw(5) << setiosflags(ios::left) << acktmp << "Length: " << setw(5) << setiosflags(ios::left) << lengthtmp;
    int ACKtmp = (sendBuffer[24] & 0xF0) ? 1 : 0;
    int syntmp = (sendBuffer[24] & 0xF) ? 1 : 0;
    int fintmp = (sendBuffer[25] & 0xF0) ? 1 : 0;
    int sttmp = (sendBuffer[25] & 0xC) ? 1 : 0;
    int ovtmp = (sendBuffer[25] & 0x3) ? 1 : 0;
    cout << "ACK: " << ACKtmp << " SYN: " << syntmp << " FIN: " << fintmp << " ST: " << sttmp << " OV: " << ovtmp << endl;
}

void recvlog()
{
    getTime();
    cout << timeRecord << " [recv] ";
    int seqtmp = (int)((unsigned char)recvBuffer[12] << 24) + (int)((unsigned char)recvBuffer[13] << 16) + (int)((unsigned char)recvBuffer[14] << 8) + (int)(unsigned char)recvBuffer[15];
    int acktmp = (int)((unsigned char)recvBuffer[16] << 24) + (int)((unsigned char)recvBuffer[17] << 16) + (int)((unsigned char)recvBuffer[18] << 8) + (int)(unsigned char)recvBuffer[19];
    int lengthtmp = (int)((unsigned char)recvBuffer[20] << 24) + (int)((unsigned char)recvBuffer[21] << 16) + (int)((unsigned char)recvBuffer[22] << 8) + (int)(unsigned char)recvBuffer[23];
    cout << "  Seq: " << setw(5) << setiosflags(ios::left) << seqtmp << "Ack: " << setw(5) << setiosflags(ios::left) << acktmp << "Length: " << setw(5) << setiosflags(ios::left) << lengthtmp;
    int ACKtmp = (recvBuffer[24] & 0xF0) ? 1 : 0;
    int syntmp = (recvBuffer[24] & 0xF) ? 1 : 0;
    int fintmp = (recvBuffer[25] & 0xF0) ? 1 : 0;
    int sttmp = (recvBuffer[25] & 0xC) ? 1 : 0;
    int ovtmp = (recvBuffer[25] & 0x3) ? 1 : 0;
    cout << "ACK: " << ACKtmp << " SYN: " << syntmp << " FIN: " << fintmp << " ST: " << sttmp << " OV: " << ovtmp << endl;
}

void SeqAndAckSet(int newSeq, bool newAck)
{
    seq = newSeq;
    int recvSeq = (int)((unsigned char)recvBuffer[12] << 24) + (int)((unsigned char)recvBuffer[13] << 16) + (int)((unsigned char)recvBuffer[14] << 8) + (int)(unsigned char)recvBuffer[15];
    ack = newAck ? recvSeq + 1 : 0;
    sendBuffer[12] = (char)(seq >> 24);
    sendBuffer[13] = (char)(seq >> 16);
    sendBuffer[14] = (char)(seq >> 8);
    sendBuffer[15] = (char)seq;
    sendBuffer[16] = (char)(ack >> 24);
    sendBuffer[17] = (char)(ack >> 16);
    sendBuffer[18] = (char)(ack >> 8);
    sendBuffer[19] = (char)ack;
}
void lengthSet(int len)
{
    length = len;
    sendBuffer[20] = (char)(length >> 24);
    sendBuffer[21] = (char)(length >> 16);
    sendBuffer[22] = (char)(length >> 8);
    sendBuffer[23] = (char)length;
}
void clearFlag()
{
    sendBuffer[24] = 0;
    sendBuffer[25] = 0;
}
void setAck()
{
    sendBuffer[24] += 0xF0;
}
void setSYN()
{
    sendBuffer[24] += 0xF;
}
void setFIN()
{
    sendBuffer[25] += 0xF0;
}
void setST()
{
    sendBuffer[25] += 0xC;
}
void setOV()
{
    sendBuffer[25] += 0x3;
}
bool checkACK()
{
    return recvBuffer[24] & 0xF0;
}
bool checkSYN()
{
    return recvBuffer[24] & 0xF;
}
bool checkFIN()
{
    return recvBuffer[25] & 0xF0;
}
bool checkST()
{
    return recvBuffer[25] & 0xC;
}
bool checkOV()
{
    return recvBuffer[25] & 0x3;
}

// 将单个数字字符转换为整数
int charToDigit(char c) {
    return c - '0';
}

// 将点分十进制IP地址转换为数值形式并存储到缓冲区
void ipToString(char* buffer, int& idx, const string& ip) {
    int tmp = 0;
    for (int i = 0; i < ip.length(); i++) {
        if (ip[i] == '.') {
            buffer[idx++] = (char)tmp;
            tmp = 0;
        }
        else {
            tmp = tmp * 10 + charToDigit(ip[i]);
        }
    }
    buffer[idx++] = (char)tmp; 
}

//建立连接
void connectEstablishment()
{
    //设置源端口与目的端口号，端口号为2字节
    sendBuffer[0] = (char)(clientPort >> 8);
    sendBuffer[1] = (char)(clientPort & 0xFF);
    sendBuffer[2] = (char)(serverPort >> 8);
    sendBuffer[3] = (char)(serverPort & 0xFF);

    //设置源IP与目标IP
    int ctrl = 4;
    ipToString(sendBuffer, ctrl, clientIp);
    ipToString(sendBuffer, ctrl, serverIp);

    //初始序号与确认序号
    SeqAndAckSet(rand(), 0);
    //握手报文长度：
    lengthSet(0);
    //标志位
    clearFlag();
    setSYN();
    //数据段
    for (int i = 28; i < packetLen; i++)
        sendBuffer[i] = 0;
    //校验和
    calCheckSum();
}

bool ackValueCheck()
{
    int recvack = (int)((unsigned char)recvBuffer[16] << 24) + (int)((unsigned char)recvBuffer[17] << 16) + (int)((unsigned char)recvBuffer[18] << 8) + (int)(unsigned char)recvBuffer[19];
    if (recvack == seq + 1)
        return true;
    else
    {
        cout << "ack错误" << endl;
        return false;
    }
}

bool getFiles(string path, vector<string>& files)
{
    //文件句柄
    _int64 hFile = 0;
    //文件信息
    struct _finddata_t fileinfo;
    string p;
    if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
        if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
        {
            // 保存文件的全路径
            files.push_back(p.assign(path).append("\\").append(fileinfo.name));

        }
        while (_findnext(hFile, &fileinfo) == 0)
        {
            if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
            {
                // 保存文件的全路径
                files.push_back(p.assign(path).append("\\").append(fileinfo.name));

            }
        }
        _findclose(hFile);
    }
    return 1;
}

void loadFile(int number)
{
    vector<char>().swap(dataContent);
    ifstream fin(files[number].c_str(), ifstream::binary);
    char t = fin.get();
    while (fin)
    {
        dataContent.push_back(t);
        t = fin.get();
    }
}

//初始化套接字
void initial()
{
    wVersionRequested = MAKEWORD(1, 1);
    int error = WSAStartup(wVersionRequested, &IpWSAData);
    if (error != 0)
    {
        cout << "Winsock初始化错误" << endl;
        exit(0);
    }
    sockSrv = socket(AF_INET, SOCK_DGRAM, 0);
    Server.sin_addr.s_addr = inet_addr(serverIp.c_str());
    Server.sin_family = AF_INET;
    Server.sin_port = htons(serverPort);
    unsigned long ul = 1;
    int ret = ioctlsocket(sockSrv, FIONBIO, (unsigned long*)&ul);
}

void transmission(int ctrl)
{
    int len = sizeof(SOCKADDR);
    int start;
    bool whether;
    switch (ctrl)
    {

    case 0:
        //只发不收
        whether = false;
        do
        {
            sendto(sockSrv, sendBuffer, sizeof(sendBuffer), 0, (SOCKADDR*)&Server, len);
            sendlog();
            whether = false;
            start = clock();
            while (clock() - start < 4000)
            {
                if (recvfrom(sockSrv, recvBuffer, sizeof(recvBuffer), 0, (SOCKADDR*)&Server, &len) > 0)
                {
                    recvlog();
                    whether = true;
                    break;
                }
            }
        } while (whether);

        break;
    case 1:
        //只收不发
        break;
    case 2:
        //先发再收
        do
        {
            sendto(sockSrv, sendBuffer, sizeof(sendBuffer), 0, (SOCKADDR*)&Server, len);
            sendlog();
            start = clock();
            whether = false;
            while (clock() - start < 2000)
            {
                if (recvfrom(sockSrv, recvBuffer, sizeof(recvBuffer), 0, (SOCKADDR*)&Server, &len) > 0)
                {
                    recvlog();
                    whether = true;
                    break;
                }
            }
        } while (!checkCheckSum() || (!whether));

        break;
    case 3:
        //先收再发
        break;
    default:
        break;
    }
}
int main()
{
    initial();
    cout << "==========" << endl;
    cout << "**发送端**" << endl;
    cout << "==========" << endl;
    connectEstablishment();
    while (1)
    {
        //第一次握手，校验失败会重传。
        transmission(2);
        if (checkACK() && checkSYN() && ackValueCheck())
        {
            //收到的包正确且为二次握手
            //准备进行三次握手的回复
            SeqAndAckSet(rand(), 1);
            //标志位
            clearFlag();
            setAck();
            calCheckSum();
            transmission(0);
            cout << "**********连接成功**********" << endl;
            break;

        }
    }
    getFiles(path, files);
    int num = files.size();
    cout << "**********开始发文件*******" << endl;
    clock_t start_time = clock();// 记录开始发送文件的时间
    for (int i = 0; i < num; i++)
    {
        char fileName[100];
        strcpy(fileName, files[i].substr(35, files[i].length() - 35).c_str());
        //要发送的第一条消息就是文件的名字
        SeqAndAckSet(0, 0);
        lengthSet(strlen(fileName));
        clearFlag();
        setST();
        //报文
        for (int j = 0; j < length; j++)
        {
            sendBuffer[j + 28] = fileName[j];
        }
        calCheckSum();
        do
        {
            transmission(2);
        } while (!(checkACK() && ackValueCheck()));

        //将文件加载到dataContent动态数组中。
        loadFile(i);
        unsigned long long fileSize = dataContent.size();
        totalDataSent += fileSize; // 更新总数据量
        for (int j = 0; j < dataContent.size(); j++)
        {
            //装载数据报文
            sendBuffer[28 + (j % messageLen)] = dataContent[j];
            if ((j % messageLen == messageLen - 1) && (j != dataContent.size() - 1))
            {
                //填满了一组报文，准备发送
                SeqAndAckSet(seq + 1, 0);
                lengthSet(messageLen);
                clearFlag();
                calCheckSum();
                do
                {
                    transmission(2);
                } while (!(checkACK() && ackValueCheck()));
            }
            if (j == dataContent.size() - 1)
            {
                //最后一组报文
                SeqAndAckSet(seq + 1, 0);
                lengthSet(j % messageLen + 1);
                clearFlag();
                setOV();
                calCheckSum();
                do
                {
                    transmission(2);
                } while (!(checkACK() && ackValueCheck()));
            }
        }
    }
    cout << "**********文件发送成功*******" << endl;

    clock_t end_time = clock(); // 记录发送文件结束的时间
    double elapsed = double(end_time - start_time) / CLOCKS_PER_SEC; // 计算总耗时
    double throughput = totalDataSent / (elapsed*1024*1024);                     //计算总吞吐率
    cout << "文件传输时间: " << elapsed << " s" << endl;
    cout << "吞吐率: " << throughput << "MB/s" << endl;

    //挥手
    cout << "**********开始关闭连接*******" << endl;
    SeqAndAckSet(rand(), 0);
    lengthSet(0);
    clearFlag();
    setFIN();
    calCheckSum();
    do
    {
        transmission(2);
    } while (!(checkCheckSum() && checkACK() && ackValueCheck()));
    closesocket(sockSrv);
    WSACleanup();
    cout << "**********连接结束*******" << endl;
    system("pause");
}



