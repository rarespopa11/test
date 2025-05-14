/**
Varianta 17 - Problema 1
1.	Cum sunt reprezentate si stocate argumentele de pe linia de comanda ? Dar variabilele de mediu transmise unui program C ? Mentionati valorile argc, argv si envp pentru urmatoarea linie:
export X=2; export Y=3; ./a.out -a 2 -b 3
*/
// Reprezentare și stocare:

// Argumentele din linia de comandă sunt transmise prin argc (număr) și argv (vector de șiruri).

// Variabilele de mediu sunt transmise prin envp (vector de șiruri char*).

// Pentru comanda:

// export X=2; export Y=3; ./a.out -a 2 -b 3

// argv = { "./a.out", "-a", "2", "-b", "3" }

// envp conține cel puțin: "X=2", "Y=3" (plus alte variabile de mediu existente).