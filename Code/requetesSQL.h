#include "sqlite3.h"

void openBDD(sqlite3* db);
void createTableBatiment(sqlite3 *db);
void requeteModele(sqlite3 *db, char *requete, char *intitule);
