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
    fd = open( "/dev/globalMem_device", O_RDWR, S_IRUSR | S_IWUSR );
    if( fd <=0 ){
        perror( "open file error!\n" );
        return -1;
    }

    memset( cBuf, 0, sizeof(cBuf) );
    read( fd, cBuf, sizeof(cBuf) );
    printf( "in /dev/globalMem_device:\n%s\n", cBuf );

    memset( cBuf, 0, sizeof(cBuf) );
    printf( "please input your string:\n" );
    scanf( "%s", cBuf );
    write( fd, cBuf, (strlen(cBuf)) );

    lseek( fd, 123, SEEK_END );

    /*
     * 通过ioctl()方法设置本字符设备的当前读写位置，最终会调用cdev对象文件操作表中的
     * unlocked_ioctl()函数（cdev.ops->unlocked_ioctl()字段） */
    ioctl( fd, 0, 0 );
    memset( cBuf, 0, sizeof(cBuf) );
    read( fd, cBuf, sizeof(cBuf) );
    printf( "in /dev/globalMem_device:\n%s\n", cBuf );

    close( fd );

    return 0;
}
