/*
* =====================================================================================
*
*       Filename:  test_static_assert.cpp
*
*    Description:  
*
*        Version:  1.0
*        Created:  2013/8/6 14:55:22
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  huys (hys), huys03@gmail.com
*        Company:  hu
*
* =====================================================================================
*/
#include <type_traits>
#include <iostream>
using namespace std;
struct A { }; 
struct B { virtual void f(){} }; 
struct C : B {}; 
int main()
{
	cout << "int:" << has_virtual_destructor<int>::value << endl;
	cout << "int:"<< is_polymorphic<int>::value << endl; 
	cout << "A: " << is_polymorphic<A>::value << endl;
	cout << "B: " << is_polymorphic<B>::value << endl;
	cout << "C: " << is_polymorphic<C>::value << endl;
	typedef int mytype[][24][60];
	cout << "(0 dim.): " << extent<mytype,0>::value << endl;
	cout << "(1 dim.): " << extent<mytype,1>::value << endl;
	cout << "(2 dim.): " << extent<mytype,2>::value << endl;
	return 0;
}

