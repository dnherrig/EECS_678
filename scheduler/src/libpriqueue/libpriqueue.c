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
	if(q->size == 0) {
		node_t *temp = malloc(sizeof(node_t));
		temp->ptr = ptr;
		temp->next = NULL;

		q->front = temp;

		q->size = 1;
		return 0;
	}
	else if(q->size == 1) {
		node_t *temp = malloc(sizeof(node_t));
		temp->ptr = ptr;
		temp->next = NULL;

		if(q->comparer(q->front->ptr, temp->ptr) == -1) {
			q->front->next = temp;

			q->size = 2;
			return 1;
		}
		else {
			temp->next = q->front;
			q->front->next = NULL;
			q->front = temp;

			q->size = 2;
			return 0;
		}
	}
	else {
		node_t *current = q->front;
		node_t *previous = q->front;
		node_t *temp = malloc(sizeof(node_t));
		temp->ptr = ptr;
		temp->next = NULL;

		int index = 1;

		for(int i = 0; i < q->size; i++) {
			if(current->next != NULL) {
				current = current->next;

				index++;
			}
		}

		if(q->comparer(current->ptr, temp->ptr) == -1) {
			current->next = temp;

			q->size++;
			return index + 1;
		}
		else {
			/*while(previous->next->ptr != current->ptr) {
				previous = previous->next;
			}*/
			for(int i = 0; i < index; i++) {
				if(previous->next != NULL) {
					previous = previous->next;
				}
			}
			previous->next = temp;
			temp->next = current;
			current->next = NULL;

			q->size++;
			return index;
		}
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
	if(q->size != 0) {
		return q->front->ptr;
	}
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
	if(q->size != 0) {
		//priqueue_remove(q, q->front->ptr);
		node_t *temp = q->front;
		void* ptr = temp->ptr;
		q->front = q->front->next;
		free(temp);

		q->size--;
		return ptr;
	}
	else if(q->size == 1) {
		//priqueue_remove(q, q->front->ptr);
		free(q->front);
		q->front = NULL;

		q->size = 0;
		return NULL;
	}
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
	return NULL;
}


/**
  Removes all instances of ptr from the queue.

  This function should not use the comparer function, but check if the data contained in each element of the queue is equal (==) to ptr.

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
