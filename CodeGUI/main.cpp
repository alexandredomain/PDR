#include "mainwindow.h"
#include <QApplication>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
extern "C" {
    #include <requetesSQL.h>
    #include <traitementXLS.h>
}
#include <sqlite3.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    sqlite3 * db;
    createTableListeBatiments(db);
    actualiserBatimentsEtSurfaces(db);

    sqlite3_open_v2("../Générés/maBaseDeDonnees", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
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
    sqlite3_close_v2(db);



    MainWindow w;
    w.show();

    return a.exec();
}
