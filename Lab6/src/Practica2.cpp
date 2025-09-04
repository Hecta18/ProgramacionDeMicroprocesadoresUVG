// src/p2_ring.cpp
#include <pthread.h>
#include <cstdio>
#include <cstddef>

constexpr std::size_t Q = 1024;

struct Ring {
    int buf[Q];
    std::size_t head = 0, tail = 0, count = 0;
    pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
    pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;
    bool stop = false;
};

void ring_push(Ring* r, int v) {
    pthread_mutex_lock(&r->m);
    while (r->count == Q && !r->stop) {
        pthread_cond_wait(&r->not_full, &r->m);
    }
    if (!r->stop) {
        r->buf[r->head] = v;
        r->head = (r->head + 1) % Q;
        r->count++;
        pthread_cond_signal(&r->not_empty);
    }
    pthread_mutex_unlock(&r->m);
}

bool ring_pop(Ring* r, int* out) {
    pthread_mutex_lock(&r->m);
    while (r->count == 0 && !r->stop) {
        pthread_cond_wait(&r->not_empty, &r->m);
    }
    if (r->count == 0 && r->stop) {
        pthread_mutex_unlock(&r->m);
        return false;
    }
    *out = r->buf[r->tail];
    r->tail = (r->tail + 1) % Q;
    r->count--;
    pthread_cond_signal(&r->not_full);
    pthread_mutex_unlock(&r->m);
    return true;
}

// Ejemplo de productor y consumidor
void* producer(void* arg) {
    Ring* ring = static_cast<Ring*>(arg);
    for (int i = 0; i < 1000; ++i) {
        ring_push(ring, i);
    }
    pthread_mutex_lock(&ring->m);
    ring->stop = true; // Indicar que el productor ha terminado
    pthread_cond_broadcast(&ring->not_empty); // Despertar consumidores
    pthread_mutex_unlock(&ring->m);
    return nullptr;
}

void* consumer(void* arg) {
    Ring* ring = static_cast<Ring*>(arg);
    int value;
    while (ring_pop(ring, &value)) {
        // Procesar el valor
        printf("Consumido: %d\n", value);
    }
    return nullptr;
}

int main() {
    Ring ring;
    pthread_t prod, cons;

    pthread_create(&prod, nullptr, producer, &ring);
    pthread_create(&cons, nullptr, consumer, &ring);

    pthread_join(prod, nullptr);
    pthread_join(cons, nullptr);

    return 0;
}