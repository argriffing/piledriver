#include <stdio.h>
#include <stdint.h>

int test_binary_out()
{
  int i;
  for (i=0; i<256; i++)
  {
    unsigned char c = (unsigned char) i;
    fwrite(&c, 1, 1, stdout);
  }
}

int test_sizeof_array(uint16_t arr[])
{
  uint16_t d[5] = {0,0,0,0,0};
  printf("%zd\n", sizeof(d));
  test_sizeof_array(d);
  printf("%zd\n", sizeof(arr));
  return 0;
}

int main(int argc, char *argv[])
{
  FILE *fin = fopen("../ohai.txt", "rt");
  char buf[1000];
  while (fgets(buf, 1000, fin) != NULL)
  {
    printf("%s\n", buf);
  }
  fclose(fin);
}
