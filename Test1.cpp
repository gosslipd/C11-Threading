// Copyright Peter Gossling 17/06/2024
//

// Require compiler options:
//
// C Language Standard:     ISO C17 (2018) Standard (/std:c17)
// Compile As:              Compile as C Code (/TC)
//

#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <stdbool.h>
#include <time.h>

cnd_t cv;               // Condition variable used to signal that a thread has comleted its turn.
mtx_t m;                // Mutex used by condition variable.

int turn = 1;           // Indicates which thread has clearance to run at any one time.
#define nThreads (4)    // Number of identical threads to create.

char c;

// Thread function run be each thread.
void thread(int threadNumber)
{
    thrd_sleep(&(struct timespec){ .tv_sec = 5 }, NULL);

    for (int i = 1; i < 10; ++i)
    {
        mtx_lock(&m);

        while (turn != threadNumber)
        {
            cnd_wait(&cv, &m);
        }

        printf("Thread%d: i = %d\n", threadNumber, i);

        ++turn;
        if (turn > nThreads)
        {
            turn = 1;
        }

        cnd_broadcast(&cv);
        mtx_unlock(&m);
    }

    printf("Thread%d exiting normally\n", threadNumber);
}

// Create multiple threads running identical code that take it in turns to run.
int main(int argc, char* argv[])
{
    printf("Multi-threading Test!\n");

    thrd_t t[nThreads];

    mtx_init(&m, mtx_plain);
    cnd_init(&cv);

    printf("Creating: ");
    for (int i = 0; i < nThreads; ++i)
    {
        int er = thrd_create(&t[i], thread, i + 1);
        printf("e%d=%d ", i, er);
    }
    printf("\n");

    printf("Joining: ");
    for (int i = 0; i < nThreads; ++i)
    {
        int er = -1;
        thrd_join(t[i], &er);
        printf("e%d=%d ", i, er);
    }
    printf("\n");

    return 0;
}
