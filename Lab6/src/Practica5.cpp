// src/p5_pipeline.cpp
#include <pthread.h>
#include <cstdio>

constexpr int TICKS = 1000;
static pthread_barrier_t barrier;
static pthread_once_t once_flag = PTHREAD_ONCE_INIT;

static void init_shared() {
    // Inicialización única, abrir archivo de log, reservar buffers, etc.
}

void* stage(void* p) {
    long id = (long)p;
    pthread_once(&once_flag, init_shared);
    for (int t = 0; t < TICKS; t++) {
        // Simulación de trabajo en la etapa
        printf("Etapa %ld, tick %d\n", id, t);
        pthread_barrier_wait(&barrier); // Sincronizar con otras etapas
    }
    return nullptr;
}

int main() {
    pthread_t h1, h2, h3;
    pthread_barrier_init(&barrier, nullptr, 3); // 3 hilos en la barrera

    pthread_create(&h1, nullptr, stage, (void*)1);
    pthread_create(&h2, nullptr, stage, (void*)2);
    pthread_create(&h3, nullptr, stage, (void*)3);

    pthread_join(h1, nullptr);
    pthread_join(h2, nullptr);
    pthread_join(h3, nullptr);

    pthread_barrier_destroy(&barrier);
    return 0;
}