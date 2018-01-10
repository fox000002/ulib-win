#include <iostream>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")

int main(void)
{
	WSADATA WsaDat;
	if(WSAStartup(MAKEWORD(2,2),&WsaDat)!=0)
	{
		std::cout<<"Winsock error - Winsock initialization failed\r\n";
		WSACleanup();
		system("PAUSE");
		return 0;
	}
	
	// Create our socket

	SOCKET Socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(Socket==INVALID_SOCKET)
	{
		std::cout<<"Winsock error - Socket creation Failed!\r\n";
		WSACleanup();
		system("PAUSE");
		return 0;
	}
	
	// Resolve IP address for hostname
	struct hostent *host;
	if((host=gethostbyname("localhost"))==NULL)
	{
		std::cout<<"Failed to resolve hostname.\r\n";
		WSACleanup();
		system("PAUSE");
		return 0;
	}
	
	// Setup our socket address structure
	SOCKADDR_IN SockAddr;
	SockAddr.sin_port=htons(8888);
	SockAddr.sin_family=AF_INET;
	SockAddr.sin_addr.s_addr=*((unsigned long*)host->h_addr);
	
	// Attempt to connect to server
	if(connect(Socket,(SOCKADDR*)(&SockAddr),sizeof(SockAddr))!=0)
	{
		std::cout<<"Failed to establish connection with server\r\n";
		WSACleanup();
		system("PAUSE");
		return 0;
	}
	
	// If iMode!=0, non-blocking mode is enabled.
	u_long iMode=0;
	ioctlsocket(Socket,FIONBIO,&iMode);
	
	// Main loop
	for(;;)
	{
		// Display message from server
		char buffer[1000];
		memset(buffer,0,999);
		std::cout << "Before recv============" << std::endl;
		int inDataLength=recv(Socket,buffer,1000,0);
		std::cout<<buffer<< std::endl;;	
		std::cout << "After recv=============" << std::endl;


		//std::cout << "Before send" << std::endl;
		send(Socket,buffer,strlen(buffer),0);
		memset(buffer,0,999);
		//std::cout << "After send" << std::endl;

		Sleep(5000);

		int nError=WSAGetLastError();
		if(nError!=WSAEWOULDBLOCK&&nError!=0)
		{
			std::cout<<"Winsock error code: "<<nError<<"\r\n";
			std::cout<<"Server disconnected!\r\n";
			// Shutdown our socket
			shutdown(Socket,SD_SEND);

			// Close our socket entirely
			closesocket(Socket);

			break;
		}
		Sleep(1000);
	}

	WSACleanup();
	system("PAUSE");
	return 0;
}