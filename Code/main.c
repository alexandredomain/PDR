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


    printf("********************* Ouverture de la base de données *********************\n");
    versionSQLite();
    sqlite3 *db; // initialisation de la base de données
    openBDD(&db);
    createTableListeBatiments(db);
    printf("***************************************************************************\n\n");


    printf("***************** Lecture des éventuels nouveaux batiments ****************\n");
    actualiserBatimentsEtSurfaces(db);
    printf("***************************************************************************\n\n");


    printf("************** Insertion des données dans la base de données **************\n");
    insertDataBatiment(db,"DIA","Elec",156.34,42795);
    printf("***************************************************************************\n\n");

    sqlite3_close_v2(&db);

    return 0;
}
