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

#pragma comment(lib, "ws2_32.lib")
using namespace std;

//�ܱ��ĳ���10028�ֽڣ��ײ�28�ֽڣ�����10000�ֽ�
const int packetLen = 10028;
const int messageLen = 10000;
string serverIp = "127.0.0.3";
string clientIp = "127.0.0.2";
USHORT serverPort = 8888;
USHORT clientPort = 8888;
int seq;
int ack;
int length = 0;

char sendBuffer[packetLen];
char recvBuffer[packetLen];
char filename[100];
string fimenametmp;
vector<char> dataContent;
char timeRecord[20] = { 0 };


//�����׽��ֿ�
//WORD�����ֽ��޷���������
WORD wVersionRequested;
WSADATA IpWSAData;
SOCKET sockSrv;
//��������Ϣ
SOCKADDR_IN  Server;
//�ͻ�����Ϣ
SOCKADDR_IN Client;
//����У���
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

    // ��ӡУ���
    cout << "���ͱ��ĵ�У���: 0x" << hex << (unsigned int)checkSum << dec << endl;
}
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
    USHORT checksum = (recvBuffer[26] << 8) + (unsigned char)recvBuffer[27];

    cout << "���ձ��ĵ�У���: 0x" << hex << (unsigned int)checksum << dec << endl;
    if (checksum + (USHORT)sum == 0xffff)
    {
        return true;
    }
    {
        cout << "ʧ��У��" << endl;
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

//�ظ���������
void connectAckPrepare()
{
    //����Դ�˿���Ŀ�Ķ˿ںţ���Ϊʼ�ձ������ӣ����Բ���Ҫ�ٸ��ġ�
    sendBuffer[0] = (char)(serverPort >> 8);
    sendBuffer[1] = (char)(serverPort & 0xFF);
    sendBuffer[2] = (char)(clientPort >> 8);
    sendBuffer[3] = (char)(clientPort & 0xFF);
    //����ԴIP��Ŀ��IP
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
            tmp += tmp * 10 + (int)serverIp[i] - 48;
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
            tmp += tmp * 10 + (int)clientIp[i] - 48;
        }
    }
    sendBuffer[ctrl++] = (char)tmp;

    //��ʼ�����ȷ�����
    //ȷ�����Ϊ�յ��ĳ�ʼ��ż�һ
    SeqAndAckSet(rand(), 1);
    //���ֱ��ĳ��ȣ�
    lengthSet(0);
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

bool ackValueCheck()
{
    int recvack = (int)((unsigned char)recvBuffer[16] << 24) + (int)((unsigned char)recvBuffer[17] << 16) + (int)((unsigned char)recvBuffer[18] << 8) + (int)(unsigned char)recvBuffer[19];
    if (recvack == seq + 1)
        return true;
    else
    {
        cout << "����" << endl;
        cout << (int)sendBuffer[12] << " " << (int)sendBuffer[13] << " " << (int)sendBuffer[14] << " " << (int)sendBuffer[15] << endl;
        cout << (unsigned int)recvBuffer[16] << " " << (unsigned int)recvBuffer[17] << " " << (unsigned int)recvBuffer[18] << " " << (int)recvBuffer[19] << endl;
        cout << recvack << endl;
        return false;
    }
}
void initial()
{
    //MAKEWORD����һ������Ϊ��λ�ֽڣ����汾�ţ��ڶ�������Ϊ��λ�ֽڣ����汾��
    wVersionRequested = MAKEWORD(1, 1);
    //����socket��İ�
    int error = WSAStartup(wVersionRequested, &IpWSAData);
    if (error != 0)
    {
        cout << "Winsock��ʼ������" << endl;
        exit(0);
    }

    //�������ڼ�����socket
    //AF_INET��TCP/IP&IPv4��SOCK_DGRAM��UDP���ݱ�
    sockSrv = socket(AF_INET, SOCK_DGRAM, 0);


    //IP��ַ
    Server.sin_addr.s_addr = inet_addr(serverIp.c_str());
    //Э���
    Server.sin_family = AF_INET;
    //���Ӷ˿ں�
    Server.sin_port = htons(serverPort);


    //�󶨶˿�
    bind(sockSrv, (SOCKADDR*)&Server, sizeof(SOCKADDR));//�᷵��һ��SOCKET_ERROR
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
        recvlog();
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
    int recvSeq = (int)((unsigned char)recvBuffer[12] << 24) + (int)((unsigned char)recvBuffer[13] << 16) + (int)((unsigned char)recvBuffer[14] << 8) + (int)(unsigned char)recvBuffer[15];
    if ((recvSeq == ack) || (recvSeq == 0))
        return true;
    else
    {
        cout << "error" << endl;
        cout << recvSeq << " " << ack << endl;
        return false;
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
                if (checkACK() && ackValueCheck())
                {
                    //�յ�������������ȷ
                    cout << "**********���ӳɹ�**********" << endl;
                    cout << "**********��ʼ���ļ�*******" << endl;
                    break;
                }
            }
            break;
        }
        else {
            //�յ���һ�����ִ��󣬻ظ�ACK=0�������ش����ص�whileѭ�������ȴ����ա�
            connectAckPrepare();
            clearFlag();
            calCheckSum();
            transmission(0);
        }
    }
    ack = 0;
    //�ȽӺ�
    while (1)
    {
        transmission(1);

        if (checkCheckSum())
        {
            if (checkFIN())
            {
                break;
            }
            if (checkST())
            {
                //ST�źţ��յ������ļ�����һ����һ���εĵ�һ�����ġ�
                int messageLength = (int)((unsigned char)recvBuffer[20] << 24) + (int)((unsigned char)recvBuffer[21] << 16) + (int)((unsigned char)recvBuffer[22] << 8) + (int)((unsigned char)recvBuffer[23]);
                for (int i = 0; i < messageLength; i++)
                {
                    filename[i] = recvBuffer[28 + i];
                }
                filename[messageLength] = '\0';
                fimenametmp = "C:\\Users\\����\\Desktop\\lab3-1���ն�\\" + (string)filename;
                cout << fimenametmp << endl;
            }
            if (checkOV())
            {
                //OV�źţ��ļ�����
                int messageLength = (int)((unsigned char)recvBuffer[20] << 24) + (int)((unsigned char)recvBuffer[21] << 16) + (int)((unsigned char)recvBuffer[22] << 8) + (int)((unsigned char)recvBuffer[23]);
                for (int i = 0; i < messageLength; i++)
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
            if ((recvBuffer[25] == 0) && checkSeq())
            {
                //�����ı���
                int messageLength = (int)((unsigned char)recvBuffer[20] << 24) + (int)((unsigned char)recvBuffer[21] << 16) + (int)((unsigned char)recvBuffer[22] << 8) + (int)((unsigned char)recvBuffer[23]);
                for (int i = 0; i < messageLength; i++)
                {
                    dataContent.push_back(recvBuffer[28 + i]);
                }
            }
            SeqAndAckSet(0, 1);
            lengthSet(0);
            clearFlag();
            setAck();
        }
        else
        {
            clearFlag();
        }
        calCheckSum();
        transmission(0);
    }
    //�յ��˵�һ�λ���
    SeqAndAckSet(rand(), 1);
    lengthSet(0);
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



