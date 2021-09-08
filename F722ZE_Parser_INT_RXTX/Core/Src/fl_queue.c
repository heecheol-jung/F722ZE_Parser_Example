#include <string.h>
#include <stdlib.h>
#include "fl_queue.h"

FL_DECLARE(void) fl_q_init(fl_queue_t* q, uint8_t* buf, uint32_t buf_size)
{
  memset(q, 0, sizeof(fl_queue_t));
  q->buf = buf;
  q->buf_size = buf_size;
}

FL_DECLARE(uint32_t) fl_q_count(fl_queue_t* q)
{
  return q->count;
}

FL_DECLARE(fl_status_t) fl_q_push(fl_queue_t* q, uint8_t data)
{
  // Queue is full.
  if (q->count >= q->buf_size)
  {
    return FL_ERROR;
  }

  if (q->head >= q->buf_size)
  {
    q->head = 0;
  }

  q->buf[q->head++] = data;
  q->count++;

  return FL_OK;
}

FL_DECLARE(fl_status_t) fl_q_pop(fl_queue_t* q, uint8_t* data)
{
  // Queue is empty.
  if (q->count == 0)
  {
    return FL_ERROR;
  }

  *data = q->buf[q->tail++];
  q->count--;

  if (q->tail >= q->buf_size)
  {
    q->tail = 0;
  }

  return FL_OK;
}
