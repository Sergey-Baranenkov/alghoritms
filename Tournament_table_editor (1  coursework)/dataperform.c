#include "dataperform.h"
#include <gtk/gtk.h>
#include "structures.h"
int pairscore(int i){
    switch(i){
        case(3):{
            return 0;
            break;
        }
        case(0):{
            return 3;
            break;
        }
        default:
            return 1;
    }
}
int getScore(struct Match match)  /* Возвращает очки по голам */
{   if((match.goals <0)||(match.missedGoals<0))return 4;
    if(match.goals > match.missedGoals) return 3;
    if(match.goals == match.missedGoals) return 1;
    return 0;
}
int getTeamNum(const gchar * sourceNum, int teamCount) /* Возвращает номер удаляемой комманды или -1, если введенный номер не существует */
{
   char teamNum[32];
   int  num;
   strcpy(teamNum, sourceNum);
   if(isValue(teamNum))
   {
     num = atoi(teamNum);
     if(num <= teamCount) return num;
   }
   return -1;

}
struct Match getGoalsByString(const char goalsString[32]) /* Парсит строку на забитые и пропущенные голы */
{
    struct Match match = {-1, -1};
    if(strstr(goalsString , ":")== NULL){
            return match;
    }
    int i = 0;
    /* Выделяет память под строку */
    char  tmpGoals[10];
    char  tmpMissed[10];
    do
    {
        if(goalsString[i]=='-')return match;
        tmpGoals[i] = goalsString[i];
        i++;
    }
    while(goalsString[i] != ':');
    tmpGoals[i]='\0';
    i++;
    int j = 0;
    do
    {
        if(goalsString[i]=='-')return match;//такого не должно быть но так сказать лучше пере чем недо
        tmpMissed[j] = goalsString[i];
        i++;
        j++;
    }
    while(goalsString[i]);
    tmpMissed[j]='\0';
    /* Конвертирует стоку в число */

    if(isValue(tmpGoals) && isValue(tmpMissed))
    {
        match.goals = atoi(tmpGoals);
        match.missedGoals = atoi(tmpMissed);
    }
    /* Освобождает память */
    return match;
}
