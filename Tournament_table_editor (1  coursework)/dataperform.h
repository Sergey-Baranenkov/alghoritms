#ifndef DATAPERFORM_H
#define DATAPERFORM_H
#include <stdio.h>
#include <stdlib.h>
#include "misc.h"
#include "structures.h"
int getScore(struct Match match); /* Определяет кол-во очков в матче */
int pairscore(int i);
int getTeamNum(const gchar * sourceNum, int teamCount);
struct Match getGoalsByString(const char goalsString[32]); /* Парсит строку на забитые и пропущенные голы */
#endif // DATAPERFORM_H
