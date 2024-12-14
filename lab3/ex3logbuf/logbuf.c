#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define MAX_LOG_LENGTH 10
#define MAX_BUFFER_SLOT 5
#define MAX_LOOPS 30

char logbuf[MAX_BUFFER_SLOT][MAX_LOG_LENGTH];

int count;
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t log_cond = PTHREAD_COND_INITIALIZER; 
void flushlog();

struct _args
{
   unsigned int interval;
};

void *wrlog(void *data)
{
    char str[MAX_LOG_LENGTH];
    int id = *(int *)data;

    usleep(20); // Mô phỏng độ trễ
    sprintf(str, "%d", id);

    // Khóa mutex để truy cập buffer an toàn
    pthread_mutex_lock(&log_mutex);

    // Nếu buffer đầy, chờ đến khi được xóa
    while (count >= MAX_BUFFER_SLOT) {
        pthread_cond_wait(&log_cond, &log_mutex);
    }

    // Ghi dữ liệu vào buffer
    strcpy(logbuf[count], str);
    count++;
    printf("wrlog(): %d\n", id);

    pthread_mutex_unlock(&log_mutex); // Mở khóa mutex
    return NULL;
}

void flushlog()
{
    int i;
    char nullval[MAX_LOG_LENGTH];

    printf("flushlog()\n");
    sprintf(nullval, "%d", -1);

    // Khóa mutex để truy cập buffer an toàn
    pthread_mutex_lock(&log_mutex);

    // In và xóa dữ liệu trong buffer
    for (i = 0; i < count; i++) {
        printf("Slot %i: %s\n", i, logbuf[i]);
        strcpy(logbuf[i], nullval);
    }
    // đảm bảo dữ liệu được in ra màn hình
    fflush(stdout);

    // Reset buffer và thông báo cho các thread đang chờ
    count = 0;
    pthread_cond_broadcast(&log_cond);

    pthread_mutex_unlock(&log_mutex); // Mở khóa mutex
}

void *timer_start(void *args)
{
   while (1)
   {
      flushlog();
      /*Waiting until the next timeout */
      usleep(((struct _args *) args)->interval);
   }
}

int main()
{
   int i;
   count = 0;
   pthread_t tid[MAX_LOOPS];
   pthread_t lgrid;
   int id[MAX_LOOPS];

   struct _args args;
   args.interval = 500e3;
   /*500 msec ~ 500 * 1000 usec */

   /*Setup periodically invoke flushlog() */
   pthread_create(&lgrid, NULL, &timer_start, (void*) &args);

   /*Asynchronous invoke task writelog */
   for (i = 0; i < MAX_LOOPS; i++)
   {
      id[i] = i;
      pthread_create(&tid[i], NULL, wrlog, (void*) &id[i]);
   }

   for (i = 0; i < MAX_LOOPS; i++)
      pthread_join(tid[i], NULL);

   sleep(5);

   return 0;
}
