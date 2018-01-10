#include <cstdio>

#include "crypto/sha.h"

/*
  "1" 0x31

  origin : 00110001
  encode : 00001100 00010000 00111101 00111101
  decode : 00110001 00000000 00000000
 */

int main()
{
    const char * test = "abc";
    //char test[] = "1";
    //char dest[1024] = {0};
    //char str[1024] = {0};
    //int i;

    typedef huys::crypto::SHA_1 SHA_1;
    typedef unsigned char uchar;

    SHA_1 engine;
	engine.digest(test);
	
    printf("src %d: %s \n", strlen(test), test);

    printf("out : \n\t%X %X %X %X %X \n", 
		engine.md(0), engine.md(1), engine.md(2), engine.md(3), engine.md(4));
	printf("Should match:\n");
    printf("\tA9993E36 4706816A BA3E2571 7850C26C 9CD0D89D\n");

    return 0;
}
