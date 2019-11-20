#ifndef STRUCTURES_H
#define STRUCTURES_H
#include <gtk/gtk.h>
struct Team
{
    char teamName[32];
    int score;
    int wins;
    int loses;
    int teamNum;
    struct Match
    {
        signed int goals;
        signed int missedGoals;
    } *matches;
};
struct Data
{
    int * teamCount;
    struct Team * teams;
    struct WidgetsGtk * widgets;
    GtkWidget * window;
    char fpname[256];
};
struct WidgetsGtk
{
    GtkWidget *grid; /* Таблица */
    GtkWidget ***dataEdit; /* Двумерный массив под textEntry */
    GtkWidget **title; /* Массив заголовков для столбцов */
    GtkWidget ** scoreLabels;
    GtkWidget ** winsLabels;
    GtkWidget *logButton;
    GtkWidget *saveButton;
    GtkWidget *savefileButton;
    GtkWidget *addTeamButton;
    GtkWidget *deleteTeamButton;
};
#endif // STRUCTURES_H
