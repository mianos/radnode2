#pragma once
#include "squeue.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

struct Rcs {
    int period;
    uint32_t running_sum;
    SQueue *vq;
    double r_avg;
    int len;
    Rcs(int period, int size) : period(period), running_sum(0), r_avg(0.0), len(0) {
        vq = new SQueue(size);
    }
    double add(uint32_t ll) {
        if (vq->isFull()) {
            running_sum -= vq->deQueue();
        } else {
            len += 1;
        }
        vq->enQueue(ll);
        running_sum += ll;
        r_avg = (double)running_sum / (double)len;
        //printf("added %d for a %d running sum %d len %d\n", ll, period, running_sum, len);
        return r_avg;
    }
#if 1
    void display() {
        int i;
        if (vq->isEmpty()) {
            printf(" \n Empty Queue\n");
        } else {
            printf("\n Items -> ");
            for( i = vq->front; i!= vq->rear; i=(i+1)% vq->size) {
                printf("%d ", vq->items[i]);
            }
            printf("%d ", vq->items[i]);
            printf("\n Rear -> %d \n", vq->rear);
        }
    }
#endif
	int scale(int *output, int new_max) {
        int ii;
		int32_t old_max = 0;
		for (ii = vq->front; ; ii = (ii + 1) % vq->size) {
            old_max = MAX(old_max, vq->items[ii]);
            if (ii == vq->rear) {
                break;
            }
       }
        if (old_max == 0) {
            printf("no existing  MAX\n");
            return 0;
        }
        double scale = (double)new_max / (double)old_max;
        //  last item is pointed to vw->rear
		for (ii = vq->front; ; ii = (ii + 1) % vq->size) {
            *output++ = vq->items[ii] * scale;
            if (ii == vq->rear) {
                break;
            }
       }
        return len;
    }
};
