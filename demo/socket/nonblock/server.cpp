#include <iostream>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
					
int main()
{
	WSADATA WsaDat;
	if(WSAStartup(MAKEWORD(2,2),&WsaDat)!=0)
	{
		std::cout<<"WSA Initialization failed!\r\n";
		WSACleanup();
		system("PAUSE");
		return 0;
	}
					
	SOCKET Socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(Socket==INVALID_SOCKET)
	{
		std::cout<<"Socket creation failed.\r\n";
		WSACleanup();
		system("PAUSE");
		return 0;
	}
					
	SOCKADDR_IN serverInf;
	serverInf.sin_family=AF_INET;
	serverInf.sin_addr.s_addr=INADDR_ANY;
	serverInf.sin_port=htons(8888);
					
	if(bind(Socket,(SOCKADDR*)(&serverInf),sizeof(serverInf))==SOCKET_ERROR)
	{
		std::cout<<"Unable to bind socket!\r\n";
		WSACleanup();
		system("PAUSE");
		return 0;
	}
		
	listen(Socket,1);
					
	SOCKET TempSock=SOCKET_ERROR;
	while(TempSock==SOCKET_ERROR)
	{
		std::cout<<"Waiting for incoming connections...\r\n";
		TempSock=accept(Socket,NULL,NULL);
	}
				
	// If iMode!=0, non-blocking mode is enabled.
	u_long iMode=1;
	ioctlsocket(Socket,FIONBIO,&iMode);
					
	Socket=TempSock;
	std::cout<<"Client connected!\r\n\r\n";
	char buffer[1000];
	memset(buffer,0,999);				
	// Main loop
	for(;;)
	{
		char *szMessage="Welcome to the server!\r\n";

		std::cout << "Before send" << std::endl;
		send(Socket,szMessage,strlen(szMessage),0);
		std::cout << "After send" << std::endl;

		
		memset(buffer,0,999);
		std::cout << "Before recv" << std::endl;
		int inDataLength=recv(Socket,buffer,1000,0);
		std::cout<<buffer;
		std::cout << "After recv" << std::endl;
					
		int nError=WSAGetLastError();
		if(nError!=WSAEWOULDBLOCK&&nError!=0)
		{
			std::cout<<"Winsock error code: "<<nError<<"\r\n";
			std::cout<<"Client disconnected!\r\n";

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