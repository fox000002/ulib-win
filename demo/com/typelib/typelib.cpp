#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#include "com/ucom.h"

int main()
{
	try
	{
		huys::UCOM::TypeLibrary tlib(L"Sterling.tlb");

		if (tlib.IsOk())
		{
			printf("TypeLibrary is Ok!\n");
		}
		else
		{
			printf("TypeLibrary is bad!\n");
		}
		
		printf("%d\n", tlib.GetCount());
		
		huys::UCOM::BString name, doc;
		
		for (int i=0; i<tlib.GetCount(); ++i)
		{
			printf("=============%d==============\n", i+1);
			
			tlib.GetDocumentation(i, name, doc);
		
			huys::UCOM::CString cname(name);
			huys::UCOM::CString cdoc(doc);
		
			printf("name : %s\n", cname.str());
			printf("doc  : %s\n", cdoc.str());
		}
		
		huys::UCOM::TypeInfo ti(tlib, 33);
		
		ti.GetDocumentation(name, doc);
		huys::UCOM::CString cname(name);
		huys::UCOM::CString cdoc(doc);
		
		printf("=============***==============\n");	
		printf("name : %s\n", cname.str());
		printf("doc  : %s\n", cdoc.str());
	}
	catch (...)
	{
		printf("Exception...");
	}
	
	return 0;
}
