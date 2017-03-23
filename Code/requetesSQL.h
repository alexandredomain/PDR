#include "sqlite3.h"

void openBDD(sqlite3* db);
void createTableBatiment(sqlite3 *db);
void insertBatiment(sqlite3 *db,  char *nom, int surface, int jour);
void update(sqlite3 *db, char* table, char *champ, char *value);
void updateWithCondition(sqlite3 *db, char* table, char *champ, char *value, char *condition);
void requeteModele(sqlite3 *db, char *requete, char *intitule);
