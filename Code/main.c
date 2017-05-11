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

    printf("********************** Ajout des données dans la base *********************\n");

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
                nomFichier[strlen(nomFichier)-5] != '_' // cas des fichiers mensuels "batiment_fluide(_MMAAAA)_.xls"
                &&
                nomFichier[0] != '_' // cas du fichier _ProdNRJ
                &&
                nomFichier[0] != 'S' // cas des fichiers "site" SB_ ou SL_ inutiles pour nous (batiments par batiments)
                &&
                !(nomFichier[strlen(nomFichier)-9] == '_' && isdigit(nomFichier[strlen(nomFichier)-8]) && nomFichier[strlen(nomFichier)-8] == '2') // cas des fichiers mensuels des mois précédents "batiment_fluide_2017.xls"
                ) {
                  snprintf(cheminFichierCSV, sizeof(cheminFichierCSV), "%s%s%s", "../Générés/", nomFichier, ".txt");

                  //printf("--------------------------------------- ------------------------------------\n");
                  //printf("%s\n", cheminFichier);

                  XLStoCSV(cheminDossier, nomFichier);
                  removeEmptyLinesCSV(nomFichier);
                  lectureEtInsertionData(db, cheminFichierCSV);
              }
              //remove(cheminFichier);
          }
      }
      closedir (dir);
    }
    else { // could not open directory
      perror ("");
      return EXIT_FAILURE;
    }
    printf("Terminé. Les données ont été ajoutées.\n");
    printf("***************************************************************************\n\n");



    // Partie interaction avec l'utilisateur : choix de faire un DPE ou un export
    printf("********************** Choix d'une action à effectuer *********************\n");

    int userChoice = 0;

    while (userChoice != 1 && userChoice != 2 && userChoice != 3) {
        printf("Sélectionnez l'action que vous voulez effectuer :\n"
                "1 : Calculer un DPE\n"
                "2 : Effectuer un export de données\n"
                "3 : Arrêter le programme\n");
        scanf("%d", &userChoice);
    }

    switch (userChoice) {
    case 1:
        DPE(db);
        break;
    case 2:
        writeDataToCSV(db);
        break;
    default:
        printf("***************************************************************************\n\n");
        sqlite3_close_v2(&db);
        return 0;
        break;
    }

    printf("***************************************************************************\n\n");
    sqlite3_close_v2(&db);

    return 0;
}
