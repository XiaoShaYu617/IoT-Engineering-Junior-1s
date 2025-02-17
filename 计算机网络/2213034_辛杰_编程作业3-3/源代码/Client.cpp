#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 

#include <iostream>
#include <vector>
#include <fstream>
#include <time.h> 
#include <io.h>
#include <string>
#include <iomanip>
#include <Winsock2.h>
#include<thread>
#include<mutex>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

//总报文长度10028字节，首部28字节，数据10000字节
const int packetLen = 10028;
const int messageLen = 10000;


/**********关于socket对象-start**********/
WORD wVersionRequested;
WSADATA IpWSAData;
SOCKET sockSrv;
//要连接的服务器
SOCKADDR_IN  Server;
//SOCKADDR_IN Server;
SOCKADDR_IN Client;
string serverIp = "127.0.0.2";
string clientIp = "127.0.0.1";
//string routerIp = "127.0.0.3";
USHORT serverPort = 8888;
USHORT clientPort = 8888;
//USHORT routerPort = 4444;
int len = sizeof(SOCKADDR);
/**********关于socket对象-end**********/

/**********关于报文信息-start***********/
char sendBuffer[packetLen];
char recvBuffer[packetLen];
char resendBuffer[packetLen];
char timeRecord[20] = { 0 };
int length = 0;
int seq;
int ack;
int recvseq;
int recvack;
int window;
USHORT resum;
USHORT sesum;
unsigned long long totalDataSent = 0;
/**********关于报文信息-end***********/

/**********关于文件系统-start**********/
vector<char> dataContent;
vector<string> files;
string path("C:\\Users\\辛杰\\Desktop\\发送端");
/**********关于文件系统-end**********/

struct DataInWindow {
    bool ack = false;
    char buffer[packetLen];
    int seq;
    int clock;
};
vector<DataInWindow> slidingWindow;
int ssthresh = 32;
int windowLength = 1;
std::mutex mtx;

int lastConfirmAck = 0;
int ConfirmCount = 0;
enum {SS,CA,QR};
int state;

//int totalRetransmission = 0;

void initial()
{
    //MAKEWORD，第一个参数为低位字节，主版本号，第二个参数为高位字节，副版本号
    wVersionRequested = MAKEWORD(1, 1);
    //进行socket库的绑定
    int error = WSAStartup(wVersionRequested, &IpWSAData);
    if (error != 0)
    {
        cout << "初始化错误" << endl;
        exit(0);
    }

    //创建用于监听的socket
    //AF_INET：TCP/IP&IPv4，SOCK_DGRAM：UDP数据报
    sockSrv = socket(AF_INET, SOCK_DGRAM, 0);
    //IP地址
    Server.sin_addr.s_addr = inet_addr(serverIp.c_str());
    //协议簇
    Server.sin_family = AF_INET;
    //连接端口号
    Server.sin_port = htons(serverPort);
    //设置非阻塞模式
    unsigned long ul = 1;
    int ret = ioctlsocket(sockSrv, FIONBIO, (unsigned long*)&ul);

    //Server.sin_addr.s_addr = inet_addr(routerIp.c_str());
    //Server.sin_family = AF_INET;
    //Server.sin_port = htons(routerPort);

    //Client.sin_addr.s_addr = inet_addr(clientIp.c_str());
    //Client.sin_family = AF_INET;
    //Client.sin_port = htons(clientPort);
    //bind(sockSrv, (SOCKADDR*)&Client, sizeof(SOCKADDR));//会返回一个SOCKET_ERROR

}

void calCheckSum()
{
    //计算校验和

    int sum = 0;
    //计算除校验码位的全部字节
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
}
bool checkCheckSum()
{
    //检查校验和

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
    if (checksum + (USHORT)sum == 0xffff)
    {
        return true;
    }
    else
    {
        cout << "失败校验" << endl;
        return false;
    }
}
void getTime()
{
    //获取当前系统时间，写入到字符数组timeRecord中
    time_t t = time(0);
    strftime(timeRecord, sizeof(timeRecord), "%H:%M:%S", localtime(&t));
}

void setPort()
{
    //将源端口号和目的端口号写入到sendBuffer之中。
    sendBuffer[0] = (char)(clientPort >> 8);
    sendBuffer[1] = (char)(clientPort & 0xFF);
    sendBuffer[2] = (char)(serverPort >> 8);
    sendBuffer[3] = (char)(serverPort & 0xFF);
}
void setIP()
{
    //将源IP与目标IP写入到sendBuffer中，
    int tmp = 0;
    int ctrl = 4;
    for (int i = 0; i < clientIp.length(); i++)
    {
        if (clientIp[i] == '.')
        {
            sendBuffer[ctrl++] = (char)tmp;
            tmp = 0;
        }
        else
        {
            tmp += tmp * 10 + (int)clientIp[i] - 48;
        }
    }
    sendBuffer[ctrl++] = (char)tmp;
    tmp = 0;
    for (int i = 0; i < serverIp.length(); i++)
    {
        if (serverIp[i] == '.')
        {
            sendBuffer[ctrl++] = (char)tmp;
            tmp = 0;
        }
        else
        {
            tmp += tmp * 10 + (int)serverIp[i] - 48;
        }
    }
    sendBuffer[ctrl++] = (char)tmp;
}
void setseq(int newSeq)
{
    seq = newSeq;
    sendBuffer[12] = (char)(seq >> 24);
    sendBuffer[13] = (char)(seq >> 16);
    sendBuffer[14] = (char)(seq >> 8);
    sendBuffer[15] = (char)seq;
}
void getseq()
{
    recvseq = (int)((unsigned char)recvBuffer[12] << 24) + (int)((unsigned char)recvBuffer[13] << 16) + (int)((unsigned char)recvBuffer[14] << 8) + (int)(unsigned char)recvBuffer[15];
}
void setack(int newack)
{
    ack = newack;
    sendBuffer[16] = (char)(ack >> 24);
    sendBuffer[17] = (char)(ack >> 16);
    sendBuffer[18] = (char)(ack >> 8);
    sendBuffer[19] = (char)ack;
}
void getresum() {
    resum = (recvBuffer[26] << 8) + (unsigned char)recvBuffer[27];
}
void getack()
{
    recvack = (int)((unsigned char)recvBuffer[16] << 24) + (int)((unsigned char)recvBuffer[17] << 16) + (int)((unsigned char)recvBuffer[18] << 8) + (int)(unsigned char)recvBuffer[19];
}
void setLength(int len)
{
    length = len;
    sendBuffer[20] = (char)(length >> 8);
    sendBuffer[21] = (char)(length);
}
void getLength()
{
    length = (int)((unsigned char)recvBuffer[20] << 8) + (int)(unsigned char)recvBuffer[21];
}
void setWindow(int newWindow)
{
    window = newWindow;
    sendBuffer[22] = (char)(window >> 8);
    sendBuffer[23] = (char)(window);
}
int getWindow()
{
    return (int)((unsigned char)recvBuffer[22] << 8) + (int)(unsigned char)recvBuffer[23];
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

void sendlog(char buffer[packetLen])
{
    //发送一条消息，打印发送的日志
    getTime();
    sesum = (buffer[26] << 8) + (unsigned char)buffer[27];
    cout << timeRecord << " [send] ";
    int seqtmp= (int)((unsigned char)buffer[12] << 24) + (int)((unsigned char)buffer[13] << 16) + (int)((unsigned char)buffer[14] << 8) + (int)(unsigned char)buffer[15];
    int acktmp= (int)((unsigned char)buffer[16] << 24) + (int)((unsigned char)buffer[17] << 16) + (int)((unsigned char)buffer[18] << 8) + (int)(unsigned char)buffer[19];

    cout << "  Seq: " << setw(5) << setiosflags(ios::left) << seqtmp << "Ack: " << setw(5) << setiosflags(ios::left) << acktmp;
    int ACKtmp = (buffer[24] & 0xF0) ? 1 : 0;
    int syntmp = (buffer[24] & 0xF) ? 1 : 0;
    int fintmp = (buffer[25] & 0xF0) ? 1 : 0;
    int sttmp = (buffer[25] & 0xC) ? 1 : 0;
    int ovtmp = (buffer[25] & 0x3) ? 1 : 0;
    cout << "ACK: " << ACKtmp << " SYN: " << syntmp << " FIN: " << fintmp << " ST: " << sttmp << " OV: " << ovtmp << " 校验和:0x" << hex << (unsigned int)sesum << dec;
    cout << "\tsendWin:";
    cout << windowLength;
    cout << "\tssthreah:";
    cout << ssthresh;
    cout << "\tstate:";
    cout << state;
    cout << endl;
}
void recvlog()
{
    //接收一条消息，打印接收日志
    
    getTime();
    cout << timeRecord << " [recv] ";
    getseq();
    getack();
    getLength();
    getresum();
    //收到Server端通告的窗口大小
    int windowLentmp=getWindow();
    cout << "  Seq: " << setw(5) << setiosflags(ios::left) << recvseq << "Ack: " << setw(5) << setiosflags(ios::left) << recvack ;
    int ACKtmp = (recvBuffer[24] & 0xF0) ? 1 : 0;
    int syntmp = (recvBuffer[24] & 0xF) ? 1 : 0;
    int fintmp = (recvBuffer[25] & 0xF0) ? 1 : 0;
    int sttmp = (recvBuffer[25] & 0xC) ? 1 : 0;
    int ovtmp = (recvBuffer[25] & 0x3) ? 1 : 0;
    cout << "ACK: " << ACKtmp << " SYN: " << syntmp << " FIN: " << fintmp << " ST: " << sttmp << " OV: " << ovtmp << " 校验和:0x" << hex << (unsigned int)resum << dec;
    if ((ACKtmp && syntmp) || fintmp) {
        cout << "\trecvWin:" << endl;
    }
    else {
        cout << "\trecvWin:" << recvack + 1 << endl;
    }
}

void connectEstablishment()
{
    //第一次握手的报文
    setPort();
    setIP();
    setseq(rand());
    setack(0);
    setLength(0);
    setWindow(0);
    //标志位
    clearFlag();
    setSYN();
    //数据段
    for (int i = 28; i < packetLen; i++)
        sendBuffer[i] = 0;
    //校验和
    calCheckSum();
}

bool checkackValue()
{
    getack();
    if (recvack == seq + 1)
        return true;
    else
    {
        cout << "错误" << endl;
        cout << (int)sendBuffer[12] << " " << (int)sendBuffer[13] << " " << (int)sendBuffer[14] << " " << (int)sendBuffer[15] << endl;
        cout << (unsigned int)recvBuffer[16] << " " << (unsigned int)recvBuffer[17] << " " << (unsigned int)recvBuffer[18] << " " << (int)recvBuffer[19] << endl;
        cout << recvack << endl;
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
            sendlog(sendBuffer);
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
            sendlog(sendBuffer);
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
void CubicRepetition()
{
    // 遍历发送窗口中的所有数据包
    slidingWindow[0].clock = clock();
    for (size_t i = 0; i < slidingWindow.size(); ++i)
    {
        // 发送当前数据包
        sendto(sockSrv, slidingWindow[i].buffer, sizeof(slidingWindow[i].buffer), 0, (SOCKADDR*)&Server, len);

        // 记录发送日志
        sendlog(slidingWindow[i].buffer);
    }
}

void recvthread()
{
    int newAckCount = 0;
    while (1)
    {
        mtx.lock();
        if (recvfrom(sockSrv, recvBuffer, sizeof(recvBuffer), 0, (SOCKADDR*)&Server, &len) > 0)
        {
            //收到了一个消息，
            if (checkCheckSum() && checkACK)
            {
                recvlog();
                getack();
                
                if (recvack == lastConfirmAck)
                {
                    if (state == SS)
                    {
                        ConfirmCount++;
                        if (ConfirmCount == 3)
                        {
                            //三次ack重复，重传窗口内第一个包
                            cout << "快速恢复" << endl;
                            ssthresh = windowLength / 2;
                            if (ssthresh == 0)
                                ssthresh++;
                            windowLength = ssthresh + 3;
                            CubicRepetition();
                            state = QR;
                        }
                    }
                    if (state == CA)
                    {
                        ConfirmCount++;
                        if (ConfirmCount == 3)
                        {
                            //三次ack重复，重传窗口内第一个包
                            cout << "快速恢复" << endl;
                            ssthresh = windowLength / 2;
                            if (ssthresh == 0)
                                ssthresh++;
                            windowLength = ssthresh + 3;
                            state = QR;
                            CubicRepetition();
                        }
                    }
                    if (state == QR)
                    {
                        windowLength++;
                    }
                }
                
                lastConfirmAck = recvack;
                if (recvack < slidingWindow[0].seq)
                {
                    //不在窗口范围内
                    //考虑进行重传
                }
                else
                {
                    if (state == SS)
                    {
                        windowLength++;
                        ConfirmCount = 0;
                        if (windowLength > ssthresh)
                        {
                            state = CA;
                        }
                    }
                    else if (state == CA)
                    {
                        //拥塞避免阶段
                        ConfirmCount = 0;
                        newAckCount++;
                        if (newAckCount >= windowLength)
                        {
                            newAckCount = 0;
                            windowLength++;
                        }
                    }
                    else if (state == QR)
                    {
                        windowLength = ssthresh;
                        ConfirmCount = 0;
                        state = CA;
                    }
                    
                    while (slidingWindow[0].seq <= recvack)
                    {
                        slidingWindow.erase(slidingWindow.begin());
                        if (slidingWindow.size() == 0)
                            break;
                    }
                }
                
            }
        }
        mtx.unlock();
    }
}
void retransmission()
{
    while (1)
    {
        mtx.lock();
        if (slidingWindow.size() > 0)
        {
            if (clock() - slidingWindow[0].clock > 1000)
            {
                //窗口中的第一个超时了
                cout << "超时重传" << endl;
                ssthresh = windowLength / 2;
                windowLength = 1;
                ConfirmCount = 0;
                state = SS;
                CubicRepetition();
            }
        }
        mtx.unlock();
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
        if (checkACK() && checkSYN() && checkackValue())
        {
            //收到的包正确且为二次握手
            //准备进行三次握手的回复           
            getseq();
            setseq(0);
            setack(recvseq + 1);
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
    state = SS;
    cout << "**********开始发文件*******" << endl;
    clock_t start_time = clock();// 记录开始发送文件的时间
    std::thread t1(recvthread);
    std::thread t2(retransmission);

    for (int i = 0; i < num; i++)
    {
        bool whethersend = false;
        while (!whethersend)
        {
            mtx.lock();
            if ((slidingWindow.size() < windowLength))
            {
                whethersend = true;
                char fileName[100];
                strcpy(fileName, files[i].substr(29, files[i].length() - 29).c_str());
                setseq(seq + 1);
                setack(0);
                setLength(strlen(fileName));
                clearFlag();
                setST();
                //报文
                for (int j = 0; j < length; j++)
                {
                    sendBuffer[j + 28] = fileName[j];
                }
                calCheckSum();
                //先将当前数据包存一下
                DataInWindow message;
                for (int x = 0; x < packetLen; x++)
                    message.buffer[x] = sendBuffer[x];
                message.seq = seq;
                message.clock = clock();
                slidingWindow.push_back(message);
                sendto(sockSrv, sendBuffer, sizeof(sendBuffer), 0, (SOCKADDR*)&Server, len);
                sendlog(sendBuffer);
            }
            mtx.unlock();
        }

        //将文件加载到dataContent动态数组中。
        loadFile(i);
        unsigned long long fileSize = dataContent.size();
        totalDataSent += fileSize; // 更新总数据量
        cout << "*****发送第" << i + 1 << "个文件" << " 该文件大小为" << dataContent.size() << "字节*****" << endl;
        for (int j = 0; j < dataContent.size(); j++)
        {
            
            //装载数据报文
            sendBuffer[28 + (j % messageLen)] = dataContent[j];
            if ((j % messageLen == messageLen - 1) && (j != dataContent.size() - 1))
            {
                //填满了一组报文，准备发送
                whethersend = false;
                while (!whethersend)
                {
                    mtx.lock();
                    if ((slidingWindow.size() < windowLength))
                    {
                        whethersend = true;
                        setseq(seq + 1);
                        setack(0);
                        setLength(messageLen);
                        clearFlag();
                        calCheckSum();
                        DataInWindow message;
                        for (int x = 0; x < packetLen; x++)
                            message.buffer[x] = sendBuffer[x];
                        message.seq = seq;
                        message.clock = clock();
                        slidingWindow.push_back(message);
                        sendto(sockSrv, sendBuffer, sizeof(sendBuffer), 0, (SOCKADDR*)&Server, len);
                        sendlog(sendBuffer);
                    }
                    mtx.unlock();
                }
            }
            if (j == dataContent.size() - 1)
            {
                //最后一组报文
                whethersend = false;
                while (!whethersend)
                {
                    mtx.lock();
                    if ((slidingWindow.size() < windowLength))
                    {
                        whethersend = true;
                        setseq(seq + 1);
                        setack(0);
                        setLength(j % messageLen + 1);
                        clearFlag();
                        setOV();
                        calCheckSum();
                        DataInWindow message;
                        for (int x = 0; x < packetLen; x++)
                            message.buffer[x] = sendBuffer[x];
                        message.seq = seq;
                        message.clock = clock();
                        slidingWindow.push_back(message);
                        sendto(sockSrv, sendBuffer, sizeof(sendBuffer), 0, (SOCKADDR*)&Server, len);
                        sendlog(sendBuffer);
                    }
                    mtx.unlock();
                }

            }
        }
    }


    //挥手
    //等待数据包全部确认完
    while (1)
    {
        mtx.lock();
        if (slidingWindow.size() == 0)
        {
            break;
        }
        mtx.unlock();
    }
    clock_t end_time = clock(); // 记录发送文件结束的时间
    double elapsed = double(end_time - start_time) / CLOCKS_PER_SEC; // 计算总耗时
    double throughput = totalDataSent / (elapsed * 1024 * 1024);                     //计算总吞吐率
    cout << "**********文件发送成功*******" << endl;
    cout << "文件传输时间: " << elapsed << " s" << endl;
    cout << "吞吐率: " << throughput << "MB/s" << endl;
    cout << "**********开始关闭连接*******" << endl;
    setseq(rand());
    setack(0);
    setLength(0);
    clearFlag();
    setFIN();
    calCheckSum();
    do
    {
        transmission(2);
    } while (!(checkCheckSum() && checkACK() && checkackValue()));
    closesocket(sockSrv);
    WSACleanup();
    cout << "**********连接结束*******" << endl;
    system("pause");
}



