#include "sqlite3.h"

void openBDD(sqlite3* db);
void requeteModele(sqlite3 *db, char *requete, char *intitule);
void createTableListeBatiments(sqlite3 *db);
void createTableBatiment(sqlite3 *db, char *nom);
void insertBatiment(sqlite3 *db, char *nom, char *site, char *surface);
void insertDataBatiment(sqlite3 *db, char *site, char *nom_fluide, char *valeur, char *jour);
void update(sqlite3 *db, char* table, char *champ, char *value, char *condition_champ, char *condition_value);
int actualiserBatimentsEtSurfaces(sqlite3 *db);
int lectureEtInsertionData(sqlite3 *db, char fichier[]);

void chercherDonneesMonoFluide(sqlite3 *db, char * nomBatiment, char * nomFluide, char * date);
double selectData(sqlite3 *db);
double getSurface(sqlite3 *db, char *id_batiment);
double DPE(sqlite3 *db);
int writeDataToCSV(sqlite3 *db, char *id_batiment, char *id_fluide, int AAAA_1, int MM_1, int JJ_1, int AAAA_2, int MM_2, int JJ_2);
