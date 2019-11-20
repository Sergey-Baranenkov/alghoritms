#include "gui.h"
void onTextChanged(GtkWidget * textEntry)
{
    const gchar * str = gtk_entry_get_text(GTK_ENTRY(textEntry)); /* содержимое измененного textEntry */
    GtkWidget * pairEntry = g_object_get_data(G_OBJECT(textEntry), "pair"); /* Парный textEntry */
    struct Team * team = g_object_get_data(G_OBJECT(textEntry), "team"); /* изменяемая команда */
    struct Team * pairTeam = g_object_get_data(G_OBJECT(textEntry), "pairTeam"); /* парная команда */
    struct Data * d = g_object_get_data(G_OBJECT(textEntry), "data"); /* виджеты */
    int score = 0, prevScore = 0;
    int isH = team->teamNum < pairTeam->teamNum;
    if(isH)
    {
        prevScore = getScore(team->matches[pairTeam->teamNum-1]);
        team->matches[pairTeam->teamNum-1] = getGoalsByString(str+'\0');
        score = getScore(team->matches[pairTeam->teamNum-1]);
    }
    else
    {
        prevScore = getScore(team->matches[pairTeam->teamNum]);
        team->matches[pairTeam->teamNum] = getGoalsByString(str+'\0');
        score = getScore(team->matches[pairTeam->teamNum]);
    }

    /* Пересчитываем кол-во очков и побед */
    if((prevScore==3)&&(score==0)){
        team->wins--;
        team->loses++;
        pairTeam->wins++;

        pairTeam->loses--;
        team->score -= 3;
        pairTeam->score += 3;
    }
    else if((prevScore==0)&&(score==3)){
        team->wins++;
        team->loses--;
        pairTeam->wins--;
        pairTeam->loses++;

        team->score += 3;
        pairTeam->score -= 3;
    }else if((prevScore==1)&&(score==3)){
        team->wins++;
        pairTeam->loses++;

        team->score += 3-1;
        pairTeam->score -= 1;
    }else if((prevScore==3)&&(score==1)){
        team->wins--;
        pairTeam->loses--;

        team->score -= 3-1;
        pairTeam->score += 1;
    }else if((prevScore==0)&&(score==1)){
        team->loses--;
        pairTeam->wins--;

        team->score += 1;
        pairTeam->score -= 2;
    }else if((prevScore==1)&&(score==0)){
        team->loses++;
        pairTeam->wins++;

        team->score -= 1;
        pairTeam->score += 2;
    }else if((prevScore==4)&&(score==3)){
        team->score+=score;
        pairTeam->loses++;
        team->wins++;
    }else if((prevScore==4)&&(score==0)){
        pairTeam->score+=3;
        pairTeam->wins++;
        team->loses++;
    }else if((prevScore==4)&&(score==1)){
        team->score+=score;
        pairTeam->score+=score;
    }else if(score==4){
        if(prevScore==3){
            team->wins--;
            team->score-=prevScore;
            pairTeam->loses--;
        }else if(prevScore==0){
            team->loses--;
            pairTeam->score-=pairscore(prevScore);
            pairTeam->wins--;
        }else if(prevScore==1){
            team->score-=prevScore;
            pairTeam->score-=prevScore;
        }
    }
    char wins[32], scoreStr[32];
    itoa(team->score, scoreStr, 10);
    itoa(team->wins, wins, 10);
    gtk_label_set_text(GTK_LABEL(d->widgets->scoreLabels[team->teamNum]), scoreStr);
    gtk_label_set_text(GTK_LABEL(d->widgets->winsLabels[team->teamNum]), wins);
    itoa(pairTeam->score, scoreStr, 10);
    itoa(pairTeam->wins, wins, 10);
    gtk_label_set_text(GTK_LABEL(d->widgets->scoreLabels[pairTeam->teamNum]), scoreStr);
    gtk_label_set_text(GTK_LABEL(d->widgets->winsLabels[pairTeam->teamNum]), wins);

    /* Изменяем парное поле ввода */
    char goals[10], missed[10];
    char * result;
        if(isH)
        {

             pairTeam->matches[team->teamNum].goals = team->matches[pairTeam->teamNum-1].missedGoals;
             pairTeam->matches[team->teamNum].missedGoals = team->matches[pairTeam->teamNum-1].goals;
             itoa(pairTeam->matches[team->teamNum].goals, goals, 10); /* Конвертирует цифровое значение забитых голов в строку */
             itoa(pairTeam->matches[team->teamNum].missedGoals, missed, 10); /* Конвертирует цифровое значение забитых голов в строку */
        }
        else
        {

            pairTeam->matches[team->teamNum-1].goals = team->matches[pairTeam->teamNum].missedGoals;
            pairTeam->matches[team->teamNum-1].missedGoals = team->matches[pairTeam->teamNum].goals;
            itoa(pairTeam->matches[team->teamNum-1].goals, goals, 10); /* Конвертирует цифровое значение забитых голов в строку */
            itoa(pairTeam->matches[team->teamNum-1].missedGoals, missed, 10); /* Конвертирует цифровое значение забитых голов в строку */
        }
        result = strcat(goals, ":");
        result = strcat(result, missed);
        gtk_entry_set_text(GTK_ENTRY(pairEntry), result);

}
void showDeleteErrorDialog(GtkWidget *parentDialog)
{
    gtk_widget_destroy(parentDialog);
    GtkWidget *dialog; /* Диалог для вывода ошибки */
    GtkWidget *label; /* Лэйбл для вывода текста ошибки */
    GtkWidget *content; /* Контэйнер диалога */
    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
    dialog = gtk_dialog_new_with_buttons("Ошибка",
                                        NULL,
                                        flags,
                                        ("Ок"),
                                        GTK_RESPONSE_NONE,
                                        NULL);
    content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    label = gtk_label_new("Введён неверный номер команды!");
    gtk_container_add(GTK_CONTAINER(content), label);
    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}
void showDeleteDialog(GtkWidget * widget, gint response, gpointer d)
{
    struct Data * data = (struct Data*)d;
    if(*data->teamCount<=2)return;
    GtkWidget *dialog; /* Диалог для ввода номера удаляемой команды */
    GtkWidget *label; /* Лэйбл для вывода подсказки */
    GtkWidget *deleteEntry; /* Поле ввода */
    GtkWidget *content; /* Контэйнер диалога */
    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
    dialog = gtk_dialog_new_with_buttons("Диалог удаления",
                                        NULL,
                                        flags,
                                        ("Удалить"),
                                        GTK_RESPONSE_ACCEPT,
                                        ("Отмена"),
                                        GTK_RESPONSE_REJECT,
                                        NULL);
    content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    label = gtk_label_new("Номер команды:");
    deleteEntry = gtk_entry_new();
    gtk_container_add(GTK_CONTAINER(content), label);
    gtk_container_add(GTK_CONTAINER(content), deleteEntry);

    gtk_widget_show_all(dialog);
    int result = gtk_dialog_run(GTK_DIALOG(dialog));
    switch(result)
    {
        case GTK_RESPONSE_ACCEPT:
        {
          const gchar * teamNum = gtk_entry_get_text(GTK_ENTRY(deleteEntry));
          int num = getTeamNum(teamNum, *(data->teamCount));
          if(num != -1)
          {
              deleteTeam(num, data);
              gtk_widget_destroy(dialog);
          }
          else showDeleteErrorDialog(dialog);
          break;
        }
        default:
        {
            gtk_widget_destroy(dialog);
            break;
        }
    }
}
void deleteTeam(int teamNum, gpointer d)
{
  struct Data * data = (struct Data*)d;
  int teamCount = *(data->teamCount);

  /* Перенумеровываем команды  */
  for(int i = 0; i < teamCount-1; i++)
  {
      data->teams[i].teamNum = i;
  }

  if(teamNum - 1 == 0)
  {
      for(int i = 1; i < teamCount; i++)
      {
          for(int j = 1; j < teamCount-1; j++)
          {
              data->teams[i-1].matches[j-1] = data->teams[i].matches[j];
          }
      }
  }
  else if(teamNum-1 == teamCount)
  {
      for(int i = teamCount-2; i >-1; i--)
      {
          for(int j = teamCount-1; j>-1;j--)
          {
              data->teams[i+1].matches[j+1] = data->teams[i].matches[j];
          }
      }
  }
  else if(teamNum-1 > 0 && teamNum-1 < teamCount-1)
  {
    for(int i = 0; i < teamNum-1; i++)
    {
        for(int j = teamNum-2; j < teamCount-2; j++)
        {
            data->teams[i].matches[j] = data->teams[i].matches[j+1];
        }
    }
    for(int i = teamNum; i < teamCount; i++)
    {
        for(int j = teamNum-1; j < teamCount-1; j++)
        {
            data->teams[i].matches[j] = data->teams[i].matches[j+1];
        }
    }
    for(int i = teamNum; i < teamCount; i++)
    {
        for(int j = 0; j < teamCount-1;j++)
        {
            data->teams[i-1].matches[j] = data->teams[i].matches[j];
        }
    }
  }
  for(int i = 0; i < teamCount-1; i++)
  {
      int currentCol = 0;
      for(int j = 0; j < teamCount-1; j++)
      {
          if(i != j)
          {
              currentCol++;
          }
      }
  }
  gtk_grid_remove_row(GTK_GRID(data->widgets->grid), teamNum); /* Удаляем поле выбранной команды */
  gtk_grid_remove_column(GTK_GRID(data->widgets->grid), teamNum); /* Удаляем столбец с матчами у выбранной команды */
  gtk_grid_remove_row(GTK_GRID(data->widgets->grid), teamCount); /* Удаляем поле с кнопками */
  gtk_grid_remove_column(GTK_GRID(data->widgets->grid), teamCount); /* Удаляем поле с кол-вом очков */
  gtk_grid_remove_column(GTK_GRID(data->widgets->grid), teamCount); /* Удаляем поле с кол-вом побед */
  /* Сдвигаем таблицу */
  for(int i = 0; i < teamCount; i++)
  {
      if(i == teamNum - 1) continue;
      int currentCol = teamNum;
      for(int j = teamNum; j < teamCount; j++)
      {
          data->widgets->dataEdit[i][currentCol] = data->widgets->dataEdit[i][j+1];
          currentCol++;

      }
  }
  for(int i = teamNum - 1; i < teamCount - 1; i++)
  {
    data->widgets->dataEdit[i] = data->widgets->dataEdit[i+1];
    strcpy(data->teams[i].teamName, data->teams[i+1].teamName);
  }

  /* Переустанавливает заголовки */
  data->widgets->title = realloc(data->widgets->title, (unsigned long)(teamCount+2) * sizeof *(data->widgets->title)); /* Уменьшаем память под заголовки */
  data->widgets->title[teamCount+1] = gtk_label_new("Кол-во побед");
  data->widgets->title[teamCount] = gtk_label_new("Кол-во очков");
  gtk_grid_attach(GTK_GRID(data->widgets->grid), data->widgets->title[teamCount],  teamCount, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(data->widgets->grid), data->widgets->title[teamCount+1],  teamCount+1, 0, 1, 1);
  gtk_widget_show(data->widgets->title[teamCount]);
  gtk_widget_show(data->widgets->title[teamCount+1]);



  /* Заполняем кол-во очков и кол-во побед */
  data->widgets->scoreLabels = realloc(data->widgets->scoreLabels, (unsigned long)(teamCount-1) * sizeof * (data->widgets->scoreLabels));
  data->widgets->winsLabels = realloc(data->widgets->winsLabels, (unsigned long)(teamCount-1) * sizeof * (data->widgets->winsLabels));

  /* Пересчитываем очки */
 for(int i = 0; i < teamCount-1; i++)
  {  data->teams[i].score=0;
     data->teams[i].wins=0;
     data->teams[i].loses=0;
     for (int j=0;j<teamCount-2;j++){

     int score = getScore(data->teams[i].matches[j]);
    if(score == 3){
            data->teams[i].wins++;
            data->teams[i].score+= 3;
     }
     else if(score==0){
            data->teams[i].loses++;
     }else if(score==1){
         data->teams[i].score += 1;
     }
     }
  }


  for(int i = 0; i < teamCount-1; i++)
  {
      char score[32], wins[32];
      itoa(data->teams[i].score, score, 10);
      itoa(data->teams[i].wins, wins, 10);
      data->widgets->scoreLabels[i] = gtk_label_new(score);
      gtk_grid_attach(GTK_GRID(data->widgets->grid), data->widgets->scoreLabels[i], teamCount, i+1, 1, 1);
      gtk_widget_show(data->widgets->scoreLabels[i]);

      data->widgets->winsLabels[i] = gtk_label_new(wins);
      gtk_grid_attach(GTK_GRID(data->widgets->grid),data->widgets->winsLabels[i], teamCount+1, i+1, 1, 1);
      gtk_widget_show(data->widgets->winsLabels[i]);
  }
  /* Перепривязываем данные в поля ввода */
  for(int i = 0; i < teamCount-1; i++)
  {
      g_object_set_data(G_OBJECT(data->widgets->dataEdit[i][0]), "team", &(data->teams[i]));
      for(int j = 1; j < teamCount; j++)
      {

         if(i+1 != j)
         {
              g_object_set_data(G_OBJECT(data->widgets->dataEdit[i][j]), "team", &(data->teams[i]));
              g_object_set_data(G_OBJECT(data->widgets->dataEdit[i][j]), "pair", data->widgets->dataEdit[j-1][i+1]);
              g_object_set_data(G_OBJECT(data->widgets->dataEdit[i][j]), "pairTeam", &(data->teams[j-1]));
         }
      }
  }


  (*data->teamCount) = (*data->teamCount) - 1;
  /* Переустанавливаем кнопки */

  data->widgets->addTeamButton = gtk_button_new_with_label("Добавить команду");
  gtk_grid_attach(GTK_GRID(data->widgets->grid), data->widgets->addTeamButton, 0, teamCount, 1 ,1);
  gtk_widget_show(data->widgets->addTeamButton);

  data->widgets->deleteTeamButton = gtk_button_new_with_label("Удалить команду");
  gtk_grid_attach(GTK_GRID(data->widgets->grid), data->widgets->deleteTeamButton, 1, teamCount, 1 ,1);
  gtk_widget_show(data->widgets->deleteTeamButton);

  data->widgets->savefileButton = gtk_button_new_with_label("Сохранить изменения");
  gtk_grid_attach(GTK_GRID(data->widgets->grid), data->widgets->savefileButton, 3, teamCount, 1 ,1);
  gtk_widget_show(data->widgets->savefileButton);

  /* Устанавливает сигналы */
  g_signal_connect(data->widgets->addTeamButton, "button_press_event", G_CALLBACK(onTeamAdd), (gpointer)d);
  g_signal_connect(data->widgets->deleteTeamButton, "button_press_event", G_CALLBACK(showDeleteDialog), (gpointer)d);
  g_signal_connect(data->widgets->savefileButton, "button_press_event", G_CALLBACK(savefile), (gpointer)d);
}
void onNameChanged(GtkWidget * textEntry)
{
    int i=0;
    struct Team * team = g_object_get_data(G_OBJECT(textEntry), "team");
    const gchar * str = gtk_entry_get_text(GTK_ENTRY(textEntry));
    while(*(str+i)!='\0'){
        if(*(str+i)==' '){
            gtk_entry_set_text(GTK_ENTRY(textEntry),team->teamName);
            showNameErrorDialog();
            return;
        }
        i++;
    }
    strcpy(team->teamName,str);
}
void showNameErrorDialog()
{

    GtkWidget *dialog; /* Диалог для вывода ошибки */
    GtkWidget *label; /* Лэйбл для вывода текста ошибки */
    GtkWidget *content; /* Контэйнер диалога */
    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
    dialog = gtk_dialog_new_with_buttons("Ошибка при изменении имени",
                                        NULL,
                                        flags,
                                        ("Ок"),
                                        GTK_RESPONSE_NONE,
                                        NULL);
    content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    label = gtk_label_new("Ошибка, пожалуйста, пишите команды слитно\n                       или используйте _");
    gtk_container_add(GTK_CONTAINER(content), label);
    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}
void onTeamAdd(GtkWidget * widget, gint response, gpointer d)
{
    struct Data * data = (struct Data*)d;
    /* Увеличивает память под teams */
    int teamCount = *(data->teamCount);
    /* Увеличиваем память под матчи для уже существуюших команд на 1 */
    struct Match m = {-1,-1};
    for(int i = 0; i < teamCount; i++)
    {
        data->teams[i].matches = realloc(data->teams[i].matches, (unsigned int)(teamCount) * sizeof *(data->teams[i].matches));
        data->teams[i].matches[teamCount-1] = m;
    }
    gtk_grid_remove_column(GTK_GRID(data->widgets->grid), teamCount+1);
    gtk_grid_remove_column(GTK_GRID(data->widgets->grid), teamCount+1);
    /* Переустанавливает заголовки */
    data->widgets->title = realloc(data->widgets->title, (unsigned int)(teamCount+3) * sizeof *(data->widgets->title));
    data->widgets->title[teamCount+2] = gtk_label_new("Кол-во побед");
    data->widgets->title[teamCount+1] = gtk_label_new("Кол-во очков");
    data->widgets->title[teamCount] = gtk_label_new("Матч");
    gtk_grid_attach(GTK_GRID(data->widgets->grid), data->widgets->title[teamCount+2],  teamCount+1+2, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(data->widgets->grid), data->widgets->title[teamCount+1],  teamCount+1+1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(data->widgets->grid), data->widgets->title[teamCount],  teamCount+1, 0, 1, 1);
    gtk_widget_show(data->widgets->title[teamCount+2]);
    gtk_widget_show(data->widgets->title[teamCount+1]);
    gtk_widget_show(data->widgets->title[teamCount]);

    for(int i = 0; i < teamCount; i++)
    {
      data->widgets->dataEdit[i] = realloc(data->widgets->dataEdit[i], (unsigned int)(teamCount+2) * sizeof (*data->widgets->dataEdit[i]));
      data->widgets->dataEdit[i][teamCount+1] = gtk_entry_new();
      gtk_entry_set_text(GTK_ENTRY(data->widgets->dataEdit[i][teamCount+1]), "-1:-1");
      gtk_grid_attach(GTK_GRID(data->widgets->grid),data->widgets->dataEdit[i][teamCount+1], teamCount+1, i+1, 1, 1);
      gtk_widget_show(data->widgets->dataEdit[i][teamCount+1]);
    }
    /* Увеличиваем счётчик команд на 1 */
    *(data->teamCount) = *(data->teamCount) + 1;
    teamCount = *(data->teamCount);
    /* Выделеяем память под еще 1 команду */
    data->teams = realloc(data->teams, (unsigned int)(teamCount) * sizeof(*(data->teams)));
    data->widgets->dataEdit = realloc(data->widgets->dataEdit, (unsigned int)(teamCount) * sizeof(*(data->widgets->dataEdit)));
    /* Выделяем память под матчи новой команды */
    data->teams[teamCount-1].matches = malloc((unsigned int)(teamCount-1) * sizeof *(data->teams[teamCount-1].matches));
    data->widgets->dataEdit[teamCount-1] = malloc((unsigned int)(teamCount+1) * sizeof *(data->widgets->dataEdit[teamCount-1]));
    /* Устанавливаем матчи новой команде */
    for(int i = 0; i < teamCount - 1; i++)
    {
        data->teams[teamCount-1].matches[i] = m;
    }

    gtk_grid_remove_row(GTK_GRID(data->widgets->grid), teamCount);
    /* Заполняет поля ввода для новой комманды ( с 1 т.к. в 0 поле под teamName ) */
    for(int i = 1; i < teamCount; i++)
    {
        data->widgets->dataEdit[teamCount-1][i] = gtk_entry_new();
        gtk_entry_set_text(GTK_ENTRY(data->widgets->dataEdit[teamCount-1][i]), "-1:-1");
        gtk_grid_attach(GTK_GRID(data->widgets->grid), data->widgets->dataEdit[teamCount-1][i], i ,teamCount, 1, 1);
        gtk_widget_show(data->widgets->dataEdit[teamCount-1][i]);
    }
    data->widgets->dataEdit[teamCount-1][teamCount] = gtk_label_new("----");
    gtk_grid_attach(GTK_GRID(data->widgets->grid), data->widgets->dataEdit[teamCount-1][teamCount], teamCount, teamCount, 1, 1);
    gtk_widget_show(data->widgets->dataEdit[teamCount-1][teamCount]);
    /* Устанавливает поле ввода для имени новой команды */
    data->teams[teamCount-1].teamNum = teamCount-1;
    strcpy(data->teams[teamCount-1].teamName, "Team_name");
    data->widgets->dataEdit[teamCount-1][0] = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(data->widgets->dataEdit[teamCount-1][0]), "Team_name");
    gtk_grid_attach(GTK_GRID(data->widgets->grid), data->widgets->dataEdit[teamCount-1][0], 0, teamCount, 1,1);
    gtk_widget_show(data->widgets->dataEdit[teamCount-1][0]);
    for(int i = 0; i < teamCount; i++)
    {
       g_object_set_data(G_OBJECT(data->widgets->dataEdit[i][teamCount]), "team", &(data->teams[i]));
       g_object_set_data(G_OBJECT(data->widgets->dataEdit[i][teamCount]), "pair", data->widgets->dataEdit[teamCount-1][i+1]);
       g_object_set_data(G_OBJECT(data->widgets->dataEdit[i][teamCount]), "pairTeam", &(data->teams[teamCount-1]));
       g_object_set_data(G_OBJECT(data->widgets->dataEdit[i][teamCount]), "data", (gpointer)d);
       g_signal_connect(data->widgets->dataEdit[i][teamCount], "focus-out-event", G_CALLBACK(onTextChanged), NULL);
    }
    for(int i = 1; i < teamCount; i++)
    {
        g_object_set_data(G_OBJECT(data->widgets->dataEdit[teamCount-1][i]), "team", &(data->teams[teamCount-1]));
        g_object_set_data(G_OBJECT(data->widgets->dataEdit[teamCount-1][i]), "pair", data->widgets->dataEdit[i-1][teamCount]);
        g_object_set_data(G_OBJECT(data->widgets->dataEdit[teamCount-1][i]), "pairTeam", &(data->teams[i-1]));
        g_object_set_data(G_OBJECT(data->widgets->dataEdit[teamCount-1][i]), "data", (gpointer)d);
        g_signal_connect(data->widgets->dataEdit[teamCount-1][i], "focus-out-event", G_CALLBACK(onTextChanged), NULL);
    }
    g_object_set_data(G_OBJECT(data->widgets->dataEdit[teamCount-1][0]), "team", &data->teams[teamCount-1]);
    g_signal_connect(data->widgets->dataEdit[teamCount-1][0], "focus-out-event", G_CALLBACK(onNameChanged), NULL);
    /* Устанавливаем кол-во побед и очков новой команде */
    data->teams[teamCount-1].wins = 0;
    data->teams[teamCount-1].score = 0;
    data->widgets->scoreLabels = realloc(data->widgets->scoreLabels, (unsigned long)(teamCount) * sizeof *(data->widgets->scoreLabels));
    data->widgets->winsLabels = realloc(data->widgets->winsLabels, (unsigned long)(teamCount) * sizeof *(data->widgets->winsLabels));

    /* Заполняем кол-во очков и кол-во побед */
    for(int i = 0; i < teamCount; i++)
    {
        char score[32], wins[32];
        itoa(data->teams[i].score, score, 10);
        itoa(data->teams[i].wins, wins, 10);
        data->widgets->scoreLabels[i] = gtk_label_new(score);
        gtk_grid_attach(GTK_GRID(data->widgets->grid), data->widgets->scoreLabels[i], teamCount+1, i+1, 1, 1);
        gtk_widget_show(data->widgets->scoreLabels[i]);
        data->widgets->winsLabels[i] = gtk_label_new(wins);
        gtk_grid_attach(GTK_GRID(data->widgets->grid),data->widgets->winsLabels[i], teamCount+2, i+1, 1, 1);
        gtk_widget_show(data->widgets->winsLabels[i]);
    }
    /* Перепривязываем данные к таблице */
    for(int i = 0; i < teamCount-1; i++)
    {
        g_object_set_data(G_OBJECT(data->widgets->dataEdit[i][0]), "team", &data->teams[i]);
        for(int j = 1; j < teamCount; j++)
        {
           if(i+1 != j)
           {
                g_object_set_data(G_OBJECT(data->widgets->dataEdit[i][j]), "team", &(data->teams[i]));
                g_object_set_data(G_OBJECT(data->widgets->dataEdit[i][j]), "pair", data->widgets->dataEdit[j-1][i+1]);
                g_object_set_data(G_OBJECT(data->widgets->dataEdit[i][j]), "pairTeam", &(data->teams[j-1]));
                g_object_set_data(G_OBJECT(data->widgets->dataEdit[i][j]), "data", (gpointer)d);
           }
        }
    }
    /* Передобавляет кнопки */
    data->widgets->addTeamButton = gtk_button_new_with_label("Добавить команду");
    gtk_grid_attach(GTK_GRID(data->widgets->grid), data->widgets->addTeamButton, 0, teamCount+1, 1 ,1);
    gtk_widget_show(data->widgets->addTeamButton);

    data->widgets->deleteTeamButton = gtk_button_new_with_label("Удалить команду");
    gtk_grid_attach(GTK_GRID(data->widgets->grid), data->widgets->deleteTeamButton, 1, teamCount+1, 1 ,1);
    gtk_widget_show(data->widgets->deleteTeamButton);

    data->widgets->savefileButton = gtk_button_new_with_label("Сохранить изменения");
    gtk_grid_attach(GTK_GRID(data->widgets->grid),data->widgets->savefileButton, 3, teamCount+1, 1 ,1);
    gtk_widget_show(data->widgets->savefileButton);

    /* Устанавливает сигналы */
    g_signal_connect(data->widgets->addTeamButton, "button_press_event", G_CALLBACK(onTeamAdd), (gpointer)data);
    g_signal_connect(data->widgets->deleteTeamButton, "button_press_event", G_CALLBACK(showDeleteDialog), (gpointer)data);
    g_signal_connect(data->widgets->savefileButton, "button_press_event", G_CALLBACK(savefile), (gpointer)data);
}
gint CloseAppWindow ()
{
    gtk_main_quit ();
    return (FALSE);
}
struct WidgetsGtk * initGrid(GtkWidget * window, struct Team * teams, int * teamCountPntr)
{
        int teamCount = *teamCountPntr;
        /* Возвращает указатель на структуру с созданными виджетами */
        struct WidgetsGtk * widgets = malloc(sizeof(*widgets));
        /* Выделение памяти */
        widgets->title =  malloc((unsigned int)(3+teamCount) * sizeof(*(widgets->title)));
        widgets->dataEdit = malloc((unsigned int)teamCount * sizeof(*(widgets->dataEdit))); /* Выделение памяти под указатели на строки */
        for(int i = 0; i < teamCount; i++)
        {
            widgets->dataEdit[i] = malloc((unsigned int)(teamCount+1) * sizeof(*widgets->dataEdit[i])); /* Выделение памяти на столбцы (Кол-во комманд + Имя) */
        }
        /* Выделяем память под столбцы для кол-ва очков и кол-ва побед */
        widgets->scoreLabels = malloc((unsigned int)teamCount * sizeof *(widgets->scoreLabels));
        widgets->winsLabels = malloc((unsigned int)teamCount * sizeof *(widgets->winsLabels));
        /* Создаём таблицу и заголовки */
        widgets->grid = gtk_grid_new();
        gtk_grid_set_column_spacing(GTK_GRID(widgets->grid), 10); /* Устанавливает расстояние между столбцами таблицы */
        gtk_container_add (GTK_CONTAINER (window), widgets->grid);

        widgets->title[0] = gtk_label_new("Имя команды");
        for(int i = 1; i < teamCount+2; i++)
        {
            widgets->title[i] = gtk_label_new("Матч");
        }
        widgets->title[teamCount+1] = gtk_label_new("Число очков");
        widgets->title[teamCount+2] = gtk_label_new("Кол-во побед");
        widgets->title[teamCount+3] = gtk_label_new("Кол-во поражений");

        for(int i = 0; i < 3+teamCount; i++)
        {
            gtk_grid_attach(GTK_GRID(widgets->grid), widgets->title[i], i, 0, 1, 1);
            gtk_widget_show(widgets->title[i]);
        }

        for(int i = 0; i < teamCount; i++)
        {
            widgets->dataEdit[i][0] = gtk_entry_new();
            gtk_entry_set_text(GTK_ENTRY(widgets->dataEdit[i][0]), teams[i].teamName);
            gtk_grid_attach(GTK_GRID(widgets->grid), widgets->dataEdit[i][0], 0, i+1, 1 ,1);
            gtk_widget_show(widgets->dataEdit[i][0]);
            int currentCol = 0;
            for(int j = 0; j < teamCount; j++)
            {
                if(i != j) /* Если матч не с самим собой */
                {
                    char goals[32], missed[32];
                    char * result;

                    itoa((teams[i]).matches[currentCol].goals, goals, 10); /* Конвертирует цифровое значение забитых голов в строку */
                    itoa((teams[i]).matches[currentCol].missedGoals, missed, 10); /* Конвертирует цифровое значение забитых голов в строку */


                    widgets->dataEdit[i][j+1] = gtk_entry_new(); /* Создаёт textEntry */
                    result = strcat(goals, ":");
                    result = strcat(result, missed);
                    gtk_entry_set_text(GTK_ENTRY(widgets->dataEdit[i][j+1]), result);
                    gtk_grid_attach(GTK_GRID(widgets->grid), widgets->dataEdit[i][j+1], j+1, i+1, 1, 1);
                    gtk_widget_show(widgets->dataEdit[i][j+1]);
                    currentCol++;
                }
                else
                {
                    widgets->dataEdit[i][j+1] = gtk_label_new("----");
                    gtk_grid_attach(GTK_GRID(widgets->grid), widgets->dataEdit[i][j+1], j+1, i+1, 1, 1);
                    gtk_widget_show(widgets->dataEdit[i][j+1]);
                }
            }
        }
        /* Заполняем кол-во очков и кол-во побед */
        for(int i = 0; i < teamCount; i++)
        {
            char score[32], wins[32];
            itoa(teams[i].score, score, 10);
            itoa(teams[i].wins, wins, 10);
            widgets->scoreLabels[i] = gtk_label_new(score);
            gtk_grid_attach(GTK_GRID(widgets->grid), widgets->scoreLabels[i], teamCount+1, i+1, 1, 1);
            gtk_widget_show(widgets->scoreLabels[i]);

            widgets->winsLabels[i] = gtk_label_new(wins);
            gtk_grid_attach(GTK_GRID(widgets->grid), widgets->winsLabels[i], teamCount+2, i+1, 1, 1);
            gtk_widget_show(widgets->winsLabels[i]);
        }
        /* Добавляем кнопки */
        widgets->addTeamButton = gtk_button_new_with_label("Добавить команду");
        gtk_grid_attach(GTK_GRID(widgets->grid), widgets->addTeamButton, 0, teamCount+1, 1 ,1);
        gtk_widget_show(widgets->addTeamButton);

        widgets->deleteTeamButton = gtk_button_new_with_label("Удалить команду");
        gtk_grid_attach(GTK_GRID(widgets->grid), widgets->deleteTeamButton, 1, teamCount+1, 1 ,1);
        gtk_widget_show(widgets->deleteTeamButton);

        //widgets->logButton = gtk_button_new_with_label("Log");
        //gtk_grid_attach(GTK_GRID(widgets->grid), widgets->logButton, 2, teamCount+1, 1 ,1);
       // gtk_widget_show(widgets->logButton);

        widgets->savefileButton = gtk_button_new_with_label("Сохранить изменения");
        gtk_grid_attach(GTK_GRID(widgets->grid), widgets->savefileButton, 3, teamCount+1, 1 ,1);
        gtk_widget_show(widgets->savefileButton);

        gtk_widget_show (window);

        gtk_widget_show(widgets->grid);
        return widgets;
}
struct Data * setSignals(GtkWidget *window, struct WidgetsGtk *widgets, struct Team *teams, int * teamCountPntr)
{
    int teamCount = *teamCountPntr;
    struct Data * d = g_new0(struct Data, 1);
    d->teams = teams;
    d->teamCount = teamCountPntr;
    d->widgets = widgets;
    d->window = window;
    /* Сигналы */
    g_signal_connect(widgets->addTeamButton, "button_press_event", G_CALLBACK(onTeamAdd), (gpointer)d);
    g_signal_connect(window, "delete_event", G_CALLBACK(CloseAppWindow), NULL);
    g_signal_connect(widgets->deleteTeamButton, "button_press_event", G_CALLBACK(showDeleteDialog), (gpointer)d);
    g_signal_connect(widgets->savefileButton, "button_press_event", G_CALLBACK(savefile), (gpointer)d);
    for(int i = 0; i < teamCount; i++)
    {
        g_object_set_data(G_OBJECT(widgets->dataEdit[i][0]), "team", &teams[i]);
        g_signal_connect(widgets->dataEdit[i][0], "focus-out-event", G_CALLBACK(onNameChanged), NULL);
        for(int j = 1; j < teamCount+1; j++)
        {
           if(i+1 != j)
           {
                //  0 2 = 1 0
                g_object_set_data(G_OBJECT(widgets->dataEdit[i][j]), "pair", widgets->dataEdit[j-1][i+1]);
                g_object_set_data(G_OBJECT(widgets->dataEdit[i][j]), "team", &teams[i]);
                g_object_set_data(G_OBJECT(widgets->dataEdit[i][j]), "pairTeam", &teams[j-1]);
                g_object_set_data(G_OBJECT(widgets->dataEdit[i][j]), "data", (gpointer)d);
                g_signal_connect(widgets->dataEdit[i][j], "focus-out-event", G_CALLBACK(onTextChanged), NULL);
           }
        }
    }
    return d;
}

void savefile(GtkWidget * widget, gint response, gpointer d,gpointer widgets,gpointer teams){
    FILE *fp;
    struct Data * data = (struct Data*)d;
    fp=fopen(data->fpname,"w+");
    int teamCount = *(data->teamCount);
    for(int i = 0; i < teamCount; i++)
    {
        fprintf(fp,"%s", data->teams[i].teamName);
        for(int j = 0; j < teamCount-1; j++)
        {

            fprintf(fp," %d:%d", data->teams[i].matches[j].goals, data->teams[i].matches[j].missedGoals);

        }
        if(i!=teamCount-1)fputs("\n",fp);
    }
    fclose(fp);
    GtkWidget *dialog; /* Диалог для вывода ошибки */
    GtkWidget *label; /* Лэйбл для вывода текста ошибки */
    GtkWidget *content; /* Контэйнер диалога */
    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
    dialog = gtk_dialog_new_with_buttons("Save file dialog",
                                        NULL,
                                        flags,
                                        ("OK"),
                                        GTK_RESPONSE_NONE,
                                        NULL);
    content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    label = gtk_label_new("Файл сохранен!");
    gtk_container_add(GTK_CONTAINER(content), label);
    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}
