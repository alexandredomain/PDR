#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
// Librairie SQLite3
#include "sqlite3.h"

void creerBDD(sqlite3 *db) {
    int codeRetour = 0;
    remove("../Générés/maBaseDeDonnees"); // pour débug à supprimer ensuite

    // ouverture ou création de la base de données
    codeRetour = sqlite3_open_v2("../Générés/maBaseDeDonnees", db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);

    if (codeRetour) {
        //affiche une erreur sur la console qui explique pourquoi la base de données n'a pas pu être ouverte (vu que certaines consoles gèrent mal les accents ,je les ai omis)
        printf("Impossible d'ouvrir la base de données : %s\n", sqlite3_errmsg(db));
        //on ferme la base de données afin de libérer la mémoire
        sqlite3_close(db);
        //on stoppe l'exécution du programme
    }
}
