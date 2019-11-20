struct Team
{
    char teamName[32];
    int score;
    int wins;
    int loses;
    struct Match
    {
        int goals;
        int missedGoals;
    } *matches;
};
