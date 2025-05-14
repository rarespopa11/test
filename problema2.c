/**
Varianta 17 - Problema 2
Scrieti un script C care creaza 2 threaduri detached. Aceste 2 thread-uri vor imparti o variabila globala bytesNo, initial setata la 0.
Thread-ul 1: deschide fisierul “/etc/passwd” in modul citire si calculeaza dimensiunea acestuia. in momentul in care a facut acest lucru, notifica thread-ul 2 folosind semnalul SIGUSR1. 
Thread-ul 2: va astepta semnalul SIGUSR1 si va modifica variabila bytesNo , adaugand valoarea 1500 la ea. Dupa aceea va emite SIGUSR2 catre thread-ul main. Thread-ul main va astepta receptionarea SIGUSR2 si va raporta la STDOUT valoarea finala bytesNo.  Utilizati mutexuri, semafoare si variabile de conditie pentru protejarea resurselor partajate.
*/
