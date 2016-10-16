#include <pebble.h>
#include "time-strings.h"

char* concat(const char *s1, const char *s2) {
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    char *result = malloc(len1 + len2 + 1);  //+2 for the zero-terminator and space between
    memcpy(result, s1, len1);
    memcpy(result + len1, s2, len2 + 1);  //+1 to copy the null-terminator
    return result;
}

char* hour_to_string(int hour, int min) {
    const char *textHour = stringHours[hour + 1];
    if (min < 5) textHour = stringHours[hour];

    const char *hourPrefix = "";

    if (min < 5 || min >= 55) {
        if (hour == 1) {
            hourPrefix = "la ";
        } else {
            hourPrefix = "les ";
        }
    } else {
        if (hour == 0 || hour == 10) {
            hourPrefix = "d'";
        } else {
            hourPrefix = "de ";
        }
    }

    return concat(hourPrefix, textHour);
}

const char* part_time_to_string(int hour, int min) {
    return stringHourParts[min];
}
