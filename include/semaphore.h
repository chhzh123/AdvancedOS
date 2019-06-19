// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn
// Ubuntu 18.04 + gcc 7.3.0

/****** semaphore.h ******/

#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include "stdio.h"
#include "task.h"

#define N_SEMAPHORE 100

typedef struct Semaphore {
    int val;
    bool used;
    process* head;
} Semaphore;
Semaphore sem_list[N_SEMAPHORE];

void sem_init() {
    for (int i = 0; i < N_SEMAPHORE; ++i){
        sem_list[i].val = 0;
        sem_list[i].used = false;
        sem_list[i].head = NULL;
    }
}

void do_sem_p(int sem_id) { // sem_wait
    disable();
    sem_list[sem_id].val--;
#ifdef DEBUG
    printf("do_sem_p: sem_list[%d] = %d\n", sem_id, sem_list[sem_id].val);
#endif
    if (sem_list[sem_id].val < 0) {
        curr_proc->next = sem_list[sem_id].head;
        sem_list[sem_id].head = curr_proc;
        do_wait();
    }
    enable();
}

void do_sem_v(int sem_id) { // sem_signal
    disable();
    sem_list[sem_id].val++;
#ifdef DEBUG
    printf("do_sem_v: sem_list[%d] = %d\n", sem_id, sem_list[sem_id].val);
#endif
    if (sem_list[sem_id].val <= 0) {
        int pid = sem_list[sem_id].head->pid;
        sem_list[sem_id].head = curr_proc->next;
        wakeup(pid);
    }
    enable();
}

int do_getsem(int val) {
    for(int i = 0; i < N_SEMAPHORE; ++i) {
        if(!sem_list[i].used) {
            sem_list[i].val = val;
            sem_list[i].used = true;
#ifdef DEBUG
            printf("do_getsem: get sem_list[%d] = %d\n", i, val);
#endif
            return i;
        }
    }
    return -1;
}

void do_freesem(int sem_id) {
    sem_list[sem_id].used = false;
    sem_list[sem_id].head = NULL;
}

#endif // SEMAPHORE_H