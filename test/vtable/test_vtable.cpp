#include <iostream>
#include <windows.h>

using std::cout;
using std::endl;

class B1
{
public:
    void f0()
    {
        cout << "B1 : " << this << " --- " << " f0" << endl;
    }
    virtual void f1() {}
    int int_in_b1;
};

class B2
{
public:
    virtual void f2() {}
    int int_in_b2;
};

class D : public B1, public B2
{
public:
    void d() {}
    void f2() {}  // override B2::f2()
    int int_in_d;
};

class Base {

public:

    virtual void f() { cout << "Base::f" << endl; }

    virtual void g() { cout << "Base::g" << endl; }

    virtual void h() { cout << "Base::h" << endl; }

};

typedef void(*Fun)();
typedef void(*XFun)(int,int,int); 

class BaseClass
{
public:
    BaseClass()
    {
        test();
    }

    virtual void fun1()
    {
        cout << " fun1 " << endl;
    }

    virtual void fun2(int x ,int y)
    {
        cout <<"x:"<< x <<" y:"<<y<< " fun2 " << endl;
    }

    void test()
    {
        //得到vtable里第一个函数地址（fun1）
        Fun fun = (Fun)*(int*)*(int*)this;
        fun();

        //得到vtable里第二个函数地址（fun2）
        XFun xfun = (XFun)*((int*)*(int*)this + 1);
        xfun(1,2,3);//
    }
};

class subclass:public BaseClass
{
public:
    virtual void fun1()
    {
        cout << "virtual void subclass:fun1()!\n" << endl;
    }
};


int v_test()
{
    subclass subclassobj ;
    cout<<"*************test suclass.test()*****************************"<<endl;
    subclassobj.test();
    cout<<"*************&subclass::test*****************************"<<endl;
    Fun *testFun = (Fun*)(*(int *)(&subclassobj));
    cout << testFun << endl;
    (*testFun)();//这里出现段错误,
    
    void * p = (int *)&subclassobj + 1;
    cout << p << endl;
    cout << &BaseClass::test << endl;
    testFun = (Fun*)p;
    (*testFun)();//这里出现段错误,
    
    return 0;
}

namespace ComHook
{
/*	VTable
	 ______
	|f1
	|______
	|f2
	|______
	|f2
	|______
*/



	class A
	{
	public:
		A(){}
		~A(){}
		virtual void f1() { cout << "Founction f1 called" << endl; } 
		virtual void f2() { cout << "Founction f2 called" << endl; }
		virtual void f3() { cout << "Founction f3 called" << endl; }
		void reset_f1();
		void reset_f2();
	private:
		int n;
	};

	void A::reset_f1()
	{
		long** pplVrtable= (long**)(this); //取得虚函数表的指针
		*pplVrtable = *pplVrtable +1;//将虚函数表的指针指向虚函数表第二个值。
	}
	
	void A::reset_f2()
	{
		long** pplVrtable= (long**)(this);

		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ::GetCurrentProcessId());

		MEMORY_BASIC_INFORMATION mbi = {0};

		if (VirtualQueryEx(hProcess, (LPVOID)(*pplVrtable), &mbi, sizeof(mbi)) != sizeof(mbi))
			return;

		DWORD dwOldProtect = 0;
		if(!::VirtualProtectEx(hProcess, mbi.BaseAddress, 4, PAGE_EXECUTE_READWRITE, &dwOldProtect)) 
			return;

		(*pplVrtable)[1] = (*pplVrtable)[2];


		DWORD dwTemp = 0;
		::VirtualProtectEx(hProcess, mbi.BaseAddress, 4, dwOldProtect, &dwTemp);
		CloseHandle(hProcess);
	}

	void test()
	{
		A* pA = new A;
		pA->reset_f1();

		cout << "Begin to call founction f1.\n" << endl;

		pA->f1();
		
		A* pA2 = new A;
		
		pA2->reset_f2();
		pA2->reset_f1();
		
		cout << "Begin to call founction f2.\n" << endl;
		
		pA2->f1();

		delete pA;
		delete pA2;
	}
}


int main()
{
    cout << "======= Virtual Method Table Testing ==================" << endl;

    B2 *b2 = new B2();
    D  *d  = new D();

    cout << "sizeof(B2) : " << sizeof(B2) << endl;
    cout << "B2: " << b2 << endl;
    cout << "int_in_b2 : +" << (char *)(&b2->int_in_b2) - (char *)b2  << endl;
    cout << "B2::f2() : " << &B2::f2 << endl;

    cout << "--------------------------------------------" << endl;
    cout << "sizeof(D) : " << sizeof(D) << endl;
    cout << "D : " << d << endl;
    cout << "dynamic_cast<B1*>(d) : " << dynamic_cast<B1*>(d) << endl;
    cout << "dynamic_cast<B2*>(d) : " << dynamic_cast<B2*>(d) << endl;
    cout << "d.int_in_b1 : +" << (char *)(&d->int_in_b1) - (char *)d  << endl;
    cout << "d.int_in_b2 : +" << (char *)(&d->int_in_b2) - (char *)d  << endl;
    cout << "d.int_in_d: +" << (char *)(&d->int_in_d) - (char *)d  << endl;
    cout << "D::d() : " << &D::d << endl;
    cout << "D::f1() : " << &D::f1 << endl;
    cout << "D::f2() : " << &D::f2 << endl;

    delete b2;
    delete d;

    cout << "--------------------------------------------" << endl;
    typedef void(*Fun)(void);
    Base b;
    Fun *ptr=(Fun*&)b;//
    cout << "sizeof(Base) : " << sizeof(Base) << endl; //
    cout << "VTable address: " << (int *)&b << endl;
    cout << (int *)(*(int *)(&b)) << endl;//
    cout << ptr << endl;//
    cout << &Base::f << endl;
    cout << ptr[0] <<endl;//
    ptr[0]();//
    
    cout << "--------------------------------------------" << endl;
    //v_test();
	
	cout << "--------------------------------------------" << endl;
	ComHook::test();
	

    return 0;
}

