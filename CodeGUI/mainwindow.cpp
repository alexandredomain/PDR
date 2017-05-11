#include "mainwindow.h"
#include "ui_mainwindow.h"
extern "C" {
    #include <requetesSQL.h>
    #include <traitementXLS.h>
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->zoneTexte->setVisible(false); //Pas d'utilité pour le moment
}

MainWindow::~MainWindow()
{
    delete ui;
}




void MainWindow::on_SelectBatiment_currentIndexChanged(int index)
{
    sqlite3 *db; // initialisation de la base de données

    const char * resultat ="";
    int codeRetour;
    QString surface;
    sqlite3_stmt *requete;
    char* sqlSELECT = "";
    asprintf(&sqlSELECT,"SELECT fluide FROM %s GROUP BY fluide ", ui->SelectBatiment->currentText().toStdString().c_str());
    ui->SelectFluide->clear();

    codeRetour = sqlite3_open_v2("../Générés/maBaseDeDonnees", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);

    codeRetour = sqlite3_prepare_v2(db, sqlSELECT, strlen(sqlSELECT), &requete, NULL);
    if (!codeRetour){
        //la préparation s'est bien déroulée on peut maintenant récupérer les résultats
        while (codeRetour == SQLITE_OK || codeRetour == SQLITE_ROW) { //tant qu'il y a des lignes disponibles on récupère ligne par ligne le résultat et on affiche les colonnes
            codeRetour = sqlite3_step(requete); //on récupère une ligne dans la table
            if (codeRetour == SQLITE_OK || codeRetour == SQLITE_ROW){
                resultat = (const char*)sqlite3_column_text(requete, 0);
                ui->SelectFluide->addItem(resultat);
            }
        }
    }

    asprintf(&sqlSELECT,"SELECT surface FROM batiments WHERE id = '%s'", ui->SelectBatiment->currentText().toStdString().c_str());
    codeRetour = sqlite3_prepare_v2(db, sqlSELECT, strlen(sqlSELECT), &requete, NULL);
    if (!codeRetour){
        //la préparation s'est bien déroulée on peut maintenant récupérer les résultats
        while (codeRetour == SQLITE_OK || codeRetour == SQLITE_ROW) { //tant qu'il y a des lignes disponibles on récupère ligne par ligne le résultat et on affiche les colonnes
            codeRetour = sqlite3_step(requete); //on récupère une ligne dans la table
            if (codeRetour == SQLITE_OK || codeRetour == SQLITE_ROW){
                surface = QString::number(sqlite3_column_int(requete, 0)) + " m²";
                ui->txtEditSurface->setText(surface);
            }
        }
        sqlite3_finalize(requete); // libère les chaines "sqlite3_column_text" éventuellement à chaque appel de appel de "sqlite3_step" ou "sqlite3_finalize"
    }

    sqlite3_close_v2(db);
}

void MainWindow::on_refreshBatiment_clicked()
{
    sqlite3 *db; // initialisation de la base de données

    const char * resultat ="";
    QString texte;
    QString ajout;
    int codeRetour;
    sqlite3_stmt *requete;
    char* sqlSELECT = "SELECT id FROM batiments ORDER BY id ASC";

    codeRetour = sqlite3_open_v2("../Générés/maBaseDeDonnees", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);

    codeRetour = sqlite3_prepare_v2(db, sqlSELECT, strlen(sqlSELECT), &requete, NULL);
    if (!codeRetour){
        //la préparation s'est bien déroulée on peut maintenant récupérer les résultats
        while (codeRetour == SQLITE_OK || codeRetour == SQLITE_ROW) { //tant qu'il y a des lignes disponibles on récupère ligne par ligne le résultat et on affiche les colonnes
            codeRetour = sqlite3_step(requete); //on récupère une ligne dans la table

            if (codeRetour == SQLITE_OK || codeRetour == SQLITE_ROW){
                resultat = (const char*)sqlite3_column_text(requete, 0);
                ui->SelectBatiment->addItem(resultat);
            }
        }
        sqlite3_finalize(requete); // libère les chaines "sqlite3_column_text" éventuellement à chaque appel de appel de "sqlite3_step" ou "sqlite3_finalize"
    }



    asprintf(&sqlSELECT,"SELECT fluide FROM %s GROUP BY fluide ORDER BY fluide ASC", ui->SelectBatiment->currentText().toStdString().c_str());
    ui->SelectFluide->clear();
    codeRetour = sqlite3_prepare_v2(db, sqlSELECT, strlen(sqlSELECT), &requete, NULL);
    if (!codeRetour){
        //la préparation s'est bien déroulée on peut maintenant récupérer les résultats
        while (codeRetour == SQLITE_OK || codeRetour == SQLITE_ROW) { //tant qu'il y a des lignes disponibles on récupère ligne par ligne le résultat et on affiche les colonnes
            codeRetour = sqlite3_step(requete); //on récupère une ligne dans la table
            if (codeRetour == SQLITE_OK || codeRetour == SQLITE_ROW){
                resultat = (const char*)sqlite3_column_text(requete, 0);
            }
        }
        sqlite3_finalize(requete); // libère les chaines "sqlite3_column_text" éventuellement à chaque appel de appel de "sqlite3_step" ou "sqlite3_finalize"
    }

    sqlite3_close_v2(db);
}

void MainWindow::on_pushButton_clicked()
{
    QString texte;
    sqlite3 *db;
    char * id_batiment;
    asprintf(&id_batiment,"%s", ui->SelectBatiment->currentText().toStdString().c_str());
    char * fluide;
    asprintf(&fluide,"%s", ui->SelectFluide->currentText().toStdString().c_str());
    int AAAA_1;
    AAAA_1 = ui->anneeDebut->toPlainText().toInt();
    int MM_1;
    MM_1 = ui->moisDebut->toPlainText().toInt();
    int JJ_1;
    JJ_1 = ui->jourDebut->toPlainText().toInt();

    int AAAA_2;
    AAAA_2 = ui->anneeFin->toPlainText().toInt();
    int MM_2;
    MM_2 = ui->moisFin->toPlainText().toInt();
    int JJ_2;
    JJ_2 = ui->jourFin->toPlainText().toInt();

    writeDataToCSV(db, id_batiment, fluide, AAAA_1, MM_1, JJ_1, AAAA_2, MM_2, JJ_2 );

    char *date_debut;
    asprintf(&date_debut, "%04d-%02d-%02d", AAAA_1, MM_1, JJ_1);
    char *date_fin;
    asprintf(&date_fin, "%04d-%02d-%02d", AAAA_2, MM_2, JJ_2);
    char *message;
    asprintf(&message,"\n\nExport_%s_%s(%s-%s).csv a bien été créé dans le dossier \"Générés\"\n\n", id_batiment, fluide, date_debut, date_fin);
    texte = ui->zoneDPE->toPlainText();
    texte = texte + message;
    ui->zoneDPE->setText(texte);

}


void MainWindow::on_btnDPE_clicked()
{
    sqlite3 * db;
    sqlite3_open_v2("../Générés/maBaseDeDonnees", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);

    char * id_batiment;
    asprintf(&id_batiment,"%s", ui->SelectBatiment->currentText().toStdString().c_str());

    int AAAA_1;
    AAAA_1 = ui->anneeDebut->toPlainText().toInt();
    int MM_1;
    MM_1 = ui->moisDebut->toPlainText().toInt();
    int JJ_1;
    JJ_1 = ui->jourDebut->toPlainText().toInt();

    int AAAA_2;
    AAAA_2 = ui->anneeFin->toPlainText().toInt();
    int MM_2;
    MM_2 = ui->moisFin->toPlainText().toInt();
    int JJ_2;
    JJ_2 = ui->jourFin->toPlainText().toInt();

    double valueDPE = DPE(db, id_batiment, AAAA_1, MM_1, JJ_1, AAAA_2, MM_2, JJ_2);
    ui->zoneTexte->setText("Le DPE est égal à : " + QString::number(valueDPE) + "\n");

    char *DPE_arrondi;
    asprintf(&DPE_arrondi, "%.2f kWhEP/m²/an", valueDPE);

    QString texte= "";

    char * A = "--------------------- ";
    char * B = "------------------- ";
    char * C = "----------------- ";
    char * D = "--------------- ";
    char * E = "------------- ";
    char * F = "----------- ";
    char * G = "--------- ";

    texte += "\n[0-50]\t|==> A ";
    if (valueDPE <= 50) texte = texte + A + DPE_arrondi;
    texte += "\n[51-90]\t|====> B ";
    if (valueDPE > 51 && valueDPE <= 90) texte = texte + B + DPE_arrondi;
    texte +="\n[91-150]\t|======> C ";
    if (valueDPE > 91 && valueDPE <= 150) texte = texte + C + DPE_arrondi;
    texte +="\n[151-230]\t|========> D ";
    if (valueDPE > 151 && valueDPE <= 230) texte = texte + D + DPE_arrondi;
    texte +="\n[231-330]\t|==========> E ";
    if (valueDPE > 231 && valueDPE <= 330) texte = texte + E + DPE_arrondi;
    texte +="\n[331-450]\t|============> F ";
    if (valueDPE > 331 && valueDPE <= 450) texte = texte + F + DPE_arrondi;
    texte +="\n[> 450]\t|==============> G ";
    if (valueDPE > 450) texte = texte + G + DPE_arrondi;

    ui->zoneDPE->setText(texte);
}
