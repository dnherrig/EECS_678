/** @file libpriqueue.h
 */

#ifndef LIBPRIQUEUE_H_
#define LIBPRIQUEUE_H_

/**
  Node Data Structure
*/
// struct _node_t;

typedef struct node_t
{
  void *ptr;
  struct node_t *next;
} node_t;

/**
  Priqueue Data Structure
*/
typedef struct _priqueue_t
{
  node_t *front;
  int(*comparer)(const void *, const void *);
  size_t size;
} priqueue_t;

int indexFinderHelper(priqueue_t *q, void *ptr);
void   priqueue_init     (priqueue_t *q, int(*comparer)(const void *, const void *));

int    priqueue_offer    (priqueue_t *q, void *ptr);
void * priqueue_peek     (priqueue_t *q);
void * priqueue_poll     (priqueue_t *q);
void * priqueue_at       (priqueue_t *q, int index);
int    priqueue_remove   (priqueue_t *q, void *ptr);
void * priqueue_remove_at(priqueue_t *q, int index);
int    priqueue_size     (priqueue_t *q);

void   priqueue_destroy  (priqueue_t *q);

#endif /* LIBPQUEUE_H_ */
