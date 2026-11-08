#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> // For using bool type

// Helper function to clear the input buffer
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Structure for a single menu item
struct MenuItem {
    char itemCode[10];
    char itemDescription[50];
    double unitPrice;
    char itemType[20]; // e.g., "Solo Package", "Barkada Package", "Family Package", "Individual Order"
};

// Structure for an item within an order
struct OrderItem {
    char itemCode[10];
    char itemDescription[50];
    int quantity;
    double unitPrice;
    double amount; // Quantity * Unit Price
};

// Structure for a complete customer order/bill
struct Bill {
    char date[15];
    int tableNo;
    struct OrderItem items[50]; // Assuming a maximum of 50 different items per order
    int itemCount;
    double totalBill;
    double discount;
    double netBill;
    double amountTendered;
    double change;
};

// Global array to store menu items
struct MenuItem menu[100];
int menuCount = 0;

// Function Prototypes
void loadMenuItems();
void saveMenuItems();
void populateInitialMenu();
void saveBillToFile(const struct Bill *bill); // NEW: Prototype for saving bills
void billingModule(struct Bill *currentBill);
void generateDailyTotalSales();
void addMenuRecord();
void updateMenuRecord();
void displayMenuItems();
void billingSubMenu();
void fileMaintenanceMenu();

// --- FILE HANDLING FUNCTIONS ---
void loadMenuItems() {
    FILE *file = fopen("menu.txt", "r");
    if (file == NULL) {
        printf("menu.txt not found. Attempting to create and populate initial menu.\n");
        populateInitialMenu(); // Call to populate if file doesn't exist
        return;
    }

    menuCount = 0;
    while (fscanf(file, "%s %s %lf %s",
                    menu[menuCount].itemCode,
                    menu[menuCount].itemDescription,
                    &menu[menuCount].unitPrice,
                    menu[menuCount].itemType) != EOF && menuCount < 100) {
        // Replace underscores with spaces in description and type
        for (int i = 0; menu[menuCount].itemDescription[i] != '\0'; i++) {
            if (menu[menuCount].itemDescription[i] == '_') {
                menu[menuCount].itemDescription[i] = ' ';
            }
        }
        for (int i = 0; menu[menuCount].itemType[i] != '\0'; i++) {
            if (menu[menuCount].itemType[i] == '_') {
                menu[menuCount].itemType[i] = ' ';
            }
        }
        menuCount++;
    }
    fclose(file);

    if (menuCount == 0) { // If file exists but is empty
        printf("menu.txt is empty. Populating initial menu.\n");
        populateInitialMenu();
    }
}

void saveMenuItems() {
    FILE *file = fopen("menu.txt", "w");
    if (file == NULL) {
        printf("Error opening menu.txt for writing.\n");
        return;
    }

    for (int i = 0; i < menuCount; i++) {
        // Replace spaces with underscores for file storage
        char tempDescription[50];
        strcpy(tempDescription, menu[i].itemDescription);
        for (int j = 0; tempDescription[j] != '\0'; j++) {
            if (tempDescription[j] == ' ') {
                tempDescription[j] = '_';
            }
        }
        char tempType[20];
        strcpy(tempType, menu[i].itemType);
        for (int j = 0; tempType[j] != '\0'; j++) {
            if (tempType[j] == ' ') {
                tempType[j] = '_';
            }
        }
        fprintf(file, "%s %s %.2lf %s\n",
                menu[i].itemCode, tempDescription, menu[i].unitPrice, tempType);
    }
    fclose(file);
}

void populateInitialMenu() {
    // Add default packages
    if (menuCount < 100) { strcpy(menu[menuCount].itemCode, "SP#1"); strcpy(menu[menuCount].itemDescription, "Solo Pack #1"); menu[menuCount].unitPrice = 120.00; strcpy(menu[menuCount].itemType, "Solo Package"); menuCount++; }
    if (menuCount < 100) { strcpy(menu[menuCount].itemCode, "SP#2"); strcpy(menu[menuCount].itemDescription, "Solo Pack #2"); menu[menuCount].unitPrice = 150.00; strcpy(menu[menuCount].itemType, "Solo Package"); menuCount++; }
    if (menuCount < 100) { strcpy(menu[menuCount].itemCode, "BP#1"); strcpy(menu[menuCount].itemDescription, "Barkada Package #1"); menu[menuCount].unitPrice = 499.00; strcpy(menu[menuCount].itemType, "Barkada Package"); menuCount++; }
    if (menuCount < 100) { strcpy(menu[menuCount].itemCode, "BP#2"); strcpy(menu[menuCount].itemDescription, "Barkada Package #2"); menu[menuCount].unitPrice = 699.00; strcpy(menu[menuCount].itemType, "Barkada Package"); menuCount++; }
    if (menuCount < 100) { strcpy(menu[menuCount].itemCode, "FP#1"); strcpy(menu[menuCount].itemDescription, "Family Package #1"); menu[menuCount].unitPrice = 899.00; strcpy(menu[menuCount].itemType, "Family Package"); menuCount++; }
    if (menuCount < 100) { strcpy(menu[menuCount].itemCode, "FP#2"); strcpy(menu[menuCount].itemDescription, "Family Package #2"); menu[menuCount].unitPrice = 1099.00; strcpy(menu[menuCount].itemType, "Family Package"); menuCount++; }
    // Add default individual items
    if (menuCount < 100) { strcpy(menu[menuCount].itemCode, "Coke1.5L"); strcpy(menu[menuCount].itemDescription, "Coke 1.5L"); menu[menuCount].unitPrice = 80.00; strcpy(menu[menuCount].itemType, "Individual Order"); menuCount++; }
    if (menuCount < 100) { strcpy(menu[menuCount].itemCode, "Rice"); strcpy(menu[menuCount].itemDescription, "Xtra Rice"); menu[menuCount].unitPrice = 50.00; strcpy(menu[menuCount].itemType, "Individual Order"); menuCount++; }

    saveMenuItems(); // Save the newly populated menu to file
}

// NEW: Function to save a completed bill to a file
void saveBillToFile(const struct Bill *bill) {
    FILE *file = fopen("sales_history.txt", "a"); // "a" for append mode
    if (file == NULL) {
        printf("Error opening sales_history.txt for writing.\n");
        return;
    }

    fprintf(file, "DATE:%s\n", bill->date);
    fprintf(file, "TABLENO:%d\n", bill->tableNo);
    fprintf(file, "ITEMCOUNT:%d\n", bill->itemCount);
    for (int i = 0; i < bill->itemCount; i++) {
        // Replace spaces with underscores for file storage
        char tempDescription[50];
        strcpy(tempDescription, bill->items[i].itemDescription);
        for (int j = 0; tempDescription[j] != '\0'; j++) {
            if (tempDescription[j] == ' ') {
                tempDescription[j] = '_';
            }
        }
        fprintf(file, "ITEM:%s %s %.2lf %d %.2lf\n",
                bill->items[i].itemCode,
                tempDescription,
                bill->items[i].unitPrice,
                bill->items[i].quantity,
                bill->items[i].amount);
    }
    fprintf(file, "TOTALBILL:%.2lf\n", bill->totalBill);
    fprintf(file, "DISCOUNT:%.2lf\n", bill->discount);
    fprintf(file, "NETBILL:%.2lf\n", bill->netBill);
    fprintf(file, "AMOUNT_TENDERED:%.2lf\n", bill->amountTendered);
    fprintf(file, "CHANGE:%.2lf\n", bill->change);
    fprintf(file, "END_BILL\n"); // Delimiter to separate bills

    fclose(file);
}


// --- BILLING SUB-MODULE FUNCTIONS ---
void billingModule(struct Bill *currentBill) {
    printf("\n"); // Add a newline for spacing
    // Centering "JOLLY - JIP RESTAURANT" and "ORDERING"
    printf("%*sJOLLY - JIP RESTAURANT\n", (80 - (int)strlen("JOLLY - JIP RESTAURANT")) / 2, "");
    printf("%*sORDERING\n\n", (80 - (int)strlen("ORDERING")) / 2, "");
    
    printf("Enter Date (MM/DD/YYYY): ");
    fgets((*currentBill).date, sizeof((*currentBill).date), stdin);
    (*currentBill).date[strcspn((*currentBill).date, "\n")] = 0; // Remove newline character

    // REMOVED: Global firstTransactionDate logic
    // if (strlen(firstTransactionDate) == 0) { strcpy(firstTransactionDate, (*currentBill).date); }

    while(true) {
        printf("TABLE NO.: ");
        if (scanf("%d", &(*currentBill).tableNo) != 1 || (*currentBill).tableNo <= 0) {
            printf("Invalid input. Please enter a positive integer for Table No.\n");
            clearInputBuffer();
        } else {
            clearInputBuffer();
            break;
        }
    }
    
    (*currentBill).itemCount = 0;
    (*currentBill).totalBill = 0.0;
    char anotherOrder;
    int orderTypeChoice;

    do {
        while(true) {
            printf("\nAdding new order...");
            printf("\n[1] Package Type\n[2] Individual Orders\n");
            printf("\nEnter choice: ");
            if (scanf("%d", &orderTypeChoice) != 1 || (orderTypeChoice != 1 && orderTypeChoice != 2)) {
                printf("Invalid input. Please enter 1 for Package Type or 2 for Individual Orders.\n");
                clearInputBuffer();
            } else {
                clearInputBuffer();
                break;
            }
        }
        
        char input[50] = "";
        struct MenuItem *selectedItem = NULL;
        char tempItemCodes[100][10];
        int tempItemCount = 0;
        int numericChoice;

        if (orderTypeChoice == 1) { // Package Type
            printf("\n========================\n");
            printf(" PACKAGES\n");
            printf("========================\n");
            
            tempItemCount = 0;
            printf("SOLO\n");
            for (int i = 0; i < menuCount; i++) {
                if (strcmp(menu[i].itemType, "Solo Package") == 0) {
                    printf("[%d] %s - %.2f\n", tempItemCount + 1, menu[i].itemDescription, menu[i].unitPrice);
                    strcpy(tempItemCodes[tempItemCount], menu[i].itemCode);
                    tempItemCount++;
                }
            }
            printf("\nBARKADA\n");
            for (int i = 0; i < menuCount; i++) {
                if (strcmp(menu[i].itemType, "Barkada Package") == 0) {
                    printf("[%d] %s - %.2f\n", tempItemCount + 1, menu[i].itemDescription, menu[i].unitPrice);
                    strcpy(tempItemCodes[tempItemCount], menu[i].itemCode);
                    tempItemCount++;
                }
            }
            printf("\nFAMILY\n");
            for (int i = 0; i < menuCount; i++) {
                if (strcmp(menu[i].itemType, "Family Package") == 0) {
                    printf("[%d] %s - %.2f\n", tempItemCount + 1, menu[i].itemDescription, menu[i].unitPrice);
                    strcpy(tempItemCodes[tempItemCount], menu[i].itemCode);
                    tempItemCount++;
                }
            }
            while(true) {
                printf("Choose package (enter number): ");
                if (scanf("%d", &numericChoice) != 1 || numericChoice <= 0 || numericChoice > tempItemCount) {
                    printf("Invalid input. Please enter a valid number from the list.\n");
                    clearInputBuffer();
                } else {
                    clearInputBuffer();
                    strcpy(input, tempItemCodes[numericChoice - 1]);
                    break;
                }
            }

        } else if (orderTypeChoice == 2) { // Individual Orders
            printf("\n--- INDIVIDUAL ORDERS ---\n");
            for (int i = 0; i < menuCount; i++) {
                if (strcmp(menu[i].itemType, "Individual Order") == 0) {
                    printf("%-10s %-30s %.2f\n", menu[i].itemCode, menu[i].itemDescription, menu[i].unitPrice);
                }
            }
            printf("Enter item code or description to order: ");
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = 0;

            if (strlen(input) == 0) {
                    printf("Item code or description cannot be empty.\n");
                    anotherOrder = 'y';
                    continue;
            }
        }

        for (int i = 0; i < menuCount; i++) {
            if (strcmp(menu[i].itemCode, input) == 0 || strcmp(menu[i].itemDescription, input) == 0) {
                selectedItem = &menu[i];
                break;
            }
        }
        
        if (selectedItem != NULL) {
            if ((*currentBill).itemCount >= 50) {
                printf("Maximum number of items per order reached.\n");
                break;
            }
            while(true) {
                printf("Enter quantity for %s: ", (*selectedItem).itemDescription);
                if (scanf("%d", &(*currentBill).items[(*currentBill).itemCount].quantity) != 1 || (*currentBill).items[(*currentBill).itemCount].quantity <= 0) {
                    printf("Invalid input. Quantity must be a positive integer.\n");
                    clearInputBuffer();
                } else {
                    clearInputBuffer();
                    break;
                }
            }
            

            strcpy((*currentBill).items[(*currentBill).itemCount].itemCode, (*selectedItem).itemCode);
            strcpy((*currentBill).items[(*currentBill).itemCount].itemDescription, (*selectedItem).itemDescription);
            (*currentBill).items[(*currentBill).itemCount].unitPrice = (*selectedItem).unitPrice;
            (*currentBill).items[(*currentBill).itemCount].amount = (*currentBill).items[(*currentBill).itemCount].quantity * (*selectedItem).unitPrice;
            (*currentBill).totalBill += (*currentBill).items[(*currentBill).itemCount].amount;
            (*currentBill).itemCount++;
        } else {
            printf("Item not found in menu. Please try again or add it via File Maintenance.\n");
        }

        while(true) {
            printf("Do you want to place another order? (y/n): ");
            scanf(" %c", &anotherOrder); 
            clearInputBuffer();
            if (anotherOrder == 'y' || anotherOrder == 'Y' || anotherOrder == 'n' || anotherOrder == 'N') {
                break;
            } else {
                printf("Invalid input. Please enter 'y' or 'n'.\n");
            }
        }
    } while (anotherOrder == 'y' || anotherOrder == 'Y');

    // --- RECEIPT GENERATION ---
    printf("\n\n%47s\n", "JOLLY - JIP RESTAURANT");
    printf("%43s\n\n", "BILLING STATEMENT");
    printf("DATE: %s\n", (*currentBill).date); // Uses user-entered date
    printf("TABLE NO.: %d\n\n", (*currentBill).tableNo);
    printf("--------------------------------------------------------------------------\n");
    printf("| %-10s | %-20s | %8s | %10s | %10s |\n",
        "ITEM CODE", "ITEM DESCRIPTION", "QUANTITY", "UNIT PRICE", "AMOUNT");
    printf("--------------------------------------------------------------------------\n");
    for (int k = 0; k < (*currentBill).itemCount; k++) {
        (*currentBill).items[k].amount = (*currentBill).items[k].quantity * (*currentBill).items[k].unitPrice;
        printf("| %-10s | %-20s | %8d | %10.2f | %10.2f |\n",
            (*currentBill).items[k].itemCode,
            (*currentBill).items[k].itemDescription,
            (*currentBill).items[k].quantity,
            (*currentBill).items[k].unitPrice,
            (*currentBill).items[k].amount);
    }
    printf("--------------------------------------------------------------------------\n");

    printf("TOTAL BILL : %.2f\n", (*currentBill).totalBill);

    // Calculate discount
    (*currentBill).discount = 0.0;

    // Simplified 2% discount logic: only check total bill, not month
    if ((*currentBill).totalBill >= 500.00 && (*currentBill).totalBill <= 2000.00) {
        (*currentBill).discount = (*currentBill).totalBill * 0.02;
    }

    char discountType;
    while(true) {
        printf("Is the customer a Senior Citizen (S) or Person with Disability (P) or None (N)? ");
        scanf(" %c", &discountType);
        clearInputBuffer();
        if (discountType == 'S' || discountType == 's' || discountType == 'P' || discountType == 'p' || discountType == 'N' || discountType == 'n') {
            break;
        } else {
            printf("Invalid input. Please enter 'S', 'P', or 'N'.\n");
        }
    }

    if (discountType == 'S' || discountType == 's') {
        (*currentBill).discount = (*currentBill).totalBill * 0.20;
    } else if (discountType == 'P' || discountType == 'p') {
        (*currentBill).discount = (*currentBill).totalBill * 0.05;
    }

    printf("DISCOUNT : %.2f\n", (*currentBill).discount);

    (*currentBill).netBill = (*currentBill).totalBill - (*currentBill).discount;
    printf("NET BILL : %.2f\n", (*currentBill).netBill);

    // Amount tendered and change
    while(true) {
        printf("AMOUNT TENDERED: ");
        if (scanf("%lf", &(*currentBill).amountTendered) != 1 || (*currentBill).amountTendered < 0) {
            printf("Invalid input. Please enter a non-negative numeric value.\n");
            clearInputBuffer();
        } else {
            clearInputBuffer();
            break;
        }
    }
    

    while ((*currentBill).amountTendered < (*currentBill).netBill) {
        double additionalAmount;
        while(true) {
            printf("Insufficient Fund. Please tender additional amount: ");
            if (scanf("%lf", &additionalAmount) != 1 || additionalAmount <= 0) {
                printf("Invalid input. Please enter a positive numeric value for additional amount.\n");
                clearInputBuffer();
            } else {
                clearInputBuffer();
                break;
            }
        }
        (*currentBill).amountTendered += additionalAmount;
    }

    (*currentBill).change = (*currentBill).amountTendered - (*currentBill).netBill;
    printf("CHANGE : %.2f\n", (*currentBill).change);

    // REMOVED: Global daily sales updates
    // dailyTotalSales += (*currentBill).totalBill;
    // dailyTotalDiscount += (*currentBill).discount;
    // dailyTotalNetBilling += (*currentBill).netBill;

    // NEW: Save the completed bill to file
    saveBillToFile(currentBill); 
}

// MODIFIED: generateDailyTotalSales to read from file
void generateDailyTotalSales() {
    char reportDate[15];
    printf("\n--- GENERATE DAILY TOTAL SALES REPORT ---\n");
    printf("Enter the date for the report (MM/DD/YYYY): ");
    fgets(reportDate, sizeof(reportDate), stdin);
    reportDate[strcspn(reportDate, "\n")] = 0; // Remove newline character

    double currentDailyTotalSales = 0.0;
    double currentDailyTotalDiscount = 0.0;
    double currentDailyTotalNetBilling = 0.0;

    FILE *file = fopen("sales_history.txt", "r");
    if (file == NULL) {
        printf("No sales history found (sales_history.txt does not exist or is empty).\n");
        printf("Total billing for the day: %.2f\n", currentDailyTotalSales);
        printf("Total Discount provided for the day: %.2f\n", currentDailyTotalDiscount);
        printf("Total Net Billing for the day: %.2f\n", currentDailyTotalNetBilling);
        return;
    }

    char line[256]; // Buffer to read lines from the file
    struct Bill tempBill; // Temporary structure to hold bill data as we parse
    int currentItemCount = 0; // To track items within a parsed bill

    // Initialize tempBill to ensure no garbage values
    memset(&tempBill, 0, sizeof(struct Bill));

    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n")] = 0; // Remove trailing newline

        if (strstr(line, "DATE:") == line) {
            sscanf(line, "DATE:%s", tempBill.date);
        } else if (strstr(line, "TABLENO:") == line) {
            sscanf(line, "TABLENO:%d", &tempBill.tableNo);
        } else if (strstr(line, "ITEMCOUNT:") == line) {
            sscanf(line, "ITEMCOUNT:%d", &tempBill.itemCount);
            currentItemCount = 0; // Reset item counter for a new bill
        } else if (strstr(line, "ITEM:") == line) {
            if (currentItemCount < 50) { // Ensure we don't overflow the items array
                char tempCode[10];
                char tempDesc[50];
                double tempPrice, tempAmount;
                int tempQty;

                // ITEM line format: ITEM:CODE DESCRIPTION PRICE QUANTITY AMOUNT
                // We need to carefully parse description which might contain spaces (underscores in file)
                char *ptr = line + strlen("ITEM:"); // Move pointer past "ITEM:"
                
                // Read itemCode
                sscanf(ptr, "%s", tempCode);
                strcpy(tempBill.items[currentItemCount].itemCode, tempCode);
                ptr += strlen(tempCode) + 1; // Move past item code and space

                // Find the start of unit price to isolate description
                char *price_start_ptr = ptr;
                int spaces_found = 0;
                while (*price_start_ptr != '\0') {
                    if (*price_start_ptr == ' ') {
                        spaces_found++;
                    }
                    if (spaces_found == 1) { // Found the space after description
                        break;
                    }
                    price_start_ptr++;
                }

                // Extract description substring
                strncpy(tempDesc, ptr, price_start_ptr - ptr);
                tempDesc[price_start_ptr - ptr] = '\0';

                // Replace underscores with spaces in description
                for (int i = 0; tempDesc[i] != '\0'; i++) {
                    if (tempDesc[i] == '_') {
                        tempDesc[i] = ' ';
                    }
                }
                strcpy(tempBill.items[currentItemCount].itemDescription, tempDesc);

                // Parse the remaining numerical values
                sscanf(price_start_ptr, "%lf %d %lf",
                       &tempPrice,
                       &tempQty,
                       &tempAmount);
                tempBill.items[currentItemCount].unitPrice = tempPrice;
                tempBill.items[currentItemCount].quantity = tempQty;
                tempBill.items[currentItemCount].amount = tempAmount;

                currentItemCount++;
            }
        }
        else if (strstr(line, "TOTALBILL:") == line) {
            sscanf(line, "TOTALBILL:%lf", &tempBill.totalBill);
        } else if (strstr(line, "DISCOUNT:") == line) {
            sscanf(line, "DISCOUNT:%lf", &tempBill.discount);
        } else if (strstr(line, "NETBILL:") == line) {
            sscanf(line, "NETBILL:%lf", &tempBill.netBill);
        } else if (strstr(line, "AMOUNT_TENDERED:") == line) {
            sscanf(line, "AMOUNT_TENDERED:%lf", &tempBill.amountTendered);
        } else if (strstr(line, "CHANGE:") == line) {
            sscanf(line, "CHANGE:%lf", &tempBill.change);
        } else if (strcmp(line, "END_BILL") == 0) { // End of a bill record
            // Process the parsed bill only if its date matches the report date
            if (strcmp(tempBill.date, reportDate) == 0) {
                currentDailyTotalSales += tempBill.totalBill;
                currentDailyTotalDiscount += tempBill.discount;
                currentDailyTotalNetBilling += tempBill.netBill;
            }
            // Reset for next bill
            memset(&tempBill, 0, sizeof(struct Bill)); // Clear tempBill for next read
            currentItemCount = 0; // Reset item counter
        }
    }
    fclose(file);

    int consoleWidth = 80;
    char reportTitleLine1[] = "DAILY TOTAL SALES REPORT";
    char reportTitleLine2[50];
    sprintf(reportTitleLine2, "As of %s", reportDate);

    int len1 = strlen(reportTitleLine1);
    int len2 = strlen(reportTitleLine2);

    printf("\n%*s%s\n", (consoleWidth - len1) / 2, "", reportTitleLine1);
    printf("%*s%s\n", (consoleWidth - len2) / 2, "", reportTitleLine2);
    printf("\n");
    printf("Total billing for the day: %.2f\n", currentDailyTotalSales);
    printf("Total Discount provided for the day: %.2f\n", currentDailyTotalDiscount);
    printf("Total Net Billing for the day: %.2f\n", currentDailyTotalNetBilling);
}

// --- FILE MAINTENANCE FUNCTIONS ---
void addMenuRecord() {
    if (menuCount >= 100) {
        printf("Menu is full. Cannot add more items.\n");
        return;
    }
    printf("\n--- ADD NEW MENU RECORD ---\n");
    
    // Check for duplicate item code
    char tempCode[10];
    while (true) {
        printf("Enter Item Code: ");
        fgets(tempCode, sizeof(tempCode), stdin);
        tempCode[strcspn(tempCode, "\n")] = 0; // Remove newline

        if (strlen(tempCode) == 0) {
            printf("Item Code cannot be empty. Please try again.\n");
            continue;
        }

        bool isDuplicate = false;
        for (int i = 0; i < menuCount; i++) {
            if (strcmp(menu[i].itemCode, tempCode) == 0) {
                isDuplicate = true;
                break;
            }
        }
        if (isDuplicate) {
            printf("Item Code already exists. Please enter a unique code.\n");
        } else {
            strcpy(menu[menuCount].itemCode, tempCode);
            break;
        }
    }

    printf("Enter Item Description (use '_' for spaces, e.g., Family_Package_1): ");
    fgets(menu[menuCount].itemDescription, sizeof(menu[menuCount].itemDescription), stdin);
    menu[menuCount].itemDescription[strcspn(menu[menuCount].itemDescription, "\n")] = 0; // Remove newline
    if (strlen(menu[menuCount].itemDescription) == 0) { // Basic check for empty description
        printf("Item Description cannot be empty. Setting to 'N/A'.\n");
        strcpy(menu[menuCount].itemDescription, "N/A");
    }
    for (int i = 0; menu[menuCount].itemDescription[i] != '\0'; i++) {
        if (menu[menuCount].itemDescription[i] == '_') {
            menu[menuCount].itemDescription[i] = ' ';
        }
    }

    while(true) {
        printf("Enter Unit Price: ");
        if (scanf("%lf", &menu[menuCount].unitPrice) != 1 || menu[menuCount].unitPrice <= 0) {
            printf("Invalid input. Unit Price must be a positive numeric value.\n");
            clearInputBuffer();
        } else {
            clearInputBuffer();
            break;
        }
    }
    
    printf("Enter Item Type (e.g., Solo_Package, Individual_Order): ");
    fgets(menu[menuCount].itemType, sizeof(menu[menuCount].itemType), stdin);
    menu[menuCount].itemType[strcspn(menu[menuCount].itemType, "\n")] = 0; // Remove newline
    if (strlen(menu[menuCount].itemType) == 0) { // Basic check for empty type
        printf("Item Type cannot be empty. Setting to 'N/A'.\n");
        strcpy(menu[menuCount].itemType, "N/A");
    }
    for (int i = 0; menu[menuCount].itemType[i] != '\0'; i++) {
        if (menu[menuCount].itemType[i] == '_') {
            menu[menuCount].itemType[i] = ' ';
        }
    }

    menuCount++;
    saveMenuItems(); // Save changes to file
    printf("Menu record added successfully.\n");
}

void updateMenuRecord() {
    printf("\n--- UPDATE MENU RECORD ---\n");
    printf("Enter Item Code of the record to update: ");
    char itemCodeToUpdate[10];
    scanf("%s", itemCodeToUpdate);
    clearInputBuffer(); // Clear buffer after scanf %s

    int foundIndex = -1;
    for (int i = 0; i < menuCount; i++) {
        if (strcmp(menu[i].itemCode, itemCodeToUpdate) == 0) {
            foundIndex = i;
            break;
        }
    }

    if (foundIndex != -1) {
        int choice;
        printf("Record found: %s - %s (%.2f)\n", menu[foundIndex].itemCode, menu[foundIndex].itemDescription, menu[foundIndex].unitPrice);
        while(true) {
            printf("1. Update Menu Description\n");
            printf("2. Update Price\n");
            printf("Enter your choice: ");
            if (scanf("%d", &choice) != 1 || (choice != 1 && choice != 2)) {
                printf("Invalid input. Please enter 1 or 2.\n");
                clearInputBuffer();
            } else {
                clearInputBuffer();
                break;
            }
        }

        if (choice == 1) {
            printf("Enter new Item Description (use '_' for spaces): ");
            fgets(menu[foundIndex].itemDescription, sizeof(menu[foundIndex].itemDescription), stdin);
            menu[foundIndex].itemDescription[strcspn(menu[foundIndex].itemDescription, "\n")] = 0; // Remove newline
            if (strlen(menu[foundIndex].itemDescription) == 0) {
                printf("Item Description cannot be empty. No change applied.\n");
            } else {
                for (int i = 0; menu[foundIndex].itemDescription[i] != '\0'; i++) {
                    if (menu[foundIndex].itemDescription[i] == '_') {
                        menu[foundIndex].itemDescription[i] = ' ';
                    }
                }
                printf("Menu description updated.\n");
            }
        } else if (choice == 2) {
            while(true) {
                printf("Enter new Unit Price: ");
                if (scanf("%lf", &menu[foundIndex].unitPrice) != 1 || menu[foundIndex].unitPrice <= 0) {
                    printf("Invalid input. Unit Price must be a positive numeric value.\n");
                    clearInputBuffer();
                } else {
                    clearInputBuffer();
                    printf("Price updated.\n");
                    break;
                }
            }
        }
        saveMenuItems(); // Save changes to file
    } else {
        printf("Item Code not found.\n");
    }
}

void displayMenuItems() {
    printf("\n--- CURRENT MENU ---\n");
    if (menuCount == 0) {
        printf("Menu is empty.\n");
        return;
    }
    printf("--------------------------------------------------------------------------------\n");
    printf("%-10s %-30s %-15s %-20s\n", "CODE", "DESCRIPTION", "PRICE", "TYPE");
    printf("--------------------------------------------------------------------------------\n");
    for (int i = 0; i < menuCount; i++) {
        printf("%-10s %-30s %-15.2f %-20s\n",
               menu[i].itemCode,
               menu[i].itemDescription,
               menu[i].unitPrice,
               menu[i].itemType);
    }
}

// --- SUB-MENU FUNCTIONS ---
void billingSubMenu() {
    int billingChoice;
    do {
        printf("\n--- BILLING SUB-MODULE ---\n");
        printf("1. ORDERING and RECEIPT\n");
        printf("2. GENERATE DAILY TOTAL SALES\n");
        while(true) {
            printf("CHOOSE TRANSACTION: ");
            if (scanf("%d", &billingChoice) != 1 || (billingChoice != 1 && billingChoice != 2)) {
                printf("Invalid input. Please enter 1 or 2.\n");
                clearInputBuffer();
            } else {
                clearInputBuffer();
                break;
            }
        }

        switch (billingChoice) {
            case 1: {
                struct Bill newBill;
                // Initialize newBill to ensure no garbage values
                memset(&newBill, 0, sizeof(struct Bill));
                billingModule(&newBill); // Pass a new bill structure for each order
                break;
            }
            case 2:
                generateDailyTotalSales(); // Call without parameters
                break;
        }
        while(true) {
            printf("\nPress '0' to return to Main Menu or any other number to stay in Billing: ");
            if (scanf("%d", &billingChoice) != 1) {
                printf("Invalid input. Please enter a number.\n");
                clearInputBuffer();
            } else {
                clearInputBuffer();
                break;
            }
        }
    } while (billingChoice != 0);
}

void fileMaintenanceMenu() {
    int fileMaintenanceChoice;
    do {
        printf("\n--- FILE MAINTENANCE ---\n");
        printf("1. ADD NEW RECORD/(s)\n");
        printf("2. UPDATE RECORD/(s)\n");
        printf("3. DISPLAY UPDATED RECORD/(s)\n");
        while(true) {
            printf("CHOOSE TRANSACTION: ");
            if (scanf("%d", &fileMaintenanceChoice) != 1 || (fileMaintenanceChoice < 1 || fileMaintenanceChoice > 3)) {
                printf("Invalid input. Please enter 1, 2, or 3.\n");
                clearInputBuffer();
            } else {
                clearInputBuffer();
                break;
            }
        }

        switch (fileMaintenanceChoice) {
            case 1:
                addMenuRecord();
                break;
            case 2:
                updateMenuRecord();
                break;
            case 3:
                displayMenuItems();
                break;
        }
        while(true) {
            printf("\nPress '0' to return to Main Menu or any other number to stay in File Maintenance: ");
            if (scanf("%d", &fileMaintenanceChoice) != 1) {
                printf("Invalid input. Please enter a number.\n");
                clearInputBuffer();
            } else {
                clearInputBuffer();
                break;
            }
        }
    } while (fileMaintenanceChoice != 0);
}


// --- MAIN FUNCTION ---
int main() {
    loadMenuItems(); // Load existing menu items at startup or populate initial menu

    int mainChoice;
    do {
        printf("\n"); // Add a newline for spacing

        // Centering the main menu titles assuming an 80-character console width
        printf("%*sJOLLY - JIP RESTAURANT\n", (80 - (int)strlen("JOLLY - JIP RESTAURANT")) / 2, "");
        printf("%*sFINANCIAL INFORMATION SYSTEM\n", (80 - (int)strlen("FINANCIAL INFORMATION SYSTEM")) / 2, "");
        printf("%*sMAIN MENU\n", (80 - (int)strlen("MAIN MENU")) / 2, "");
        
        printf("1. BILLING SUB-MODULE\n");
        printf("2. FILE MAINTENANCE\n");
        printf("0. EXIT\n"); // Explicit Exit option
        while(true) {
            printf("CHOOSE TRANSACTION: ");
            if (scanf("%d", &mainChoice) != 1 || (mainChoice < 0 || mainChoice > 2)) {
                printf("Invalid input. Please enter 0, 1, or 2.\n");
                clearInputBuffer();
            } else {
                clearInputBuffer();
                break;
            }
        }

        switch (mainChoice) {
            case 1:
                billingSubMenu();
                break;
            case 2:
                fileMaintenanceMenu();
                break;
            case 0:
                printf("\nExiting JOLLY-JIP RESTAURANT Financial Information System. Goodbye!\n");
                break;
        }
    } while (mainChoice != 0);

    return 0;
}
