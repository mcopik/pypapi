#include <assert.h>
#include <stdlib.h>

#include <papi.h>

typedef void (*HighAPICallback)(long long *);

#define BUFFER_SIZE 100

static long long ** buffer = NULL;
static int buffer_size = BUFFER_SIZE, buffer_counter = 0;

long long * overflow_buffer(int size, int event_count, HighAPICallback callback)
{
  static int counter = 0, cur_buffer_size = 0, events = 0;
  static long long * values = NULL;
  //static HighAPICallback callback_ptr = NULL;
  if(size > 0) {
    buffer = calloc(BUFFER_SIZE, sizeof(long long*));
    buffer_size = BUFFER_SIZE;
    buffer_counter = 0;

    values = malloc(sizeof(long long) * size * event_count);
    buffer[buffer_counter++] = values;
    events = event_count;
    counter = 0;
    cur_buffer_size = size;

    //callback_ptr = callback;
    return values;
  } else if(size < 0) {
    free(buffer);
    free(values);
    buffer_size = buffer_counter = 0;
    counter = cur_buffer_size = events = 0;
    //callback_ptr = NULL;
    return NULL;
  } else {
    if(counter == cur_buffer_size) {

      if(buffer_counter == buffer_size) {
        buffer_size *= 2;
        buffer = realloc(buffer, sizeof(long long*) * buffer_size);
      }
      buffer[buffer_counter++] = values;
      values = calloc(cur_buffer_size * events, sizeof(long long));
      //printf("%p\n", values);
      //fflush(stdout);
      //(*callback_ptr)(values);
      counter = 0;
    }
    return &values[events * counter++];
  }
}

int overflow_buffer_count(void)
{
  return buffer_counter;
}

long long * overflow_buffer_access(int cnt)
{
  return buffer[cnt];
}

void overflow_buffer_allocate(int size, int event_count, HighAPICallback callback)
{
  assert(size > 0);
  assert(event_count > 0);
  assert(callback);
  overflow_buffer(size, event_count, callback);
}

void overflow_buffer_deallocate(void)
{
  overflow_buffer(-1, 0, NULL);
}

long long * overflow_buffer_get(void)
{
  return overflow_buffer(0, 0, NULL);
}

void overflow_C_callback(
    int EventSet, void* address,
    long long overflow_vector, void* ctx)
{
  long long * data = overflow_buffer_get();
  PAPI_read(EventSet, data);
}

