#ifndef GUI_H
#define GUI_H
#include <gtk/gtk.h>
#include "structures.h"
#include "misc.h"
#include "dataperform.h"
struct WidgetsGtk * initGrid(GtkWidget * window, struct Team * teams, int * teamCount);
void savefile(GtkWidget * widget, gint response, gpointer d,gpointer widgets,gpointer teams);
void onTextChanged(GtkWidget * textEntry);
void onNameChanged(GtkWidget * textEntry);
void showNameErrorDialog();
void onTeamAdd(GtkWidget * widget, gint response, gpointer d);
void deleteTeam(int teamNum, gpointer d);
void showDeleteErrorDialog(GtkWidget *parentDialog);
void showDeleteDialog(GtkWidget * widget, gint response, gpointer d);
gint CloseAppWindow (void);
struct Data *setSignals(GtkWidget * window, struct WidgetsGtk * widgets, struct Team * teams, int *teamCount);
#endif // GUI_H
