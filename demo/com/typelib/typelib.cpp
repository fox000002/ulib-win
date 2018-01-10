#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#include "com/ucom.h"

int main()
{
	try
	{
		huys::UCOM::TypeLibrary tlib(L"ProRes_ENG.dll");

		if (tlib.IsOk())
		{
			printf("TypeLibrary is Ok!\n");
		}
		else
		{
			printf("TypeLibrary is bad!\n");
		}
	}
	catch (...)
	{
		printf("Exception...");
	}
	
	return 0;
}
