#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<vector>
#include<fstream>
#include<io.h>
#include<time.h>    
#include<stdlib.h>
#include<iomanip>
#include <Winsock2.h>
#include<thread>
#include <chrono>  // ����ʱ���

// ����һ�������������ϴη���ȷ�ϵ�ʱ��
std::chrono::steady_clock::time_point lastAckTime = std::chrono::steady_clock::now();
const std::chrono::milliseconds ackDelay(20);  // ����ȷ����Ϣ����ʱ������ 20 ����

#pragma comment(lib, "ws2_32.lib")
using namespace std;

//�ܱ��ĳ���10028�ֽڣ��ײ�28�ֽڣ�����10000�ֽ�
const int packetLen = 10028;
const int messageLen = 10000;

/**********����socket����-start**********/
WORD wVersionRequested;
WSADATA IpWSAData;
SOCKET sockSrv;
//��������Ϣ
SOCKADDR_IN  Server;
//�ͻ�����Ϣ
SOCKADDR_IN Client;
string serverIp = "127.0.0.3";
string clientIp = "127.0.0.2";
USHORT serverPort = 8888;
USHORT clientPort = 8888;
int len = sizeof(SOCKADDR);
/**********����socket����-end**********/

/**********���ڱ�����Ϣ-start***********/
char sendBuffer[packetLen];
char recvBuffer[packetLen];
char timeRecord[20] = { 0 };
int seq;
int ack;
int recvseq;
int recvack;
int length = 0;
int window;
int recvWindow;
int lastConfirmSeq = 0;
USHORT resum;
USHORT sesum;
/**********���ڱ�����Ϣ-end***********/

char filename[100];
string fimenametmp;
vector<char> dataContent;

struct DataInWindow {
    bool ack = false;
    char buffer[packetLen];
    int length;
    int seq;
};
vector<DataInWindow> slidingWindow;
int windowLength = 1;

void initial()
{
    //MAKEWORD����һ������Ϊ��λ�ֽڣ����汾�ţ��ڶ�������Ϊ��λ�ֽڣ����汾��
    wVersionRequested = MAKEWORD(1, 1);
    //����socket��İ�
    int error = WSAStartup(wVersionRequested, &IpWSAData);
    if (error != 0)
    {
        cout << "��ʼ������" << endl;
        exit(0);
    }

    //�������ڼ�����socket
    //AF_INET��TCP/IP&IPv4��SOCK_DGRAM��UDP���ݱ�
    sockSrv = socket(AF_INET, SOCK_DGRAM, 0);
    Server.sin_addr.s_addr = inet_addr(serverIp.c_str());
    Server.sin_family = AF_INET;
    Server.sin_port = htons(serverPort);

    //�󶨶˿�
    bind(sockSrv, (SOCKADDR*)&Server, sizeof(SOCKADDR));//�᷵��һ��SOCKET_ERROR
    //������ģʽ
    /*unsigned long ul = 1;
    int ret = ioctlsocket(sockSrv, FIONBIO, (unsigned long*)&ul);*/
}
void calCheckSum()
{
    int sum = 0;
    //����ǰ26�ֽ�
    for (int i = 0; i < packetLen; i += 2)
    {
        if (i == 26)
            continue;
        //ÿ�����ϼ�16λ
        sum += (sendBuffer[i] << 8) + sendBuffer[i + 1];
        //���ֽ�λ,�������λ
        if (sum >= 0x10000)
        {
            sum -= 0x10000;
            sum += 1;
        }
    }
    //ȡ��
    USHORT checkSum = ~(USHORT)sum;
    sendBuffer[26] = (char)(checkSum >> 8);
    sendBuffer[27] = (char)checkSum;

}

//У������
bool checkCheckSum()
{
    int sum = 0;
    //�����У����λ��ȫ���ֽ�
    for (int i = 0; i < packetLen; i += 2)
    {
        if (i == 26)
            continue;
        //ÿ�����ϼ�16λ
        sum += (recvBuffer[i] << 8) + recvBuffer[i + 1];
        //���ֽ�λ,�������λ
        if (sum >= 0x10000)
        {
            sum -= 0x10000;
            sum += 1;
        }
    }
    //checksum����
    //ȡ��
    USHORT checksum = (recvBuffer[26] << 8) + (unsigned char)recvBuffer[27];

    if (checksum + (USHORT)sum == 0xffff)
    {
        return true;
    }
    {
        cout << "У��ʧ�ܣ����·���" << endl;
        return false;
    }
}
void getTime()
{
    time_t t = time(0);
    strftime(timeRecord, sizeof(timeRecord), "%H:%M:%S", localtime(&t));
}




void setPort()
{
    //��Դ�˿ںź�Ŀ�Ķ˿ں�д�뵽sendBuffer֮�С�
    sendBuffer[0] = (char)(serverPort >> 8);
    sendBuffer[1] = (char)(serverPort & 0xFF);
    sendBuffer[2] = (char)(clientPort >> 8);
    sendBuffer[3] = (char)(clientPort & 0xFF);
}
void setIP()
{
    //��ԴIP��Ŀ��IPд�뵽sendBuffer�У�
    int tmp = 0;
    int ctrl = 4;
    for (int i = 0; i < serverIp.length(); i++)
    {
        if (serverIp[i] == '.')
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
    for (int i = 0; i < clientIp.length(); i++)
    {
        if (clientIp[i] == '.')
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
void getresum() {
    resum = (recvBuffer[26] << 8) + (unsigned char)recvBuffer[27];
}
void getsesum() {
    sesum = (sendBuffer[26] << 8) + (unsigned char)sendBuffer[27];
}
void setack(int newack)
{
    ack = newack;
    sendBuffer[16] = (char)(ack >> 24);
    sendBuffer[17] = (char)(ack >> 16);
    sendBuffer[18] = (char)(ack >> 8);
    sendBuffer[19] = (char)ack;
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
void getWindow()
{
    recvWindow = (int)((unsigned char)recvBuffer[22] << 8) + (int)(unsigned char)recvBuffer[23];
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

//�ظ���������
void connectAckPrepare()
{
    setPort();
    setIP();
    getseq();
    setseq(rand());
    setack(recvseq + 1);
    setLength(0);
    setWindow(windowLength - slidingWindow.size());
    //��־λ--�ڶ�������
    clearFlag();
    setAck();
    setSYN();
    //����
    for (int i = 28; i < packetLen; i++)
        sendBuffer[i] = 0;
    //У���
    calCheckSum();
}

bool checkackValue()
{
    getack();
    if (recvack == seq + 1)
        return true;
    else
    {
        cout << "ack����" << endl;
        cout << (int)sendBuffer[12] << " " << (int)sendBuffer[13] << " " << (int)sendBuffer[14] << " " << (int)sendBuffer[15] << endl;
        cout << (unsigned int)recvBuffer[16] << " " << (unsigned int)recvBuffer[17] << " " << (unsigned int)recvBuffer[18] << " " << (int)recvBuffer[19] << endl;
        cout << recvack << endl;
        return false;
    }
}
void sendlog()
{
    //����һ����Ϣ����ӡ���͵���־
    getTime();
    getsesum();
    cout << timeRecord << " [send] ";
    cout << "  Seq: " << setw(5) << setiosflags(ios::left) << seq << "Ack: " << setw(5) << setiosflags(ios::left) << ack;
    int ACKtmp = (sendBuffer[24] & 0xF0) ? 1 : 0;
    int syntmp = (sendBuffer[24] & 0xF) ? 1 : 0;
    int fintmp = (sendBuffer[25] & 0xF0) ? 1 : 0;
    int sttmp = (sendBuffer[25] & 0xC) ? 1 : 0;
    int ovtmp = (sendBuffer[25] & 0x3) ? 1 : 0;
    cout << "ACK: " << ACKtmp << " SYN: " << syntmp << " FIN: " << fintmp << " ST: " << sttmp << " OV: " << ovtmp << " У���:0x" << hex << (unsigned int)sesum << dec;
    cout << endl;
}
void recvlog()
{
    //����һ����Ϣ����ӡ������־
    getTime();
    cout << timeRecord << " [recv] ";
    getseq();
    getack();
    getLength();
    getresum();
    cout << "  Seq: " << setw(5) << setiosflags(ios::left) << recvseq << "Ack: " << setw(5) << setiosflags(ios::left) << recvack;
    int ACKtmp = (recvBuffer[24] & 0xF0) ? 1 : 0;
    int syntmp = (recvBuffer[24] & 0xF) ? 1 : 0;
    int fintmp = (recvBuffer[25] & 0xF0) ? 1 : 0;
    int sttmp = (recvBuffer[25] & 0xC) ? 1 : 0;
    int ovtmp = (recvBuffer[25] & 0x3) ? 1 : 0;
    cout << "ACK: " << ACKtmp << " SYN: " << syntmp << " FIN: " << fintmp << " ST: " << sttmp << " OV: " << ovtmp << " У���:0x" << hex << (unsigned int)resum << dec;
    cout << endl;
}
void transmission(int ctrl)
{
    int len = sizeof(SOCKADDR);
    int start;
    bool whether;
    switch (ctrl)
    {
    case 0:
        //ֻ������
        sendto(sockSrv, sendBuffer, sizeof(sendBuffer), 0, (SOCKADDR*)&Client, len);
        sendlog();
        break;
    case 1:
        //ֻ�ղ���
        while (1)
        {
            if (recvfrom(sockSrv, recvBuffer, sizeof(recvBuffer), 0, (SOCKADDR*)&Client, &len) > 0)
            {
                recvlog();
                break;
            }
        }
        break;
    case 2:
        //�ȷ�����
        do
        {
            sendto(sockSrv, sendBuffer, sizeof(sendBuffer), 0, (SOCKADDR*)&Client, len);
            sendlog();
            start = clock();
            whether = false;
            while (clock() - start < 2000)
            {
                if (recvfrom(sockSrv, recvBuffer, sizeof(recvBuffer), 0, (SOCKADDR*)&Client, &len) > 0)
                {
                    whether = true;
                    recvlog();
                    break;
                }
            }

        } while (!checkCheckSum() || (!whether));

        break;
    case 3:
        break;
    default:
        break;
    }
}
bool checkSeq()
{
    if (recvseq == ack + 1)
        return true;
    else
    {
        cout << "��������,���ݲ�����" << endl;
        //cout << "last ack is " << ack << " and now recv seq is " << recvseq << endl;
        return false;
    }
}
void messageProcessing()
{
    if (checkST())
    {

        //ST�źţ��յ������ļ�����һ����һ���εĵ�һ�����ġ�
        getLength();
        for (int i = 0; i < length; i++)
        {
            filename[i] = recvBuffer[28 + i];
        }
        filename[length] = '\0';
        fimenametmp = "C:\\Users\\����\\Desktop\\���ն�\\" + (string)filename;
        cout << fimenametmp << endl;
    }
    if (checkOV())
    {
        //OV�źţ��ļ�����
        getLength();
        for (int i = 0; i < length; i++)
        {
            dataContent.push_back(recvBuffer[28 + i]);
        }
        ofstream fout(fimenametmp.c_str(), ofstream::binary);
        for (int i = 0; i < dataContent.size(); i++)
        {
            fout << dataContent[i];
        }
        vector<char>().swap(dataContent);
    }
    if (recvBuffer[25] == 0)
    {
        //�����ı���
        getLength();
        for (int i = 0; i < length; i++)
        {
            dataContent.push_back(recvBuffer[28 + i]);
        }
    }
}
int main()
{
    initial();
    cout << "==========" << endl;
    cout << "**���ն�**" << endl;
    cout << "==========" << endl;
    while (1)
    {
        transmission(1);
        //�ȴ�������������
        if (checkCheckSum() && checkSYN())
        {
            //�ظ�ack
            connectAckPrepare();
            while (1)
            {
                transmission(2);
                if (checkACK() && checkackValue())
                {
                    getseq();
                    setack(recvseq);
                    //�յ�������������ȷ
                    cout << "**********���ӳɹ�**********" << endl;
                    cout << "**********��ʼ�����ļ�*******" << endl;
                    break;
                }
            }
            break;
        }
        else {
            //�յ���һ�����ִ���,�����д����ȴ����Ͷ��Զ��ش���

        }
    }
    while (1)
    {
        if (checkCheckSum())
        {
            recvfrom(sockSrv, recvBuffer, sizeof(recvBuffer), 0, (SOCKADDR*)&Client, &len);
            recvlog();
            if (checkFIN())
            {
                break;
            }
            getseq();
            if (checkSeq())
            {
                messageProcessing();
                setack(recvseq);
                lastConfirmSeq = recvseq;
                setseq(0);
                setLength(0);
                clearFlag();
                setAck();
                calCheckSum();
                sendto(sockSrv, sendBuffer, sizeof(sendBuffer), 0, (SOCKADDR*)&Client, len); // ����ȷ����Ϣ
                sendlog();
            }
            else if(recvseq> lastConfirmSeq)
            {
                //У������ȷ������Ų���ȷ��ֱ������
                //�ۼ�ȷ���£��ظ����ڵ�ackֱ��������
                setack(lastConfirmSeq);
                setseq(0);
                setLength(0);
                clearFlag();
                setAck();
                calCheckSum();
                sendto(sockSrv, sendBuffer, sizeof(sendBuffer), 0, (SOCKADDR*)&Client, len);
                sendlog();
            }
        }
        else
        {
            //У������󣬲������
            clearFlag();
        }
    }
    //�յ��˵�һ�λ���
    getseq();
    setseq(rand());
    setack(recvseq + 1);
    setLength(0);
    clearFlag();
    setFIN();
    setAck();
    calCheckSum();
    transmission(0);
    cout << "**********�ļ����ճɹ�*******" << endl;
    cout << "**********���ӽ���*******" << endl;
    closesocket(sockSrv);
    WSACleanup();
    system("pause");
}



