#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
// Librairie LIBXLS
#include "xls.h"


void removeEmptyLines() {
    FILE * fichierCSV = NULL;
    FILE * fichierCSVfinal = NULL;

    fichierCSV = fopen("../Générés/conversionCSV-temp.txt", "rw");
    fichierCSVfinal = fopen("../Générés/conversionCSV.txt", "wb");
    int c = 0;
    char line[100];
    while(fgets(line, sizeof(line), fichierCSV) != NULL) {      //read each line of the file
        if (line[0] != '\n') {
            fprintf(fichierCSVfinal, "%s", fgets(line, sizeof(line), fichierCSV));
        }
        else {
            c++;
        }
    }
    printf("Suppression de %i lignes vides dans le fichier CSV...\n", c);
    fclose(fichierCSV);
    fclose(fichierCSVfinal);
    remove("../Générés/conversionCSV-temp.txt");

}

void XLStoCSV (char * cheminFichierExcel) {
    int i;

    xlsWorkBook * pWB;
    xlsWorkSheet * pWS;

    struct st_row_data * row;
    WORD t, tt;

    FILE * fichierExcel = NULL;
    FILE * fichierCSV = NULL;

    // Ouverture du fichier Excel
    //const char cheminFichierExcel[50] = "/home/.....xls";
    fichierExcel = fopen(cheminFichierExcel, "r");

    // Affichage d'un message d'erreur si le chemin est incorrecte
    if (fichierExcel == NULL) {
        printf("Impossible d'ouvrir le fichier à l'emplacement:\n%s\n", cheminFichierExcel);
    }

    // Sinon on commence la lecture
    else {
        fclose(fichierExcel); // on ferme le fichier original - si on est là, c'est qu'il existe.

        fichierCSV = fopen("../Générés/conversionCSV-temp.txt", "wb"); // notre fichier CSV à écrire - s'il n'existe pas, on le crée

        pWB = xls_open(cheminFichierExcel, "UTF-8"); // ouverture du fichier XLS à lire

        // process workbook if found
        if (pWB != NULL) {

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
                        fprintf(fichierCSV, "\n");
                    }
                    else {
                        lineWritten = 1;
                    }

                    for (tt = 0; tt <= pWS -> rows.lastcol; tt++) {
                        if (!row -> cells.cell[tt].ishiden) {
                            // display the colspan as only one cell, but reject rowspans (they can't be converted to CSV)
                            if (row -> cells.cell[tt].rowspan > 1) {
                                //printf("%d,%d: rowspan=%i", tt, t, row -> cells.cell[tt].rowspan);
                                fprintf(fichierCSV, "%d,%d: rowspan=%i", tt, t, row -> cells.cell[tt].rowspan);
                            }

                            // display the value of the cell (either numeric or string)
                            if (row -> cells.cell[tt].id == 0x27e || row -> cells.cell[tt].id == 0x0BD || row -> cells.cell[tt].id == 0x203) {
                                //printf("%.15g;", row -> cells.cell[tt].d);
                                fprintf(fichierCSV, "%.15g;", row -> cells.cell[tt].d);
                            }
                        }
                    }
                }
            }
            fclose(fichierCSV);
            xls_close(pWB);
        }
    }
}
