#include <stdio.h>

#include "SpreaderDetector.h"

void test2file(char *const *argv) {
    //ptest.txt mtest.txt
    SpreaderDetector *spr = SpreaderDetectorAlloc();
    if (!spr){
        printf("not good");
        return;
    }
    SpreaderDetectorReadPeopleFile(spr ,argv[1]);
    SpreaderDetectorReadMeetingsFile(spr,argv[2]);
    SpreaderDetectorCalculateInfectionChances(spr);
    int result = SpreaderDetectorPrintRecommendTreatmentToAll(spr,"outtest1.txt");
    printf("SpreaderDetectorPrintRecommendTreatmentToAll = %d\n",result);
    // free meetings
    for (size_t k_i = 0; k_i < spr->meeting_size; ++k_i) {
        MeetingFree(&spr->meetings[k_i]);
    }
    // free people
    for (size_t k_j = 0; k_j < spr->people_size; ++k_j) {
        PersonFree(&spr->people[k_j]);
    }
    // free spreader detector
    SpreaderDetectorFree(&spr);
    printf("finshed");
}

int main(int argc, char *argv[]) {
    printf("argc :  %d\n",argc);
    test2file(argv);
}
