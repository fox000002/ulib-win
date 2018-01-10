#include <vector> // the general-purpose vector container
#include <algorithm> // remove and remove_if
 
bool is_odd(int i) { // unary predicate returning true if and only if the argument is odd
  return i % 2;  
}
bool is_even(int i) { // unary predicate returning true if and only if the argument is even
  return !is_odd(i);
}
 
int main()
{
  using namespace std;
  int elements[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  // create a vector that holds the numbers from 0-9.
  vector<int> v(elements, elements + 10); 
 
  // use the erase-remove idiom to remove all elements with the value 5
  v.erase(remove(v.begin(), v.end(), 5), v.end()); 
 
  // use the erase-remove idiom to remove all odd numbers
  v.erase( remove_if(v.begin(), v.end(), is_odd), v.end() );
 
  // use the erase-remove idiom to remove all even numbers
  v.erase( remove_if(v.begin(), v.end(), is_even), v.end() ); 
  
  return 0;
}

