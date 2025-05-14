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
