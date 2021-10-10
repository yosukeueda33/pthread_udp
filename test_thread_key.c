// https://stackoverflow.com/questions/31244806/pthread-key-create-when-does-the-destructor-function-invoked

// test of thread-specific data

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

static pthread_once_t once = PTHREAD_ONCE_INIT;
static pthread_key_t tidKey;

static void destructor(void *buf) {
    unsigned long *_tid = buf;
    printf("destroy, tid: %lu\n", *_tid);
    free(buf);
}

static void createKey(void) {
    int s = pthread_key_create(&tidKey, destructor);
    if(s != 0) {
        printf("failed to create key\n");
        exit(-1);
    }
}

void *store_tid() {
    int s;
    unsigned long *buf;

    // create key
    s = pthread_once(&once, createKey);
    if(s != 0) {
        printf("failed to create key\n");
        exit(-1);
    }

    buf = pthread_getspecific(tidKey);
    if(buf == NULL) { // thread call this function for the first time,
        buf = malloc(sizeof(unsigned long));
        if(buf == NULL) {
            printf("failed to allocate memory, %s\n", strerror(errno));
            exit(-1);
        }
        // register buffer to specified key & current thread,
        s = pthread_setspecific(tidKey, buf);
        if(s != 0) {
            printf("failed to setspecific\n");
            exit(-1);
        }
    }

    // store tid to buffer,
    *buf = (unsigned long)pthread_self();
    printf("set tid to: %lu\n", *buf);

    return buf;
}

void tsd_test() {
    unsigned long *tidp_a = store_tid();
    printf("tid - before call another thread: %lu\n", *tidp_a);

    int s;
    pthread_t t2;
    s = pthread_create(&t2, NULL, &store_tid, NULL);
    if(s != 0) {
        printf("failed to create thread\n");
        exit(-1);
    }

    s = pthread_join(t2, NULL);
    if(s != 0) {
        printf("failed to join thread\n");
        exit(-1);
    }

    printf("tid - after call another thread: %lu\n", *tidp_a);
}

int main(int argc, char *argv[]) {
    tsd_test();
    return 0;
}
