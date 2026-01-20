#include "task.h"
#include <assert.h>

#define MAX_TASKS 16
typedef Task* TaskHeap[MAX_TASKS];

static TaskHeap tasks;
static unsigned long num_tasks = 0;

static void swap(int a, int b) {
	assert(a < num_tasks);
	assert(b < num_tasks);
	Task* temp = tasks[a];
	tasks[a] = tasks[b];
	tasks[b] = temp;
}

// Returns true if the deadline of the task at index a is sooner than the task at index b
// If the two tasks have the same deadline, then the higher priority one is "sooner"
static bool is_sooner(int a, int b) {
	assert(a < num_tasks);
	assert(b < num_tasks);
	cycles_t a_dedl = tasks[a]->real_deadline;
	cycles_t b_dedl = tasks[b]->real_deadline;
	if (a_dedl == b_dedl) return tasks[a]->priority > tasks[b]->priority;
	return a_dedl < b_dedl;
}

static void heapify_up(int idx) {
	assert(idx >= 0);
	assert(idx < num_tasks);
	if (idx == 0) return;
	int parent_idx = (idx - 1) / 2;
	if (is_sooner(idx, parent_idx)) {
		swap(idx, parent_idx);
		heapify_up(parent_idx);
	}
}

static void heapify_down(int idx) {
	assert(idx >= 0);
	assert(idx < num_tasks);
	int smallest = idx;
	int left = 2 * idx + 1;
	int right = left + 1;

	if (left < num_tasks && is_sooner(left, smallest)) {
		smallest = left;
	}

	if (right < num_tasks && is_sooner(right, smallest)) {
		smallest = right;
	}

	if (smallest != idx) {
		swap(idx, smallest);
		heapify_down(smallest);
	}
}

bool is_full() {
	return num_tasks >= MAX_TASKS;
}

bool is_empty() {
	return num_tasks == 0;
}

Task* pop_task() {
	assert(!is_empty());

	Task* t = tasks[0];
	tasks[0] = tasks[num_tasks - 1];
	heapify_down(0);
	--num_tasks;
	t->added = false;

	return t;
}

void add_task(Task* t, cycles_t curr_time) {
	assert(!is_full());
	assert(!t->added);

	t->real_deadline = curr_time + t->deadline;
	if (t->period > 0) {
		t->next_add = curr_time + t->period;
	}
	t->added = true;

	tasks[num_tasks] = t;
	++num_tasks;
	heapify_up(num_tasks - 1);
}
