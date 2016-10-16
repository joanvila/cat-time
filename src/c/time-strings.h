static const char *stringHours[] = {
    "dotze",
    "una",
    "dues",
    "tres",
    "quatre",
    "cinc",
    "sis",
    "set",
    "vuit",
    "nou",
    "deu",
    "onze",
    "dotze",
    "una"
};

static const char *stringHourParts[] = {
    "un quart",
    "dos quarts",
    "tres quarts"
};

char* concat(const char *s1, const char *s2);
char* hour_to_string(int hour, int min);
const char* part_time_to_string(int hour, int min);
