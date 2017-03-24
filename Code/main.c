#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Header perso
#include "traitementXLS.h"
#include "requetesSQL.h"
#include "sqlite3.h"

int main(int pintArgc, char * ptstrArgv[]) {

    sqlite3 *db; // initialisation de la base de données

    printf("*************** Traitement du fichier *.xls en format texte ***************\n");
    XLStoCSV("../Relevés/SB_CO2.xls");
    removeEmptyLinesCSV();
    printf("***************************************************************************\n\n");



    printf("************** Insertion des données dans la base de données **************\n");
    versionSQLite();
    openBDD(&db);
    createTableListeBatiments(db);
    createTableBatiment(db, "DIA");
    insertDataBatiment(db,"DIA","Elec",156.34,42795);
    /*
    updateWithCondition(db, "batiment", "nom", "update", "surface > 54");
    */
    sqlite3_close_v2(&db);
    printf("***************************************************************************\n\n");
    return 0;
}
