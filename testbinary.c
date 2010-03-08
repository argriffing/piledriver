#include <stdio.h>

int main(int argc, char *argv[])
{
  int i;
  for (i=0; i<256; i++)
  {
    unsigned char c = (unsigned char) i;
    fwrite(&c, 1, 1, stdout);
  }
}
