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
              if (nomFichier[strlen(nomFichier)-5] != '_' && nomFichier[strlen(nomFichier)-5] != '2' && nomFichier[strlen(nomFichier)-5] != '1' && nomFichier[strlen(nomFichier)-5] != '7' && nomFichier[0] != '_' && nomFichier[3] != 'f' && nomFichier[4] != 'B') {

                  char batiment[80] = "";
                  char fluide[40]   = "";
                  char partie1[40]  = "";
                  char partie2[40]  = "";
                  char partie3[40]  = "";

                  sscanf(cheminFichier, "../Relevés/%[^_]_%[^_.]_%[^._].xls", &partie1, &partie2, &partie3);

                  if (partie3[0]=='\0') { // type BAT_FLUIDE.xls
                      snprintf(batiment, sizeof(batiment), "%s", partie1);
                      snprintf(fluide, sizeof(fluide), "%s", partie2);
                  }
                  else { // TYPE BAT1_BAT2_FLUIDE.xls
                      snprintf(batiment, sizeof(batiment), "%s_%s", partie1, partie2);
                      snprintf(fluide, sizeof(fluide), "%s", partie3);
                  }

                  snprintf(cheminFichierCSV, sizeof(cheminFichierCSV), "%s%s%s", "../Générés/", nomFichier, ".txt");

                  XLStoCSV(cheminDossier, nomFichier);
                  removeEmptyLinesCSV(nomFichier);
                  lectureEtInsertionData(db, cheminFichierCSV);

          }
              //printf("%d\n", strlen(cheminFichier)); // pour vérifier la longueur des fichiers
          }
      }
      closedir (dir);
    }
    else { /* could not open directory */
      perror ("");
      return EXIT_FAILURE;
    }

   sqlite3_close_v2(&db);

    demanderUtilisateurMonoFluide(db);
    return 0;
}
