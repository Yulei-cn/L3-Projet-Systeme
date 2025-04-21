#include <stdio.h>      // printf, scanf
#include <stdlib.h>     // exit, malloc
#include <string.h>     // memset, strcpy, strlen
#include <unistd.h>     // read, write, close, sleep
#include <sys/types.h>  // 基本系统类型
#include <sys/socket.h> // socket, connect, write
#include <sys/un.h>     // sockaddr_un 本地套接字结构

#define BUFFER_SIZE 1024   // 定义缓冲区大小为1024字节
