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

//�ܱ��ĳ���10028�ֽڣ��ײ�28�ֽڣ�����10000�ֽ�
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
string path("C:\\Users\\����\\Desktop\\lab3-1���Ͷ�");

char sendBuffer[packetLen];
char recvBuffer[packetLen];
unsigned long long totalDataSent = 0;
vector<char> dataContent;
char timeRecord[20] = { 0 };


//�����׽��ֿ�
//WORD�����ֽ��޷���������
WORD wVersionRequested;
WSADATA IpWSAData;
SOCKET sockSrv;
//Ҫ���ӵķ�����
SOCKADDR_IN  Server;


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


    cout << "���ձ��ĵ�У���: 0x" << hex << (unsigned int)checksum << dec << endl;
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

// �����������ַ�ת��Ϊ����
int charToDigit(char c) {
    return c - '0';
}

// �����ʮ����IP��ַת��Ϊ��ֵ��ʽ���洢��������
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

//��������
void connectEstablishment()
{
    //����Դ�˿���Ŀ�Ķ˿ںţ��˿ں�Ϊ2�ֽ�
    sendBuffer[0] = (char)(clientPort >> 8);
    sendBuffer[1] = (char)(clientPort & 0xFF);
    sendBuffer[2] = (char)(serverPort >> 8);
    sendBuffer[3] = (char)(serverPort & 0xFF);

    //����ԴIP��Ŀ��IP
    int ctrl = 4;
    ipToString(sendBuffer, ctrl, clientIp);
    ipToString(sendBuffer, ctrl, serverIp);

    //��ʼ�����ȷ�����
    SeqAndAckSet(rand(), 0);
    //���ֱ��ĳ��ȣ�
    lengthSet(0);
    //��־λ
    clearFlag();
    setSYN();
    //���ݶ�
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
        cout << "ack����" << endl;
        return false;
    }
}

bool getFiles(string path, vector<string>& files)
{
    //�ļ����
    _int64 hFile = 0;
    //�ļ���Ϣ
    struct _finddata_t fileinfo;
    string p;
    if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
        if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
        {
            // �����ļ���ȫ·��
            files.push_back(p.assign(path).append("\\").append(fileinfo.name));

        }
        while (_findnext(hFile, &fileinfo) == 0)
        {
            if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
            {
                // �����ļ���ȫ·��
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

//��ʼ���׽���
void initial()
{
    wVersionRequested = MAKEWORD(1, 1);
    int error = WSAStartup(wVersionRequested, &IpWSAData);
    if (error != 0)
    {
        cout << "Winsock��ʼ������" << endl;
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
        //ֻ������
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
        //ֻ�ղ���
        break;
    case 2:
        //�ȷ�����
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
        //�����ٷ�
        break;
    default:
        break;
    }
}
int main()
{
    initial();
    cout << "==========" << endl;
    cout << "**���Ͷ�**" << endl;
    cout << "==========" << endl;
    connectEstablishment();
    while (1)
    {
        //��һ�����֣�У��ʧ�ܻ��ش���
        transmission(2);
        if (checkACK() && checkSYN() && ackValueCheck())
        {
            //�յ��İ���ȷ��Ϊ��������
            //׼�������������ֵĻظ�
            SeqAndAckSet(rand(), 1);
            //��־λ
            clearFlag();
            setAck();
            calCheckSum();
            transmission(0);
            cout << "**********���ӳɹ�**********" << endl;
            break;

        }
    }
    getFiles(path, files);
    int num = files.size();
    cout << "**********��ʼ���ļ�*******" << endl;
    clock_t start_time = clock();// ��¼��ʼ�����ļ���ʱ��
    for (int i = 0; i < num; i++)
    {
        char fileName[100];
        strcpy(fileName, files[i].substr(35, files[i].length() - 35).c_str());
        //Ҫ���͵ĵ�һ����Ϣ�����ļ�������
        SeqAndAckSet(0, 0);
        lengthSet(strlen(fileName));
        clearFlag();
        setST();
        //����
        for (int j = 0; j < length; j++)
        {
            sendBuffer[j + 28] = fileName[j];
        }
        calCheckSum();
        do
        {
            transmission(2);
        } while (!(checkACK() && ackValueCheck()));

        //���ļ����ص�dataContent��̬�����С�
        loadFile(i);
        unsigned long long fileSize = dataContent.size();
        totalDataSent += fileSize; // ������������
        for (int j = 0; j < dataContent.size(); j++)
        {
            //װ�����ݱ���
            sendBuffer[28 + (j % messageLen)] = dataContent[j];
            if ((j % messageLen == messageLen - 1) && (j != dataContent.size() - 1))
            {
                //������һ�鱨�ģ�׼������
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
                //���һ�鱨��
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
    cout << "**********�ļ����ͳɹ�*******" << endl;

    clock_t end_time = clock(); // ��¼�����ļ�������ʱ��
    double elapsed = double(end_time - start_time) / CLOCKS_PER_SEC; // �����ܺ�ʱ
    double throughput = totalDataSent / (elapsed*1024*1024);                     //������������
    cout << "�ļ�����ʱ��: " << elapsed << " s" << endl;
    cout << "������: " << throughput << "MB/s" << endl;

    //����
    cout << "**********��ʼ�ر�����*******" << endl;
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
    cout << "**********���ӽ���*******" << endl;
    system("pause");
}



