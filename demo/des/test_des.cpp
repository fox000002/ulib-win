#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tchar.h>

#include "ufile.h"

#include "crypto/des.h"

using huys::crypto::des;
using huys::crypto::triple_des;

static FILE *input;
static FILE *output;

const unsigned char ENCRYPTION_SIGNATURE[16] =
{
  85, 66, 67, 127, 128, 248, 92, 152, 15, 252, 175, 38, 158, 218, 22, 141
};

const char HELP[] =
  "\n"
  "des  password  input  output\n"
  "\n"
  "This utility encrypts or decrypts the input file and writes the result\n"
  "to the output file.\n"
  "\n"
  "The password may contain any characters appropriate for a command-line\n"
  "argument.\n"
  "\n"
  "For example, the following command lines encrypt and decrypt a file:\n"
  "\n"
  "   des password  message1.txt  message.cry\n"
  "   des password  message.cry  message2.txt\n"
  "\n"
  "The files message1.txt and message2.txt will be identical, and the file\n"
  "message.cry will be thoroughly scrambled.\n"
  "\n"
  "The algorithm used is DES with a 56-bit key derived from the password.";

static void error_exit(const char *format, ...)
{
  fprintf(stderr, "\ndes: ");
  vfprintf(stderr, format, (char *)format + 1);
  fprintf(stderr, "\n");
  exit(1);
}

static int get_byte(void)
{
  int c = getc(input);
  if (c == EOF)
    error_exit("Unexpected end of file");
  return c;
}

int main(int argc, char **argv)
{
  if (argc < 4)
  {
    puts(HELP);
    return 1;
  }

  #ifdef TRIPLEDES
    triple_des crypto;
  #else
    des crypto;
  #endif
  // get key
  crypto.password(argv[1]);

  // open input and output files
  input = fopen(argv[2], "rb");
  if (input == NULL)
    error_exit("Can't open %s", argv[2]);
  output = fopen(argv[3], "wb");
  if (output == NULL)
    error_exit("Can't open %s", argv[3]);

  // get input file length
  long size;
  {
    //struct stat s;
    //fstat(fileno(input), &s);
    //size = s.st_size;
	UCFile f(argv[2]);
	size = f.size();
  }

  // read first 16 bytes of input file and determine whether it is an
  // encryption signature
  int /* boolean */ encrypt;
  {
    unsigned char buffer[sizeof(ENCRYPTION_SIGNATURE)];
    if (fread(buffer, sizeof(buffer), 1, input) != 1 ||
      memcmp(buffer, ENCRYPTION_SIGNATURE, sizeof(buffer)) != 0)
    {
      rewind(input);
      // write encryption signature to output
      fwrite(ENCRYPTION_SIGNATURE, sizeof(ENCRYPTION_SIGNATURE), 1, output);
      // write number of padding bytes to output file
      putc( - (int) size & 7, output);
      encrypt = 1 /* true */;
    }
    else
    {
      if (((int) size & 7) != 1)
        error_exit("Size error in input file");
      size -= sizeof(ENCRYPTION_SIGNATURE) + 1 + get_byte();
      encrypt = 0 /* false */;
    }
  }

  while (size > 0)
  {
    int count;
    if (size < 8)
      count = size;
    else
      count = 8;
    size -= count;
    static unsigned char buffer[64];
    int i;
    if (encrypt)
    {
      i = 0;
      while (count-- != 0)
      {
        int c = get_byte();
        buffer[i++] = c >> 7 & 1;
        buffer[i++] = c >> 6 & 1;
        buffer[i++] = c >> 5 & 1;
        buffer[i++] = c >> 4 & 1;
        buffer[i++] = c >> 3 & 1;
        buffer[i++] = c >> 2 & 1;
        buffer[i++] = c >> 1 & 1;
        buffer[i++] = c & 1;
      }
      while (i < 64)
        buffer[i++] = 0;
      crypto.encrypt(buffer);
      for (i = 0; i < 64; i += 8)
      {
        putc(buffer[i] << 7 |
          buffer[i+1] << 6 |
          buffer[i+2] << 5 |
          buffer[i+3] << 4 |
          buffer[i+4] << 3 |
          buffer[i+5] << 2 |
          buffer[i+6] << 1 |
          buffer[i+7], output);
      }
    }
    else
    {
      for (i = 0; i < 64; i+= 8)
      {
        int c = get_byte();
        buffer[i] = c >> 7 & 1;
        buffer[i+1] = c >> 6 & 1;
        buffer[i+2] = c >> 5 & 1;
        buffer[i+3] = c >> 4 & 1;
        buffer[i+4] = c >> 3 & 1;
        buffer[i+5] = c >> 2 & 1;
        buffer[i+6] = c >> 1 & 1;
        buffer[i+7] = c & 1;
      }
      crypto.decrypt(buffer);
      for (i = 0; count-- != 0; i += 8)
      {
        putc(buffer[i] << 7 |
          buffer[i+1] << 6 |
          buffer[i+2] << 5 |
          buffer[i+3] << 4 |
          buffer[i+4] << 3 |
          buffer[i+5] << 2 |
          buffer[i+6] << 1 |
          buffer[i+7], output);
      }
    }
  }
  fclose(input);
  fclose(output);
  return 0;
}