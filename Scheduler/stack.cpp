#include <iostream>

// this is the base class of objects

using namespace std;

class GObj {
   static int counter; // static class counter telling me how many objects were created
protected: 
   int id; // unique ID for this object
public:
   GObj() { id = counter++; cout << "Created<" << id << "> " << this << endl; }
   ~GObj() { cout << "Destroy<" << id << "> " << this << endl; }
};

int GObj::counter = 0;

int main(void)
{
     for (int i=0; i<3; i++) {
         GObj gobj1;
         GObj gobj2;
	 cout << endl;
     }
     return 0;
}


