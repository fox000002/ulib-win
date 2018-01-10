#include <iostream>
#include <algorithm>
#include <vector>
#include <string>

using namespace std;

class AddressBook
{
public:
	// using a template allows us to ignore the differences between functors, function pointers 
	// and lambda
	template<typename Func>
	std::vector<std::string> findMatchingAddresses (Func func)
	{ 
		std::vector<std::string> results;
		for ( auto itr = _addresses.begin(), end = _addresses.end(); itr != end; ++itr )
		{
			// call the function passed into findMatchingAddresses and see if it matches
			if ( func( *itr ) )
			{
				results.push_back( *itr );
			}
		}
		return results;
	}
	
	AddressBook()
	{
		_addresses.push_back("kate.org");
		_addresses.push_back("John.com");
	}

private:
	std::vector<std::string> _addresses;
};

AddressBook global_address_book;

vector<string> findAddressesFromOrgs ()
{
	return global_address_book.findMatchingAddresses( 
		// we're declaring a lambda here; the [] signals the start
		[] (const string& addr) { return addr.find( ".org" ) != string::npos; } 
	);
}

vector<string> findAddressesForName(string name)
{
	return global_address_book.findMatchingAddresses( 
	    //	notice that the lambda function uses the the variable 'name'
		[&] (const string& addr) { return addr.find( name ) != string::npos; } 
	);
}

int main()
{
	{
		auto func = [] () { cout << "Hello world" << endl; };
		func(); // now call the function
	}

	{
		char s[]="Hello World!";
		int Uppercase = 0; //modified by the lambda
		for_each(s, s+sizeof(s), [&Uppercase] (char c) {
			if (isupper(c))
			Uppercase++;
		});
		cout<< Uppercase <<" uppercase letters in: " << s <<endl;
		
	}
	
	{
		vector<string> && results = findAddressesFromOrgs();
		std::for_each(std::begin(results), std::end(results), [](string addr) {std::cout << addr << std::endl;});
	}
	
	{
		vector<string> && results = findAddressesForName("John");
		std::for_each(std::begin(results), std::end(results), [](string addr) {std::cout << addr << std::endl;});
	}	
}
