#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
// Librairie SQLite3
#include "sqlite3.h"

void versionSQLite() {
    printf("SQLite_version %s\n", sqlite3_libversion());
}

void openBDD(sqlite3 *db) {
    int codeRetour = 0;
    remove("../Générés/maBaseDeDonnees"); // pour débug à supprimer ensuite

    // ouverture (ou création si n'existe pas) de la base de données
    codeRetour = sqlite3_open_v2("../Générés/maBaseDeDonnees", db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);

    if (codeRetour) { //affiche une erreur sur la console qui explique pourquoi la base de données n'a pas pu être ouverte)
        printf("Impossible d'ouvrir la base de données : %s\n", sqlite3_errmsg(db));
        sqlite3_close_v2(db); //on ferme la base de données afin de libérer la mémoire
    }
    else {
        printf("Ouverture de la base de données... Ok.\n");
    }
}

void createTableBatiment(sqlite3 *db){
    int codeRetour = 0;
    char *feedbackErrorSQL = NULL;

    codeRetour = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS batiment (id INTEGER PRIMARY KEY AUTOINCREMENT, nom TEXT, surface INTEGER, date_jour TEXT)", NULL, 0, &feedbackErrorSQL);

    if (codeRetour && feedbackErrorSQL != NULL){
        printf(feedbackErrorSQL);
        sqlite3_free(feedbackErrorSQL);
        feedbackErrorSQL = NULL;
    }

    else {
        printf("Création de la TABLE batiment... Ok.\n");
    }
}
