#include <iostream>
#include <memory>
#include <algorithm>
#include <cstdint>

// this code won't compile!
// enum Color {RED, GREEN, BLUE}; 
//enum Feelings {EXCITED, MOODY, BLUE};

// this code will compile (if your compiler supports C++11 strongly typed enums)
enum class Color {RED, GREEN, BLUE}; 
enum class Feelings {EXCITED, MOODY, BLUE};

// forward declaration
enum class Mood;
 
void assessMood (Mood m)
{ }
  
// later on:
enum class Mood { EXCITED, MOODY, BLUE };

// we only have three colors, so no need for ints!
enum class Colors : char { RED = 1, GREEN = 2, BLUE = 3 };

enum class Colors2 : std::int8_t { RED = 1, GREEN = 2, BLUE = 3 };

int main()
{
	Color color = Color::GREEN;
	if ( Color::RED == color )
	{
		std::cout << "the color is red" << std::endl;
	}
	else if  ( Color::GREEN == color )
	{
		std::cout << "the color is green" << std::endl;
	}
	
	return 0;
}
