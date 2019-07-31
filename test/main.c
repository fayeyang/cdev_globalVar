#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main( int argc, char argv[] ){

    char cBuf[100];
    int fd;
    
    fd = 0;
    fd = open( "/dev/globalMem_device_0", O_RDWR, S_IRUSR | S_IWUSR );
    if( fd <=0 ){
        perror( "open file error!\n" );
        return -1;
    }

    memset( cBuf, 0, sizeof(cBuf) );
    read( fd, cBuf, sizeof(cBuf) );
    printf( "in /dev/globalMem_device_0:\n%s\n", cBuf );

    ioctl( fd, 0, 0 );

    memset( cBuf, 0, sizeof(cBuf) );
    printf( "please input your string:\n" );
    scanf( "%s", cBuf );
    write( fd, cBuf, (strlen(cBuf)) );

    memset( cBuf, 0, sizeof(cBuf) );
    read( fd, cBuf, sizeof(cBuf) );
    printf( "in /dev/globalMem_device_0:\n%s\n", cBuf );

    close( fd );

    return 0;
}
