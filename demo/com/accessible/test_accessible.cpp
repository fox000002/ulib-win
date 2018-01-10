#include "resource.h"

#include <windows.h>
#include <tchar.h>
#include <ocidl.h>
#include <olectl.h>
#include <oleacc.h>
#include <stdio.h>

#ifndef OBJID_WINDOW
#define     OBJID_WINDOW        ((LONG)0x00000000)
#endif

#ifndef CHILDID_SELF
#define     CHILDID_SELF        0
#endif

HRESULT PrintName(IAccessible* pAcc, long childId)
{
    if (pAcc == NULL)
    {
        return E_INVALIDARG;
    }
    BSTR bstrName;
    VARIANT varChild;
    varChild.vt = VT_I4;
    varChild.lVal = childId;
    HRESULT hr = pAcc->get_accName(varChild, &bstrName);
    _tprintf(_T("Name: %S \n"), bstrName);
    SysFreeString(bstrName);
    return hr;
}

HRESULT PrintDescription(IAccessible* pAcc, long childId)
{
    if (pAcc == NULL)
    {
        return E_INVALIDARG;
    }
    BSTR bstrDescription;
    VARIANT varChild;
    varChild.vt = VT_I4;
    varChild.lVal = childId;
    HRESULT hr = pAcc->get_accDescription(varChild, &bstrDescription);
    _tprintf(_T("Description: %S \n"), bstrDescription);
    SysFreeString(bstrDescription);
    return hr;
}

HRESULT PrintValue(IAccessible* pAcc, long childId)
{
    if (pAcc == NULL)
    {
        return E_INVALIDARG;
    }
    BSTR bstrValue;
    VARIANT varChild;
    varChild.vt = VT_I4;
    varChild.lVal = childId;
    HRESULT hr = pAcc->get_accValue(varChild, &bstrValue);
    _tprintf(_T("Value: %S \n"), bstrValue);
    SysFreeString(bstrValue);
    return hr;
}

HRESULT PrintState(IAccessible* pAcc, long childId)
{
    if (pAcc == NULL)
    {
        return E_INVALIDARG;
    }
    VARIANT varChild;
    varChild.vt = VT_I4;
    varChild.lVal = childId;
    VARIANT varState;
    varState.vt = VT_I4;
    varState.lVal = 0;

    HRESULT hr = pAcc->get_accState(varChild, &varState);

    if (FAILED(hr))
    {
        _tprintf(_T("Failed to get_accState.\n"));


        switch (hr)
        {
        case E_INVALIDARG:
            _tprintf(_T("Error : E_INVALIDARG\n"));
            break;
        default:
            break;
        }

        return S_FALSE;
    }

    _tprintf(_T("Value: %ld \n"), varState.lVal);
    return hr;
}

int main()
{
    HWND hwnd = ::FindWindow(_T("CalcFrame"), _T("Calculator"));

    if (!hwnd)
    {
        _tprintf(_T("Failed to find window : %s.\n"), _T("Calculator"));
        return -1;
    }

    IAccessible *paccMainWindow = NULL;
    HRESULT hr = AccessibleObjectFromWindow(hwnd, OBJID_WINDOW, IID_IAccessible, (void**)&paccMainWindow);

    if (FAILED(hr))
    {
        _tprintf(_T("Failed to AccessibleObjectFromWindow.\n"));
        return -1;
    }

    long count = 0;
    paccMainWindow->get_accChildCount(&count);

    _tprintf(_T("Child Count : %ld\n"), count);

    IAccessible *paccChildWindow = NULL;

    VARIANT var_child;
    var_child.vt = VT_I4;
    var_child.lVal = CHILDID_SELF;

    hr = paccMainWindow->get_accChild(var_child, (IDispatch **)&paccChildWindow);

    if (FAILED(hr))
    {
        _tprintf(_T("Failed to get_accChild.\n"));
        return -1;
    }

    PrintName(paccChildWindow, CHILDID_SELF);

	//for (long i=1; i<=count; ++i)
	//{
		var_child.lVal = 0x0006064C;
		hr = paccMainWindow->get_accChild(var_child, (IDispatch **)&paccChildWindow);
		if (FAILED(hr))
		{
			_tprintf(_T("Failed to get_accChild.\n"));
		}
		else
		{
			PrintName(paccChildWindow, CHILDID_SELF);
		}
	//}
	
	
    for (long i=1; i<=count; ++i)
    {
        PrintName(paccMainWindow, i);
    }

    for (long i=1; i<=count; ++i)
    {
        PrintDescription(paccMainWindow, i);
    }

    for (long i=1; i<=count; ++i)
    {
        PrintValue(paccMainWindow, i);
    }

    for (long i=1; i<=count; ++i)
    {
        PrintState(paccMainWindow, i);
    }

    return 0;
}
