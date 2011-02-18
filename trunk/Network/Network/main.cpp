// server.c //////////////////////////////////

#include <winsock2.h>
#include <stdio.h>
#pragma comment(lib,"ws2_32.lib")

const int SERVER_PORT=15001; /*the default serverport number*/
int visits=0; /*the number client connect*/

int main(int argc,char* argv[]){
	printf("deded");
   struct sockaddr_in serverAddr; /*server address*/
   struct sockaddr_in clientAddr; /*client address*/

   int serverSock; /*server socket, and client sock*/
   char buf[100]; /*buffer the message send and receive*/
   int len=sizeof(struct sockaddr);
   char ip[15]; /*client address*/
   int serverPort,clientPort; /*client and server address*/

   WSADATA wsaData;
   WSAStartup(0x0202,&wsaData); /*windows socket startup */

   memset((char*)&serverAddr,0,sizeof(serverAddr));
   serverAddr.sin_family=AF_INET; /*set server address protocol family*/
   serverAddr.sin_addr.s_addr=INADDR_ANY;

   if (argc>1){
     serverPort=atoi(argv[1]);
   }else{
     serverPort=SERVER_PORT;
   }

   if (serverPort>0)
     serverAddr.sin_port=htons((u_short)serverPort);/*set port*/
   else {
     printf("bad port number %s\n",argv[1]);
     exit(1);
   }

   serverSock=socket(PF_INET,SOCK_DGRAM,0);/*create a socket*/
   if (serverSock<0){
     printf("socket creating failed\n");
     exit(1);
   }

   if (bind(serverSock,(LPSOCKADDR)&serverAddr,len)<0){/*bind a server address and port*/
     printf("bind failed\n");
     exit(1);
  }

   // Set Non Blocking Mode, specified via last parameter
   // 0 Disabled 
   // !0 Enabled 
   unsigned long int nonBlockingMode = 1;
   int result = ioctlsocket( serverSock, FIONBIO, &nonBlockingMode );

   if ( result )
   {
     printf("ioctlsocket failed\n");
     exit(1);
   }


  while(1){
	  printf("while");
   // Setup the Client Address ( IP & port )
   struct sockaddr_in clientAddr;
   // this information will be filled in when recvfrom executes

   // Check for messages
   FD_SET set;
   timeval timeVal;

   FD_ZERO( &set );
   FD_SET( serverSock, &set );

   timeVal.tv_sec = 0;
   timeVal.tv_usec = 0;

   result = select( FD_SETSIZE, &set, NULL, NULL, &timeVal );

   if( result == 0 )
   {
      Sleep( 300 );
      continue;
   }

   if( result == SOCKET_ERROR )
   {
     printf("select error\n");
     exit(1);
   }

    memset(buf,0,sizeof(buf));
    recvfrom(serverSock,buf,sizeof(buf),0,(LPSOCKADDR)&clientAddr,&len);

    visits++;
    strcpy(ip,inet_ntoa(clientAddr.sin_addr));
    clientPort=ntohs((u_short)clientAddr.sin_port);

    printf("\nA client come from ip:%s port:%d .\nThis server has been contacted %d time%s\n",ip,clientPort,visits,visits==1?".":"s.");

    while(1)
   {
     printf("\n--->From the client: ");
     printf("%s",buf);

     memset(buf,0,sizeof(buf));
     printf("\nServer--->: ");
     gets(buf);
     sendto(serverSock,buf,sizeof(buf),0,(LPSOCKADDR)&clientAddr,len);
     if(strcmp(buf,"end")==0) break; /* send "end" to close client and exit loop*/
     if(strcmp(buf,"close")==0) return 1; /* send "close" to close both */

     memset(buf,0,sizeof(buf));
     recvfrom(serverSock,buf,sizeof(buf),0,(LPSOCKADDR)&clientAddr,&len);

     // moderator note: This code is a buffer overflow attack waiting to happen

     if(strcmp(buf,"end")==0) break; /* receive "end" to exit loop */
   }
}

   return 1;
}

