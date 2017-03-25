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
    insertBatiment(db,"DIA", "LAHURE", "345");
    /*
    updateWithCondition(db, "batiment", "nom", "update", "surface > 54");
    */
    sqlite3_close_v2(&db);
    printf("***************************************************************************\n\n");




    printf("***************** Lecture des éventuels nouveaux batiments ****************\n");
    printf("***************************************************************************\n\n");



    FILE* fichierCSVSurfaces = fopen("../Relevés/Surfaces.csv", "r");

    char line[160];
    char nom_site[50];
    char nom_batiment[50];
    char surface[50];

    int codeRetour;
    int isPresentInTableBatiments = -1;

    fgets(line, 160, fichierCSVSurfaces); // on passe la première qui ne sert à rien (titres de colonnes)
    fgets(line, 160, fichierCSVSurfaces); // On met la deuxième ligne dans la variable line

    while (!feof(fichierCSVSurfaces))  { // tant que le fichier n'est pas fini
        sscanf(line, "%[^;];%[^;];%s", &nom_site, &nom_batiment, &surface); // récupération des données de la ligne

        sqlite3_stmt *requete;
        char* sqlSELECT = "";
        asprintf(&sqlSELECT,"SELECT COUNT(*) FROM batiments WHERE lower(nom) = lower('%s')", nom_batiment);
        codeRetour = sqlite3_prepare_v2(db, sqlSELECT, strlen(sqlSELECT), &requete, NULL);
        free(sqlSELECT);

        if (!codeRetour){
            //la préparation s'est bien déroulée on peut maintenant récupérer les résultats
            while (codeRetour == SQLITE_OK || codeRetour == SQLITE_ROW) { //tant qu'il y a des lignes disponibles on récupère ligne par ligne le résultat et on affiche les colonnes
                codeRetour = sqlite3_step(requete); //on récupère une ligne dans la table
                if (codeRetour == SQLITE_OK || codeRetour == SQLITE_ROW){
                    isPresentInTableBatiments = sqlite3_column_int(requete, 0); //on récupère la premiére colonne (ici le nombre COUNT(*))
                }
            }
            sqlite3_finalize(requete); // libère les chaines "sqlite3_column_text" éventuellement à chaque appel de appel de "sqlite3_step" ou "sqlite3_finalize"
        }
        if (isPresentInTableBatiments == 0) {
            insertBatiment(db,nom_batiment, nom_site, surface);
            createTableBatiment(db, nom_batiment);
        }
        else if (isPresentInTableBatiments == 1) {
            update(db, "batiments", "surface", surface);
        }

        printf("\n");
        fgets(line, 160, fichierCSVSurfaces);
    }

    fclose(fichierCSVSurfaces);

    return 0;
}
