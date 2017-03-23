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
    insertBatiment(db, "Test1", 54, 44563);
    insertBatiment(db, "Test2", 55, 44231);
    insertBatiment(db, "Test3", 60, 44731);
    updateWithCondition(db, "batiment", "nom", "update", "surface > 54");

    sqlite3_close_v2(&db);

    printf("***************************************************************************\n\n");
    return 0;
}
