#include <iostream>
#include <string>
#include <regex>
 
int main()
{
    std::string fnames[] = {"foo.txt", "bar.txt", "zoidberg"};
 
    std::regex txt_regex("[a-z]+\\.txt");
    for (const auto &fname : fnames) {
        std::cout << fname << ": " << std::regex_match(fname, txt_regex) << '\n';
    }
}
