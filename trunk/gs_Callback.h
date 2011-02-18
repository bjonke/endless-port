#include <cstdlib>
#include <vector>
#include <iostream>

typedef void (*voidfunction)(void);

using namespace std;
class gs_Callback
{
private :
vector< voidfunction> list;

public :
void registerMethod(voidfunction toadd)
{
list.push_back(toadd);
}
void callAllMethods()
{
for ( unsigned int i = 0; i < list.size(); i++)
list[i]();
}
};