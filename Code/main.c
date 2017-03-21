#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Header perso
#include "traitementXLS.h"
#include "requetesSQL.h"
#include "sqlite3.h"

int main(int pintArgc, char * ptstrArgv[]) {

    printf("*************** Traitement du fichier *.xls en format texte ***************\n");
    XLStoCSV("../Relevés/SB_CO2.xls");
    removeEmptyLinesCSV();
    printf("***************************************************************************\n\n");



    printf("************** Insertion des données dans la base de données **************\n");
    sqlite3 *db; // bdd

    versionSQLite();
    openBDD(&db);
    createTableBatiment(db);

    // Requête factice avec prise en charge de la date
    // SELECT date('1899-12-29', '+42737 day');
    char *requete = NULL;
    asprintf(&requete, "INSERT INTO batiment (nom, surface, date_jour) VALUES ('%s', %d, date('1899-12-30', '+%i day'));", "Tom", 20, 42644);
    int codeRetour = 0;
    char *feedbackErrorSQL = NULL;

    codeRetour = sqlite3_exec(db, requete, NULL, 0, &feedbackErrorSQL);
    if (codeRetour && feedbackErrorSQL != NULL) {
        printf(feedbackErrorSQL);
        sqlite3_free(feedbackErrorSQL);
        feedbackErrorSQL = NULL;
    }

    sqlite3_close_v2(&db);

    printf("***************************************************************************\n\n");
    return 0;
}
