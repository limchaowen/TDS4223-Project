#include <iostream>
#include <fstream>
#include <cstring>
#include <conio.h>
#include <windows.h>
#include <cctype>
#include <cstdlib>
#include <limits>
#include <iomanip>
#include <stdexcept>
#include <ctime>
#include <direct.h>

using namespace std;

// Color codes for design
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define BLUE    "\033[34m"
#define YELLOW  "\033[33m"

void setColor(const char* color) { cout << color; }
void resetColor() { cout << RESET; }

// Product structure with constructor
struct Product {
    char name[100];
    float price;
    char category[50];
    int quantity; // For cart purposes
    Product(const char* n = "", float p = 0.0, const char* c = "", int q = 0) {
        strncpy(name, n, 99); name[99] = '\0';
        price = p;
        strncpy(category, c, 49); category[49] = '\0';
        quantity = q;
    }
};

// Node for linked list
struct Node {
    Product data;
    char time[50]; // Added to store timestamp
    Node* next;
    Node(const Product& p, const char* t = "") : data(p), next(nullptr) {
        strncpy(time, t, 49); time[49] = '\0';
    }
};

// Customer class
class Customer {
private:
    Node* productList; // Linked list of all products
    Node* cartList;
    Node* orderHistory; // Linked list for order history
    int productCount;
    int cartCount;
    int orderHistoryCount;
    char username[50];
    char password[50];
    bool isLoggedIn;

    void clearList(Node*& head) {
        while (head) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }

    void insertAtEnd(Node*& head, const Product& p, const char* t = "") {
        Node* newNode = new Node(p, t);
        if (!head) {
            head = newNode;
            return;
        }
        Node* current = head;
        while (current->next) current = current->next;
        current->next = newNode;
    }

    bool loadProducts(const char* filename) {
        ifstream file(filename);
        char cwd[256];
        if (!file) {
            _getcwd(cwd, sizeof(cwd));
            return false;
        }
        clearList(productList);
        productCount = 0;
        char line[150];
        while (file.getline(line, 150)) {
            char* token = strtok(line, ",");
            if (token) {
                char name[100];
                strncpy(name, token, 99); name[99] = '\0';
                char* nameEnd = name + strlen(name) - 1;
                while (nameEnd > name && isspace((unsigned char)*nameEnd)) nameEnd--;
                *(nameEnd + 1) = '\0';
                while (isspace((unsigned char)*name)) memmove(name, name + 1, strlen(name));
                token = strtok(NULL, ",");
                if (token) {
                    char priceStr[20];
                    strncpy(priceStr, token, 19); priceStr[19] = '\0';
                    char* priceEnd = priceStr + strlen(priceStr) - 1;
                    while (priceEnd > priceStr && isspace((unsigned char)*priceEnd)) priceEnd--;
                    *(priceEnd + 1) = '\0';
                    while (isspace((unsigned char)*priceStr)) memmove(priceStr, priceStr + 1, strlen(priceStr));
                    token = strtok(NULL, ",");
                    if (token) {
                        char category[50];
                        strncpy(category, token, 49); category[49] = '\0';
                        char* catEnd = category + strlen(category) - 1;
                        while (catEnd > category && isspace((unsigned char)*catEnd)) catEnd--;
                        *(catEnd + 1) = '\0';
                        while (isspace((unsigned char)*category)) memmove(category, category + 1, strlen(category));
                        char* priceStart = strstr(priceStr, "RM");
                        float price = 0.0;
                        if (priceStart && sscanf(priceStart + 2, "%f", &price) == 1) {
                            Product p(name, price, category, 0);
                            insertAtEnd(productList, p);
                            productCount++;
                        }
                    }
                }
            }
        }
        file.close();
        return productCount > 0;
    }

    bool loadCategories(const char* filename, char categories[][50], int& catCount) {
        ifstream file(filename);
        if (!file) {
            return false;
        }
        catCount = 0;
        char line[50];
        while (file.getline(line, 50) && catCount < 10) {
            strncpy(categories[catCount], line, 49);
            categories[catCount++][49] = '\0';
        }
        file.close();
        return catCount > 0;
    }

    void loadOrderHistory(const char* filename) {
        ifstream file(filename);
        if (!file) {
            return; // File might not exist yet, which is fine for new users
        }
        clearList(orderHistory);
        orderHistoryCount = 0;
        char line[200];
        while (file.getline(line, 200)) {
            char tempUser[50], tempTime[50], tempName[100];
            int tempQty;
            float tempPrice;
            // Parse the line: username\t timestamp\t name\t quantity\t total price
            if (sscanf(line, "%s\t%[^\t]\t%[^\t]\t%d\tRM%f", tempUser, tempTime, tempName, &tempQty, &tempPrice) == 5) {
                if (strcmp(tempUser, username) == 0) {
                    // Load only the orders for the current user
                    Node* currentProduct = productList;
                    char tempCategory[50] = ""; // Default to empty if not found
                    while (currentProduct) {
                        if (strcmp(currentProduct->data.name, tempName) == 0) {
                            strncpy(tempCategory, currentProduct->data.category, 49);
                            tempCategory[49] = '\0';
                            break;
                        }
                        currentProduct = currentProduct->next;
                    }
                    Product p(tempName, tempPrice, tempCategory, tempQty);
                    insertAtEnd(orderHistory, p, tempTime);
                    orderHistoryCount++;
                }
            }
        }
        file.close();
    }

public:
    Customer() : productList(nullptr), cartList(nullptr), orderHistory(nullptr), 
                 productCount(0), cartCount(0), orderHistoryCount(0), isLoggedIn(false) {
        username[0] = '\0'; password[0] = '\0';
        // Initialize products silently
        if (!loadProducts("flowers.txt")) {
            // Handle failure silently or log to file if needed
        }
    }
    ~Customer() {
        clearList(productList);
        clearList(cartList);
        // Do not clear orderHistory here to allow reloading in next session
    }

    bool registerCustomer(const char* filename) {
        system("cls");
        setColor(BLUE); cout << "\n=== Customer Registration ===\n"; resetColor();
        cout << "Enter new username: ";
        char newUsername[50];
        cin.getline(newUsername, 50);
        if (strlen(newUsername) == 0) {
            setColor(RED); cout << "Username cannot be empty.\n"; resetColor();
            cout << "\nPress Enter to return...";
            _getch();
            return false;
        }
        ifstream checkFile(filename);
        char line[150];
        while (checkFile.getline(line, 150)) {
            char tempUser[50];
            if (sscanf(line, "%s", tempUser) == 1) {
                if (strcmp(newUsername, tempUser) == 0) {
                    checkFile.close();
                    setColor(RED); cout << "Username already exists.\n"; resetColor();
                    cout << "\nPress Enter to return...";
                    _getch();
                    return false;
                }
            }
        }
        checkFile.close();
        cout << "Enter password: ";
        char newPassword[50];
        cin.getline(newPassword, 50);
        if (strlen(newPassword) == 0) {
            setColor(RED); cout << "Password cannot be empty.\n"; resetColor();
            cout << "\nPress Enter to return...";
            _getch();
            return false;
        }
        ofstream file(filename, ios::app);
        if (!file) {
            setColor(RED); cout << "Failed to open registration file.\n"; resetColor();
            cout << "\nPress Enter to return...";
            _getch();
            return false;
        }
        file << newUsername << " " << newPassword << "\n"; // Removed the '0'
        file.close();
        setColor(GREEN); cout << "Registration successful!\n"; resetColor();
        cout << "\nPress Enter to return...";
        _getch();
        return true;
    }

    bool login(const char* filename) {
        system("cls");
        setColor(BLUE); cout << "\n=== Customer Login ===\n"; resetColor();
        int attempts = 3;
        while (attempts > 0) {
            cout << "Enter username: ";
            cin.getline(username, 50);
            if (strlen(username) == 0) {
                setColor(RED); cout << "Username cannot be empty.\n"; resetColor();
                continue;
            }
            cout << "Enter password: ";
            cin.getline(password, 50);
            if (strlen(password) == 0) {
                setColor(RED); cout << "Password cannot be empty.\n"; resetColor();
                continue;
            }
            ifstream file(filename);
            char line[150];
            bool found = false;
            while (file.getline(line, 150)) {
                char tempUser[50], tempPass[50];
                if (sscanf(line, "%s %s", tempUser, tempPass) == 2) { // Removed tempAdmin check
                    if (strcmp(username, tempUser) == 0 && strcmp(password, tempPass) == 0) {
                        file.close();
                        isLoggedIn = true;
                        clearList(orderHistory); // Clear existing history before loading new user's history
                        loadOrderHistory("orders.txt"); // Load only this user's order history
                        setColor(GREEN); cout << "Login successful!\n"; resetColor();
                        return true;
                    }
                    found = true;
                }
            }
            file.close();
            if (found) {
                attempts--;
                setColor(RED); cout << "Invalid credentials.\n"; resetColor();
                if (attempts > 0) {
                    cout << "Attempts remaining: " << attempts << "\n";
                    Sleep(1500); system("cls");
                    setColor(BLUE); cout << "\n=== Customer Login ===\n"; resetColor();
                } else {
                    setColor(RED); cout << "Too many failed attempts. Returning to welcome menu.\n"; resetColor();
                    Sleep(2000);
                    return false;
                }
            }
        }
        return false;
    }

    void logout() {
        if (isLoggedIn) {
            isLoggedIn = false;
            setColor(RED); cout << "Customer logged out.\n"; resetColor();
            cout << "\nPress Enter to return to welcome menu...";
            _getch();
        } else {
            setColor(RED); cout << "Not logged in.\n"; resetColor();
        }
    }

    void searchProduct() {
        system("cls");
        setColor(BLUE); cout << "\n=== Search Product ===\n"; resetColor();
        if (!productList) {
            setColor(RED); cout << "No products available.\n"; resetColor();
            cout << "\nPress Enter to return...";
            _getch();
            return;
        }
        char target[100];
        cout << "Enter part of Product Name to search: ";
        cin.getline(target, 100);
        if (strlen(target) == 0) {
            setColor(RED); cout << "No input provided.\n"; resetColor();
            cout << "\nPress Enter to return...";
            _getch();
            return;
        }
        char targetLower[100];
        strcpy(targetLower, target);
        for (int i = 0; targetLower[i]; i++) {
            targetLower[i] = tolower((unsigned char)targetLower[i]);
        }
        Node* current = productList;
        bool found = false;
        while (current) {
            char nameLower[100];
            strcpy(nameLower, current->data.name);
            for (int i = 0; nameLower[i]; i++) {
                nameLower[i] = tolower((unsigned char)nameLower[i]);
            }
            if (strstr(nameLower, targetLower)) {
                if (!found) {
                    setColor(GREEN); cout << "Found matches: " << endl; resetColor();
                    found = true;
                }
                setColor(GREEN); cout << "Name: " << current->data.name << ", "; resetColor();
                setColor(BLUE); cout << "Price: RM" << fixed << setprecision(2) << current->data.price << ", "; resetColor();
                setColor(GREEN); cout << "Category: " << current->data.category << endl; resetColor();
            }
            current = current->next;
        }
        if (!found) {
            setColor(RED); cout << "No products found matching '" << target << "'.\n"; resetColor();
        }
        cout << "\nPress Enter to return...";
        _getch();
    }

    void sortProducts() {
        system("cls");
        setColor(BLUE); cout << "\n=== Sort Products ===\n"; resetColor();
        if (!productList) {
            setColor(RED); cout << "No products available.\n"; resetColor();
            cout << "\nPress Enter to return...";
            _getch();
            return;
        }
        char categories[10][50];
        int catCount = 0;
        if (!loadCategories("categories.txt", categories, catCount)) {
            setColor(RED); cout << "Failed to load categories.\n"; resetColor();
            cout << "\nPress Enter to return...";
            _getch();
            return;
        }
        cout << "Sort Options:\n";
        cout << "1. Show All Products\n";
        for (int i = 0; i < catCount; i++) {
            cout << i + 2 << ". " << categories[i] << endl;
        }
        cout << "Select option (1-" << catCount + 1 << ", or 0 to cancel): ";
        int choice;
        while (!(cin >> choice) || choice < 0 || choice > catCount + 1) {
            setColor(RED); cout << "Invalid input. Please enter a number between 0 and " << catCount + 1 << ": "; resetColor();
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cin.ignore();
        if (choice == 0) {
            setColor(RED); cout << "Cancelled.\n"; resetColor();
            cout << "\nPress Enter to return...";
            _getch();
            return;
        }
        Node* current = productList;
        Node* sortedList = nullptr;
        int count = 0;
        while (current) {
            insertAtEnd(sortedList, current->data);
            count++;
            current = current->next;
        }
        for (int i = 0; i < count - 1; i++) {
            current = sortedList;
            Node* next = current->next;
            for (int j = 0; j < count - i - 1; j++) {
                if (strcmp(current->data.name, next->data.name) > 0) {
                    Product temp = current->data;
                    current->data = next->data;
                    next->data = temp;
                }
                current = next;
                next = next->next;
            }
        }
        current = sortedList;
        int i = 1;
        bool found = false;
        // Calculate maximum lengths dynamically
        int maxNameLength = 0;
        int maxCategoryLength = 0;
        Node* temp = productList;
        while (temp) {
            int nameLen = strlen(temp->data.name);
            int catLen = strlen(temp->data.category);
            if (nameLen > maxNameLength) maxNameLength = nameLen;
            if (catLen > maxCategoryLength) maxCategoryLength = catLen;
            temp = temp->next;
        }
        // Ensure minimum width for labels
        maxNameLength = max(maxNameLength, 4); // Minimum 4 for "Name"
        maxCategoryLength = max(maxCategoryLength, 8); // Minimum 8 for "Category"
        string headerLine = "+-----+";
        headerLine += string(maxNameLength + 2, '-') + "+-----------+";
        headerLine += string(maxCategoryLength + 2, '-') + "+";
        string header = "| No. | " + string(maxNameLength, ' ') + " | Price     | ";
        header += string(maxCategoryLength, ' ') + " |";
        cout << headerLine << endl;
        cout << "| No. | " << left << setw(maxNameLength) << "Name" << " | Price     | " 
             << left << setw(maxCategoryLength) << "Category" << " |" << endl;
        cout << headerLine << endl;
        while (current) {
            if (choice == 1 || strcmp(current->data.category, categories[choice - 2]) == 0) {
                if (!found) found = true;
                char priceStr[10];
                sprintf(priceStr, "RM%.2f", current->data.price);
                cout << "| ";
                setColor(YELLOW); cout << setw(3) << setfill(' ') << i; resetColor();
                cout << " | ";
                setColor(GREEN); cout << left << setw(maxNameLength) << current->data.name; resetColor();
                cout << " | ";
                setColor(BLUE); cout << left << setw(9) << setfill(' ') << priceStr; resetColor();
                cout << " | ";
                setColor(GREEN); cout << left << setw(maxCategoryLength) << current->data.category; resetColor();
                cout << " |" << endl;
                i++;
            }
            current = current->next;
        }
        cout << headerLine << endl;
        clearList(sortedList);
        if (!found) {
            setColor(RED); cout << "No products found in this category.\n"; resetColor();
        }
        cout << "\nPress Enter to return...";
        _getch();
    }

    void addProductToCart() {
        system("cls");
        setColor(BLUE); cout << "\n=== Add Product to Cart ===\n"; resetColor();
        if (!isLoggedIn) {
            setColor(RED); cout << "Please login first.\n"; resetColor();
            cout << "\nPress Enter to return...";
            _getch();
            return;
        }
        if (!productList) {
            setColor(RED); cout << "No products available.\n"; resetColor();
            cout << "\nPress Enter to return...";
            _getch();
            return;
        }
        displayProducts();
        cout << "\nEnter the number of the product to add to cart (1-" << productCount << ", or 0 to cancel): ";
        int choice;
        while (!(cin >> choice) || choice < 0 || choice > productCount) {
            setColor(RED); cout << "Invalid input. Please enter a number between 0 and " << productCount << ": "; resetColor();
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cin.ignore();
        if (choice == 0) {
            setColor(RED); cout << "Cancelled.\n"; resetColor();
            cout << "\nPress Enter to return...";
            _getch();
            return;
        }
        Node* current = productList;
        for (int i = 1; i < choice && current; i++) current = current->next;
        if (!current) {
            setColor(RED); cout << "Product not found.\n"; resetColor();
            cout << "\nPress Enter to return...";
            _getch();
            return;
        }
        int q;
        cout << "Enter Quantity (max 5): ";
        while (!(cin >> q) || q <= 0 || q > 5) {
            setColor(RED); cout << "Invalid quantity. Please enter a number between 1 and 5: "; resetColor();
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cin.ignore();
        Product p = current->data;
        p.quantity = q;
        insertAtEnd(cartList, p);
        cartCount++;
        setColor(GREEN); cout << q << " " << p.name << " added to cart!\n"; resetColor();
        cout << "\nPress Enter to return...";
        _getch();
    }

    void displayProducts() {
        system("cls");
        setColor(BLUE); cout << "\n=== Available Products ===\n"; resetColor();
        if (!productList) {
            setColor(RED); cout << "No products available.\n"; resetColor();
            cout << "\nPress Enter to return...";
            _getch();
            return;
        }
        Node* current = productList;
        int i = 1;
        // Calculate maximum lengths dynamically
        int maxNameLength = 0;
        int maxCategoryLength = 0;
        Node* temp = productList;
        while (temp) {
            int nameLen = strlen(temp->data.name);
            int catLen = strlen(temp->data.category);
            if (nameLen > maxNameLength) maxNameLength = nameLen;
            if (catLen > maxCategoryLength) maxCategoryLength = catLen;
            temp = temp->next;
        }
        // Ensure minimum width for labels
        maxNameLength = max(maxNameLength, 4); // Minimum 4 for "Name"
        maxCategoryLength = max(maxCategoryLength, 8); // Minimum 8 for "Category"
        string headerLine = "+-----+";
        headerLine += string(maxNameLength + 2, '-') + "+-----------+";
        headerLine += string(maxCategoryLength + 2, '-') + "+";
        string header = "| No. | " + string(maxNameLength, ' ') + " | Price     | ";
        header += string(maxCategoryLength, ' ') + " |";
        cout << headerLine << endl;
        cout << "| No. | " << left << setw(maxNameLength) << "Name" << " | Price     | " 
             << left << setw(maxCategoryLength) << "Category" << " |" << endl;
        cout << headerLine << endl;
        while (current) {
            char priceStr[10];
            sprintf(priceStr, "RM%.2f", current->data.price);
            cout << "| ";
            setColor(YELLOW); cout << setw(3) << setfill(' ') << i; resetColor();
            cout << " | ";
            setColor(GREEN); cout << left << setw(maxNameLength) << current->data.name; resetColor();
            cout << " | ";
            setColor(BLUE); cout << left << setw(9) << setfill(' ') << priceStr; resetColor();
            cout << " | ";
            setColor(GREEN); cout << left << setw(maxCategoryLength) << current->data.category; resetColor();
            cout << " |" << endl;
            current = current->next;
            i++;
        }
        cout << headerLine << endl;
        cout << "\nPress Enter to return...";
        _getch();
    }

    void removeFromCart() {
        system("cls");
        setColor(BLUE); cout << "\n=== Remove from Cart ===\n"; resetColor();
        if (!isLoggedIn) {
            setColor(RED); cout << "Please login first.\n"; resetColor();
            cout << "\nPress Enter to return...";
            _getch();
            return;
        }
        if (!cartList) {
            setColor(RED); cout << "Cart is empty.\n"; resetColor();
            cout << "\nPress Enter to return...";
            _getch();
            return;
        }
        displayCart();
        cout << "\nEnter the number of the product to remove (1-" << cartCount << ", or 0 to cancel): ";
        int choice;
        while (!(cin >> choice) || choice < 0 || choice > cartCount) {
            setColor(RED); cout << "Invalid input. Please enter a number between 0 and " << cartCount << ": "; resetColor();
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cin.ignore();
        if (choice == 0) {
            setColor(RED); cout << "Cancelled.\n"; resetColor();
            cout << "\nPress Enter to return...";
            _getch();
            return;
        }
        Node* current = cartList;
        Node* prev = nullptr;
        for (int i = 1; i < choice && current; i++) {
            prev = current;
            current = current->next;
        }
        if (!current) {
            setColor(RED); cout << "Product not found.\n"; resetColor();
            cout << "\nPress Enter to return...";
            _getch();
            return;
        }
        if (prev) prev->next = current->next;
        else cartList = current->next;
        delete current;
        cartCount--;
        setColor(GREEN); cout << "Product removed from cart!\n"; resetColor();
        cout << "\nPress Enter to return...";
        _getch();
    }

    void updateCartQuantity() {
        system("cls");
        setColor(BLUE); cout << "\n=== Update Cart Quantity ===\n"; resetColor();
        if (!isLoggedIn) {
            setColor(RED); cout << "Please login first.\n"; resetColor();
            cout << "\nPress Enter to return...";
            _getch();
            return;
        }
        if (!cartList) {
            setColor(RED); cout << "Cart is empty.\n"; resetColor();
            cout << "\nPress Enter to return...";
            _getch();
            return;
        }
        displayCart();
        cout << "\nEnter the number of the product to update (1-" << cartCount << ", or 0 to cancel): ";
        int choice;
        while (!(cin >> choice) || choice < 0 || choice > cartCount) {
            setColor(RED); cout << "Invalid input. Please enter a number between 0 and " << cartCount << ": "; resetColor();
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cin.ignore();
        if (choice == 0) {
            setColor(RED); cout << "Cancelled.\n"; resetColor();
            cout << "\nPress Enter to return...";
            _getch();
            return;
        }
        Node* current = cartList;
        for (int i = 1; i < choice && current; i++) current = current->next;
        if (!current) {
            setColor(RED); cout << "Product not found.\n"; resetColor();
            cout << "\nPress Enter to return...";
            _getch();
            return;
        }
        int q;
        cout << "Enter new Quantity (max 5): ";
        while (!(cin >> q) || q <= 0 || q > 5) {
            setColor(RED); cout << "Invalid quantity. Please enter a number between 1 and 5: "; resetColor();
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cin.ignore();
        current->data.quantity = q;
        setColor(GREEN); cout << "Quantity updated to " << q << " for " << current->data.name << "!\n"; resetColor();
        cout << "\nPress Enter to return...";
        _getch();
    }

    void order() {
        system("cls");
        setColor(BLUE); cout << "\n=== Order ===\n"; resetColor();
        if (!isLoggedIn) {
            setColor(RED); cout << "Please login first.\n"; resetColor();
            cout << "\nPress Enter to return...";
            _getch();
            return;
        }
        if (!cartList) {
            setColor(RED); cout << "Cart is empty.\n"; resetColor();
            cout << "\nPress Enter to return...";
            _getch();
            return;
        }
        Node* current = cartList;
        int i = 1;
        while (current) {
            setColor(YELLOW); cout << i++ << ". "; resetColor();
            setColor(GREEN); cout << "Name: " << current->data.name << ", "; resetColor();
            setColor(BLUE); cout << "Price: RM" << fixed << setprecision(2) << current->data.price << ", "; resetColor();
            setColor(RED); cout << "Qty: " << current->data.quantity << ", "; resetColor();
            setColor(GREEN); cout << "Category: " << current->data.category << endl; resetColor();
            current = current->next;
        }
        cout << "\nDo you want to order? (1 for Yes, 0 for No): ";
        int choice;
        while (!(cin >> choice) || (choice != 0 && choice != 1)) {
            setColor(RED); cout << "Invalid input. Please enter 0 or 1: "; resetColor();
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cin.ignore();
        if (choice == 1) {
            ofstream file("orders.txt", ios::app);
            if (!file) {
                setColor(RED); cout << "Failed to open orders file.\n"; resetColor();
                cout << "\nPress Enter to return...";
                _getch();
                return;
            }
            time_t now = time(0);
            char* dt = ctime(&now);
            dt[strlen(dt) - 1] = '\0';
            float total = 0.0;
            system("cls");
            setColor(BLUE); cout << "\n=== Invoice ===\n"; resetColor();
            cout << "Date: " << dt << endl;
            cout << "Username: " << username << endl;
            cout << "\nItems Ordered:" << endl;
            // Calculate maximum lengths dynamically
            int maxNameLength = 4; // Minimum for "Name"
            int maxPriceLength = 7; // Minimum for "Price" (e.g., "RM159.00")
            int maxQtyLength = 3; // Minimum for "Qty"
            int maxTotalLength = 7; // Minimum for "Total" (e.g., "RM300.00")
            current = cartList;
            while (current) {
                int nameLen = strlen(current->data.name);
                if (nameLen > maxNameLength) maxNameLength = nameLen;
                char priceStr[10], totalStr[10];
                sprintf(priceStr, "RM%.2f", current->data.price);
                sprintf(totalStr, "RM%.2f", current->data.price * current->data.quantity);
                int priceLen = strlen(priceStr);
                int totalLen = strlen(totalStr);
                if (priceLen > maxPriceLength) maxPriceLength = priceLen;
                if (totalLen > maxTotalLength) maxTotalLength = totalLen;
                char qtyStr[4];
                sprintf(qtyStr, "%d", current->data.quantity);
                int qtyLen = strlen(qtyStr);
                if (qtyLen > maxQtyLength) maxQtyLength = qtyLen;
                current = current->next;
            }
            // Build dynamic table line
            string line = "+";
            line += string(4, '-') + "+"; // No. column
            line += string(maxNameLength + 2, '-') + "+"; // Name column
            line += string(maxPriceLength + 2, '-') + "+"; // Price column
            line += string(maxQtyLength + 2, '-') + "+"; // Qty column
            line += string(maxTotalLength + 2, '-') + "+"; // Total column
            cout << line << endl;
            cout << "| No.| " << left << setw(maxNameLength) << "Name" << " | " 
                 << left << setw(maxPriceLength) << "Price" << " | " 
                 << left << setw(maxQtyLength) << "Qty" << " | " 
                 << left << setw(maxTotalLength) << "Total" << " |" << endl;
            cout << line << endl;
            current = cartList;
            i = 1;
            while (current) {
                float itemTotal = current->data.price * current->data.quantity;
                total += itemTotal;
                char priceStr[10], totalStr[10];
                sprintf(priceStr, "RM%.2f", current->data.price);
                sprintf(totalStr, "RM%.2f", itemTotal);
                char qtyStr[4];
                sprintf(qtyStr, "%d", current->data.quantity);
                cout << "| ";
                setColor(YELLOW); cout << setw(2) << setfill(' ') << i; resetColor();
                cout << " | ";
                setColor(GREEN); cout << left << setw(maxNameLength) << current->data.name; resetColor();
                cout << " | ";
                setColor(BLUE); cout << left << setw(maxPriceLength) << setfill(' ') << priceStr; resetColor();
                cout << " | ";
                setColor(RED); cout << right << setw(maxQtyLength) << setfill(' ') << qtyStr; resetColor();
                cout << " | ";
                setColor(BLUE); cout << left << setw(maxTotalLength) << setfill(' ') << totalStr; resetColor();
                cout << " |" << endl;
                file << username << "\t" << dt << "\t" << current->data.name << "\t" << current->data.quantity 
                     << "\tRM" << fixed << setprecision(2) << itemTotal << endl;
                Product p = current->data;
                insertAtEnd(orderHistory, p, dt);
                orderHistoryCount++;
                current = current->next;
                i++;
            }
            cout << line << endl;
            char totalStr[10];
            sprintf(totalStr, "RM%.2f", total);
            cout << "Total Amount: " << left << setw(maxTotalLength + 10) << setfill(' ') << totalStr << endl;
            cout << line << endl;
            file.close();
            clearList(cartList);
            cartCount = 0;
            setColor(GREEN); cout << "Order successful!\n"; resetColor();
            cout << "\nPress Enter to return...";
            _getch();
        } else {
            setColor(RED); cout << "Order cancelled.\n"; resetColor();
            cout << "\nPress Enter to return...";
            _getch();
        }
    }

    void viewOrderHistory() {
        system("cls");
        setColor(BLUE); cout << "\n=== Purchase Histories ===\n"; resetColor();
        if (!orderHistory) {
            setColor(RED); cout << "No purchase history available.\n"; resetColor();
            cout << "\nPress Enter to return...";
            _getch();
            return;
        }
        Node* current = orderHistory;
        int i = 1;
        // Calculate maximum lengths dynamically
        int maxNameLength = 4; // Minimum for "Order"
        int maxQtyLength = 3; // Minimum for "Qty"
        int maxTimeLength = 10; // Minimum for "Time" (e.g., "Thu Jun 19")
        while (current) {
            int nameLen = strlen(current->data.name);
            if (nameLen > maxNameLength) maxNameLength = nameLen;
            char qtyStr[4];
            sprintf(qtyStr, "%d", current->data.quantity);
            int qtyLen = strlen(qtyStr);
            if (qtyLen > maxQtyLength) maxQtyLength = qtyLen;
            int timeLen = strlen(current->time);
            if (timeLen > maxTimeLength) maxTimeLength = timeLen;
            current = current->next;
        }
        // Build dynamic table line
        string line = "+";
        line += string(4, '-') + "+"; // No. column
        line += string(maxNameLength + 2, '-') + "+"; // Order column
        line += string(maxQtyLength + 2, '-') + "+"; // Qty column
        line += string(maxTimeLength + 2, '-') + "+"; // Time column
        cout << line << endl;
        cout << "| No.| " << left << setw(maxNameLength) << "Order" << " | " 
             << left << setw(maxQtyLength) << "Qty" << " | " 
             << left << setw(maxTimeLength) << "Time" << " |" << endl;
        cout << line << endl;
        current = orderHistory;
        while (current) {
            char qtyStr[4];
            sprintf(qtyStr, "%d", current->data.quantity);
            cout << "| ";
            setColor(YELLOW); cout << setw(2) << setfill(' ') << i; resetColor();
            cout << " | ";
            setColor(GREEN); cout << left << setw(maxNameLength) << current->data.name; resetColor();
            cout << " | ";
            setColor(RED); cout << right << setw(maxQtyLength) << setfill(' ') << qtyStr; resetColor();
            cout << " | ";
            setColor(BLUE); cout << left << setw(maxTimeLength) << current->time; resetColor();
            cout << " |" << endl;
            current = current->next;
            i++;
        }
        cout << line << endl;
        cout << "\nPress Enter to return...";
        _getch();
    }

    void displayCart() {
        system("cls");
        setColor(BLUE); cout << "\n=== Your Cart ===\n"; resetColor();
        if (!cartList) {
            setColor(RED); cout << "Cart is empty.\n"; resetColor();
            cout << "\nPress Enter to return...";
            _getch();
            return;
        }
        Node* current = cartList;
        int i = 1;
        while (current) {
            setColor(YELLOW); cout << i++ << ". "; resetColor();
            setColor(GREEN); cout << "Name: " << current->data.name << ", "; resetColor();
            setColor(BLUE); cout << "Price: RM" << fixed << setprecision(2) << current->data.price << ", "; resetColor();
            setColor(RED); cout << "Qty: " << current->data.quantity << ", "; resetColor();
            setColor(GREEN); cout << "Category: " << current->data.category << endl; resetColor();
            current = current->next;
        }
        cout << "\nPress Enter to return...";
        _getch();
    }
};

// Main function with customer menu
int main() {
    Customer cust;
    const char* loginFilename = "login.txt";
    const char* productsFilename = "flowers.txt";
    const char* ordersFilename = "orders.txt";

    bool loggedIn = false;
    while (true) {
        while (!loggedIn) {
            system("cls");
            setColor(BLUE); cout << "\n==== Welcome ====\n"; resetColor();
            cout << "1. Customer Login\n";
            cout << "2. Register\n";
            cout << "3. Exit Program\n";
            cout << "Enter choice (1-3): ";
            string input;
            int initialChoice;
            getline(cin, input);
            try {
                size_t pos;
                initialChoice = stoi(input, &pos);
                string remaining = input.substr(pos);
                remaining.erase(0, remaining.find_first_not_of(" \t\n\r"));
                if (!remaining.empty()) throw invalid_argument("");
            } catch (const invalid_argument&) {
                setColor(RED); cout << "Invalid input. Please enter 1, 2, or 3.\n"; resetColor();
                Sleep(1500);
                continue;
            } catch (const out_of_range&) {
                setColor(RED); cout << "Input number is out of range. Please enter 1, 2, or 3.\n"; resetColor();
                Sleep(1500);
                continue;
            }
            switch (initialChoice) {
                case 1:
                    if (cust.login(loginFilename)) loggedIn = true;
                    break;
                case 2:
                    cust.registerCustomer(loginFilename);
                    break;
                case 3:
                    setColor(RED); cout << "Exiting program.\n"; resetColor();
                    Sleep(1000);
                    return 0;
                default:
                    setColor(RED); cout << "Invalid choice. Please enter 1, 2, or 3.\n"; resetColor();
                    Sleep(1500);
            }
        }

        while (loggedIn) {
            system("cls");
            setColor(BLUE); cout << "\n==== Customer Menu ====\n"; resetColor();
            cout << "1. Add Product to Cart\n";
            cout << "2. Search Product\n";
            cout << "3. Sort Products\n";
            cout << "4. Remove from Cart\n";
            cout << "5. Update Cart Quantity\n";
            cout << "6. Order\n";
            cout << "7. View Purchase Histories\n";
            cout << "8. Logout\n";
            cout << "Enter choice (1-8): ";
            string input;
            int choice;
            getline(cin, input);
            try {
                size_t pos;
                choice = stoi(input, &pos);
                string remaining = input.substr(pos);
                remaining.erase(0, remaining.find_first_not_of(" \t\n\r"));
                if (!remaining.empty()) throw invalid_argument("");
            } catch (const invalid_argument&) {
                setColor(RED); cout << "Invalid input. Please enter 1 to 8.\n"; resetColor();
                Sleep(1500);
                continue;
            } catch (const out_of_range&) {
                setColor(RED); cout << "Input number is out of range. Please enter 1 to 8.\n"; resetColor();
                Sleep(1500);
                continue;
            }
            switch (choice) {
                case 1: cust.addProductToCart(); break;
                case 2: cust.searchProduct(); break;
                case 3: cust.sortProducts(); break;
                case 4: cust.removeFromCart(); break;
                case 5: cust.updateCartQuantity(); break;
                case 6: cust.order(); break;
                case 7: cust.viewOrderHistory(); break;
                case 8:
                    cust.logout();
                    loggedIn = false;
                    break;
                default:
                    setColor(RED); cout << "Invalid choice. Please enter 1 to 8.\n"; resetColor();
                    Sleep(1500);
            }
        }
    }
    return 0;
}