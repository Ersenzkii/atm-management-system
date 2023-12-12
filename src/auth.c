#include <termios.h>
#include "header.h"

void disableEcho() {

    struct termios oflags, nflags;

    tcgetattr(fileno(stdin), &oflags);
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;
    nflags.c_lflag |= ECHONL;
    if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0) {
        perror("tcsetattr");
        exit(1);
    }
}

void enableEcho() {

    struct termios oflags, nflags;

    if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0) {
        perror("tcsetattr");
        exit(1);
    }
}

void loginMenu(char a[50], char pass[50])
{
    struct termios oflags, nflags;

    system("clear");
    printf("\n\n\n\t\t\t\t   Bank Management System\n\t\t\t\t\t User Login:");
    scanf("%s", a);

    // disabling echo
    tcgetattr(fileno(stdin), &oflags);
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;
    nflags.c_lflag |= ECHONL;

    if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0)
    {
        perror("tcsetattr");
        return exit(1);
    }
    printf("\n\n\n\n\n\t\t\t\tEnter the password to login:");
    scanf("%s", pass);

    // restore terminal
    if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0)
    {
        perror("tcsetattr");
        return exit(1);
    }
};

int isUsernameTaken(const char *username) {
    FILE *file = fopen("./data/users.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    char currentUsername[50];
    while (fscanf(file, "%*d %49s %*s", currentUsername) == 1) {
        if (strcmp(username, currentUsername) == 0) {
            fclose(file);
            return 1;  // Username is already taken
        }
    }

    fclose(file);
    return 0;  // Username is not taken
}

int getLastId() {
    FILE *file = fopen("./data/users.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    int lastId = -1;
    int id;
    char name[50];
    char password[50];

    // Continue reading until the end of file or an error occurs
    while (fscanf(file, "%d %49s %49s", &id, name, password) == 3) {
        lastId = id;
    }

    fclose(file);
    return lastId;
}

void registerMenu() {
    system("clear");
    printf("\n\n\n\t\t\t\t   Bank Management System\n\t\t\t\t\t User Registration:");

    // Get the last ID from the file
    int lastId = getLastId();

    char username[50];
    char password[50];

    // Increment the ID for the new user
    int newId = lastId + 1;

    // Get username
    printf("\n\n\n\n\t\t\t\tEnter your username: ");
    scanf("%s", username);

    // Check if username is already taken
    if (isUsernameTaken(username)) {
        printf("\n\t\t\t\tUsername '%s' is already taken. Please choose a different one.\n", username);

        sleep(2);

        registerMenu();  // Retry registration
        return;
    }
    
    // Disable echo for password entry
    disableEcho();

    // Get password
    printf("\n\t\t\t\tEnter your password: ");
    scanf("%s", password);


    // Re-enter password
    char reenterPass[50];
    printf("\n\t\t\t\tRe-enter your password: ");
    scanf("%s", reenterPass);

    // Enable echo after password entry
    enableEcho();

    // Check if passwords match
    if (strcmp(password, reenterPass) != 0) {
        printf("\n\t\t\t\tPasswords do not match. Please try again.\n");

        sleep(2);

        registerMenu();
        return;  // Make sure to return after recursive call
    }

    // Open file for writing
    FILE *file = fopen("./data/users.txt", "a+");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    // Write new user information to file
    fprintf(file, "%d %s %s\n", newId, username, password);

    // Close file
    fclose(file);

    printf("\n\t\t\t\tRegistration successful!\n");

    sleep(2);
}


const char *getPassword(struct User u)
{
    FILE *fp;
    struct User userChecker;

    if ((fp = fopen("./data/users.txt", "r")) == NULL)
    {
        printf("Error! opening file");
        exit(1);
    }

    while (fscanf(fp, "%d %s %s", &userChecker.id, userChecker.name, userChecker.password) != EOF)
    {
        printf("%d %s %s", userChecker.id, userChecker.name, userChecker.password);
        if (strcmp(userChecker.name, u.name) == 0)
        {
            fclose(fp);
            char *buff = userChecker.password;
            return buff;
        }
    }

    fclose(fp);
    return "no user found";
}

