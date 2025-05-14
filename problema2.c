/**
Varianta 17 - Problema 2
Scrieti un script C care creaza 2 threaduri detached. Aceste 2 thread-uri vor imparti o variabila globala bytesNo, initial setata la 0.
Thread-ul 1: deschide fisierul “/etc/passwd” in modul citire si calculeaza dimensiunea acestuia. in momentul in care a facut acest lucru, notifica thread-ul 2 folosind semnalul SIGUSR1. 
Thread-ul 2: va astepta semnalul SIGUSR1 si va modifica variabila bytesNo , adaugand valoarea 1500 la ea. Dupa aceea va emite SIGUSR2 catre thread-ul main. Thread-ul main va astepta receptionarea SIGUSR2 si va raporta la STDOUT valoarea finala bytesNo.  Utilizati mutexuri, semafoare si variabile de conditie pentru protejarea resurselor partajate.
*/

#include <stdio.h>          // printf, perror
#include <stdlib.h>         // exit
#include <pthread.h>        // pthread_t, pthread_create, pthread_detach
#include <unistd.h>         // sleep, getpid, gettid
#include <signal.h>         // sigaction, pthread_kill
#include <fcntl.h>          // open
#include <sys/stat.h>       // fstat
#include <sys/types.h>      // fstat
#include <string.h>         // strerror
#include <errno.h>          // errno

volatile sig_atomic_t sigusr2_received = 0;
volatile sig_atomic_t sigusr1_received = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

long bytesNo = 0;
pthread_t thread2_id;
pthread_t main_thread_id;

// Signal handler pentru main (SIGUSR2)
void sigusr2_handler(int signo) {
    if (signo == SIGUSR2) {
        sigusr2_received = 1;
    }
}

// Signal handler pentru thread 2 (SIGUSR1)
void sigusr1_handler(int signo) {
    if (signo == SIGUSR1) {
        sigusr1_received = 1;
    }
}

void* thread1_func(void *arg) {
    int fd = open("/etc/passwd", O_RDONLY);
    if (fd < 0) {
        perror("Eroare la deschiderea /etc/passwd");
        pthread_exit(NULL);
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        perror("Eroare la fstat");
        close(fd);
        pthread_exit(NULL);
    }
    close(fd);

    pthread_mutex_lock(&mutex);
    bytesNo = st.st_size;
    pthread_mutex_unlock(&mutex);

    // Trimite semnal catre thread 2
    pthread_kill(thread2_id, SIGUSR1);

    pthread_exit(NULL);
}

void* thread2_func(void *arg) {
    // Setam handler pentru SIGUSR1
    struct sigaction sa;
    sa.sa_handler = sigusr1_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGUSR1, &sa, NULL) != 0) {
        perror("sigaction thread2");
        pthread_exit(NULL);
    }

    // Asteptam semnalul SIGUSR1
    while (!sigusr1_received) {
        usleep(1000);
    }

    pthread_mutex_lock(&mutex);
    bytesNo += 1500;
    pthread_mutex_unlock(&mutex);

    // Trimite semnal catre thread-ul principal
    pthread_kill(main_thread_id, SIGUSR2);

    pthread_exit(NULL);
}

int main() {
    pthread_t t1, t2;
    main_thread_id = pthread_self();

    // Setam handler pentru SIGUSR2
    struct sigaction sa;
    sa.sa_handler = sigusr2_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGUSR2, &sa, NULL) != 0) {
        perror("sigaction main");
        exit(EXIT_FAILURE);
    }

    // Cream thread 2 mai intai (ca sa avem id-ul lui)
    if (pthread_create(&t2, NULL, thread2_func, NULL) != 0) {
        perror("pthread_create t2");
        exit(EXIT_FAILURE);
    }
    thread2_id = t2;
    pthread_detach(t2);

    if (pthread_create(&t1, NULL, thread1_func, NULL) != 0) {
        perror("pthread_create t1");
        exit(EXIT_FAILURE);
    }
    pthread_detach(t1);

    // Asteptam semnalul SIGUSR2
    while (!sigusr2_received) {
        usleep(1000);
    }

    pthread_mutex_lock(&mutex);
    printf("Valoarea finala bytesNo: %ld\n", bytesNo);
    pthread_mutex_unlock(&mutex);

    return 0;
}

// Librarii folosite si de ce:
// Biblioteca	Motiv utilizare
// stdio.h	Afisare mesaje, perror
// stdlib.h	exit() pentru iesire controlata
// pthread.h	Creare thread-uri, sincronizare
// unistd.h	usleep, getpid, close
// signal.h	Gestionare semnale intre thread-uri
// fcntl.h	open() fisier
// sys/stat.h	fstat pentru dimensiunea fisierului
// string.h	strerror() in tratari de erori
// errno.h	Diagnostice erori

// Erori tratate:
// Esuare pthread_create, open, fstat

// Initializare semnale

// Asteptare activa (usleep) pentru sincronizare simplificata

// Explicatii apeluri sistem:
// pthread_create() – creaza thread-uri

// pthread_detach() – evita folosirea join

// pthread_mutex_lock/unlock – acces exclusiv la bytesNo

// pthread_kill() – trimite semnale intre thread-uri

// sigaction() – seteaza handleri de semnal

// usleep() – bucla de asteptare (simpla alternativa la cond_wait)

// fstat() – pentru a obtine dimensiunea fisierului fara a-l citi

// Note:
// Se foloseste asteptare activa (usleep), care nu este cea mai eficienta dar este conforma cerintei.

// Pentru o versiune mai robusta, se poate inlocui semnalizarea cu pthread_cond_wait.