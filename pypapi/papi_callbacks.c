#include <assert.h>
#include <stdlib.h>

#include <papi.h>


#define BUFFER_SIZE 100

static long long ** buffer = NULL;
static int buffer_size = BUFFER_SIZE, buffer_counter = 0;
static int counter = 0, cur_buffer_size = 0, events = 0;

long long * overflow_buffer(int size, int event_count)
{
  static long long * values = NULL;
  if(size > 0) {
    buffer = calloc(BUFFER_SIZE, sizeof(long long*));
    buffer_size = BUFFER_SIZE;
    buffer_counter = 0;

    values = malloc(sizeof(long long) * size * event_count);
    buffer[buffer_counter++] = values;
    events = event_count;
    cur_buffer_size = size;

    return values;
  } else if(size < 0) {
    //for(int i = 0; i < buffer_counter; ++i)
    //free(buffer[i]);
    values = NULL;
    free(buffer);
    buffer = NULL;
    buffer_size = buffer_counter = 0;
    counter = cur_buffer_size = events = 0;
    return NULL;
  } else {
    if(counter == cur_buffer_size) {

      if(buffer_counter == buffer_size) {
        buffer_size *= 2;
        buffer = realloc(buffer, sizeof(long long*) * buffer_size);
      }
      buffer[buffer_counter++] = values;
      values = calloc(cur_buffer_size * events, sizeof(long long));
      counter = 0;
    }
    return &values[events * counter++];
  }
}

int overflow_buffer_count(void)
{
  return buffer_counter;
}

int overflow_buffer_size(int cnt)
{
  return cnt == (buffer_counter - 1) ? counter * events : cur_buffer_size * events;
}

long long * overflow_buffer_access(int cnt)
{
  return buffer[cnt];
}

void overflow_buffer_allocate(int size, int event_count)
{
  assert(size > 0);
  assert(event_count > 0);
  overflow_buffer(size, event_count);
}

void overflow_buffer_deallocate(void)
{
  overflow_buffer(-1, 0);
}

long long * overflow_buffer_get(void)
{
  return overflow_buffer(0, 0);
}

void overflow_C_callback(
    int EventSet, void* address,
    long long overflow_vector, void* ctx)
{
  long long * data = overflow_buffer_get();
  PAPI_read(EventSet, data);
}

