#include <windows.h>

#include <stdio.h>

int main()
{
    long x = -1000;

    VARIANT v;

    VARIANT e;

    VARIANT t;

	VARIANT vVal;

    v.vt = VT_I4;
    v.lVal = x;

    VarAbs(&v, &e);
    printf("%d --> %d\n", v.lVal, e.lVal);


    VarAdd(&v, &e, &t);
    printf("%d + %d --> %d\n", v.lVal, e.lVal, t.lVal);

    VarMul(&v, &e, &t);
    printf("%d * %d --> %d\n", v.lVal, e.lVal, t.lVal);

	
	V_VT(&vVal) = VT_I4;
	V_I4(&vVal) = 10;

	printf("value : %d\n", vVal.intVal);

    return 0;
}

