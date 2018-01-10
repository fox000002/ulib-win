#include <iostream>

#include "adt/utable.h"
#include "adt/upair.h"

void print(int *p)
{
    std::cout << *p << std::endl;
}

int main()
{
    using std::cout;
    using std::endl;

    typedef huys::ADT::UTable<int, const char *> UTableIC;

    UTableIC ic;

    ic.add(1, "ccccc");
    ic.add(2, "ccccc");

    cout << ic << endl;

    if (ic.inTable(2))
    {
        cout << "2 is in table!" << endl;
    }
    
    cout << ic[2] << endl;
    
    ic.add(2, "aaaaa");
    
	typedef huys::ADT::UPair<int, long> MyPair;
    typedef huys::ADT::UTable<int, MyPair> UTriTable;
	
	UTriTable utt;
	
	utt.add(1, MyPair(1, 2));
	utt.add(2, MyPair(3, 24));
	
	const MyPair & v = utt[2];
	cout << v.first() << " " << v.second() << endl; 
	
	const MyPair & v2 = utt[1];
	cout << v2.first() << " " << v2.second() << endl; 
	
	utt.add(2, MyPair(3, 24));
	
    return 0;
}

