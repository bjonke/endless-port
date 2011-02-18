/**
 * @file sockan.cpp
 *
 * @brief The actor that you play.
 *
 * @author Rickard Skeppström
 *
 * @version 12.0
 *
 */

#include <iostream>
#include <winsock2.h>

#pragma comment(lib, "Ws2_32.lib")

#define WSAErr() do{cerr << "WSAError: " << WSAGetLastError() << endl;}while(false)
#define PORT 27015
 
//lets not rape the global namespace or compiler with 'úsing namespace std;'
using std::cout;
using std::cerr;
using std::cin;
using std::endl;

inline bool InitSock()
{
       #if PLATFORM == PLATFORM_WIN
           WSADATA WsaData;
           return WSAStartup(MAKEWORD(2,2), &WsaData) == NO_ERROR;
       #else
           return true;
       #endif
}
 
inline void StopSock()
{
       #if PLATFORM == PLATFORM_WIN
           WSACleanup();//just btw, this can fail too, just like WSAStartup...
       #endif
}

class Address
{
    public:
        Address()
        {
                 address = 0;//and port?
        }
        //i'd say use inet_addr here, much safer and easier :)
        Address( unsigned int a, unsigned int b, unsigned int c, unsigned int d, unsigned short prt )
        {
                 address = (a << 24) | (b << 16) | (c << 8) | d;
                 port = prt;
        }
        //hint: initializer lists allow this without funny na
        Address( unsigned int addrss, unsigned short prt )
        {
                 address = addrss;
                 port = prt;
        }
 
        unsigned int GetAddress() const//technically an IP is an unsigned long
        {
                 return address;
        }
 
        unsigned short GetPort() const
        {
                 return port;
        }
 
    private:
 
        unsigned int address;
        unsigned short port;
};
 
 
class Socket
{
    public:
 
        Socket()
        {
                handle = socket( AF_INET, SOCK_DGRAM, 0 );
                if (handle == INVALID_SOCKET)
                   WSAErr();
        }
 
        bool Open( unsigned short port )
        {
             //set our ports etc
             sockaddr_in address;
             address.sin_family = AF_INET;
             address.sin_addr.s_addr = INADDR_ANY;
             address.sin_port = htons(port);
             int result = bind(handle,(const sockaddr*) &address,sizeof(sockaddr_in));
             if(result == SOCKET_ERROR)//incase another value below zero gets reserved to mean something other than 'érror'
                WSAErr();
 
			 DWORD nonblocking = 1;
			 ioctlsocket(handle, FIONBIO, &nonblocking);
 
            return 0;
        }
 
        void Close()
        {
             #if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNI
                 //close( socket );
             #elif PLATFORM == PLATFORM_WIN
                   closesocket( socket );
             #endif
        }
 
        bool Send( const Address & destination, const char * data, int size )
        {
             unsigned int dest_addr = destination.GetAddress();
             unsigned short dest_port = destination.GetPort();
 
             sockaddr_in address;
             address.sin_family = AF_INET;
             address.sin_addr.s_addr = htonl(dest_addr);
             address.sin_port = htons(dest_port);
 
			if (connect(handle, (sockaddr*)&address, sizeof(sockaddr_in)) ==
					SOCKET_ERROR) {
				handle = INVALID_SOCKET;
			}
			 send(handle,data,size,0);

             //int sent_bytes = sendto( handle, data, size, 0, (sockaddr*)&address, sizeof(address) );
             //if ( sent_bytes < size || handle == INVALID_SOCKET)//why would the socket explode after?
             //{
             //   WSAErr();
             //   return false;
             //}
 
             return true;
        }
 
        int Receive( Address & sender, void * data, int size )
        {
            #if PLATFORM == PLATFORM_WIN
                typedef int socklen_t;      //if your gonna be 'fancy' with types, alteast use std::size_t...
            #endif
            socklen_t fromLength = sizeof(sockaddr_in);
            sockaddr_in pAddr;
            cout << "recieving data..." << endl;
            int received_bytes = recvfrom( handle, (char*)data, size, 0,  (sockaddr*)&pAddr, &fromLength);//this passed sender for the sockaddr...(the dangers of c-casts arise!)
            if(received_bytes == SOCKET_ERROR)
            {
                WSAErr();
                return -1;
            }
 
            sender = Address(pAddr.sin_addr.s_addr,pAddr.sin_port);//seeing as there are no set methods...
            return received_bytes;
        }
 
    private:
 
        SOCKET handle;
};

int client(Socket s)
{
    cout<<"CLIENT\n";
 
    const char d[] = "Welcome to forgotten dreams!";
	//char buffer[1024];
    Address a = Address(127,0,0,1,PORT);//127.0.0.1 is localhost (127.0.0.0 is the windows system gateway)
    s.Send(a, d, sizeof(d));
	//s.Receive(a,buffer,sizeof(buffer));
    return 0;
}
