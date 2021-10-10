// https://www.geekpage.jp/programming/linux-network/select.php
// https://linuxprograms.wordpress.com/2008/01/23/piping-in-threads/
// https://linuxprograms.wordpress.com/2008/01/23/piping-in-threads/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <assert.h>
#include <sys/time.h> 
#include <string.h>
#include <signal.h> 

#define BUF_SIZE 2048

static pthread_key_t tidKey;
fd_set fds, readfds;
int maxfd;

int fd[2];

void
sigusr1_handler(int sig)
{
  printf("signal called\n");
}

void udp_server() {
    int sock;
    struct sockaddr_in addr;
    char buf[2048];

    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = INADDR_ANY;


    sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    bind(sock, (struct sockaddr *)&addr, sizeof(addr));

    /* fd_setの初期化します */
    FD_ZERO(&readfds);

     /* selectで待つ読み込みソケットとしてsock1を登録します */
    FD_SET(sock, &readfds);

    // For pipe.
    FD_SET(fd[0], &readfds);

     /* selectで監視するファイルディスクリプタの最大値を計算します */
    if (sock > fd[0]) {
        maxfd = sock;
    } else {
        maxfd = fd[0];
    }

    while(1){
        memcpy(&fds, &readfds, sizeof(fd_set));
        select(maxfd+1, &fds, NULL, NULL, NULL);
        
        /* sock1に読み込み可能データがある場合 */
        if (FD_ISSET(sock, &fds)) {
            /* sock1からデータを受信して表示します */
            memset(buf, 0, sizeof(buf));
            recv(sock, buf, sizeof(buf), 0);
            printf("%s\n", buf);
        }

        /* sock2に読み込み可能データがある場合 */
        if (FD_ISSET(fd[0], &fds)) {
            /* sock2からデータを受信して表示します */
            // memset(buf, 0, sizeof(buf));
            // recv(fd[0], buf, sizeof(buf), 0);
            char ch;
            int result = read(fd[0],&ch,1);
            printf("got char:%c\n", ch);
            break;
        }
    }

    close(sock);
    printf("Socket closed.\n");
}

int main(){
    // Create thread.
    int s;
    void *res;
    pthread_t udp_server_thread;
    s = pthread_create(&udp_server_thread, NULL, (void *)udp_server, NULL);
    if(s != 0) {
        printf("failed to create thread\n");
        exit(-1);
    }

    int result = pipe (fd);
    if (result < 0){
        perror("pipe ");
        exit(1);
    }

    // Wait keyboard input.
    char ch;
    while(1){
        printf("q terminates main and udp thread:");
        ch = getchar();
        if(ch == 'q')    // quit inputted.
        {
            // s = pthread_cancel(&udp_server_thread);
            // s = pthread_kill(&udp_server_thread, SIGUSR1);
            // if (s != 0){
            //    printf("Cancelation error");
            // } else {
            //     break;
            // }
            result = write(fd[1], &ch,1);
            if (result != 1){
                // perror ("write");
                printf("Cancelation error");
                exit (2);
            }
            s = pthread_join(udp_server_thread, res);
            if (s != 0) {
                perror("pthread_create() error");
                exit(3);
            }

            break;
        }
        printf("\n");
        getchar();
    }

    return 0;
}