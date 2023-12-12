#include "header.h"
#include <stdbool.h>

const char *RECORDS = "./data/records.txt";
const char *USERS = "./data/users.txt";

int getLastAccountId() {
    FILE *file = fopen(RECORDS, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    int lastAccountId = 0;
    char line[100];  // Adjust the size accordingly

    // Read the file line by line
    while (fgets(line, sizeof(line), file) != NULL) {
        int accountId;
        // Try to extract an integer (account ID) from the line
        if (sscanf(line, "%d", &accountId) == 1) {
            lastAccountId = accountId;
        }
    }

    fclose(file);
    return lastAccountId;
}

int getAccountFromFile(FILE *ptr, const char *username, struct Record *r) {
    int fieldsRead = fscanf(ptr, "%d %d %s %d %d/%d/%d %s %d %lf %s",
                            &r->id,
                            &r->userId,
                            r->name,  // Use r->name directly instead of a separate variable
                            &r->accountNbr,
                            &r->deposit.month,
                            &r->deposit.day,
                            &r->deposit.year,
                            r->country,
                            &r->phone,
                            &r->amount,
                            r->accountType);

    if (fieldsRead == 11 && strcmp(r->name, username) == 0) {
        return 1;  // Return 1 on successful match
    } else {
        return 0;  // Return 0 when there's no match or incomplete read
    }
}

void stayOrReturn(int notGood, void f(struct User u), struct User u)
{
    int option;
    if (notGood == 0)
    {
        system("clear");
        printf("\n✖ Record not found!!\n");
    invalid:
        printf("\nEnter 0 to try again, 1 to return to main menu and 2 to exit:");
        scanf("%d", &option);
        if (option == 0)
            f(u);
        else if (option == 1)
            mainMenu(u);
        else if (option == 2)
            exit(0);
        else
        {
            printf("Insert a valid operation!\n");
            goto invalid;
        }
    }
    else
    {
        printf("\nEnter 1 to go to the main menu and 0 to exit:");
        scanf("%d", &option);
    }
    if (option == 1)
    {
        system("clear");
        mainMenu(u);
    }
    else
    {
        system("clear");
        exit(1);
    }
}


int getUserIdByUsername(const char *username) {
    FILE *file = fopen(USERS, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    int userId;
    char currentUsername[50];
    char password[50];

    // Read the file line by line
    while (fscanf(file, "%d %s %s", &userId, currentUsername, password) == 3) {
        // Compare the read username with the target username
        if (strcmp(currentUsername, username) == 0) {
            fclose(file);
            return userId;
        }
    }

    // Username not found
    fclose(file);
    return -1;
}

void success(struct User u)
{
    int option;
    printf("\n✔ Success!\n\n");
invalid:
    printf("Enter 1 to go to the main menu and 0 to exit!\n");
    scanf("%d", &option);
    system("clear");
    if (option == 1)
    {
        mainMenu(u);
    }
    else if (option == 0)
    {
        exit(1);
    }
    else
    {
        printf("Insert a valid operation!\n");
        goto invalid;
    }
}

void saveAccountToFile(FILE *ptr, struct User *u, struct Record *r) {
    fprintf(ptr, "%d %d %s %d %d/%d/%d %s %d %.2lf %s\n\n",
            r->id,
            r->userId,
            u->name,
            r->accountNbr,
            r->deposit.month,
            r->deposit.day,
            r->deposit.year,
            r->country,
            r->phone,
            r->amount,
            r->accountType);
}

void createNewAcc(struct User u) {
    struct Record r;
    struct Record cr;
    char userName[50];
    FILE *pf = fopen(RECORDS, "a+");

    noAccount:
    system("clear");
    printf("\t\t\t===== New record =====\n");

    // Get the last account ID from the file
    int lastAccountId = getLastAccountId();

    // Increment the account ID for the new account
    r.id = lastAccountId + 1;

    int userId = getUserIdByUsername(u.name);
    printf("Debug: User ID: %d\n", userId); // Debug print

     r.userId = userId;

    printf("\nEnter today's date(mm/dd/yyyy):");
    scanf("%d/%d/%d", &r.deposit.month, &r.deposit.day, &r.deposit.year);
    printf("\nEnter the account number:");
    scanf("%d", &r.accountNbr);

    while (getAccountFromFile(pf, userName, &cr)) {
        if (strcmp(userName, u.name) == 0 && cr.accountNbr == r.accountNbr) {
            printf("✖ This Account already exists for this user\n\n");
            goto noAccount;
        }
    }
    printf("\nEnter the country:");
    scanf("%s", r.country);
    printf("\nEnter the phone number:");
    scanf("%d", &r.phone);
    printf("\nEnter amount to deposit: $");
    scanf("%lf", &r.amount);
    printf("\nChoose the type of account:\n\t-> saving\n\t-> current\n\t-> fixed01(for 1 year)\n\t-> fixed02(for 2 years)\n\t-> fixed03(for 3 years)\n\n\tEnter your choice:");
    scanf("%s", r.accountType);

    // Save the new account to the file
    saveAccountToFile(pf, &u, &r);

    fclose(pf);
    success(u);
}

void checkAllAccounts(struct User u) {
    struct Record r;
    FILE *pf = fopen(RECORDS, "r");

    system("clear");
    printf("\t\t====== All accounts for user, %s =====\n\n", u.name);

    printf("%-15s%-15s%-15s%-15s%-15s%-15s%-15s\n",
           "Account No.", "Deposit Date", "Country", "Phone No.", "Amount", "Account Type", "User ID");
    printf("________________________________________________________________________\n");

    while (fscanf(pf, "%d %d %s %d %d/%d/%d %s %d %lf %s",
                  &r.id,
                  &r.userId,
                  r.name,
                  &r.accountNbr,
                  &r.deposit.month,
                  &r.deposit.day,
                  &r.deposit.year,
                  r.country,
                  &r.phone,
                  &r.amount,
                  r.accountType) == 11) {
        if (strcmp(r.name, u.name) == 0) {
            printf("%-15d%-2d/%-2d/%-8d%-15s%-15d%-15.2lf%-15s%-15d\n",
                   r.accountNbr, r.deposit.day, r.deposit.month, r.deposit.year,
                   r.country, r.phone, r.amount, r.accountType, r.userId);
        }
    }

    fclose(pf);
    success(u);
}

void checkOneAccount(struct User u) {
    struct Record r;
    FILE *pf = fopen(RECORDS, "r");

    system("clear");
    printf("\t\t====== Check Account by Number for user, %s =====\n\n", u.name);

    int accountNumber;
    printf("Enter the account number to check: ");
    scanf("%d", &accountNumber);

    int found = 0; // Flag to indicate whether the account was found

    while (fscanf(pf, "%d %d %s %d %d/%d/%d %s %d %lf %s",
                  &r.id,
                  &r.userId,
                  r.name,
                  &r.accountNbr,
                  &r.deposit.month,
                  &r.deposit.day,
                  &r.deposit.year,
                  r.country,
                  &r.phone,
                  &r.amount,
                  r.accountType) == 11) {
        if (strcmp(r.name, u.name) == 0 && r.accountNbr == accountNumber) {
            found = 1;
            printf("%-15s%-15s%-15s%-15s%-15s%-15s%-15s\n",
                   "Account No.", "Deposit Date", "Country", "Phone No.", "Amount", "Account Type", "User ID");
            printf("________________________________________________________________________\n");
            printf("%-15d%-2d/%-2d/%-8d%-15s%-15d%-15.2lf%-15s%-15d\n",
                   r.accountNbr, r.deposit.day, r.deposit.month, r.deposit.year,
                   r.country, r.phone, r.amount, r.accountType, r.userId);

            double interest = 0.0;

            // Calculate interest based on account type
            if (strcmp(r.accountType, "savings") == 0) {
                interest = 0.07 * r.amount;
            } else if (strcmp(r.accountType, "fixed01") == 0) {
                interest = 0.04 * r.amount;
            } else if (strcmp(r.accountType, "fixed02") == 0) {
                interest = 0.05 * r.amount;
            } else if (strcmp(r.accountType, "fixed03") == 0) {
                interest = 0.08 * r.amount;
            }

            if (interest > 0.0) {
                printf("\nYou will get $%.2lf as interest on day %d of every month.\n", interest, r.deposit.day);
            } else {
                printf("\nYou will not get interests because the account is of type current.\n");
            }

            break; // Exit the loop after finding the account
        }
    }

    if (!found) {
        printf("Account with number %d not found for user %s.\n", accountNumber, u.name);
    }

    fclose(pf);
    success(u);
}

void editAccountDetails(struct User u) {
    struct Record r;
    FILE *pf = fopen(RECORDS, "r");
    FILE *tempFile = fopen("temp.txt", "w");

    if (pf == NULL || tempFile == NULL) {
        perror("Error opening file");
        return;
    }

    system("clear");
    printf("\t\t====== Edit Account Details for user, %s =====\n\n", u.name);

    int accountNumber;
    printf("Enter the account number to edit: ");
    scanf("%d", &accountNumber);

    char fieldToEdit[20];
    printf("Enter the field to edit (country or phone): ");
    scanf("%s", fieldToEdit);

    // Convert the field name to lowercase for case-insensitive comparison
    for (int i = 0; fieldToEdit[i]; i++) {
        fieldToEdit[i] = tolower(fieldToEdit[i]);
    }

    int found = 0; // Flag to indicate whether the account was found

    while (fscanf(pf, "%d %d %s %d %d/%d/%d %s %d %lf %s",
                  &r.id,
                  &r.userId,
                  r.name,
                  &r.accountNbr,
                  &r.deposit.month,
                  &r.deposit.day,
                  &r.deposit.year,
                  r.country,
                  &r.phone,
                  &r.amount,
                  r.accountType) == 11) {
        if (strcmp(r.name, u.name) == 0 && r.accountNbr == accountNumber) {
            found = 1;

            if (strcmp(fieldToEdit, "country") == 0) {
                printf("Enter the new country: ");
                scanf("%s", r.country);
            } else if (strcmp(fieldToEdit, "phone") == 0) {
                printf("Enter the new phone number: ");
                scanf("%d", &r.phone);
            } else {
                printf("Invalid field. Only 'country' or 'phone' can be edited.\n");
                fclose(pf);
                fclose(tempFile);
                remove("temp.txt");
                return;
            }

            fprintf(tempFile, "%d %d %s %d %d/%d/%d %s %d %.2lf %s\n\n",
                    r.id,
                    r.userId,
                    r.name,
                    r.accountNbr,
                    r.deposit.month,
                    r.deposit.day,
                    r.deposit.year,
                    r.country,
                    r.phone,
                    r.amount,
                    r.accountType);

        } else {
            fprintf(tempFile, "%d %d %s %d %d/%d/%d %s %d %.2lf %s\n\n",
                    r.id,
                    r.userId,
                    r.name,
                    r.accountNbr,
                    r.deposit.month,
                    r.deposit.day,
                    r.deposit.year,
                    r.country,
                    r.phone,
                    r.amount,
                    r.accountType);
        }
    }

    fclose(pf);
    fclose(tempFile);

    if (!found) {
        printf("Account with number %d not found for user %s.\n", accountNumber, u.name);
        remove("temp.txt");
    } else {
        remove(RECORDS);
        rename("temp.txt", RECORDS);
        printf("Account details updated successfully!\n");
    }
    printf("\n");
    success(u);
}

void deleteAccount(struct User u) {
    system("clear");
    printf("\t\t====== Delete Account for user, %s =====\n\n", u.name);

    int accountNumber;
    printf("Enter the Account Number you want to delete : ");
    scanf("%d", &accountNumber);

    FILE *pf = fopen(RECORDS, "r");
    FILE *tempFile = fopen("temp.txt", "w");

    if (pf == NULL || tempFile == NULL) {
        perror("Error while opening file.");
        return;
    }

    struct Record r;
    int found = 0; // Indicateur pour indiquer si le compte a été trouvé
    int previousId = -1; // Stocke l'ID précédent pour ajuster les ID des lignes supérieures

    while (fscanf(pf, "%d %d %s %d %d/%d/%d %s %d %lf %s",
                  &r.id,
                  &r.userId,
                  r.name,
                  &r.accountNbr,
                  &r.deposit.month,
                  &r.deposit.day,
                  &r.deposit.year,
                  r.country,
                  &r.phone,
                  &r.amount,
                  r.accountType) == 11) {
        if (r.accountNbr == accountNumber) {
            found = 1;
        } else {
            if (found) {
                // Si le compte a été trouvé, ajustez l'ID des lignes supérieures
                r.id = r.id - 1;
            }
            fprintf(tempFile, "%d %d %s %d %d/%d/%d %s %d %.2lf %s\n\n",
                    r.id,
                    r.userId,
                    r.name,
                    r.accountNbr,
                    r.deposit.month,
                    r.deposit.day,
                    r.deposit.year,
                    r.country,
                    r.phone,
                    r.amount,
                    r.accountType);
        }
        previousId = r.id;
    }

    fclose(pf);
    fclose(tempFile);

    if (!found) {
        printf("Account with number %d not found.\n", accountNumber);
        remove("temp.txt");
    } else {
        remove(RECORDS);
        rename("temp.txt", RECORDS);
        printf("Account Deleted!\n");
    }
    success(u);
}

void transferOwnership(struct User u) {
    system("clear");
    printf("\t\t====== Transfer Ownership for user, %s =====\n\n", u.name);

    int accountNumber;
    printf("Enter the account number to transfer ownership: ");
    scanf("%d", &accountNumber);

    char newOwnerName[50];
    printf("Enter the name of the new owner: ");
    scanf("%s", newOwnerName);

    FILE *pf = fopen(RECORDS, "r");
    FILE *tempFile = fopen("temp.txt", "w");

    if (pf == NULL || tempFile == NULL) {
        perror("Error opening file");
        return;
    }

    struct Record r;
    int found = 0; // Flag to indicate whether the account was found

    while (fscanf(pf, "%d %d %s %d %d/%d/%d %s %d %lf %s",
                  &r.id,
                  &r.userId,
                  r.name,
                  &r.accountNbr,
                  &r.deposit.month,
                  &r.deposit.day,
                  &r.deposit.year,
                  r.country,
                  &r.phone,
                  &r.amount,
                  r.accountType) == 11) {
        if (r.accountNbr == accountNumber) {
            found = 1;
            // Update the owner's name
            strncpy(r.name, newOwnerName, sizeof(r.name));
        }
        fprintf(tempFile, "%d %d %s %d %d/%d/%d %s %d %.2lf %s\n",
                r.id,
                r.userId,
                r.name,
                r.accountNbr,
                r.deposit.month,
                r.deposit.day,
                r.deposit.year,
                r.country,
                r.phone,
                r.amount,
                r.accountType);
    }

    fclose(pf);
    fclose(tempFile);

    if (!found) {
        printf("Account with number %d not found.\n", accountNumber);
        transferOwnership(u);
        remove("temp.txt");
        return;
    }

    // Check if the new owner exists in the users file
    FILE *userFile = fopen(USERS, "r");
    if (userFile == NULL) {
        perror("Error opening users file");
        remove("temp.txt");
        return;
    }

    int newOwnerId = -1;
    char userName[50];
    char userPassword[50];
    while (fscanf(userFile, "%d %s %s", &r.userId, userName, userPassword) == 3) {
        if (strcmp(userName, newOwnerName) == 0) {
            newOwnerId = r.userId;
            break;
        }
    }

    fclose(userFile);

    if (newOwnerId == -1) {
        printf("New owner '%s' not found in users file.\n", newOwnerName);
        remove("temp.txt");
        transferOwnership(u);
        return;
    }

    // Update the user ID in the temporary file
    tempFile = fopen("temp.txt", "r");
    pf = fopen(RECORDS, "w");

    if (pf == NULL || tempFile == NULL) {
        perror("Error opening file");
        remove("temp.txt");
        return;
    }

    while (fscanf(tempFile, "%d %d %s %d %d/%d/%d %s %d %lf %s",
                  &r.id,
                  &r.userId,
                  r.name,
                  &r.accountNbr,
                  &r.deposit.month,
                  &r.deposit.day,
                  &r.deposit.year,
                  r.country,
                  &r.phone,
                  &r.amount,
                  r.accountType) == 11) {
        if (r.accountNbr == accountNumber) {
            // Update the user ID
            r.userId = newOwnerId;
        }
        fprintf(pf, "%d %d %s %d %d/%d/%d %s %d %.2lf %s\n\n",
                r.id,
                r.userId,
                r.name,
                r.accountNbr,
                r.deposit.month,
                r.deposit.day,
                r.deposit.year,
                r.country,
                r.phone,
                r.amount,
                r.accountType);
    }

    fclose(pf);
    fclose(tempFile);

    printf("Ownership transferred successfully!\n");
    success(u);
}

int getFileSize(FILE *fp)
{
    int size;
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    rewind(fp);
    return size;
}



void transfer(struct User u)
{
    FILE *fp, *fpTemp;
    struct Record r;
    int accountNumber, option;
    float newAmount;
invalidAccount:
    if ((fp = fopen(RECORDS, "r")) == NULL || (fpTemp = fopen("./data/recordsTemp.txt", "w+")) == NULL)
    {
        printf("Error! opening file");
        exit(1);
    }
    else if (getFileSize(fp) == 0)
    {
        system("clear");
        printf("Impossible to make a transaction, the file is empty !\n\n");
        exit(1);
    }
    system("clear");
    bool isFound = false;
    printf("\nEnter the account number in which you want to make a transaction: ");
    scanf("%d", &accountNumber);
    system("clear");
    while (getAccountFromFile(fp, r.name, &r))
    {
        if (r.accountNbr == accountNumber)
        {
            if (strcmp(r.name, u.name) == 0)
            {
                isFound = true;
            invalidOption:
                printf("\n\t\t======= Transaction on account nÂ°%d, user %s =======\n\n", r.accountNbr, r.name);
                printf("\nWhat do you want to do ?\n\n");
                printf("[1]- Deposit money\n\n");
                printf("[2]- Withdraw money\n\n");
                printf("Your choice: ");
                scanf("%d", &option);
                if (option == 1)
                {
                    printf("Enter the amount to deposit: ");
                    scanf("%e", &newAmount);
                    r.amount += newAmount;
                }
                else if (option == 2)
                {
                notEnoughMoney:
                    printf("Enter the amount to withdraw: ");
                    scanf("%e", &newAmount);
                    if (r.amount >= newAmount)
                    {
                        r.amount -= newAmount;
                    }
                    else
                    {
                        printf("Impossible to withdraw money because your amount is insufficient !\n\n");
                        goto notEnoughMoney;
                    }
                }
                else
                {
                    printf("Invalid option !");
                    goto invalidOption;
                }
            }
        }
        saveAccountToFile(fpTemp, &u, &r);
    }
    if (!isFound)
    {
        printf("\nInvalid account number !");
        fclose(fp);
        fclose(fpTemp);
        goto invalidAccount;
    }
    fclose(fp);
    fclose(fpTemp);
    remove(RECORDS);
    rename("./data/recordsTemp.txt", RECORDS);
    success(u);
}