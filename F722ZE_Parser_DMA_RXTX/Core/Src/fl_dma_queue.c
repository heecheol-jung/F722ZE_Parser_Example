#include "fl_dma_queue.h"
#include <string.h>
#include <stdlib.h>

FL_DECLARE(void) fl_dma_q_init(fl_dma_queue_t* q, FL_DMA_HANDLE dma_handle, uint8_t* buf, uint32_t buf_size)
{
  memset(q, 0, sizeof(fl_dma_queue_t));
  q->dma_handle = dma_handle;
  q->buf = buf;
  q->buf_size = buf_size;
}

FL_DECLARE(uint32_t) fl_dma_q_count(fl_dma_queue_t* q)
{
  uint32_t old_head = q->head;

  q->head = q->buf_size - q->dma_handle->Instance->NDTR;
  if (q->head > old_head)
  {
    // |        |             |         |            |
    // start    tail          old_head  q->head     end of buffer
    // of buffer
    q->count += (q->head - old_head);
  }
  else if (q->head < old_head)  // roll over
  {
    // |        |             |          |            |
    // start    q->head       q->tail    old_head     end of buffer
    // of buffer
    q->count += (q->head + (q->buf_size - old_head));
  }

  return q->count;
}

FL_DECLARE(fl_status_t) fl_dma_q_pop(fl_dma_queue_t* q, uint8_t* data)
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
