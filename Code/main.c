#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Header perso
#include "traitementXLS.h"
#include "requetesSQL.h"
#include "sqlite3.h"

int main(int pintArgc, char * ptstrArgv[]) {

    XLStoCSV("../Relevés/GCE1_Elec.xls");
    removeEmptyLines();

    ///////////////////
    // TEST SQLITE 3 //
    ///////////////////

    printf("SQLite_version %s\n", sqlite3_libversion());

    sqlite3 *db;
    creerBDD(&db);
    char *feedbackErrorSQL = NULL;
    int codeRetour = 0;

    codeRetour = sqlite3_exec(db, "CREATE TABLE batiment (Id INTEGER PRIMARY KEY AUTOINCREMENT, Nom TEXT, Surface INTEGER, Date_jour TEXT)", NULL, 0, &feedbackErrorSQL);

    //si on a une erreur avec un message d'erreur alors on libére la mémoire pour le message d'erreur
    if (codeRetour && feedbackErrorSQL != NULL){
        printf(feedbackErrorSQL);
        //ici on ignore l'erreur
        sqlite3_free(feedbackErrorSQL);
        feedbackErrorSQL = NULL;
    }

    else {
        printf("Insertion d'un nouveau batiment !\n");

        // Requête factice avec prise en charge de la date
        // SELECT date('1899-12-29', '+42737 day');
        char *requete = NULL;
        asprintf(&requete, "INSERT INTO batiment (Nom, Surface, Date_jour) VALUES ('%s', %d, date('1899-12-30', '+%i day'));", "Tom", 20, 42736);

        codeRetour = sqlite3_exec(db, requete, NULL, 0, &feedbackErrorSQL);
        if (codeRetour && feedbackErrorSQL != NULL){
            printf(feedbackErrorSQL);
            sqlite3_free(feedbackErrorSQL);
            feedbackErrorSQL = NULL;
        }
    }

;

    sqlite3_close(db);

    ///////////////////////
    // FIN TEST SQLITE 3 //
    ///////////////////////
}
