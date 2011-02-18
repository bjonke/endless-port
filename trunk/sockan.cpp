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
 
int main()
{
}