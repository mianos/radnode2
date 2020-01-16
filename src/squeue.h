#pragma once

#include <stdint.h>
#include <limits.h>
#include <stdlib.h>

struct SQueue {
    int front = -1;
    int  rear = -1;
    int overflows = 0;
    int size;
    uint32_t *items;
    int used = 0;

    SQueue(int size_i) : size(size_i) {
        items = new uint32_t[size];
    }
    ~SQueue() {
        delete []items;
    }
    int isFull() const {
        if ((front == rear + 1) || (front == 0 && rear == size-1))
            return 1;
        return 0;
   }
    int isEmpty() const {
        if (front == -1)
            return 1;
        return 0;
    }

    int enQueue(uint32_t element) {
        if (isFull()) {
            overflows++;
            return 0;
        }
        if (front == -1) {
            front = 0;
        }
        rear = (rear + 1) % size;
        items[rear] = element;
        used++;
        return 1;
    }

    uint32_t deQueue() {
        int element;
        if (isEmpty()) {
            return UINT_MAX;
        } else {
            element = items[front];
            if (front == rear){
                front = -1;
                rear = -1;
            }  else {
                front = (front + 1) % size;
            }
            used--;
            return element;
        }
    }
    int count() const {
        return used;
    }
    void display() const {
        if (isEmpty()) {
            printf(" \n Empty Queue\n");
        } else {
            printf("\n Front -> %d ",front);
            printf("\n Items -> ");
            for (auto ii = front; ;ii=(ii+1)%size) {
                printf("%d ",items[ii]);
                if (ii == rear) {
                    break;
                }
            }
            printf("\n");
        }
    }
};
