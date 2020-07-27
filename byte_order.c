#include <stdio.h>
 
#define BIG_ENDIAN 0
#define LITTLE_ENDIAN 1
 
int TestByteOrder() {
        short int word = 0x0001;
        char *b = (char *)&word;
        return (b[0] ? LITTLE_ENDIAN : BIG_ENDIAN);
}
 
int main() {
        int r = testbyteorder();
        printf("%s\n", r == little_endian ? "little endian" : "big endian");
        return r;
}
