#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <stdatomic.h>
#include <sched.h>
#include <sys/time.h>

#define CATCH(exit_cond, ...)                       \
    do {                                            \
        if(exit_cond) {                             \
            fprintf(stderr, __VA_ARGS__);           \
            exit(EXIT_FAILURE);                     \
        }                                           \
    } while(0)

#define NUM_PLAYERS 5


pthread_barrierattr_t pt_barrierattr;
pthread_barrier_t pt_barrier;

struct player_data {
    int die_roll;
    bool eliminated;
} pdata[NUM_PLAYERS];
bool game_over = false;

int roll_die() { return (rand() / (RAND_MAX / 6)) + 1; }
void* player(void* arg) {
    uintptr_t pnum = (uintptr_t)arg;

    while (!game_over) {
        // if the player hasn't been eliminated yet, roll a die
        if (!pdata[pnum].eliminated) {
            pdata[pnum].die_roll = roll_die();
            printf("Player %"PRIdPTR" rolled a %d\n", pnum, pdata[pnum].die_roll);
        }

        // wait until everyone has rolled their die
        // one thread gets to decide who goes on to the next round
        if (pthread_barrier_wait(&pt_barrier) == PTHREAD_BARRIER_SERIAL_THREAD) {
            int lowest = pdata[0].die_roll;
            // find lowest roll
            for (int i = 1; i < NUM_PLAYERS; i++) {
                if (!pdata[i].eliminated && pdata[i].die_roll < lowest) {
                    lowest = pdata[i].die_roll;
                }
            }
            // eliminate all players who rolled that number
            int alive_players = 0;
            for (int i = 0; i < NUM_PLAYERS; i++) {
                if (!pdata[i].eliminated) {
                    if (pdata[i].die_roll == lowest) {
                        pdata[i].eliminated = true;
                        printf("Eliminating player %d\n", i);
                    } else {
                        alive_players++;
                    }
                }
            }
            // check if someone has won or everyone has lost
            puts("---------------------");
            if (alive_players < 2) {
                game_over = true;
                if (alive_players == 1) {
                    int winning_player = -1;
                    for (int i = 0; i < NUM_PLAYERS; i++) {
                        if (!pdata[i].eliminated) {
                            winning_player = i;
                        }
                    }
                    printf("Player %d has won the game!\n", winning_player);
                } else {
                    printf("All players were eliminated!\n");
                }
            }
        }
        // wait for the next round to start
        pthread_barrier_wait(&pt_barrier);
    }

    return NULL;
}

int main() {
    pthread_attr_t pt_attr;
    pthread_attr_init(&pt_attr);

    pthread_barrierattr_init(&pt_barrierattr);
    pthread_barrierattr_setpshared(&pt_barrierattr, 1);
    pthread_barrier_init(&pt_barrier, &pt_barrierattr, NUM_PLAYERS);

    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC_RAW, &time);
    srand(time.tv_nsec);

    pthread_t players[NUM_PLAYERS];
    for (int pnum = 0; pnum < NUM_PLAYERS; pnum++) {
        pthread_create(&players[pnum], &pt_attr, &player, (void*)(uintptr_t)pnum);
    }

    for (int pnum = 0; pnum < NUM_PLAYERS; pnum++) {
        pthread_join(players[pnum], NULL);
    }

    return EXIT_SUCCESS;
}
