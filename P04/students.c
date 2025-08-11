#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Students {
    int id;
    char name[30];
    char course[30];
    int batch;
} stud;

stud student;
stud *studPtr;

stud getDetails(stud);
void getPtrDetails(stud *);

int main(int argc, char *argv[]) {
    printf("First student's credentials...\n");
    student = getDetails(student);

    // Second student (pointer)
    studPtr = malloc(sizeof(stud)); // Allocate memory

    printf("\nSecond student's credentials...\n");
    getPtrDetails(studPtr);

    // Output students' credentials
    printf("\nStudents:");
    printf("\n(n%d) %s - ", student.id, student.name);
    printf("Course: %s, Batch: %d", student.course, student.batch);
    printf("\n(n%d) %s - ", studPtr->id, studPtr->name);
    printf("Course: %s, Batch: %d\n", studPtr->course, studPtr->batch);
    return 0;
}

stud getDetails(stud student) {
    printf("Enter student ID: ");
    // use dot notation when writing to struct
    scanf("%d", &student.id);
    // address-of operator when writing to non-pointer elements
    printf("Enter student name: ");
    scanf("%s", student.name);

    printf("Enter student batch number: ");
    scanf("%d", &student.batch);

    printf("Enter student's course code: ");
    scanf("%s", student.course);
    return student;
}

void getPtrDetails(stud *studPtr) {
    printf("Enter student ID: ");
    // use arrow notation when writing to struct pointer
    scanf("%d", &studPtr->id);

    printf("Enter student name: ");
    scanf("%s", studPtr->name);

    printf("Enter student batch number: ");
    scanf("%d", &studPtr->batch);

    printf("Enter student's course code: ");
    scanf("%s", studPtr->course);
}