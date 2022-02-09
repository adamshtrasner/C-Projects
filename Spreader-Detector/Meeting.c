#include "Meeting.h"
#include <stdio.h>

Meeting *MeetingAlloc(Person *person_1, Person *person_2, double measure, double distance){
    // Checks if memory allocation succeeded
    Meeting *meeting = malloc(sizeof(struct Meeting));
    if (!meeting){
        return NULL;
    }

    // Setting values to the right fields in meeting
    meeting->person_1 = person_1;
    meeting->person_2 = person_2;
    meeting->measure = measure;
    meeting->distance = distance;

    return meeting;
}

void MeetingFree(Meeting **p_meeting){
    Meeting *meeting = *p_meeting;
    if (!meeting){
        // error
        return;
    }

    free(meeting);
    meeting = NULL;

}

Person *MeetingGetPerson(const Meeting * const meeting, size_t person_ind){
    if (!meeting){
        return NULL;
    }

    if (!meeting->person_1 || !meeting->person_2){
        return NULL;
    }

    return (person_ind == 1) ? meeting->person_1 : ((person_ind == 2) ? meeting->person_2 : NULL);
}
