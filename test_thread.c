// https://kaworu.jpn.org/c/pthread_%EF%BC%92%E3%81%A4%E3%81%AE%E3%82%B9%E3%83%AC%E3%83%83%E3%83%89%E3%82%92%E5%8B%95%E3%81%8B%E3%81%99
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
 
void f1(void);
void f2(void);
 
int
main(int argc, char *argv[])
{
    pthread_t thread1, thread2;
    int ret1,ret2;
 
    ret1 = pthread_create(&thread1,NULL,(void *)f1,NULL);
    ret2 = pthread_create(&thread2,NULL,(void *)f2,NULL);
 
    if (ret1 != 0) {
            err(EXIT_FAILURE, "can not create thread 1: %s", strerror(ret1) );
    }
    if (ret2 != 0) {
            err(EXIT_FAILURE, "can not create thread 2: %s", strerror(ret2) );
    }
 
    printf("execute pthread_join thread1\n");
    ret1 = pthread_join(thread1,NULL);
    if (ret1 != 0) {
            err(EXIT_FAILURE, ret1, "can not join thread 1");
    }
 
    printf("execute pthread_join thread2\n");
    ret2 = pthread_join(thread2,NULL);
    if (ret2 != 0) {
            err(EXIT_FAILURE, ret2, "can not join thread 2");
    }
 
    printf("done\n");
    return 0;
}
 
void
f1(void)
{
    size_t i;
 
    for(i=0; i<8; i++){
            printf("Doing one thing\n");
            usleep(3);
    }
}
 
void
f2(void)
{
    size_t i;
 
    for(i=0; i<4; i++){
            printf("Doing another\n");
            usleep(2);
    }
}