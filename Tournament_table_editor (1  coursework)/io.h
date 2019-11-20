#ifndef IO_H
#define IO_H
#include <stdio.h>
#include "structures.h"
#include "dataperform.h"
struct Team * loadTable(FILE *fp, int *qTeams);
void saveTable(FILE *fp);
#endif // IO_H
