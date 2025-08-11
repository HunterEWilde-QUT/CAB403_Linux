#include <stdio.h>
#include <string.h>

typedef struct Books {
    char title[7];
    char author[20];
    int pubYear;
} book;

int main(int argc, char *argv[]) {
    book myBook1; // Structure declared
    // Structure parameters defined after declaration
    strcpy(myBook1.title, "CAB403");
    strcpy(myBook1.author, "Tommy Smith");
    myBook1.pubYear = 1997;
    // Structure declared & defined in 1 line
    book myBook2 = {"CAB222", "Jeremy Cox", 2012};

    printf("Books:\n");
    printf("%s by %s (%d)\n", myBook1.title, myBook1.author, myBook1.pubYear);
    printf("%s by %s (%d)\n", myBook2.title, myBook2.author, myBook2.pubYear);
    return 0;
}