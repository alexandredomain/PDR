#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
// Librairie SQLite3
#include "sqlite3.h"


void versionSQLite() {
    printf("SQLite_version %s\n", sqlite3_libversion());
}

void openBDD(sqlite3 *db) {
    int codeRetour = 0;

    // remove("../Générés/maBaseDeDonnees"); // pour débug à supprimer ensuite

    // ouverture (ou création si n'existe pas) de la base de données
    codeRetour = sqlite3_open_v2("../Générés/maBaseDeDonnees", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);

    if (codeRetour!=0) { //affiche une erreur sur la console qui explique pourquoi la base de données n'a pas pu être ouverte)
        printf("Impossible d'ouvrir la base de données : %s\n", sqlite3_errmsg(db));
        sqlite3_close_v2(db); //on ferme la base de données afin de libérer la mémoire
    }
    else {
        printf("Ouverture de la base de données... Ok.\n");
        sqlite3_close_v2(db);
    }
}

void requeteModele(sqlite3 *db, char *requete, char *intitule) {
    int codeRetour = 0;
    char *feedbackErrorSQL = NULL;
    // Mise en forme du message de console
    char prefix[100] = "";
    strcat(prefix, intitule);
    strcat(prefix, "... ");

    codeRetour = sqlite3_open_v2("../Générés/maBaseDeDonnees", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);

    if (codeRetour!=0) { //affiche une erreur sur la console qui explique pourquoi la base de données n'a pas pu être ouverte)
        printf("Impossible d'ouvrir la base de données : %s\n", sqlite3_errmsg(db));
        sqlite3_close_v2(db); //on ferme la base de données afin de libérer la mémoire
    }

    codeRetour = sqlite3_exec(db, requete, NULL, 0, &feedbackErrorSQL);
    printf("Code Retour dans la fonction requeteModele : %d --> %s\n", codeRetour, sqlite3_errmsg(db));

    if (codeRetour && feedbackErrorSQL != NULL){
        printf(strcat(prefix, "Erreur : "));
        printf(feedbackErrorSQL);
        printf("\n");
        sqlite3_free(feedbackErrorSQL);
        feedbackErrorSQL = NULL;
    }

    else {
        printf(strcat(prefix, "Ok.\n"));
    }
    sqlite3_close_v2(db);
}

void createTableListeBatiments(sqlite3 *db) {
    requeteModele(db, "CREATE TABLE IF NOT EXISTS BATIMENTS (id TEXT PRIMARY KEY, site TEXT, surface INTEGER, nom TEXT);", "Création de la table \"BATIMENTS\"");
}

void createTableBatiment(sqlite3 *db, char *nom) {
    char *requete = NULL;
    asprintf(&requete, "CREATE TABLE IF NOT EXISTS %s (fluide TEXT, valeur REAL, date TEXT);", nom);
    char *intitule=NULL;
    asprintf(&intitule, "Création de la table \"%s\"", nom);
    requeteModele(db, requete, intitule);
}

void insertBatiment(sqlite3 *db, char *id, char *site, char *surface, char *nom) {
    char *requete = NULL;
    asprintf(&requete, "INSERT INTO BATIMENTS (id, site, surface, nom) VALUES ('%s', '%s', '%s', '%s');", id, site, surface, nom);
    char *intitule=NULL;
    asprintf(&intitule, "Insertion d'un nouveau batiment \"%s\" dans la table \"batiments\"", id);
    requeteModele(db, requete, intitule);
}

void insertDataBatiment(sqlite3 *db, char *site, char *nom_fluide, char *valeur, char *jour) {
    char *requete = NULL;
    asprintf(&requete, "INSERT INTO %s (fluide, valeur, date) SELECT '%s', '%s', date('1899-12-30', '+%s day') WHERE NOT EXISTS(SELECT 1 FROM %s WHERE fluide = '%s' AND valeur = '%s' AND date = date('1899-12-30', '+%s day'));", site, nom_fluide, valeur, jour, site, nom_fluide, valeur, jour);
    char *intitule=NULL;
    asprintf(&intitule, "Insertion d'une nouvelle ligne à la table \"%s\"", site);
    requeteModele(db, requete, intitule);
}

void update(sqlite3 *db, char* table, char *champ, char *value, char *condition_champ, char *condition_value) {
    char *requete = NULL;
    asprintf(&requete, "UPDATE %s SET %s = '%s' WHERE lower(%s) = lower('%s');", table, champ, value, condition_champ, condition_value);
    char *intitule=NULL;
    asprintf(&intitule, "Update de la table \"%s\" : %s %s = %s", table, champ, condition_value, value);
    requeteModele(db, requete, intitule);
}


int actualiserBatimentsEtSurfaces(sqlite3 *db) {
    FILE* fichierCSVSurfaces = fopen("../Surfaces.csv", "r");

    sqlite3_open_v2("../Générés/maBaseDeDonnees", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);

    if (fichierCSVSurfaces == NULL) { // pas de fichier Surfaces.csv --> rien à faire
        printf("Pas d'actualisation des batiments et de leurs surfaces\n");
    }

    // Sinon on commence la lecture
    else {

        char line[160];
        char nom_site[50];
        char id_batiment[50];
        char nom_batiment[70];
        char surface[50];

        int codeRetour;
        int isPresentInTableBatiments = -1;

        fgets(line, 160, fichierCSVSurfaces); // on passe la première qui ne sert à rien (titres de colonnes)
        fgets(line, 160, fichierCSVSurfaces); // On met la deuxième ligne dans la variable line

        while (!feof(fichierCSVSurfaces))  { // tant que le fichier n'est pas fini
            sscanf(line, "%[^;];%[^;];%[^;];%[A-Za-zéèêàù&0-9_- ]", &nom_site, &id_batiment, &surface, &nom_batiment); // récupération des données de la ligne

            sqlite3_stmt *requete;
            char* sqlSELECT = "";
            asprintf(&sqlSELECT,"SELECT COUNT(*) FROM batiments WHERE lower(id) = lower('%s')", id_batiment);
            codeRetour = sqlite3_prepare_v2(db, sqlSELECT, strlen(sqlSELECT), &requete, NULL);
            printf("%s\n", sqlSELECT);
            printf("%d\n", codeRetour);

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
                insertBatiment(db, id_batiment, nom_site, surface, nom_batiment);
                createTableBatiment(db, id_batiment);
            }
            else if (isPresentInTableBatiments == 1) {
                update(db, "batiments", "surface", surface, "id", id_batiment);
            }
            fgets(line, 160, fichierCSVSurfaces); // ligne suivante
        }

        fclose(fichierCSVSurfaces);
        // remove("../Surfaces.csv"); // à décommenter
        return 1;
    }
    sqlite3_close_v2(db);
    return 0;
}

int lectureEtInsertionData(sqlite3 *db, char fichier[]){
    FILE *fichierCSV = fopen(fichier, "r");

    if (fichierCSV != NULL) {

        char line[160];
        char batiment[160]= "";
        char fluide[40]   = "";
        char date[50]     = "";
        char valeur[50]   = "";
        char partie1[40]  = "";
        char partie2[40]  = "";
        char partie3[40]  = "";
        char partie4[40]  = "";
        char partie5[40]  = "";



        sscanf(fichier, "../Générés/%[^_]_%[^_.]_%[^_.]_%[^_.]_%[^_.].xls.txt", &partie1, &partie2, &partie3, &partie4, &partie5);

        if (partie3[0]=='\0' || isdigit(partie3[0])) { // type BAT_FLUIDE(_20000101).xls
            snprintf(batiment, sizeof(batiment), "%s", partie1);
            snprintf(fluide, sizeof(fluide), "%s", partie2);
        }
        else if (partie4[0]=='\0' || isdigit(partie4[0])) { // type BAT1_BAT2_FLUIDE(_20000101).xls
            snprintf(batiment, sizeof(batiment), "%s_%s", partie1, partie2);
            snprintf(fluide, sizeof(fluide), "%s", partie3);
        }
        else if (partie5[0]=='\0' || isdigit(partie5[0])) {
            snprintf(batiment, sizeof(batiment), "%s_%s_%s", partie1, partie2, partie3);
            snprintf(fluide, sizeof(fluide), "%s", partie4);
        }
        else {
            snprintf(batiment, sizeof(batiment), "%s_%s_%s_%s", partie1, partie2, partie3, partie4);
            snprintf(fluide, sizeof(fluide), "%s", partie5);
        }

        fgets(line, 160, fichierCSV); // on passe la première qui ne sert à rien (total)
        fgets(line, 160, fichierCSV); // On passe la moyenne
        fgets(line, 160, fichierCSV);

        while (!feof(fichierCSV))  {
             sscanf(line, "%[^;];%[^;];", &date, &valeur); // récupération des données de la ligne
             insertDataBatiment(db, batiment, fluide, valeur, date);
             fgets(line, 160, fichierCSV);
        }
        if (feof(fichierCSV)) {
            printf("Fin des entrées dans la BDD\n");
        }
        fclose(fichierCSV); // on ferme le fichier CSV

        remove(fichier); // on supprimer le CSV car ne sert plus à rien
        return 1;
    }
    return 0;
}


void chercherDonneesMonoFluide(sqlite3 *db, char *nomBatiment, char *nomFluide, char *date){
    int codeRetour;
    sqlite3_stmt *requete;
    char* sqlSELECT = "";
    asprintf(&sqlSELECT,"SELECT %s FROM %s WHERE fluide = '%s' AND date = '%s'", "valeur", nomBatiment, nomFluide, date);
    codeRetour = sqlite3_prepare_v2(db, sqlSELECT, strlen(sqlSELECT), &requete, NULL);
    if (!codeRetour){
        //la préparation s'est bien déroulée on peut maintenant récupérer les résultats
        while (codeRetour == SQLITE_OK || codeRetour == SQLITE_ROW) { //tant qu'il y a des lignes disponibles on récupère ligne par ligne le résultat et on affiche les colonnes
            codeRetour = sqlite3_step(requete); //on récupère une ligne dans la table
            if (codeRetour == SQLITE_OK || codeRetour == SQLITE_ROW){
                printf("Consommation d'%s du batîment %s le %s : %f\n",nomFluide, nomBatiment, date,sqlite3_column_double(requete, 0));
            }
        }
        sqlite3_finalize(requete); // libère les chaines "sqlite3_column_text" éventuellement à chaque appel de appel de "sqlite3_step" ou "sqlite3_finalize"
    }
}



double selectData(sqlite3 *db){
    int i=0;
    int codeRetour;
    int numchoisi; // sert de selection des choix affichés dans la console

    sqlite3_stmt *requete;
    char* sqlSELECT = "";

    //******************************************************************//
    printf("******************** SELECTION BATIMENT ********************\n\n");
    //******************************************************************//

    asprintf(&sqlSELECT,"SELECT nom, id FROM batiments ORDER BY nom ASC");
    int compteur = 1;
    codeRetour = sqlite3_prepare_v2(db, sqlSELECT, strlen(sqlSELECT), &requete, NULL);
    if (!codeRetour){
        while (codeRetour == SQLITE_OK || codeRetour == SQLITE_ROW) { //tant qu'il y a des lignes disponibles on récupère ligne par ligne le résultat et on affiche les colonnes
            codeRetour = sqlite3_step(requete); //on récupère une ligne dans la table
            if (codeRetour == SQLITE_OK || codeRetour == SQLITE_ROW){
                printf("- %d : %s\n", compteur, sqlite3_column_text(requete, 0));
                compteur++;
            }
        }
        sqlite3_finalize(requete); // libère les chaines "sqlite3_column_text" éventuellement à chaque appel de appel de "sqlite3_step" ou "sqlite3_finalize"
    }

    char *id_batiment;
    printf("Sélectionnez le numéro correspondant au bâtiment voulu dans la liste ci-dessus :\n");
    scanf("%d", &numchoisi);

    asprintf(&sqlSELECT,"SELECT id FROM batiments ORDER BY nom ASC LIMIT 1 OFFSET %d", (numchoisi-1));
    codeRetour = sqlite3_prepare_v2(db, sqlSELECT, strlen(sqlSELECT), &requete, NULL);
    if (!codeRetour){
        if (codeRetour == SQLITE_OK || codeRetour == SQLITE_ROW) { //tant qu'il y a des lignes disponibles on récupère ligne par ligne le résultat et on affiche les colonnes
            codeRetour = sqlite3_step(requete); //on récupère une ligne dans la table
            if (codeRetour == SQLITE_OK || codeRetour == SQLITE_ROW){
                id_batiment = sqlite3_column_text(requete, 0);
            }
        }
    }

    //******************************************************************//
    printf("******************** SELECTION FLUIDE ********************\n\n");
    //******************************************************************//

    asprintf(&sqlSELECT,"SELECT fluide FROM %s GROUP BY fluide ORDER BY fluide ASC", id_batiment);
    compteur=1;
    codeRetour = sqlite3_prepare_v2(db, sqlSELECT, strlen(sqlSELECT), &requete, NULL);
    if (!codeRetour){
        while (codeRetour == SQLITE_OK || codeRetour == SQLITE_ROW) { //tant qu'il y a des lignes disponibles on récupère ligne par ligne le résultat et on affiche les colonnes
            codeRetour = sqlite3_step(requete); //on récupère une ligne dans la table
            if (codeRetour == SQLITE_OK || codeRetour == SQLITE_ROW){
                printf("- %d : %s\n", compteur, sqlite3_column_text(requete, 0));
                compteur++;
            }
        }
        sqlite3_finalize(requete); // libère les chaines "sqlite3_column_text" éventuellement à chaque appel de appel de "sqlite3_step" ou "sqlite3_finalize"
    }

    char *id_fluide;
    printf("Sélectionnez le numéro correspondant au bâtiment voulu dans la liste ci-dessus :\n");
    scanf("%d", &numchoisi);


    asprintf(&sqlSELECT,"SELECT fluide FROM %s GROUP BY fluide ORDER BY fluide ASC LIMIT 1 OFFSET %d", id_batiment, (numchoisi-1));
    codeRetour = sqlite3_prepare_v2(db, sqlSELECT, strlen(sqlSELECT), &requete, NULL);
    if (!codeRetour){
        if (codeRetour == SQLITE_OK || codeRetour == SQLITE_ROW) { //tant qu'il y a des lignes disponibles on récupère ligne par ligne le résultat et on affiche les colonnes
            codeRetour = sqlite3_step(requete); //on récupère une ligne dans la table
            if (codeRetour == SQLITE_OK || codeRetour == SQLITE_ROW){
                id_fluide = sqlite3_column_text(requete, 0);
            }
        }
    }

    //******************************************************************//
    printf("******************** SELECTION DATE ********************\n\n");
    //******************************************************************//

    char date[15];
    printf("Sélectionnez la date voulue (au format \"AAAA-MM-JJ\")\n");
    scanf("%s", date);

    double valeur = 0.; // valeur à retourner

    asprintf(&sqlSELECT,"SELECT %s FROM %s WHERE fluide = '%s' AND date = '%s'", "valeur", id_batiment, id_fluide, date);
    codeRetour = sqlite3_prepare_v2(db, sqlSELECT, strlen(sqlSELECT), &requete, NULL);
    if (!codeRetour){
        if (codeRetour == SQLITE_OK || codeRetour == SQLITE_ROW) { //tant qu'il y a des lignes disponibles on récupère ligne par ligne le résultat et on affiche les colonnes
            codeRetour = sqlite3_step(requete); //on récupère une ligne dans la table
            if (codeRetour == SQLITE_OK || codeRetour == SQLITE_ROW){
                valeur = sqlite3_column_double(requete, 0);
                printf("\n------------------->\n");
                printf("Consommation d'%s du batîment %s le %s : %f\n", id_fluide, id_batiment, date, valeur);
                printf("------------------->\n");
            }
        }
        sqlite3_finalize(requete); // libère les chaines "sqlite3_column_text" éventuellement à chaque appel de appel de "sqlite3_step" ou "sqlite3_finalize"
    }

    return valeur;
}

double getSurface(sqlite3 *db, char *id_batiment) {
    sqlite3_stmt *requete;
    char* sqlSELECT = "";
    int codeRetour;

    double surface;

    asprintf(&sqlSELECT,"SELECT surface FROM batiments WHERE lower(id)=lower('%s')", id_batiment);
    printf("id bat %s\n", id_batiment);
    printf("requete %s\n", sqlSELECT);
    codeRetour = sqlite3_prepare_v2(db, sqlSELECT, strlen(sqlSELECT), &requete, NULL);
    if (!codeRetour){
        if (codeRetour == SQLITE_OK || codeRetour == SQLITE_ROW) { //tant qu'il y a des lignes disponibles on récupère ligne par ligne le résultat et on affiche les colonnes
            codeRetour = sqlite3_step(requete); //on récupère une ligne dans la table
            if (codeRetour == SQLITE_OK || codeRetour == SQLITE_ROW){
                surface = sqlite3_column_double(requete, 0);
            }
        }
        sqlite3_finalize(requete); // libère les chaines "sqlite3_column_text" éventuellement à chaque appel de appel de "sqlite3_step" ou "sqlite3_finalize"
    }
    else {
        return -1;
    }

    return surface;
}

double DPE(sqlite3 *db, char *id_batiment, int AAAA_1, int MM_1, int JJ_1, int AAAA_2, int MM_2, int JJ_2){

    char * date_debut;
    char * date_fin;

    asprintf(&date_debut, "%04d-%02d-%02d", AAAA_1, MM_1, JJ_1);
    asprintf(&date_fin, "%04d-%02d-%02d", AAAA_2, MM_2, JJ_2);


    char *fluide[] = {
        "Elec",
        "Gaz",
        "Fioul",
        //"Eau", pas utile pr le DPE
        //"CO2", pas utile pr le DPE
    };
    int nb_fluides = 3; // à faire correspondre avec les fluides ci-dessus



    // Calcul

    double valeur_moyenne_conso;
    int nb_jours;
    double surface = getSurface(db, id_batiment);
    double kWhEP;

    int i;
    sqlite3_stmt *requete;
    char* sqlSELECT;

    int codeRetour;    for (i=0; i<nb_fluides; i++) {
        sqlSELECT = "";
        asprintf(&sqlSELECT,"SELECT AVG(valeur), COUNT(valeur) FROM %s WHERE fluide='%s' AND date >= '%s' AND date <= '%s'", id_batiment, fluide[i], date_debut, date_fin);
        printf("%s\n", sqlSELECT);
        int compteur = 1;
        codeRetour = sqlite3_prepare_v2(db, sqlSELECT, strlen(sqlSELECT), &requete, NULL);
        if (!codeRetour){
            while (codeRetour == SQLITE_OK || codeRetour == SQLITE_ROW) { //tant qu'il y a des lignes disponibles on récupère ligne par ligne le résultat et on affiche les colonnes
                codeRetour = sqlite3_step(requete); //on récupère une ligne dans la table
                if (codeRetour == SQLITE_OK || codeRetour == SQLITE_ROW){
                    valeur_moyenne_conso = sqlite3_column_double(requete, 0);
                    nb_jours =  sqlite3_column_int(requete, 1);
                    if (nb_jours != 0) {
                        if (fluide[i] = "Elec") {
                            kWhEP += 2.58*valeur_moyenne_conso;
                        }
                        else {
                            kWhEP += valeur_moyenne_conso;
                        }
                    }
                }
            }
            sqlite3_finalize(requete); // libère les chaines "sqlite3_column_text" éventuellement à chaque appel de appel de "sqlite3_step" ou "sqlite3_finalize"
        }

    }

    double DPE = (kWhEP*365.25)/surface;

    return DPE;
}

int writeDataToCSV(sqlite3 *db, char *id_batiment, char *id_fluide, int AAAA_1, int MM_1, int JJ_1, int AAAA_2, int MM_2, int JJ_2){

    char *date_debut;
    asprintf(&date_debut, "%04d-%02d-%02d", AAAA_1, MM_1, JJ_1);
    char *date_fin;
    asprintf(&date_fin, "%04d-%02d-%02d", AAAA_2, MM_2, JJ_2);

    sqlite3_stmt *requete;
    char* sqlSELECT;
    int codeRetour;


    FILE * fichierCSV = NULL;
    char cheminFichierCSV[80];
    snprintf(cheminFichierCSV, sizeof(cheminFichierCSV), "%sExport_%s_%s(%s-%s).csv", "../Générés/", id_batiment, id_fluide, date_debut, date_fin);
    fichierCSV = fopen(cheminFichierCSV, "wb"); // notre fichier CSV à écrire - s'il n'existe pas, on le crée

    fprintf(fichierCSV, "%s pour le batiment %s;Date;%s\n", id_fluide, id_batiment, id_fluide);

    sqlite3_open_v2("../Générés/maBaseDeDonnees", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);


    asprintf(&sqlSELECT,"SELECT date, valeur FROM %s WHERE fluide = '%s' AND date >= '%s' AND date <= '%s' ORDER BY date ASC", id_batiment, id_fluide, date_debut, date_fin);
    codeRetour = sqlite3_prepare_v2(db, sqlSELECT, strlen(sqlSELECT), &requete, NULL);
    printf("%d\n", codeRetour);
    if (!codeRetour){
        while (codeRetour == SQLITE_OK || codeRetour == SQLITE_ROW) { //tant qu'il y a des lignes disponibles on récupère ligne par ligne le résultat et on affiche les colonnes
            codeRetour = sqlite3_step(requete); //on récupère une ligne dans la table
            if (codeRetour == SQLITE_OK || codeRetour == SQLITE_ROW){
                fprintf(fichierCSV, ";%s;%.2f\n", sqlite3_column_text(requete, 0), sqlite3_column_double(requete, 1));
            }
        }
        sqlite3_finalize(requete); // libère les chaines "sqlite3_column_text" éventuellement à chaque appel de appel de "sqlite3_step" ou "sqlite3_finalize"
    }
    fclose(fichierCSV);
    printf("\n\nExport_%s_%s(%s-%s).csv a bien été créé dans le dossier \"Générés\"\n\n", id_batiment, id_fluide, date_debut, date_fin);
    sqlite3_close_v2(db);
    return 1;
}


