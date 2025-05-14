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
