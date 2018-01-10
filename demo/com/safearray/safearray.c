#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

/*
typedef struct tagSAFEARRAY {
USHORT         cDims;
USHORT         fFeatures;
ULONG          cbElements;
ULONG          cLocks;
PVOID          pvData;
SAFEARRAYBOUND rgsabound[1];
} SAFEARRAY, *LPSAFEARRAY;
*/

/*
typedef struct tagSAFEARRAYBOUND
{
ULONG cElements;
LONG lLbound;
} 	SAFEARRAYBOUND;

typedef struct tagSAFEARRAYBOUND *LPSAFEARRAYBOUND;
*/

#define NUM_ELEMENT 5

void test1()
{
	HRESULT hr;
	long index = 0;
	long i = 0;

	BYTE bVal[NUM_ELEMENT] = {'a', 'b', 'c', 'd', 'e'};

	VARIANT varChunk;
	SAFEARRAY *psa;
	SAFEARRAYBOUND rgsabound[1];

	{
		rgsabound[0].cElements = NUM_ELEMENT;
		rgsabound[0].lLbound = 0;
		psa = SafeArrayCreate(VT_UI1,1,rgsabound);

		for(index=0; index<NUM_ELEMENT; index++)
		{
			hr = SafeArrayPutElement(psa, &index, &bVal[index]);
			if(FAILED(hr))
			{
				printf("Error in SafeArrayPutElement\n");
			}
		}

		varChunk.vt = VT_ARRAY|VT_UI1;
		varChunk.parray = psa;

		{
			BYTE buf[NUM_ELEMENT];
			for(index=0;index<NUM_ELEMENT;index++)
			{
				SafeArrayGetElement(varChunk.parray, &index, buf+index);
				printf("%ld : %c\n", index, buf[index]);
			}
		}
	}


	{
		BYTE *buf;
		BSTR* bstr;
		long LBound;
		long UBound;

		/*  */
		SafeArrayAccessData(psa, (void **)&buf);
		for(index=0; index<NUM_ELEMENT; index++)
		{
			buf = bVal;
		}
		SafeArrayUnaccessData(psa);
		varChunk.vt = VT_ARRAY|VT_UI1;
		varChunk.parray = psa;

		/* */
		if(varChunk.vt = VT_ARRAY | VT_BSTR)
		{
			SafeArrayAccessData(varChunk.parray, (void **)&bstr);
			SafeArrayGetLBound(varChunk.parray, 1, &LBound);
			SafeArrayGetUBound(varChunk.parray, 1, &UBound);
			for(i = LBound; i < UBound; i ++)
			{
				/*  */
			}
			SafeArrayUnaccessData(varChunk.parray);
		}
	}
}

void test2()
{
	//����SAFEARRAY���飬ÿ��Ԫ��Ϊlong�ͣ���������һά����
	long nData[10]={1,2,3,4,5,6,7,8,9,10};
	long i = 0;
	long* pValue=NULL;
	long Low = 0;
	long High = 0;
	
	SAFEARRAY* pArray=NULL;
	HRESULT hr = SafeArrayAllocDescriptor(1,&pArray);//����SAFEARRAY�ṹ�Ķ���
	pArray->cbElements = sizeof(nData[0]);
	pArray->rgsabound[0].cElements=10;
	pArray->rgsabound[0].lLbound=0;
	pArray->pvData = nData;
	pArray->fFeatures = FADF_AUTO|FADF_FIXEDSIZE;//FADF_AUTOָ����ջ�Ϸ������ݣ����Ҵ�С�����Ըı䣨�̶�Ϊ10��

	//����SAFEARRAY����
	SafeArrayAccessData(pArray,(void**)&pValue);
	hr=SafeArrayGetLBound(pArray,1,&Low);//ά��������1��ʼ
	hr=SafeArrayGetUBound(pArray,1,&High);//ά��������1��ʼ

	printf("=============================\n");
	for (i=Low; i<High; ++i)
	{
		printf("%ld : %ld\n", i, pValue[i]);
	}
	
	SafeArrayUnaccessData(pArray);
	SafeArrayDestroy(pArray);
}

void test3()
{
	//����SAFEARRAY���飬ÿ��Ԫ��Ϊlong�ͣ���������һά����
	long nData[10]={1,2,3,4,5,6,7,8,9,10};
	long* pData=NULL;
	long l = 0, h = 0;
	long Size;
	long Idx;
	long* pValue=NULL;
	long Low = 0,High = 0;
	long i = 0;
	
	SAFEARRAY* pArray=NULL;
	HRESULT hr = SafeArrayAllocDescriptor(1,&pArray);//����SAFEARRAY�ṹ�Ķ���
	
	pArray->cbElements=sizeof(nData[0]);
	pArray->rgsabound[0].cElements=10;
	pArray->rgsabound[0].lLbound=0;
	SafeArrayAllocData(pArray);

	SafeArrayAccessData(pArray,(void**)&pData);
	SafeArrayGetLBound(pArray,1,&l);
	SafeArrayGetUBound(pArray,1,&h);
	Size=h-l+1;
	SafeArrayAccessData(pArray,(void**)&pData);
	for(Idx=l;Idx<Size;++Idx)
	{
		pData[Idx]=nData[Idx];
	}
	SafeArrayUnaccessData(pArray);

	//����SAFEARRAY����
	SafeArrayAccessData(pArray,(void**)&pValue);
	hr=SafeArrayGetLBound(pArray,1,&Low);//ά��������1��ʼ
	hr=SafeArrayGetUBound(pArray,1,&High);//ά��������1��ʼ
	
	printf("=============================\n");
	for (i=Low; i<High; ++i)
	{
		printf("%ld : %ld\n", i, pValue[i]);
	}
	
	SafeArrayUnaccessData(pArray);
	SafeArrayDestroy(pArray);
}

void test4()
{
     SAFEARRAY* pArray=NULL;

     HRESULT hr=SafeArrayAllocDescriptor(2,&pArray);

     pArray->rgsabound[0].lLbound=0;

     pArray->rgsabound[0].cElements=3;

     pArray->rgsabound[1].lLbound=0;

     pArray->rgsabound[1].cElements=3;

 

     pArray->cbElements=sizeof(long);

     hr=SafeArrayAllocData(pArray);

 

     long lDimension[2];

     long x=1;

     //Ϊ��һ�и�ֵ

     for(long i=0;i<3;++i)

     {

          lDimension[1]=0;//��

          lDimension[0]=i;//��

          SafeArrayPutElement(pArray,lDimension,&x);

         x++;

     }

     //Ϊ�ڶ��и�ֵ

     for(long i=0;i<3;++i)

     {

          lDimension[1]=1;//��

          lDimension[0]=i;//��

          SafeArrayPutElement(pArray,lDimension,&x);

         x++;

     }

    

     //��ȡSafeArray�еڶ��е����е�����

     long y(0);

     lDimension[1]=1;

     lDimension[0]=2;

     SafeArrayGetElement(pArray,lDimension,&y);

 

     SafeArrayDestroy(pArray);
}

void test5()
{
	SAFEARRAYBOUND Bound[1];
    Bound[0].lLbound=0;
    Bound[0].cElements=10;

    SAFEARRAY* pArray = SafeArrayCreate(VT_I4,1,Bound);

    long* pData=NULL;

     HRESULT hr = SafeArrayAccessData(pArray,(void**)&pData);

     long Low(0),High(0);

     SafeArrayGetLBound(pArray,1,&Low);

     SafeArrayGetUBound(pArray,1,&High);

     long Size=High-Low+1;

     for(long Idx=Low;Idx<Size;++Idx)

     {

          pData[Idx]=Idx;

          cout<<pData[Idx]<<endl;

     }

     SafeArrayUnaccessData(pArray);

     SafeArrayDestroy(pArray);
}

void test6()
{
	SAFEARRAYBOUND Bound[2];

     Bound[0].lLbound=0;

     Bound[0].cElements=3;

     Bound[1].lLbound=0;

     Bound[1].cElements=3;

     SAFEARRAY* pArray=SafeArrayCreate(VT_I4,2,Bound);

    

     long Demen[2];

     for(long i=0;i<3;++i)

     {

          for(long j=0;j<3;++j)

         {

              Demen[1]=i;

              Demen[0]=j;

              long x=i*j;

              SafeArrayPutElement(pArray,Demen,&x);

         }

     }

 

     //���ʶ�ά����

     for(long i=0;i<3;++i)

     {

          for(long j=0;j<3;++j)

         {

              Demen[1]=i;

              Demen[0]=j;

              long x(0);

              SafeArrayGetElement(pArray,Demen,&x);

               cout<<"("<<i<<","<<j<<")  "<<x<<endl;

         }

     }

     SafeArrayDestroy(pArray);
}

void test7()
{


}

int main()
{
	test1();
	
	test2();

	test3();
	
	return 0;
}

