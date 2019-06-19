// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn

#include "stdio.h"
#include "api.h"

enum FRUITS {
	NONE = 0,
	APPLE,
	PEAR,
	BANANA
} fruit_plate;

char words[100];
int fruit_index;

void put_words(char* w)
{
	strcpy(words,w);
}

void put_fruit()
{
	fruit_index = (fruit_index + 1) % 4;
	if (fruit_index == 0)
		fruit_index++;
	fruit_plate = fruit_index;
}

int sem_word, sem_fruit, sem_word_full, sem_fruit_full;

void main(){
	sem_word = get_sem(0);
	sem_fruit = get_sem(0);
	sem_word_full = get_sem(0);
	sem_fruit_full = get_sem(0);
	fruit_plate = fruit_index = 0;
	if (fork()){
		while(1){
			sem_wait(sem_word); // p0
			sem_wait(sem_fruit); // p1
			char fruit_name[10];
			if (fruit_plate == APPLE)
				strcpy(fruit_name,"APPLE");
			else if (fruit_plate == PEAR)
				strcpy(fruit_name,"PEAR");
			else if (fruit_plate == BANANA)
				strcpy(fruit_name,"BANANA");
			printf("%s %s!\n",words,fruit_name);
			sem_signal(sem_word_full); // v2
			fruit_plate = 0;
			sem_signal(sem_fruit_full); // v3
		}
	} else if (fork()) {
		while(1){
			put_words("Father will live forever!");
			sem_signal(sem_word); // v0
			sem_wait(sem_word_full); // p2
		}
	} else {
		while(1){
			put_fruit();
			sem_signal(sem_fruit); // v1
			sem_wait(sem_fruit_full); // p3
		}
	}
}