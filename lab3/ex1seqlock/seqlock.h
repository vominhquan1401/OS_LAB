#ifndef PTHREAD_H
#include <pthread.h>
#endif

typedef struct pthread_seqlock { /* TODO: implement the structure */
   unsigned int sequence;
   pthread_mutex_t mutex;
} pthread_seqlock_t;

static inline void pthread_seqlock_init(pthread_seqlock_t *rw)
{
   /* TODO: ... */
   if(rw==NULL){
      return EINVAL;//thông số đầu vào không hợp lệ
   }
   rw->sequence=0;
   if(pthread_mutex_init(rw->mutex,NULL)!=0){//khởi tạo mutex thất bại
      return errno;
   }
   return 0;
}

static inline void pthread_seqlock_wrlock(pthread_seqlock_t *rw)
{
   /* TODO: ... */
   if(rw==NULL){
      return EINVAL;
   }
   if(pthread_mutex_destroy(rw->mutex)!=0){
      return errno;
   }
   return 0;
}

static inline void pthread_seqlock_wrunlock(pthread_seqlock_t *rw)
{
   /* TODO: ... */
   if(rw==NULL){
      return EINVAL;
   }
   rw->sequence++; // Tăng sequence lên số chẵn
   pthread_mutex_unlock(&rw->mutex);
}

static inline unsigned pthread_seqlock_rdlock(pthread_seqlock_t *rw)
{
   /* TODO: ... */
   if(rw==NULL){
      return EINVAL;
   }
   while(rw->sequence%2!=0); // chờ đến khi seq là số chẵn
   return 0;
}


static inline unsigned pthread_seqlock_rdunlock(pthread_seqlock_t *rw)
{
   /* TODO: ... */
   if(rw==NULL){
      return EINVAL;
   }
   rw->sequence++; // Tăng sequence lên số chẵn
   pthread_mutex_unlock(&rw->mutex);
    return 0;
}

