#include "Person.h"
#include <stdio.h>



Person *PersonAlloc(IdT id, char *name, size_t age, int is_sick){
    // Checks if person memory allocation succeeded
    Person *person = malloc(sizeof(struct Person));
    if (!person){
        return NULL;
    }

    // Checks if name memory allocation succeeded
    person->name = malloc(sizeof(char) * (strlen(name) + 1));
    if (!person->name){
        return NULL;
    }
    strcpy(person->name, name);

    // Checks if meetings allocation succeeded
    person->meetings = malloc(sizeof(Meeting *) * PERSON_INITIAL_SIZE);
    if (!person->meetings){
        return NULL;
    }

    // Setting values to the right fields in person
    person->id = id;
    person->age = age;
    person->is_sick = is_sick;

    // Setting default values
    person->num_of_meetings = 0;
    person->meetings_capacity = PERSON_INITIAL_SIZE;
    person->infection_rate = (is_sick) ? 1:0;

    return person;
}


void PersonFree(Person **p_person){
    if (!p_person){
        return;
    }
    if (!(*p_person)){
        return;
    }

    Person *person = *p_person;
    free(person->name);
    free(person->meetings);
    free(person);
    person = NULL;

}

Meeting *PersonGetMeetingById(const Person *const person, IdT id){

    if (!person->meetings){
        return NULL;
    }

    size_t i = 0;
    while (i != person->num_of_meetings){
        if (id == person->meetings[i]->person_2->id){
            return person->meetings[i];
        }
        i ++;
    }
    return NULL;
}

int PersonCompareById(const Person *person_1, const Person *person_2){

    if (person_1->id == person_2->id){
        return 0;
    }
    return (person_1->id > person_2->id) ? 1 : -1;
}

int PersonCompareByName(const Person *person_1, const Person *person_2){

    int cmp = strcmp(person_1->name, person_2->name);
    if (!cmp){
        return 0;
    }
    return (cmp > 0) ? 1 : -1;
}

int PersonCompareByInfectionRate(const Person *person_1, const Person *person_2){
    if (person_1->infection_rate == person_2->infection_rate){
        return 0;
    }
    return (person_1->infection_rate > person_2->infection_rate) ? -1 : 1;
}

int PersonCompareByAge(const Person *person_1, const Person *person_2){
    if (person_1->age == person_2->age){
        return 0;
    }
    return (person_1->age > person_2->age) ? -1 : 1;
}