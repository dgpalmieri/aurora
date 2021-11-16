/*      File: test_atomics.cpp */

#include <iostream>
#include <limits>
#include <climits>
#include "atomics.hpp"

/*-------------------------------------------------------------------*/
/* https://stackoverflow.com/a/13981171 */

uLong subtract(uByte4 S1, uLong S2)
{
  uLong S;
  S = S2 - S1;
  return S;
}

/*-------------------------------------------------------------------*/
/* https://stackoverflow.com/a/3618674 */
/* https://stackoverflow.com/a/589684 */

/* int main( int argc, char * argv[] ) */
int main()
{

  const int min_int = std::numeric_limits<int>::min();
  const int max_int = std::numeric_limits<int>::max();

  std::cout << INT_MAX << " minus " << std::numeric_limits<short>::max() << " is " \
      << subtract(std::numeric_limits<short>::max(), INT_MAX) << std::endl; 
  std::cout << "minimum integer is: " << min_int << std::endl;
  std::cout << "maximum integer is: " << max_int << std::endl;

  std::cout<< "size of short: " << sizeof (short) << std::endl;
  std::cout<< "size of int: " << sizeof (int) << std::endl;
  std::cout<< "size of long: " << sizeof (long) << std::endl;
  std::cout<< "size of long long: " << sizeof (long long) << std::endl;
  std::cout<< "size of uLong: " << sizeof (uLong) << std::endl;
  std::cout<< "size of uByte1: " << sizeof (uByte1) << std::endl;
  std::cout<< "size of uByte4: " << sizeof (uByte4) << std::endl;
  std::cout<< "size of ubyte8: " << sizeof (uByte8) << std::endl;
  std::cout<< "intensity number is " << INTEN_NUM << std::endl;
  std::cout<< "intensity bits are " << INTEN_BIT << std::endl;

  return 1;

}
