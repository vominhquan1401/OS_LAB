#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct pthread_seqlock {
    unsigned int sequence;
    pthread_mutex_t mutex;
} pthread_seqlock_t;

static inline void pthread_seqlock_init(pthread_seqlock_t *rw) {
    if (rw == NULL) {
        return; // thông số đầu vào không hợp lệ
    }
    rw->sequence = 0;
    if (pthread_mutex_init(&rw->mutex, NULL) != 0) { // khởi tạo mutex thất bại
      return;
    }
}

static inline void pthread_seqlock_wrlock(pthread_seqlock_t *rw) {
    if (rw == NULL) {
        return;
    }
    pthread_mutex_lock(&rw->mutex); // khóa mutex cho writer
    rw->sequence++; // tăng sequence lên số lẻ (bắt đầu ghi)
}

static inline void pthread_seqlock_wrunlock(pthread_seqlock_t *rw) {
    if (rw == NULL) {
        return;
    }
    rw->sequence++; // kết thúc ghi, sequence trở về số chẵn
    pthread_mutex_unlock(&rw->mutex);
}

static inline unsigned pthread_seqlock_rdlock(pthread_seqlock_t *rw) {
    if (rw == NULL) {
        return -1;
    }
    unsigned int seq_temp;
    do {
        seq_temp = rw->sequence; // lưu lại giá trị sequence
        // Đợi nếu sequence là số lẻ (đang ghi)
        while (seq_temp % 2 != 0) {
            seq_temp = rw->sequence;
        }
        // Kiểm tra lại sequence để đảm bảo không bị writer thay đổi
    } while (seq_temp != rw->sequence);
    return 1;
}

static inline unsigned pthread_seqlock_rdunlock(pthread_seqlock_t *rw) {
    if (rw == NULL) {
        return -1;
    }
    return 0;
}
