#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <gtk/gtk.h>
#include "io.h"
#include "gui.h"
int main(int   argc,    char *argv[])
{
    GdkRGBA color;
    FILE *fp;
    char fpname[256];
    char titleName[256];
    printf("Input a formatted tournament file\n\nIf u want to create a new tournament table, just input its name here:\n");
    while(1){
        scanf("%s",fpname);
        if(!strstr(fpname,".txt")){
            printf("\nIt is not even a .txt file!!!\n");
            continue;
        }
        if(strlen(fpname)==4){
            printf("\nYou should name your database!!!\n");
            continue;
        }
        if ((fp = fopen(fpname, "r")) == NULL)
        {
            fp = fopen(fpname,"w+");
            break;
        }else
        {
            break;
        }
    }
    printf("\nIf the program won`t start for few seconds check the database format or your PC characteristics\n");
    int teamCount = 0;
    struct Team * teams = loadTable(fp, &teamCount);
    fclose(fp); /* Закрывает файл */
    sscanf(fpname, "%[^.]", titleName);
    /* Пользовательский интерфейс */
    gtk_init (&argc, &argv);
    /* Отображает виджеты */
    GtkWidget * window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gdk_rgba_parse (&color, "lightgreen");
    gtk_widget_override_background_color(GTK_WIDGET(window),GTK_STATE_FLAG_NORMAL, &color);
    gtk_window_set_title (GTK_WINDOW (window), titleName);
    struct WidgetsGtk * widgets = initGrid(window, teams, &teamCount);
    struct Data * d = setSignals(window,widgets,teams, &teamCount);
    strcpy(d->fpname,fpname);
    /* Запускает поток GTK */
    //system("cls");
    gtk_main ();
    /* Освобождает память */
    for(int i = 0; i < teamCount; i++) free(teams[i].matches);
    free(teams);
    for(int i = 0; i < teamCount; i++) free(widgets->dataEdit[i]);
    free(widgets->dataEdit);
    free(widgets);
    free(d);
    return 0;
}
