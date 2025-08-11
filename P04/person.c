#include <stdio.h>
#include <string.h>

struct Person {
    char name[20];
    int citNo;
    float salary;
} person1, person2, people[2];

struct Person getUserInput(struct Person);

int main() {
    strcpy(person1.name, "George Orwell");
    person1.citNo = 1984;
    person1.salary = 2500;
    // Print first person's credentials
    printf("Name: %s\n", person1.name);
    printf("Citizen No.: %d\n", person1.citNo);
    printf("Salary: %.2f\n", person1.salary);
    // Get user input
    person2 = getUserInput(person2);
    // Print second person's credentials
    printf("\nWelcome, comrade!\nName: %s\nCitizen No.: %d\nSalary: %.2f\n", person2.name, person2.citNo, person2.salary);
    return 0;
}

struct Person getUserInput(struct Person p) {
    printf("\nPlease enter your credentials.\nName: ");
    scanf("%s", p.name); // Get name

    printf("Citizen No.: ");
    scanf("%d", &p.citNo); // Get citizen number

    printf("Salary: ");
    scanf("%f", &p.salary); // Get salary

    return p;
}