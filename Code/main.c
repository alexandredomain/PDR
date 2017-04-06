#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>

// Header perso
#include "traitementXLS.h"
#include "requetesSQL.h"
#include "sqlite3.h"

int main(int pintArgc, char * ptstrArgv[]) {

    printf("********************* Ouverture de la base de données *********************\n");
    versionSQLite();
    sqlite3 *db; // initialisation de la base de données
    openBDD(&db);
    createTableListeBatiments(db);
    printf("***************************************************************************\n\n");


    printf("***************** Lecture des éventuels nouveaux batiments ****************\n");
    actualiserBatimentsEtSurfaces(db);
    printf("***************************************************************************\n\n");


    DIR *dir;
    struct dirent *ent;
    char cheminDossier[14] = "../Relevés/";
    char nomFichier[32];
    char cheminFichier[50];
    char cheminFichierCSV[55];

    if ((dir = opendir(cheminDossier)) != NULL) {
      while ((ent = readdir (dir)) != NULL) {
          if (ent->d_type == DT_REG) {
              snprintf(nomFichier, sizeof(nomFichier), "%s", ent->d_name);
              snprintf(cheminFichier, sizeof(cheminFichier), "%s%s", cheminDossier, nomFichier);
              //if (strstr(nomFichier, "_20[1-9]") == NULL) {
              if (
                nomFichier[strlen(nomFichier)-5] != '_' // cas des fichiers mensuels "batiment_fluide_.xls"
                &&
                nomFichier[0] != '_' // cas du fichier _ProdNRJ
                &&
                (nomFichier[0] != 'S' && nomFichier[1] != 'L' && nomFichier[2] != '_') // cas du fichier "site" SL_
                &&
                (nomFichier[0] != 'S' && nomFichier[1] != 'B' && nomFichier[2] != '_') // cas du fichier "site" SB_
                &&
                (nomFichier[strlen(nomFichier)-11] == '_' && isdigit(nomFichier[strlen(nomFichier)-10]) && nomFichier[strlen(nomFichier)-10] == '2') // cas des fichiers journaliers des mois précédents "batiment_fluide_201704.xls"
                &&
                !(nomFichier[strlen(nomFichier)-9] == '_' && isdigit(nomFichier[strlen(nomFichier)-8]) && nomFichier[strlen(nomFichier)-8] == '2') // cas des fichiers mensuels des mois précédents "batiment_fluide_2017.xls"
                ) {

                  snprintf(cheminFichierCSV, sizeof(cheminFichierCSV), "%s%s%s", "../Générés/", nomFichier, ".txt");

                  printf("--------------------------------------- ------------------------------------\n");
                  printf("%s\n", cheminFichier);

                  XLStoCSV(cheminDossier, nomFichier);
                  removeEmptyLinesCSV(nomFichier);
                  lectureEtInsertionData(db, cheminFichierCSV);

            }
          }
      }
      closedir (dir);
    }
    else { // could not open directory
      perror ("");
      return EXIT_FAILURE;
    }

    selectData(db); // demande monofluide

    sqlite3_close_v2(&db);

    return 0;
}