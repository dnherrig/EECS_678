/** @file libpriqueue.c
 */

#include <stdlib.h>
#include <stdio.h>

#include "libpriqueue.h"


/**
  Initializes the priqueue_t data structure.

  Assumtions
    - You may assume this function will only be called once per instance of priqueue_t
    - You may assume this function will be the first function called using an instance of priqueue_t.
  @param q a pointer to an instance of the priqueue_t data structure
  @param comparer a function pointer that compares two elements.
  See also @ref comparer-page
 */
void priqueue_init(priqueue_t *q, int(*comparer)(const void *, const void *))
{
	//initialize queue members
	q->front = NULL;
	q->comparer = comparer;
	q->size = 0;
}


/**
  Inserts the specified element into this priority queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr a pointer to the data to be inserted into the priority queue
  @return The zero-based index where ptr is stored in the priority queue, where 0 indicates that ptr was stored at the front of the priority queue.
 */
int priqueue_offer(priqueue_t *q, void *ptr)
{
	//empty
	if(q->size == 0) {
		//initialize new node
		node_t *temp = malloc(sizeof(node_t));
		temp->ptr = ptr;
		temp->next = NULL;

		//front of the queue is the new node
		q->front = temp;

		//adjust queue size and return
		q->size = 1;
		return 0;
	}
	//only one node
	else if(q->size == 1) {
		//initialize new node
		node_t *temp = malloc(sizeof(node_t));
		temp->ptr = ptr;
		temp->next = NULL;

		//check for swap cases
		if(q->comparer(q->front->ptr, temp->ptr) == -1) {
			//swap not needed
			q->front->next = temp;

			//adjust queue size and return
			q->size = 2;
			return 1;
		}
		else {
			//swap needed
			temp->next = q->front;
			q->front->next = NULL;
			q->front = temp;

			//adjust queue size and return
			q->size = 2;
			return 0;
		}
	}
	//every other case
	else {
		//initialize new node
		node_t *temp = malloc(sizeof(node_t));
		temp->ptr = ptr;
		temp->next = NULL;

		//changing pointer values is a lot easier than moving pointers
		void *value;
		//set a traversal node at the front
		node_t *current = q->front;

		//return value. where the new value is placed
		int index = 0;

		//begin sorting alg
		for(int i = 0; i < q->size; i++) {
			if(q->comparer(temp->ptr, current->ptr) < 0) {
				while(current != NULL) {
					//store temp ptr value
					value = temp->ptr;
					//reassign temp ptr value to curr ptr value
					temp->ptr = current->ptr;
					//reassign curr ptr value with the saved temp ptr value
					current->ptr = value;

					//check prevents segfault, also sets the back of queue to temp
					if(current->next == NULL) {
						current->next = temp;
						break;
					}

					//iterate for traversal
					current = current->next;
				}

				//iterate index for return
				index++;
				break;
			}

			//puts the new node on back of queue
			//originally placed new value but changed
			if(current->next == NULL) {
				current->next = temp;
				break;
			}

			//iterate for traversal if condition not met
			current = current->next;
		}

		//adjust queue size and return
		q->size++;
		return index;
	}
}


/**
  Retrieves, but does not remove, the head of this queue, returning NULL if
  this queue is empty.

  @param q a pointer to an instance of the priqueue_t data structure
  @return pointer to element at the head of the queue
  @return NULL if the queue is empty
 */
void *priqueue_peek(priqueue_t *q)
{
	//queue is not empty
	if(q->size != 0) {
		return q->front->ptr;
	}
	//queue is empty
	else {
		return NULL;
	}
}


/**
  Retrieves and removes the head of this queue, or NULL if this queue
  is empty.

  @param q a pointer to an instance of the priqueue_t data structure
  @return the head of this queue
  @return NULL if this queue is empty
 */
void *priqueue_poll(priqueue_t *q)
{
	//if the queue is not empty
	if(q->size != 0) {
		//priqueue_remove(q, q->front->ptr);
		//remove front and adjust queue
		node_t *temp = q->front;
		void* ptr = temp->ptr;
		q->front = q->front->next;
		free(temp);

		//adjust size and return
		q->size--;
		return ptr;
	}
	//if the queue is just one node
	else if(q->size == 1) {
		//priqueue_remove(q, q->front->ptr);
		free(q->front);
		q->front = NULL;

		//adjust size and return
		q->size = 0;
		return NULL;
	}
	//the queue is empty
	else {
		return NULL;
	}
}


/**
  Returns the element at the specified position in this list, or NULL if
  the queue does not contain an index'th element.

  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of retrieved element
  @return the index'th element in the queue
  @return NULL if the queue does not contain the index'th element
 */
void *priqueue_at(priqueue_t *q, int index)
{
	//create node for traversal
	node_t *current = q->front;

	//traverse through the queue and look for value at index
	for(int i = 0; i < q->size; i++) {
		//i and index line up
		if(i == index) {
			return current->ptr;
		}

		//safely traverse through the queue
		if(current->next != NULL) {
			current = current->next;
		}
	}

	//return null if queue does not contain index'th element
	return NULL;
}


/**
  Removes all instances of ptr from the queue.

  This function should not use the comparer functio//adjust queue size and returnn, but check if the data contained in each element of the queue is equal (==) to ptr.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr address of element to be removed
  @return the number of entries removed
 */
int priqueue_remove(priqueue_t *q, void *ptr)
{
	return 0;
}


/**
  Removes the specified index from the queue, moving later elements up
  a spot in the queue to fill the gap.

  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of element to be removed
  @return the element removed from the queue
  @return NULL if the specified index does not exist
 */
void *priqueue_remove_at(priqueue_t *q, int index)
{
	return 0;
}


/**
  Returns the number of elements in the queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @return the number of elements in the queue
 */
int priqueue_size(priqueue_t *q)
{
	return q->size;
}


/**
  Destroys and frees all the memory associated with q.

  @param q a pointer to an instance of the priqueue_t data structure
 */
void priqueue_destroy(priqueue_t *q)
{

}
