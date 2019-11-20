#include <stdlib.h>
#include <stdio.h>
#include "io.h"
struct Team * loadTable(FILE *fp, int *qTeams)
{
    /* Возвращает массив структур и записывает кол-во команд во второй аргумент */
    struct Team * teams = malloc(1 * sizeof *teams);
    int teamCount = 0; /* Тек.кол-во комманд */
    char tmp[256];
    fseek(fp,0,SEEK_END);
    long int isNotEmpty=ftell(fp);
    rewind(fp);
    while(!feof(fp))
    {
        int elementsCount = 1;
        if(fscanf(fp, "%[^\n]\n", tmp) != 0)
        {
            for(int i = 0; i < (int)strlen(tmp); i++)
            {
                if(tmp[i] == ' ')
                {
                    elementsCount++;
                }
            }
            char * tmpName = strtok(tmp, " ");
            if (isNotEmpty){strcpy(teams[teamCount].teamName, tmpName);}
            else strcpy(teams[teamCount].teamName,"Team_name");

            teams[teamCount].teamNum = teamCount;
            int matchesCount = elementsCount - 1; /* Кол-во матчей = кол-во элементов - 1 элемент */
            teams[teamCount].score = 0;
            teams[teamCount].wins = 0;
            teams[teamCount].loses = 0;
            teams[teamCount].matches = malloc((unsigned int)matchesCount * sizeof *teams[teamCount].matches); /* Выделяет память под массив матчей */
            for(int i = 0; i < matchesCount; i++)
            {
                char * match = strtok(NULL, " "); /* NULL показывает то, что мы продолжаем работать с прошлой строкой */
                teams[teamCount].matches[i] = getGoalsByString(match);
                int score = getScore(teams[teamCount].matches[i]);
                if(score!=4)teams[teamCount].score += score;
                if(score == 3) teams[teamCount].wins++;
                else if(score==0) teams[teamCount].loses++;
            }
            teamCount++;
            teams = realloc(teams, (unsigned int)(teamCount+1) * sizeof *teams);

        }
    }
    *qTeams = teamCount;
    return teams;
}
