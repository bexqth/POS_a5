#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>
#include <pthread.h>
#include <stdlib.h>

typedef struct buffer_t{
    int *array;
    int capacity;
    int index;
    int pocetPrvyPachatel;
    int pocetDruhyPachatel;
} buffer_t;

void buffer_init(buffer_t *buff, int capacity) {
    buff->array = malloc(sizeof(int)*capacity);
    buff->capacity = capacity;
    buff->index = 0;
    buff->pocetPrvyPachatel = 0;
    buff->pocetDruhyPachatel = 0;
}

void buffer_destroy(buffer_t *buff) {
    free(buff->array);
}

bool buffer_push(buffer_t *buff, int data) {
    if(buff->index < buff->capacity) {
        buff->array[++buff->index] = data;
        return true;
    }
    return false;
}

int buffer_pull (buffer_t *buff) {
    if(buff->capacity > 0) {
        return buff->array[--buff->index];
    }
    return -1;
}

typedef struct thread_data_t {
    pthread_mutex_t mutex;
    pthread_cond_t holmes;
    pthread_cond_t policia;
    buffer_t buff;
} thread_data_t;

void thread_data_init(thread_data_t *data, int capacity) {
    pthread_mutex_init(&data->mutex, NULL);
    pthread_cond_init(&data->holmes, NULL);
    pthread_cond_init(&data->policia, NULL);
    buffer_init(&data->buff, capacity);
}

void thread_data_destroy(thread_data_t *data) {
    pthread_mutex_destroy(&data->mutex);
    pthread_cond_destroy(&data->holmes);
    pthread_cond_destroy(&data->policia);
    buffer_destroy(&data->buff);
}

int pridajDokaz(int min, int max) {
    float r = rand() / (float) RAND_MAX; //<0,1>
    return min + r * ( max - min ); //<min, max>
}

void* policia_fun(void* data) {
    thread_data_t *data_t = (thread_data_t*) data;

    //while() {
    for(int i = 0; i < 25; i++) {
        pthread_mutex_lock(&data_t->mutex);
        int dokaz = pridajDokaz(1,2);

        while(!buffer_push(&data_t->buff,dokaz)) {
            pthread_cond_wait(&data_t->policia, &data_t->mutex);
        }

        pthread_mutex_unlock(&data_t->mutex);
        pthread_cond_signal(&data_t->holmes);

    }

}

void vysetriDokaz(buffer_t *buff, int dokaz) {
    if(dokaz == 1) {
        buff->pocetPrvyPachatel++;
    } else {
        buff->pocetDruhyPachatel++;
    }

    printf("pocet dokazov prvy pachatel %d \n", buff->pocetPrvyPachatel);
    printf("pocet dokazov druhy pachatel %d \n", buff->pocetDruhyPachatel);
}

void* holmes_fun(void* data) {
    thread_data_t *data_t = (thread_data_t*) data;
    for(int i = 0; i < 25; i++) {
        pthread_mutex_lock(&data_t->mutex);

        while(data_t->buff.index == 0) {
            pthread_cond_wait(&data_t->holmes, &data_t->mutex);
        }

        int dokaz = buffer_pull(&data_t->buff);
        vysetriDokaz(&data_t->buff, dokaz);
        /*printf("pocet dokazov prvy pachatel %d \n", data_t->buff.pocetPrvyPachatel);
        printf("pocet dokazov druhy pachatel %d \n", data_t->buff.pocetDruhyPachatel);*/
        pthread_mutex_unlock(&data_t->mutex);
        pthread_cond_signal(&data_t->policia);

    }


    /*if(data_t->buff.pocetPrvyPachatel > data_t->buff.pocetDruhyPachatel) {
        printf("prvy pochatel je vinny \n");
    } else {
        printf("druhy pochatel je vinny \n");
    }*/
}


int main() {
    thread_data_t data;
    thread_data_init(&data, 25);
    pthread_t policia, holmes;

    pthread_create(&policia, NULL, policia_fun, &data);
    pthread_create(&holmes, NULL, holmes_fun, &data);

    pthread_join(policia, NULL);
    pthread_join(holmes, NULL);

    if(data.buff.pocetPrvyPachatel > data.buff.pocetDruhyPachatel) {
        printf("prvy pochatel je vinny \n");
    } else {
        printf("druhy pochatel je vinny \n");
    }

    thread_data_destroy(&data);

    return 0;
}
