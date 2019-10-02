

extern "Python" void overflow_callback(long long *);

typedef void (*HighAPICallback)(long long *);

int overflow_buffer_count(void);
long long * overflow_buffer_access(int cnt);

void overflow_buffer_allocate(int size, int event_count, HighAPICallback callback);
void overflow_buffer_deallocate(void);
void overflow_C_callback(
    int EventSet, void* address,
    long long overflow_vector, void* ctx);
