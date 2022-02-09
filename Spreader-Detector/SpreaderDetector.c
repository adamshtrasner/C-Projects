#include "SpreaderDetector.h"
#include <stdio.h>

#define BASE 10
#define READ_MODE "r"
#define WRITE_MODE "w"
#define CRNA(distance, measure) (((measure)*(MIN_DISTANCE))/((distance)*(MAX_MEASURE)))

// Functions of my own :

/**
 *
 * @param pDetector (struct **SpreaderDetector) pointer to pointer
 * to spreader detector
 * @param pPerson (struct *Person) pointer to person
 * @return same as SpreaderDetectorAddPerson
 */

int AddPerson(SpreaderDetector **pDetector, Person *pPerson) {
    SpreaderDetector *sd = *pDetector;
    Person **tmp = NULL;
    if (sd->people_cap == sd->people_size){
        tmp = realloc(sd->people, sizeof(Person *) * ((sd->people_cap)*SPREADER_DETECTOR_GROWTH_FACTOR));
        if (!tmp){
            return 0;
        }
        else{
            sd->people = tmp;
            sd->people_cap *= SPREADER_DETECTOR_GROWTH_FACTOR;
        }
    }
    sd->people[sd->people_size++] = pPerson;
    return 1;
}


/**
 * the function adds the meeting to the spreader detector and to the person
 * which is person_1 in the meeting
 * @param pDetector (struct **SpreaderDetector) pointer to pointer to
 * spreader detector
 * @param pMeeting (struct *Meeting) pointer to meeting
 * @return 1 if adding the meeting  has been successful
 *         0 otherwise
 */
int AddMeeting(SpreaderDetector **pDetector, Meeting *pMeeting) {
    SpreaderDetector *sd = *pDetector;
    Meeting **tmp = NULL;
    // Adding the meeting to the spreader detector
    if (sd->meeting_cap == sd->meeting_size){
        tmp = realloc(sd->meetings, sizeof(Meeting *) *
                                  (sd->meeting_cap*SPREADER_DETECTOR_GROWTH_FACTOR));
        if (!tmp){
            return 0;
        }
        else{
            sd->meetings = tmp;
            sd->meeting_cap *= SPREADER_DETECTOR_GROWTH_FACTOR;
        }
    }
    sd->meetings[sd->meeting_size++] = pMeeting;

    // Adding the meeting to the person (person_1)
    Person *p1 = MeetingGetPerson(pMeeting, 1);
    if (p1->meetings_capacity == p1->num_of_meetings){
        tmp = realloc(p1->meetings, sizeof(Meeting *) *
                                    (p1->meetings_capacity*PERSON_GROWTH_FACTOR));
        if (!tmp){
            return 0;
        }
        else{
            p1->meetings = tmp;
            p1->meetings_capacity *= PERSON_GROWTH_FACTOR;
        }
    }
    p1->meetings[p1->num_of_meetings++] = pMeeting;

    return 1;
}


/**
 *
 * @param pDetector (struct *SpreaderDetector) a pointer to a spreader detector
 * @param id an Id of a person
 * @return a pointer to the person which has the given Id if the person exists in the spreader detector
 *         NULL otherwise
 */
Person *GetPersonById(SpreaderDetector *pDetector, IdT id) {
    size_t i = 0;
    while (i != pDetector->people_size){
        if (pDetector->people[i]->id == id){
            return pDetector->people[i];
        }
        i++;
    }
    return NULL;
}


/**
 * The function runs the crna algorithm on each person that exists
 * in the spreader detector recursively.
 * @param sp (struct *SpreaderDetector) a pointer to a spreader detector
 * @param pPerson (struct **Person) a pointer to pointer to a person
 */
void CalculateInfectionRate(SpreaderDetector *sp, Person **pPerson) {
    Person *p = *pPerson;
    if (p->num_of_meetings == 0){
        return;
    }
    size_t i = 0;
    double score = 0;
    while(i != p->num_of_meetings){
        score = (p->infection_rate) *
                (CRNA(p->meetings[i]->distance, p->meetings[i]->measure));

        p->meetings[i]->person_2->infection_rate =
                    (p->meetings[i]->person_2->age > AGE_THRESHOLD) ?
                    (score + INFECTION_RATE_ADDITION_DUE_TO_AGE) : score;

        // the infection rate might be higher than 1
        // (and that's impossible) if the age threshold is added,
        // if so we change the infection rate to 1.
        if (p->meetings[i]->person_2->infection_rate > 1){
            p->meetings[i]->person_2->infection_rate = 1;
        }

        CalculateInfectionRate(sp, &(p->meetings[i]->person_2));
        i++;
    }
}


// End of functions of my own

// Start of given functions from the header:

SpreaderDetector *SpreaderDetectorAlloc(){
    // Check is memory allocation succeeded
    SpreaderDetector *spreader_det = malloc(sizeof(struct SpreaderDetector));
    if (!spreader_det){
        return NULL;
    }

    SpreaderDetector **p_spreader = &spreader_det;

    // Checks if meeting memory allocation succeeded
    spreader_det->meetings = malloc(sizeof(Meeting *) * SPREADER_DETECTOR_INITIAL_SIZE);
    if (!spreader_det->meetings){
        return NULL;
    }

    // Checks if people memory allocation succeeded
    spreader_det->people =  malloc(sizeof(Person *) * SPREADER_DETECTOR_INITIAL_SIZE);
    if (!spreader_det->people){
        return NULL;
    }

    spreader_det->people_size = spreader_det->meeting_size = 0;
    spreader_det->people_cap = spreader_det->meeting_cap = SPREADER_DETECTOR_INITIAL_SIZE;

    return *p_spreader;
}

void SpreaderDetectorFree(SpreaderDetector **p_spreader_detector){
    Meeting **meetings = (*p_spreader_detector)->meetings;
    Person **people = (*p_spreader_detector)->people;
    free(meetings);
    free(people);
    free(*p_spreader_detector);
    *p_spreader_detector = NULL;
}

int SpreaderDetectorAddPerson(SpreaderDetector *spreader_detector, Person *person){
    if (!spreader_detector || !person){
        return 0;
    }

    else{
        size_t i = 0;
        while (i != spreader_detector->people_size){
            if (spreader_detector->people[i] == person ||
                spreader_detector->people[i]->id == person->id){
                // person already exists or there's a different person with the same Id
                return 0;
            }
            i++;
        }
        return AddPerson(&spreader_detector, person);
    }
}

int SpreaderDetectorAddMeeting(SpreaderDetector *spreader_detector, Meeting *meeting){
    if ((!spreader_detector) || (!meeting) || (!spreader_detector->people_size)){
        return 0;
    }

    return AddMeeting(&spreader_detector, meeting);

}

void SpreaderDetectorReadMeetingsFile(SpreaderDetector *spreader_detector, const char *path){
    FILE *meetings_file = fopen(path, READ_MODE);

    IdT id_1 = 0, id_2 = 0;
    double measure = 0, distance = 0;
    Person *person_1 = NULL, *person_2 = NULL;

    char line[MAX_LEN_OF_LINE];

    while (fgets(line, MAX_LEN_OF_LINE, meetings_file)){

        id_1 = strtoul(strtok(line, " "), NULL, BASE);
        id_2 = strtoul(strtok(NULL, " "), NULL, BASE);
        distance = strtod(strtok(NULL, " "), NULL);
        measure = strtod(strtok(NULL, " "), NULL);

        person_1 = GetPersonById(spreader_detector, id_1);
        person_2 = GetPersonById(spreader_detector, id_2);

        // if person_1 and person_2 exist in the spreader detector
        if (person_1 && person_2){
            SpreaderDetectorAddMeeting(spreader_detector, MeetingAlloc(person_1, person_2, measure, distance));
        }

    }

    fclose(meetings_file);
}

void SpreaderDetectorReadPeopleFile(SpreaderDetector *spreader_detector, const char *path){
    FILE *people_file = fopen(path, READ_MODE);

    IdT id = 0;
    size_t age = 0;
    char *name = NULL, *is_sick_str = NULL;
    int is_sick = 0;

    char line[MAX_LEN_OF_LINE];

    while (fgets(line, MAX_LEN_OF_LINE, people_file)){

        name = strtok(line, " ");
        id = strtoul(strtok(NULL, " "), NULL, BASE);
        age = strtod(strtok(NULL, " "), NULL);
        is_sick_str = strtok(NULL, " ");

        if (!is_sick_str){
            is_sick = 0;
        }
        else{
            is_sick = 1;
        }

        SpreaderDetectorAddPerson(spreader_detector, PersonAlloc(id, name, age, is_sick));
    }

    fclose(people_file);
}

double SpreaderDetectorGetInfectionRateById(SpreaderDetector *spreader_detector, IdT id){

    if (!spreader_detector){
        return -1;
    }

    size_t i = 0;
    while (i != spreader_detector->people_size){
        if (spreader_detector->people[i]->id == id){
            return spreader_detector->people[i]->infection_rate;
        }
        i++;
    }

    return -1;

}

void SpreaderDetectorCalculateInfectionChances(SpreaderDetector *spreader_detector){

    if (!spreader_detector){
        return;
    }

    size_t i = 0;
    // finding the person who is sick
    while (spreader_detector->people[i]->infection_rate != 1){
        i++;
    }

    // Starting to calculate infection rates from the "root" - the sick person
    CalculateInfectionRate(spreader_detector, &(spreader_detector->people[i]));
}



int SpreaderDetectorPrintRecommendTreatmentToAll(SpreaderDetector *spreader_detector, const char *file_path){
    if (!spreader_detector){
        return 0;
    }
    FILE *out = fopen(file_path, WRITE_MODE);
    size_t i = 0;
    Person *person = NULL;
    while (i != spreader_detector->people_size){
        person = spreader_detector->people[i];
        if (person->infection_rate > MEDICAL_SUPERVISION_THRESHOLD){
            fprintf(out, MEDICAL_SUPERVISION_THRESHOLD_MSG, person->name, person->id,
                    person->age, person->infection_rate);
        }
        else{
            if (person->infection_rate > REGULAR_QUARANTINE_THRESHOLD){
                fprintf(out, REGULAR_QUARANTINE_MSG, person->name, person->id,
                        person->age, person->infection_rate);
            }
            else{
                fprintf(out, CLEAN_MSG, person->name, person->id,
                        person->age, person->infection_rate);
            }
        }

        i++;
    }
    return 1;
}

size_t SpreaderDetectorGetNumOfPeople(SpreaderDetector *spreader_detector){
    if (!spreader_detector){
        return 0;
    }
    return spreader_detector->people_size;
}

size_t SpreaderDetectorGetNumOfMeetings(SpreaderDetector *spreader_detector){
    if (!spreader_detector){
        return 0;
    }
    return spreader_detector->meeting_size;
}
