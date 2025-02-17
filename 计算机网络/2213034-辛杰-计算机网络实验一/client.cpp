#include<iostream>
#include<winsock.h>
#include<cstring>
#include<windows.h>
#include<queue>
#pragma comment(lib,"ws2_32.lib")//链接ws2_32.lib库文件到此项目当中
using namespace std;
void initialization();
char name[100];
string messList[100];
string quit("/quit");
int mess = 0;
bool online = true;
int connectResult = 0;
SOCKET s_server;
SOCKADDR_IN server_addr;

void clsScreen()
{
	Sleep(100);
	system("cls");
	cout << "=======================" << endl;
	cout << "昵称：" << name << endl;
	cout << "=======================" << endl;
	for (int i = 0; i < mess; ++i)
		cout << messList[i].c_str() << endl;
	cout << "请输入消息:";
}


DWORD WINAPI newMessage(LPVOID lparam)
{
	SOCKET ClientSocket = (SOCKET)(LPVOID)lparam;
	while (online)
	{
		Sleep(10);
		char recv_buf[200];
		Sleep(100);
		recv(ClientSocket, recv_buf, 200, 0);
		//cout << recv_buf << endl;
		string str(recv_buf);
		messList[mess] = str;
		++mess;
		clsScreen();
	}
	return 0;
}
DWORD WINAPI sentMessage(LPVOID lparam)
{
	SOCKET ClientSocket = (SOCKET)(LPVOID)lparam;
	bool first = true;
	while (online)
	{
		if (first)
		{
			cout << "输入你的姓名:";
			first = false;
			char name_buf[100];
			cin.getline(name_buf, 100);
			strcpy_s(name, name_buf);
			send(ClientSocket, name_buf, 100, 0);
			cout << "请输入消息:";
		}
		else
		{
		char sent_buf[100];
		cin.getline(sent_buf,100);
		char sent_buf1[100];
		char cn[10] = ":";
		strcpy_s(sent_buf1, name);
		strcat_s(sent_buf1, cn);
		strcat_s(sent_buf1, sent_buf);
		string o(sent_buf);
		if (o == quit)
		{
			closesocket(s_server);
			online = false;
		}
		send(ClientSocket, sent_buf, 100, 0);
		messList[mess] = sent_buf1;
		++mess;
		clsScreen();
		}
	}
	online = false;
	return 0;
}
int main() {

	initialization();

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	server_addr.sin_port = htons(5010);
	s_server = socket(AF_INET, SOCK_STREAM, 0);

	connectResult=connect(s_server, (SOCKADDR *)&server_addr, sizeof(SOCKADDR));
	if (connectResult == SOCKET_ERROR) {
		int wsaLastError = WSAGetLastError();
		std::cout << "未连接到服务器: " << wsaLastError << std::endl;
		closesocket(s_server);
		WSACleanup();
		return 0;
	}
	else {
		cout << "=======================" << endl;
		cout << "欢迎来到聊天室！(p≧w≦q)" << endl;
		cout << "=======================" << endl;
	}

	HANDLE hThread = CreateThread(NULL, NULL, newMessage, LPVOID(s_server), 0, NULL);
	HANDLE hThread2 = CreateThread(NULL, NULL, sentMessage, LPVOID(s_server), 0, NULL);
	WaitForSingleObject(hThread, INFINITE);
	WaitForSingleObject(hThread2, INFINITE);
	CloseHandle(hThread);
	CloseHandle(hThread2);
	closesocket(s_server);
	WSACleanup();
	return 0;
}

//确保了程序能够使用 Winsock 提供的网络功能
void initialization() {
	WORD w_req = MAKEWORD(2, 2);
	WSADATA wsadata;
	int err;
	err = WSAStartup(w_req, &wsadata);
	if (err != 0) {
		cout << "Winsock 初始化失败" << endl;
	}
}
