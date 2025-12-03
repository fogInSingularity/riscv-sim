extern long read( int fd, char* data, long maxlen);
extern long write( int fd, const char* data, long len);
// extern __attribute__((noreturn)) void exit( long status);

#include <stdbool.h>

int atoi( const char* str)
{
    while ( ' ' == *str || '\t' == *str )
    {
        ++str;
    }
 
    bool negative = false;
 
    if ( *str == '+' )
    {
        ++str;
    } else if ( *str == '-' )
    {
        ++str;
        negative = true;
    }
 
    int result = 0;
    for (; '0' <= *str && *str <= '9'; ++str)
    {
        int digit = *str - '0';
        result *= 10;
        result -= digit; // calculate in negatives to support INT_MIN, LONG_MIN,..
    }
 
    return negative ? result : -result;
}

int main( int argc, char* argv[])
{
    if ( argc != 2 )
    {
        int* ptr = 0x0;
        *ptr = 666;
    }
    return atoi( argv[1]);
}
