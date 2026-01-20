#pragma once
#include <stdbool.h>

typedef unsigned long cycles_t;

// All time periods are in cycles
struct _Task {
	void (*fcn)();
	int priority;
	long period; // <0 if aperiodic
	cycles_t deadline; // Offset from when it starts
	cycles_t runtime; // How long the task actually takes to compute (worst case)
	cycles_t next_add; // If this task is periodic, what is the next time we should add it to the list?
	cycles_t real_deadline; // The actual real time that this instance of a task needs to complete right now
	bool added;
} typedef Task;

bool is_full();
bool is_empty();
Task* pop_task();
void add_task(Task* t, cycles_t curr_time);
