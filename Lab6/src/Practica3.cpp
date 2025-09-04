// src/p3_rw.cpp
#include <pthread.h>
#include <vector>
#include <cstdio>
#include <cstdint>

constexpr int NBUCKET = 1024;

struct Node {
    int k, v;
    Node* next;
};

struct Map {
    Node* b[NBUCKET]{};
    pthread_rwlock_t rw = PTHREAD_RWLOCK_INITIALIZER;
};

int map_get(Map* m, int k) {
    pthread_rwlock_rdlock(&m->rw);
    // Simulación de búsqueda
    pthread_rwlock_unlock(&m->rw);
    return 0; // Retornar valor simulado
}

void map_put(Map* m, int k, int v) {
    pthread_rwlock_wrlock(&m->rw);
    // Simulación de inserción
    pthread_rwlock_unlock(&m->rw);
}

void* reader(void* arg) {
    Map* map = static_cast<Map*>(arg);
    for (int i = 0; i < 100; ++i) {
        map_get(map, i);
    }
    return nullptr;
}

void* writer(void* arg) {
    Map* map = static_cast<Map*>(arg);
    for (int i = 0; i < 100; ++i) {
        map_put(map, i, i * 2);
    }
    return nullptr;
}

int main() {
    Map map;
    pthread_t readers[5], writers[2];

    for (int i = 0; i < 5; ++i) {
        pthread_create(&readers[i], nullptr, reader, &map);
    }
    for (int i = 0; i < 2; ++i) {
        pthread_create(&writers[i], nullptr, writer, &map);
    }

    for (int i = 0; i < 5; ++i) {
        pthread_join(readers[i], nullptr);
    }
    for (int i = 0; i < 2; ++i) {
        pthread_join(writers[i], nullptr);
    }

    return 0;
}