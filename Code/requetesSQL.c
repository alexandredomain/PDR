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
    free(feedbackErrorSQL);
}

void createTableListeBatiments(sqlite3 *db) {
    requeteModele(db, "CREATE TABLE IF NOT EXISTS BATIMENTS (id INTEGER PRIMARY KEY AUTOINCREMENT, nom TEXT, site TEXT, surface INTEGER);", "Création de la table \"BATIMENTS\"");
}

void createTableBatiment(sqlite3 *db, char *nom) {
    char *requete = NULL;
    asprintf(&requete, "CREATE TABLE IF NOT EXISTS %s (fluide TEXT, valeur REAL, date TEXT);", nom);
    char *intitule=NULL;
    asprintf(&intitule, "Création de la table \"%s\"", nom);
    requeteModele(db, requete, intitule);
    free(requete);
    free(intitule);
}

void insertBatiment(sqlite3 *db, char *nom, char *site, char *surface) {
    char *requete = NULL;
    asprintf(&requete, "INSERT INTO BATIMENTS (nom, site, surface) VALUES ('%s', '%s', '%s');", nom, site, surface);
    char *intitule=NULL;
    asprintf(&intitule, "Insertion d'un nouveau batiment \"%s\" dans la table \"batiments\"", nom);
    requeteModele(db, requete, intitule);
    free(requete);
    free(intitule);
}

void insertDataBatiment(sqlite3 *db, char *site, char *nom_fluide, double valeur, int jour) {
    char *requete = NULL;
    asprintf(&requete, "INSERT INTO %s (fluide, valeur, date) VALUES ('%s', %f, date('1899-12-30', '+%i day'));", site, nom_fluide, valeur, jour);
    char *intitule=NULL;
    asprintf(&intitule, "Insertion d'une nouvelle ligne à la table \"%s\"", site);
    requeteModele(db, requete, intitule);
    free(requete);
    free(intitule);
}

void update(sqlite3 *db, char* table, char *champ, char *value) {
    char *requete = NULL;
    asprintf(&requete, "UPDATE %s SET %s = '%s';", table, champ, value);
    char *intitule=NULL;
    asprintf(&intitule, "Update de la table \"%s\"", table);
    requeteModele(db, requete, intitule);
    free(requete);
    free(intitule);
}

void updateWithCondition(sqlite3 *db, char* table, char *champ, char *value, char *condition) {
    char *requete = NULL;
    asprintf(&requete, "UPDATE %s SET %s = '%s' WHERE %s;", table, champ, value, condition);
    char *intitule=NULL;
    asprintf(&intitule, "Update de la table \"%s\"", table);
    requeteModele(db, requete, intitule);
    free(requete);
    free(intitule);
}
