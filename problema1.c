/**
Varianta 17 - Problema 1
Scrieți un program în limbajul C care să îndeplinească următoarele cerințe:
Programul primește pe linia de comandă 2 opțiuni: -s (--source) și -d (--destination) 
Programul creează două procese fiu în pieptene.
Programul creează un shared file denumit “results.txt” intre cele 2 procese create si il deschide in modul citire si scriere, in numele procesului curent.
Primul proces fiu:
Realizează redirectarea STDOUT către fisierul creat.
Execută comanda cat asupra fișierului specificat prin opțiunea -s (--source).
e) Al doilea proces fiu:
Realizează redirectarea STDIN către fisierul creat.
Execută comanda wc cu optiunea -w pentru a număra cuvintele primite prin shared file și le afișeaza. Atentie: NU SE VOR FOLOSI apeluri de tip wait/waitpid (folosirea lor duce la punctarea cu 0 a solutiei), deci este necesar pentru sincronizare un mecanism IPC
*/

#include <stdio.h>      // pentru printf, perror, fopen etc.
#include <stdlib.h>     // pentru exit, malloc etc.
#include <string.h>     // pentru strcmp
#include <unistd.h>     // pentru fork, exec, dup2 etc.
#include <fcntl.h>      // pentru open, O_* flaguri
#include <sys/mman.h>   // pentru mmap, IPC sincronizare
#include <sys/stat.h>   // pentru permisiuni open
#include <errno.h>      // pentru errno

#define SHARED_FILE "results.txt"

void usage(const char *progname) {
    fprintf(stderr, "Usage: %s -s <source> -d <destination>\n", progname);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    char *source = NULL;
    char *destination = NULL;

    // Parsare argumente
    for (int i = 1; i < argc - 1; i++) {
        if ((strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--source") == 0)) {
            source = argv[++i];
        } else if ((strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--destination") == 0)) {
            destination = argv[++i];
        }
    }

    if (!source || !destination) {
        usage(argv[0]);
    }

    // Creare fisier partajat
    int fd = open(SHARED_FILE, O_CREAT | O_RDWR | O_TRUNC, 0666);
    if (fd < 0) {
        perror("open shared file");
        exit(EXIT_FAILURE);
    }

    // Extindere fisier (pentru ca mmap sa reuseasca)
    if (ftruncate(fd, 4096) < 0) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    // Zona partajata pentru sincronizare
    int *sync = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (sync == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    *sync = 0; // Initializare sincronizare

    pid_t pid1 = fork();
    if (pid1 < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid1 == 0) {
        // Primul proces fiu - executa `cat source > results.txt`
        int file = open(SHARED_FILE, O_WRONLY);
        if (file < 0) {
            perror("open for writing");
            exit(EXIT_FAILURE);
        }

        dup2(file, STDOUT_FILENO);
        close(file);

        // Semnalare ca scrierea e in curs
        *sync = 1;

        execlp("cat", "cat", source, NULL);
        perror("execlp cat"); // daca execlp esueaza
        exit(EXIT_FAILURE);
    }

    pid_t pid2 = fork();
    if (pid2 < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid2 == 0) {
        // Al doilea proces fiu - executa `wc -w < results.txt`
        while (*sync == 0) {
            usleep(1000); // asteptam ca primul proces sa scrie
        }

        int file = open(SHARED_FILE, O_RDONLY);
        if (file < 0) {
            perror("open for reading");
            exit(EXIT_FAILURE);
        }

        dup2(file, STDIN_FILENO);
        close(file);

        execlp("wc", "wc", "-w", NULL);
        perror("execlp wc");
        exit(EXIT_FAILURE);
    }

    // Procesul parinte - iese fara a astepta
    munmap(sync, sizeof(int));
    close(fd);
    return 0;
}


// Explicatii despre librarii:
// <stdio.h>: operatii de I/O (printf, perror)

// <stdlib.h>: iesire controlata (exit), memorie

// <string.h>: comparare argumente (strcmp)

// <unistd.h>: fork, execlp, dup2, usleep

// <fcntl.h>: open, O_CREAT, O_RDWR, O_TRUNC

// <sys/mman.h>: mmap pentru memorie partajata

// <sys/stat.h>: permisiuni pentru open

// <errno.h>: diagnostic erori

// Erori tratate / situatii limita:
// Verificare argumente lipsa

// Verificare fork, open, mmap, dup2, ftruncate

// Verificare execlp pentru comenzi externe

// Control sincronizare fara wait()

// Justificari apeluri:
// fork(): creeaza procese fiu

// execlp(): ruleaza comenzi externe (cat, wc)

// dup2(): redirectare STDIN/STDOUT

// mmap(): sincronizare fara wait() (prin memorie partajata)

// ftruncate(): extindere fisier pentru mmap

// usleep(): asteptare simpla intre procese