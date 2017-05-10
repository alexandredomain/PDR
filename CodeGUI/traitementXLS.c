#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
// Librairie LIBXLS
#include "xls.h"


void removeEmptyLinesCSV(char nomFichier[]) {
    FILE * fichierCSVtemp = NULL;
    FILE * fichierCSV = NULL;

    char cheminFichierCSVtemp[90];
    snprintf(cheminFichierCSVtemp, sizeof(cheminFichierCSVtemp), "%s%s-Temp.txt", "../Générés/", nomFichier);

    char cheminFichierCSV[80];
    snprintf(cheminFichierCSV, sizeof(cheminFichierCSV), "%s%s.txt", "../Générés/", nomFichier);

    fichierCSVtemp = fopen(cheminFichierCSVtemp, "rw");
    fichierCSV = fopen(cheminFichierCSV, "wb");

    char line[100];
    int c = 0;

    while(fgets(line, sizeof(line), fichierCSVtemp) != NULL) {      //read each line of the file
        if (line[0] != '\n') {
            fprintf(fichierCSV, "%s", line);
        }
        else {
            c++;
        }
    }
    printf("Suppression de %i lignes vides dans le fichier CSV... Ok.\n", c);

    fclose(fichierCSVtemp);
    remove(cheminFichierCSVtemp);
    fclose(fichierCSV);
}

void XLStoCSV(char cheminDossier[], char nomFichier[]) {
    int i;

    xlsWorkBook * pWB;
    xlsWorkSheet * pWS;

    struct st_row_data * row;
    WORD t, tt;

    FILE * fichierExcel = NULL;
    FILE * fichierCSVtemp = NULL;

    char cheminFichierXLS[70];
    snprintf(cheminFichierXLS, sizeof(cheminFichierXLS), "%s%s", cheminDossier, nomFichier);

    char cheminFichierCSVtemp[80];
    snprintf(cheminFichierCSVtemp, sizeof(cheminFichierCSVtemp), "%s%s-Temp.txt", "../Générés/", nomFichier);

    // Ouverture du fichier Excel
        //const char cheminFichierXLS[50] = "/home/.....xls";
    fichierExcel = fopen(cheminFichierXLS, "r");

    // Affichage d'un message d'erreur si le chemin est incorrecte
    if (fichierExcel == NULL) {
        printf("Impossible d'ouvrir le fichier à l'emplacement:\n%s\n", cheminFichierXLS);
    }

    // Sinon on commence la lecture
    else {
        fclose(fichierExcel); // on ferme le fichier original - si on est là, c'est qu'il existe.

        fichierCSVtemp = fopen(cheminFichierCSVtemp, "wb"); // notre fichier CSV à écrire - s'il n'existe pas, on le crée

        pWB = xls_open(cheminFichierXLS, "UTF-8"); // ouverture du fichier XLS à lire

        // process workbook if found
        if (pWB != NULL) {
            printf("Conversion en cours... ");
            // process all sheets
            for (i = 0; i < pWB -> sheets.count; i++) {
                int lineWritten = 0;

                // open and parse the sheet
                pWS = xls_getWorkSheet(pWB, i);
                xls_parseWorkSheet(pWS);

                // process all rows of the sheet
                for (t = 0; t <= pWS -> rows.lastrow; t++) {
                    row = & pWS -> rows.row[t];

                    // process cells
                    if (lineWritten) {
                        //printf("\n");
                        fprintf(fichierCSVtemp, "\n");
                    }
                    else {
                        lineWritten = 1;
                    }

                    for (tt = 0; tt <= pWS -> rows.lastcol; tt++) {
                        if (!row -> cells.cell[tt].ishiden) {
                            // display the colspan as only one cell, but reject rowspans (they can't be converted to CSV)
                            if (row -> cells.cell[tt].rowspan > 1) {
                                //printf("%d,%d: rowspan=%i", tt, t, row -> cells.cell[tt].rowspan);
                                fprintf(fichierCSVtemp, "%d,%d: rowspan=%i", tt, t, row -> cells.cell[tt].rowspan);
                            }

                            // display the value of the cell (either numeric or string)
                            if (row -> cells.cell[tt].id == 0x27e || row -> cells.cell[tt].id == 0x0BD || row -> cells.cell[tt].id == 0x203) {
                                //printf("%.15g;", row -> cells.cell[tt].d);
                                fprintf(fichierCSVtemp, "%.15g;", row -> cells.cell[tt].d);
                            }
                        }
                    }
                }
            }
            fclose(fichierCSVtemp);
            xls_close(pWB);
            printf("Conversion terminée.\n");
        }
    }
}
