#include <iostream>
#include <functional>

using namespace std;
using namespace std::placeholders;

float div(float a, float b)
{ 
	return a/b;
}

int main()
{

	cout << "6/1 = " << div(6,1) << endl;
	cout << "6/2 = " << div(6,2) << endl;
	cout << "6/3 = " << div(6,3) << endl;
	function<float(float, float)> inv_div = bind(div, _2, _1);
	cout << "1/6 = " << inv_div(6,1) << endl;
	cout << "2/6 = " << inv_div(6,2) << endl;
	cout << "3/6 = " << inv_div(6,3) << endl;
	function<float(float)> div_by_6 = bind(div, _1, 6);
	cout << "1/6 = " << div_by_6 (1) << endl;
	cout << "2/6 = " << div_by_6 (2) << endl;
	cout << "3/6 = " << div_by_6 (3) << endl;

	return 0;
}
