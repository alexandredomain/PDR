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

void createTableBatiment(sqlite3 *db) {
    requeteModele(db, "CREATE TABLE IF NOT EXISTS batiment (id INTEGER PRIMARY KEY AUTOINCREMENT, nom TEXT, surface INTEGER, date_jour TEXT);", "Création de la TABLE batiment");
}

void insertBatiment(sqlite3 *db, char *nom, int surface, int jour) {
    char *requete = NULL;
    asprintf(&requete, "INSERT INTO batiment (nom, surface, date_jour) VALUES ('%s', %d, date('1899-12-30', '+%i day'));", nom, surface, jour);
    requeteModele(db, requete, "Insertion d'un batiment");
}

void update(sqlite3 *db, char* table, char *champ, char *value) {
    char *requete = NULL;
    asprintf(&requete, "UPDATE %s SET %s = '%s';", table, champ, value);
    requeteModele(db, requete, "Update de la table");
}

void updateWithCondition(sqlite3 *db, char* table, char *champ, char *value, char *condition) {
    char *requete = NULL;
    asprintf(&requete, "UPDATE %s SET %s = '%s' WHERE %s;", table, champ, value, condition);
    requeteModele(db, requete, "Update de la table");
}

void requeteModele(sqlite3 *db, char *requete, char *intitule) {
    int codeRetour = 0;
    char *feedbackErrorSQL = NULL;
    // Mise en forme du message de console
    char prefix[100] = "";
    strcat(prefix, intitule);
    strcat(prefix, "... ");

    codeRetour = sqlite3_exec(db, requete, NULL, 0, &feedbackErrorSQL);

    if (codeRetour && feedbackErrorSQL != NULL){
        printf(strcat(prefix, "Erreur : "));
        printf(feedbackErrorSQL);
        printf("\n");
        sqlite3_free(feedbackErrorSQL);
        feedbackErrorSQL = NULL;
    }

    else {
        printf(strcat(prefix, "Ok.\n"));
    }
}
