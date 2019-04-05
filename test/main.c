#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

int main( int argc, char argv[] ){

    char cBuf[100];
    int fd;
    
    fd = 0;
    fd = open( "/dev/globalVar", O_RDWR, S_IRUSR | S_IWUSR );
    if( fd <=0 ){
        perror( "in open func" );
        return -1;
    }

    memset( cBuf, 0, sizeof(cBuf) );
    read( fd, cBuf, 100 );
    printf( "in /dev/globalVar:\n%s\n", cBuf );

    memset( cBuf, 0, sizeof(cBuf) );
    printf( "please input your string:\n" );
    scanf( "%s", cBuf );
    write( fd, cBuf, (strlen(cBuf)) );

    memset( cBuf, 0, sizeof(cBuf) );
    read( fd, cBuf, 100 );
    printf( "in /dev/globalVar:\n%s\n", cBuf );

    close( fd );

    return 0;
}
