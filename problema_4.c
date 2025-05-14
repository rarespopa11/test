/**
Varianta 17 - Problema 4

3. Scrieti un script C care preia ca argument pe linia de comanda un director.
Scriptul va deschide directorul, va traversa si selecta DOAR fisierele regulare din acesta,
iar pentru fiecare din acestea, va crea un link simbolic in /tmp/<nume_fisier>,
unde nume_fisier este numele fisierului original.

Pentru fiecare fisier regular din directorul dat, se va crea un thread separat,
care va fi responsabil de crearea linkului simbolic.
La final, se va astepta terminarea tuturor thread-urilor.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <pthread.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#define MAX_PATH 4096

typedef struct {
    char source_path[MAX_PATH];
    char target_path[MAX_PATH];
} thread_data;

void* create_symlink(void* arg) {
    thread_data* data = (thread_data*)arg;

    // Dacă linkul deja există, îl ștergem
    unlink(data->target_path);

    if (symlink(data->source_path, data->target_path) == -1) {
        fprintf(stderr, "Eroare la crearea linkului simbolic pentru %s: %s\n",
                data->source_path, strerror(errno));
    } else {
        printf("Link simbolic creat: %s -> %s\n", data->target_path, data->source_path);
    }

    free(data); // Eliberăm memoria alocată
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Utilizare: %s <director>\n", argv[0]);
        return EXIT_FAILURE;
    }

    DIR* dir = opendir(argv[1]);
    if (!dir) {
        perror("Nu se poate deschide directorul");
        return EXIT_FAILURE;
    }

    struct dirent* entry;
    struct stat st;
    pthread_t threads[1024]; // presupunem un max de 1024 fisiere
    int thread_count = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char full_path[MAX_PATH];
        snprintf(full_path, MAX_PATH, "%s/%s", argv[1], entry->d_name);

        if (stat(full_path, &st) == -1) {
            perror("stat");
            continue;
        }

        if (S_ISREG(st.st_mode)) {
            thread_data* data = malloc(sizeof(thread_data));
            if (!data) {
                perror("malloc");
                continue;
            }

            snprintf(data->source_path, MAX_PATH, "%s", full_path);
            snprintf(data->target_path, MAX_PATH, "/tmp/%s", entry->d_name);

            if (pthread_create(&threads[thread_count], NULL, create_symlink, data) != 0) {
                perror("pthread_create");
                free(data);
            } else {
                thread_count++;
            }
        }
    }

    closedir(dir);

    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }

    return EXIT_SUCCESS;
}
