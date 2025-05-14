/**
Varianta 17 - Problema 3
Scrieti un script C care:
Creaza un proces fiu. Eroarea la acest pas va duce la incheierea executiei programului.
In cadrul procesului fiu:
Deschide fisierul /etc/paths in modul citire (pentru procesul curent) 
Redirectioneaza STDIN catre file descriptorul asociat fisierului de mai sus 
Redirectioneaza STDOUT catre un fisier denumit data.txt.
Fiul va numara (fara a folosi un apel de tipul exec/system/popen etc) liniile care au numar impar de caractere. La final, va scrie rezultatul la STDOUT. 
Procesul parinte va citi din fisierul data.txt rezultatul obtinut de fiu. Nu folositi apeluri de tip wait/waitpid! (e necesar un mecanism de sincronizare via IPC)
*/

#include <stdio.h>          // fopen, fread, fprintf, perror
#include <stdlib.h>         // exit
#include <unistd.h>         // fork, dup2, close, read, lseek
#include <fcntl.h>          // open, O_*
#include <sys/mman.h>       // mmap, PROT_*, MAP_SHARED
#include <sys/stat.h>       // pentru fstat, permisiuni
#include <string.h>         // strlen
#include <errno.h>          // errno

#define SYNC_SIZE sizeof(int)
#define SYNC_READY 1

int main() {
    int *sync = mmap(NULL, SYNC_SIZE, PROT_READ | PROT_WRITE,
                     MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (sync == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    *sync = 0;

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // === PROCESUL FIU ===
        int fd_in = open("/etc/paths", O_RDONLY);
        if (fd_in < 0) {
            perror("open /etc/paths");
            exit(EXIT_FAILURE);
        }

        int fd_out = open("data.txt", O_CREAT | O_WRONLY | O_TRUNC, 0666);
        if (fd_out < 0) {
            perror("open data.txt");
            close(fd_in);
            exit(EXIT_FAILURE);
        }

        // Redirectionari
        dup2(fd_in, STDIN_FILENO);
        dup2(fd_out, STDOUT_FILENO);
        close(fd_in);
        close(fd_out);

        // Citire si procesare din STDIN
        char line[1024];
        int count = 0;
        while (fgets(line, sizeof(line), stdin)) {
            size_t len = strlen(line);

            // Eliminam newline daca exista
            if (line[len - 1] == '\n') {
                len--;
            }

            if (len % 2 == 1) {
                count++;
            }
        }

        printf("%d\n", count); // scrie in STDOUT -> redirectionat catre data.txt

        *sync = SYNC_READY; // semnalam parintelui ca e gata
        munmap(sync, SYNC_SIZE);
        exit(0);
    }

    // === PROCESUL PARINTE ===
    // Asteptam semnalul de finalizare
    while (*sync != SYNC_READY) {
        usleep(1000); // asteptare activa
    }

    // Citim rezultatul din data.txt
    FILE *f = fopen("data.txt", "r");
    if (!f) {
        perror("fopen data.txt");
        exit(EXIT_FAILURE);
    }

    int result;
    if (fscanf(f, "%d", &result) == 1) {
        printf("Linii cu numar impar de caractere: %d\n", result);
    } else {
        fprintf(stderr, "Eroare la citirea rezultatului\n");
    }

    fclose(f);
    munmap(sync, SYNC_SIZE);
    return 0;
}

// Explicatii pentru librarii:
// Librarie	Motiv utilizare
// stdio.h	fopen, fscanf, perror, printf, fgets
// stdlib.h	exit, alocari
// unistd.h	fork, dup2, usleep, close
// fcntl.h	open, O_CREAT, O_WRONLY, O_TRUNC, O_RDONLY
// sys/mman.h	mmap pentru IPC sincronizare
// sys/stat.h	permisiuni pentru open()
// string.h	strlen, manipulare de siruri
// errno.h	diagnostic erori (perror)

// Erori tratate:
// fork() – daca procesul nu se poate crea, se opreste executia

// open() pentru ambele fisiere

// mmap() – verificare MAP_FAILED

// fscanf() – tratam caz in care nu se citeste corect

// Justificari ale apelurilor:
// fork() – creare proces fiu

// open(), dup2() – pentru redirectionare intrare/iesire

// mmap() – sincronizare IPC fara wait()

// fgets() – citire linie cu linie din fisierul redirectionat in STDIN

// strlen() – determinarea lungimii liniei

// printf() – scriere rezultat in data.txt (via STDOUT redirectionat)

// fscanf() – citire rezultat in parinte

// Comentarii si note:
// Nu folosim exec, system, wait/waitpid, conform cerintei.

// Folosim sincronizare simpla via mmap si usleep, care e usor de inteles si implementat.

// Alternativ, s-ar putea folosi semafoare POSIX (sem_t), dar ar adauga complexitate neceruta.