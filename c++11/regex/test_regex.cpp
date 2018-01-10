#include <iostream>
#include <string>
//#include <regex>
#include <boost/regex.hpp>

int main()
{
    std::string fnames[] = {"foo.txt", "bar.txt", "zoidberg"};
 
    boost::regex txt_regex("[a-z]+\\.txt");
    for (const auto &fname : fnames)
	{
        std::cout << fname << ": " << boost::regex_match(fname, txt_regex) << '\n';
    }
}
