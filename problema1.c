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
