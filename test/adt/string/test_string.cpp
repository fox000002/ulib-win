#include <iostream>
#include <vector>
#include <wchar.h>

#include "adt/ustring.h"
#include "adt/uvector.h"

typedef huys::ADT::UVector<huys::ADT::UStringAnsi> StringVector;

huys::ADT::UStringAnsi test_unicode(const char* buf)
{
    std::vector<char> unicode(10);
    unicode[0] = '1';
    unicode[1] = '2';
    unicode[2] = '\0';

    return &unicode[0];
}

class CA
{
public:
    CA()
    {
        std::cout << "CA()!" << std::endl;
        p = new char[2];
    }

    ~CA()
    {
        std::cout << "~CA()!" << std::endl;
        delete[] p;
    }

    char *p;
};

class CB
{
public:
    CB(CA *)
    {
      std::cout << "CB()!" << std::endl;
    }
    ~CB()
    {
        std::cout << "~CB()!" << std::endl;
    }
};

char * createCB()
{
    CA a;
    return a.p;
}

void use_wchar()
{
   wprintf (L"Characters: %lc %lc \n", L'a', 65);
   wprintf (L"Decimals: %d %ld\n", 1977, 650000L);
   wprintf (L"Preceding with blanks: %10d \n", 1977);
   wprintf (L"Preceding with zeros: %010d \n", 1977);
   wprintf (L"Some different radixes: %d %x %o %#x %#o \n", 100, 100, 100, 100, 100);
   wprintf (L"floats: %4.2f %+.0e %E \n", 3.1416, 3.1416, 3.1416);
   wprintf (L"Width trick: %*d \n", 5, 10);
   wprintf (L"%ls \n", L"A wide string");
}

int main()
{
    using std::cout;
    using std::endl;


    typedef huys::ADT::UStringAnsi UString;

    cout << "=========================" << endl;

    UString s("1111");

    cout << s.c_str() << endl;

    cout << s.substr(1, 2).c_str() << endl;

    UString s2("2222");

    cout << s2.length() << endl;

    s2 += s;

    cout << s2.length() << endl;

    cout << s2.c_str() << endl;

    cout << (s2+"3333").c_str() << endl;

    cout << (s2<<"4444").c_str() << endl;

    cout << s2.reverse().c_str() << endl;

    cout << "=========================" << endl;

    cout << test_unicode(NULL).c_str() << endl;

    cout << "BEGIN calling createCB!" << endl;

    createCB();

    cout << "End calling createCB!" << endl;


    StringVector v;

    v.push_back("hello");

    cout << "v's size : " << v.size() << endl;

    UString str = "123456";

    if (-1 != str.findString("456", 3))
    {
        cout << "find '456' in " << str.c_str() << endl;
    }

    if (str.compare_length("123321", 3))
    {
        cout << "equal" << endl;
    }

    use_wchar();

    return 0;
}
