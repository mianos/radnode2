#ifdef ARDUINO 
#define printf Serial.printf
#endif

#ifdef __APPLE__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "squeue.h"
#include "maq.h"

int count_10s = 0;
Rcs rs60s  =  Rcs(60, 6);
Rcs rs60mins = Rcs(360, 60);

void graph(int *vars, int size) {
//    if (size != 60 || size != 6) {
//        return;
//    }
    printf("size %d: ", size);
    for (int ii = 0; ii < size; ii++) {
        printf("%2d ", vars[ii]);
//        if (vars[ii] > 16) {
//            printf("\n");
//            exit(1);
//        }
    }
    printf("\n");
}

int    main(int argc, char *argv[]) {
    printf("hello world\n");
    for (auto ii = 0; ii < 10; ii++) {
        //int rcl = rand() % 5;
        int rcl = ii;
        double rs60 = rs60s.add(rcl);
        printf("\n===\nAdded %d count %d\n", rcl, rs60s.vq->count());
        rs60s.vq->display();
        int32_t remapped[rs60s.len];
        int out_size = rs60s.scale(remapped, 10);
        graph(remapped, out_size);
    }
#if 0
    for (;;) {
        int rcl = rand() % 5;
        double rs60 = rs60s.add(rcl);
        printf("Added %d\n", rs60s.vq->size);
        int32_t remapped[rs60s.len];
        int out_size = rs60s.scale(remapped, 16);
        graph(remapped, out_size);
        if (!(++count_10s % 6)) {
           double rs60min = rs60mins.add(rs60s.running_sum);
           int32_t rem60[rs60mins.len];
           int out_size = rs60mins.scale(rem60, 16);
           graph(rem60, out_size);
           usleep(100000); 
        }
    }
#endif
    return 0;
}
#endif
