#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <conio.h>
#include <windows.h>
#include <cctype>
#include <cstdlib>
#include <limits>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <direct.h>
using namespace std;
#define RESET       "\033[0m"
#define RED         "\033[31m"
#define GREEN       "\033[32m"
#define YELLOW      "\033[33m"
#define LIGHT_BLUE  "\033[94m"
void setColor(const char* color) {
    cout << color;
}
void resetColor() {
    cout << RESET;
}
void getConsoleSize(int& width, int& height) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    } else {
        width = 80;
        height = 25;
    }
}
void showLoading() {
    int console_width, console_height;
    getConsoleSize(console_width, console_height);
    int barWidth = 50;
    string loadingText = "Loading...";
    string prefix = loadingText + " [";
    string suffix = "]";
    int content_width = prefix.length() + barWidth + suffix.length();
    int horizontal_padding = (console_width > content_width) ? (console_width - content_width) / 2 : 0;
    int vertical_padding = (console_height > 1) ? (console_height / 2) - 1 : 0;
    for (int i = 0; i < vertical_padding; ++i) {
        cout << "\n";
    }
    cout << string(horizontal_padding, ' ') + prefix << flush;
    int total_steps = barWidth;
    int duration_ms = 2000;
    int sleep_ms_per_step = (total_steps > 0) ? duration_ms / total_steps : 0;
    if (sleep_ms_per_step == 0 && total_steps > 0) sleep_ms_per_step = 1;
    for (int i = 0; i <= total_steps; ++i) {
        cout << string(i, '\xDB');
        cout << string(total_steps - i, '\xB0');
        cout << suffix << flush;
        cout << "\r" + string(horizontal_padding, ' ') + prefix << flush;
        if (i < total_steps) {
            Sleep(sleep_ms_per_step);
        }
    }
    cout << string(total_steps, '\xDB') + suffix << endl;
}
string getPassword() {
    string password;
    char ch;
    cout << flush;
    while ((ch = _getch()) != 13 && ch != 10) {
        if (ch == 8) {
            if (!password.empty()) {
                password.erase(password.length() - 1);
                cout << "\b \b" << flush;
            }
        } else if (ch >= 32 && ch <= 126) {
            password += ch;
            cout << '*' << flush;
        }
    }
    cout << endl;
    return password;
}
int manualMax(int a, int b) {
    return (a > b) ? a : b;
}
string intToString(int val) {
    stringstream ss;
    ss << val;
    return ss.str();
}
int custom_stoi(const string& str, size_t* pos = 0, int base = 10) {
    stringstream ss(str);
    long result;
    ss >> result;
    if (ss.fail()) {
        throw invalid_argument("stoi: invalid argument");
    }
    if (result > numeric_limits<int>::max() || result < numeric_limits<int>::min()) {
        throw out_of_range("stoi: out of range");
    }
    if (pos) {
        *pos = ss.tellg();
        if (*pos == (size_t)-1) *pos = str.length();
    }
    return static_cast<int>(result);
}
double custom_stod(const string& str, size_t* pos = 0) {
    stringstream ss(str);
    double result;
    ss >> result;
    if (ss.fail()) {
        throw invalid_argument("stod: invalid argument");
    }
    if (pos) {
        *pos = ss.tellg();
        if (*pos == (size_t)-1) *pos = str.length();
    }
    return result;
}
string manualToLower(const string& str) {
    string lower = str;
    for (size_t i = 0; i < lower.length(); ++i) {
        lower[i] = static_cast<char>(tolower(static_cast<unsigned char>(lower[i])));
    }
    return lower;
}
size_t manualFind(const string& str, char ch, size_t start_pos = 0) {
    for (size_t i = start_pos; i < str.length(); ++i) {
        if (str[i] == ch) {
            return i;
        }
    }
    return string::npos;
}
bool manualContains(const string& str, const string& substr) {
    if (substr.empty()) return true;
    string lowerStr = manualToLower(str);
    string lowerSubstr = manualToLower(substr);
    if (lowerSubstr.length() > lowerStr.length()) return false;
    for (size_t i = 0; i <= lowerStr.length() - lowerSubstr.length(); ++i) {
        bool match = true;
        for (size_t j = 0; j < lowerSubstr.length(); ++j) {
            if (lowerStr[i + j] != lowerSubstr[j]) {
                match = false;
                break;
            }
        }
        if (match) return true;
    }
    return false;
}
bool manualFileContains(const string& filename, const string& search_term) {
    ifstream file(filename.c_str());
    if (!file) {
        return false;
    }
    string line;
    string lower_search_term = manualToLower(search_term);
    while (getline(file, line)) {
        line.erase(0, line.find_first_not_of(" \t\n\r"));
        line.erase(line.find_last_not_of(" \t\n\r") + 1);
        if (!line.empty()) {
            size_t commaPos = manualFind(line, ',');
            string storedUsername = (commaPos == string::npos) ? line : line.substr(0, commaPos);
            storedUsername.erase(0, storedUsername.find_first_not_of(" \t\n\r"));
            storedUsername.erase(storedUsername.find_last_not_of(" \t\n\r") + 1);
            if (manualToLower(storedUsername) == lower_search_term) {
                file.close();
                return true;
            }
        }
    }
    file.close();
    return false;
}
struct Product {
    string name;
    string price;
    string category;
    int stock;
    Product(string n = "", string p = "", string c = "", int s = 0)
        : name(n), price(p), category(c), stock(s) {}
    bool operator<(const Product& other) const {
        return manualToLower(name) < manualToLower(other.name);
    }
     bool operator==(const Product& other) const {
        return manualToLower(name) == manualToLower(other.name);
    }
};
class ProductList {
private:
    Product* productsArray;
    int productCount;
    int productCapacity;
    string* categoryArray;
    int categoryCount;
    int categoryCapacity;
    string* getPurchasedProductNames(const string& ordersFilename, int& count) const {
        ifstream file(ordersFilename.c_str());
        if (!file) {
            count = 0;
            return NULL;
        }
        const int initialCapacity = 20;
        string* tempNames = new string[initialCapacity];
        int capacity = initialCapacity;
        count = 0;
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string user, time, name, qty, price;
            if (getline(ss, user, '\t') && getline(ss, time, '\t') && getline(ss, name, '\t')) {
                bool found = false;
                string lowerName = manualToLower(name);
                for (int i = 0; i < count; ++i) {
                    if (manualToLower(tempNames[i]) == lowerName) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    if (count == capacity) {
                        int newCapacity = capacity * 2;
                        string* newArray = new string[newCapacity];
                        for (int i = 0; i < count; ++i) newArray[i] = tempNames[i];
                        delete[] tempNames;
                        tempNames = newArray;
                        capacity = newCapacity;
                    }
                    tempNames[count++] = name;
                }
            }
        }
        file.close();
        return tempNames;
    }
    bool isProductPurchased(const string& productName, const string* purchasedNames, int count) const {
        string lowerProductName = manualToLower(productName);
        for (int i = 0; i < count; i++) {
            if (manualToLower(purchasedNames[i]) == lowerProductName) {
                return true;
            }
        }
        return false;
    }
    void resizeCategoryArray(int newCapacity) {
        if (newCapacity < categoryCount) return;
        if (newCapacity == 0) newCapacity = 1;
        string* newArray = new string[newCapacity];
        for (int i = 0; i < categoryCount; ++i) {
            newArray[i] = categoryArray[i];
        }
        delete[] categoryArray;
        categoryArray = newArray;
        categoryCapacity = newCapacity;
    }
    void resizeProductsArray(int newCapacity) {
        if (newCapacity < productCount) return;
        if (newCapacity == 0) newCapacity = 10;
        Product* newArray = new Product[newCapacity];
        for (int i = 0; i < productCount; ++i) {
            newArray[i] = productsArray[i];
        }
        delete[] productsArray;
        productsArray = newArray;
        productCapacity = newCapacity;
    }
public:
    ProductList() : productsArray(NULL), productCount(0), productCapacity(0),
                    categoryArray(NULL), categoryCount(0), categoryCapacity(0) {}
    ~ProductList() {
        delete[] productsArray;
        productsArray = NULL;
        productCount = 0;
        productCapacity = 0;
        delete[] categoryArray;
        categoryArray = NULL;
        categoryCount = 0;
        categoryCapacity = 0;
    }
    ProductList(const ProductList& other) = delete;
    ProductList& operator=(const ProductList& other) = delete;
    bool isValidPrice(const string& price) const {
        if (price.length() < 4 || manualToLower(price.substr(0, 3)) != "rm ") return false;
        string numPart = price.substr(3);
        bool hasDecimal = false;
        bool hasDigit = false;
        size_t decimalPos = string::npos;
        for (size_t i = 0; i < numPart.length(); ++i) {
            char c = numPart[i];
            if (isdigit(static_cast<unsigned char>(c))) {
                hasDigit = true;
            } else if (c == '.') {
                if (hasDecimal) return false;
                hasDecimal = true;
                decimalPos = i;
            } else {
                return false;
            }
        }
        if (!hasDigit || !hasDecimal || (numPart.length() - 1 - decimalPos) != 2) return false;
        try {
            size_t pos;
            custom_stod(numPart, &pos);
            return pos == numPart.length();
        } catch (...) {
            return false;
        }
    }
    void insertionSort(Product* arr, int n, bool ascending, string sortBy) const {
        if (arr == NULL || n <= 1) return;
        sortBy = manualToLower(sortBy);
        for (int i = 1; i < n; i++) {
            Product key = arr[i];
            int j = i - 1;
            if (sortBy == "category") {
                while (j >= 0) {
                    bool comparison = ascending ? (manualToLower(arr[j].category) > manualToLower(key.category)) : (manualToLower(arr[j].category) < manualToLower(key.category));
                    if (comparison) {
                        arr[j + 1] = arr[j];
                        j--;
                    } else {
                        break;
                    }
                }
            } else if (sortBy == "name") {
                while (j >= 0) {
                    bool comparison = ascending ? (manualToLower(arr[j].name) > manualToLower(key.name)) : (manualToLower(arr[j].name) < manualToLower(key.name));
                    if (comparison) {
                        arr[j + 1] = arr[j];
                        j--;
                    } else {
                        break;
                    }
                }
            }
            arr[j + 1] = key;
        }
    }
    void insertionSortCategories(string* arr, int n, bool ascending) {
        if (arr == NULL || n <= 1) return;
        for (int i = 1; i < n; i++) {
            string key = arr[i];
            string lowerKey = manualToLower(key);
            int j = i - 1;
            while (j >= 0) {
                bool comparison = ascending ? (manualToLower(arr[j]) > lowerKey) : (manualToLower(arr[j]) < lowerKey);
                if (comparison) {
                    arr[j + 1] = arr[j];
                    j--;
                } else {
                    break;
                }
            }
            arr[j + 1] = key;
        }
    }
    int binarySearch(const string& searchTerm) const {
        if (productCount == 0) return -1;
        string lowerSearchTerm = manualToLower(searchTerm);
        int low = 0;
        int high = productCount - 1;
        while (low <= high) {
            int mid = low + (high - low) / 2;
            string lowerMidName = manualToLower(productsArray[mid].name);
            if (lowerMidName == lowerSearchTerm) {
                return mid;
            } else if (lowerMidName < lowerSearchTerm) {
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        }
        return -1;
    }
    int binarySearchCategory(const string& searchTerm) const {
        if (categoryCount == 0) return -1;
        string lowerSearchTerm = manualToLower(searchTerm);
        int low = 0;
        int high = categoryCount - 1;
        while (low <= high) {
            int mid = low + (high - low) / 2;
            string lowerMidName = manualToLower(categoryArray[mid]);
            if (lowerMidName == lowerSearchTerm) {
                return mid;
            } else if (lowerMidName < lowerSearchTerm) {
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        }
        return -1;
    }
    void searchCategory(const string& searchTerm) {
        system("cls");
        setColor(LIGHT_BLUE); cout << "\n=== Search Category ===\n"; resetColor();
        if (searchTerm.empty()) {
            setColor(RED); cout << "Search term cannot be empty.\n"; resetColor();
            cout << "\nPress any key to return to menu..."; _getch(); return;
        }
        if (categoryCount == 0) {
            setColor(RED); cout << "No categories available to search.\n"; resetColor();
            cout << "\nPress any key to return to menu..."; _getch(); return;
        }
        int foundIndex = binarySearchCategory(searchTerm);
        if (foundIndex == -1) {
            setColor(RED); cout << "No exact category found matching '" << searchTerm << "'.\n"; resetColor();
            cout << "\nPress any key to return to menu..."; _getch();
        } else {
            setColor(GREEN); cout << "=== Exact Match Category Found ===\n"; resetColor();
            int maxNumLength = (int)string("No.").length();
            int maxCategoryNameLength = (int)string("Category Name").length();
            maxNumLength = manualMax(maxNumLength, (int)intToString(1).length());
            maxCategoryNameLength = manualMax(maxCategoryNameLength, (int)categoryArray[foundIndex].length());
            const int COLUMN_PADDING_SPACES = 2;
            int numColWidth = maxNumLength + COLUMN_PADDING_SPACES;
            int categoryNameColWidth = maxCategoryNameLength + COLUMN_PADDING_SPACES;
            string headerLine = "+" + string(numColWidth, '-') + "+" + string(categoryNameColWidth, '-') + "+";
            cout << headerLine << endl;
            cout << "|" << left << setw(numColWidth) << " No."
                 << "|" << left << setw(categoryNameColWidth) << " Category Name"
                 << "|" << endl;
            cout << headerLine << endl;
            cout << "|" << left << setw(numColWidth) << (" " + intToString(1));
            cout << "|" << left << setw(categoryNameColWidth) << (" " + categoryArray[foundIndex]);
            cout << "|" << endl;
            cout << headerLine << endl;
            cout << "Enter category ID (1) to view products, or 0 to return: ";
            string input;
            int id;
            getline(cin, input);
            try {
                size_t pos;
                id = custom_stoi(input, &pos);
                string remaining = input.substr(pos);
                remaining.erase(0, remaining.find_first_not_of(" \t\n\r"));
                if (!remaining.empty()) {
                    throw invalid_argument("Contains non-numeric characters");
                }
            } catch (...) {
                setColor(RED); cout << "Invalid input. Please enter a number.\n"; resetColor();
                cout << "\nPress any key to return to menu..."; _getch(); return;
            }
            if (id == 0) {
                setColor(RED); cout << "Returning to menu.\n"; resetColor();
            } else if (id == 1) {
                system("cls");
                displayProductNamesByCategory(categoryArray[foundIndex]);
            } else {
                setColor(RED); cout << "Invalid category ID.\n"; resetColor();
                cout << "\nPress any key to return to menu..."; _getch();
            }
        }
        cout << "\nPress any key to return to menu..."; _getch();
    }
    void loadCategories(const string& filename) {
        delete[] categoryArray;
        categoryArray = NULL;
        categoryCount = 0;
        categoryCapacity = 0;
        ifstream inFile(filename.c_str());
        if (!inFile) {
            cerr << "Categories file not found: " << filename << ". Creating default.\n";
            ofstream outFile(filename.c_str());
            if (outFile) {
                outFile << "Uncategorized\n";
                outFile.close();
                cout << "Default category 'Uncategorized' created.\n";
                categoryCapacity = 10;
                categoryArray = new string[categoryCapacity];
                categoryArray[0] = "Uncategorized";
                categoryCount = 1;
            } else {
                cerr << "Error: Cannot create default categories file: " << filename << endl;
                throw runtime_error("Initialization failed: Cannot create categories file.");
            }
        } else {
            string line;
            categoryCapacity = 10;
            categoryArray = new string[categoryCapacity];
            categoryCount = 0;
            while (getline(inFile, line)) {
                size_t first = line.find_first_not_of(" \t\n\r");
                if (string::npos != first) {
                    size_t last = line.find_last_not_of(" \t\n\r");
                    line = line.substr(first, (last - first + 1));
                } else {
                    line = "";
                }
                if (!line.empty()) {
                    if (categoryCount == categoryCapacity) {
                        resizeCategoryArray(categoryCapacity * 2);
                    }
                    categoryArray[categoryCount++] = line;
                }
            }
            inFile.close();
            if (categoryCount == 0) {
                cout << "Warning: Categories file was empty or contained only whitespace. Adding a default category.\n";
                delete[] categoryArray;
                categoryCapacity = 10;
                categoryArray = new string[categoryCapacity];
                categoryArray[0] = "Uncategorized";
                categoryCount = 1;
                ofstream outFile(filename.c_str());
                if (outFile) {
                    outFile << "Uncategorized\n";
                    outFile.close();
                } else {
                    cerr << "Error saving default category to " << filename << endl;
                }
            }
        }
        insertionSortCategories(categoryArray, categoryCount, true);
        bool uncategorizedExists = false;
        for (int i = 0; i < categoryCount; ++i) {
            if (manualToLower(categoryArray[i]) == "uncategorized") {
                uncategorizedExists = true;
                break;
            }
        }
        if (!uncategorizedExists) {
            cout << "Warning: 'Uncategorized' category not found after initial load/create. Adding it.\n";
            if (categoryCount == categoryCapacity) {
                resizeCategoryArray(categoryCapacity * 2);
            }
            categoryArray[categoryCount++] = "Uncategorized";
            insertionSortCategories(categoryArray, categoryCount, true);
            ofstream outFile(filename.c_str());
            if (outFile) {
                for (int i = 0; i < categoryCount; ++i) {
                    outFile << categoryArray[i] << endl;
                }
                outFile.close();
            } else {
                cerr << "Error saving 'Uncategorized' category to " << filename << endl;
            }
        }
    }
    void addCategory(const string& filename) {
        system("cls");
        setColor(LIGHT_BLUE); cout << "\n=== Add New Category ===\n"; resetColor();
        string newCategory;
        cout << "Enter new category name: ";
        getline(cin, newCategory);
        newCategory.erase(0, newCategory.find_first_not_of(" \t\n\r"));
        newCategory.erase(newCategory.find_last_not_of(" \t\n\r") + 1);
        if (newCategory.empty()) {
            setColor(RED); cout << "Category name cannot be empty.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        bool exists = false;
        for (int i = 0; i < categoryCount; ++i) {
            if (manualToLower(categoryArray[i]) == manualToLower(newCategory)) {
                exists = true;
                break;
            }
        }
        if (exists) {
            setColor(RED); cout << "Category '" << newCategory << "' already exists (case-insensitive match).\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        if (categoryCount == categoryCapacity) {
            resizeCategoryArray(categoryCapacity == 0 ? 10 : categoryCapacity * 2);
        }
        categoryArray[categoryCount++] = newCategory;
        insertionSortCategories(categoryArray, categoryCount, true);
        try {
            ofstream outFile(filename.c_str(), ios::out | ios::trunc);
            if (!outFile) throw runtime_error("Cannot open categories file for writing: " + filename);
            for (int i = 0; i < categoryCount; ++i) {
                outFile << categoryArray[i] << endl;
            }
            outFile.close();
            setColor(GREEN); cout << "Category '" << newCategory << "' added successfully.\n"; resetColor();
        } catch (const runtime_error& e) {
            cerr << "Error saving categories: " << e.what() << endl;
            int addedIndex = -1;
            string lowerNewCat = manualToLower(newCategory);
            for (int i = 0; i < categoryCount; ++i) {
                if (manualToLower(categoryArray[i]) == lowerNewCat) {
                    addedIndex = i;
                    break;
                }
            }
            if (addedIndex != -1) {
                for (int i = addedIndex; i < categoryCount - 1; ++i) categoryArray[i] = categoryArray[i + 1];
                categoryCount--;
                if (categoryCount > 0 && categoryCapacity / categoryCount >= 2) {
                    resizeCategoryArray(categoryCapacity / 2);
                } else if (categoryCount == 0 && categoryCapacity > 1) {
                    resizeCategoryArray(1);
                }
            }
            setColor(YELLOW); cout << "Warning: Category was added in memory but failed to save to file. Data might be inconsistent.\n"; resetColor();
        }
        cout << "\nPress any key to return..."; _getch();
    }
    void deleteCategory(const string& categoriesFilename, const string& productsFilename) {
    system("cls");
    setColor(LIGHT_BLUE); cout << "\n=== Delete Category ===\n"; resetColor();
    int displayableCategoryCount = 0;
    for (int i = 0; i < categoryCount; ++i) {
        if (manualToLower(categoryArray[i]) != "uncategorized") {
            displayableCategoryCount++;
        }
    }
    if (displayableCategoryCount == 0) {
        setColor(RED); cout << "No categories available to delete.\n"; resetColor();
        cout << "\nPress any key to return..."; _getch(); return;
    }
    setColor(GREEN); cout << "=== Select Category to Delete ===\n"; resetColor();
    int maxNumLength = (int)string("No.").length();
    int maxCategoryNameLength = (int)string("Category Name").length();
    int* displayMap = new int[displayableCategoryCount];
    int currentDisplayIndex = 0;
    for (int i = 0; i < categoryCount; ++i) {
        if (manualToLower(categoryArray[i]) == "uncategorized") continue;
        maxNumLength = manualMax(maxNumLength, (int)intToString(currentDisplayIndex + 1).length());
        maxCategoryNameLength = manualMax(maxCategoryNameLength, (int)categoryArray[i].length());
    }
    const int COLUMN_PADDING_SPACES = 2;
    int numColWidth = maxNumLength + COLUMN_PADDING_SPACES;
    int categoryNameColWidth = maxCategoryNameLength + COLUMN_PADDING_SPACES;
    string headerLine = "+" + string(numColWidth, '-') + "+" + string(categoryNameColWidth, '-') + "+";
    cout << headerLine << endl;
    cout << "|" << left << setw(numColWidth) << " No."
         << "|" << left << setw(categoryNameColWidth) << " Category Name"
         << "|" << endl;
    cout << headerLine << endl;
    currentDisplayIndex = 0;
    for (int i = 0; i < categoryCount; ++i) {
        if (manualToLower(categoryArray[i]) == "uncategorized") {
            continue;
        }
        displayMap[currentDisplayIndex] = i;
        cout << "|" << left << setw(numColWidth) << (" " + intToString(currentDisplayIndex + 1));
        cout << "|" << left << setw(categoryNameColWidth) << (" " + categoryArray[i]);
        cout << "|" << endl;
        currentDisplayIndex++;
    }
    cout << headerLine << endl;
    cout << "Enter category ID (1-" << displayableCategoryCount << ") to delete, or 0 to return: ";
    string input;
    int id;
    getline(cin, input);
    try {
        size_t pos;
        id = custom_stoi(input, &pos);
        string remaining = input.substr(pos);
        remaining.erase(0, remaining.find_first_not_of(" \t\n\r"));
        if (!remaining.empty()) {
            throw invalid_argument("Contains non-numeric characters");
        }
    } catch (...) {
        setColor(RED); cout << "Invalid input. Please enter a number.\n"; resetColor();
        delete[] displayMap;
        return;
    }
    if (id == 0) {
        setColor(RED); cout << "Deletion cancelled.\n"; resetColor();
        delete[] displayMap;
        return;
    }
    if (id < 1 || id > displayableCategoryCount) {
        setColor(RED); cout << "Invalid category ID.\n"; resetColor();
        delete[] displayMap;
        return;
    }
    int originalIndexToRemove = displayMap[id - 1];
    string categoryToDelete = categoryArray[originalIndexToRemove];
    delete[] displayMap;
    bool inUse = false;
    for (int i = 0; i < productCount; ++i) {
        if (manualToLower(productsArray[i].category) == manualToLower(categoryToDelete)) {
            inUse = true;
            break;
        }
    }
    if (inUse) {
        setColor(RED);
        cout << "This category cannot be deleted because it is still in use by products.\n";
        resetColor();
        return;
    }
    cout << "\nAre you sure you want to delete the empty category '" << categoryToDelete << "'? (y/n): ";
    string response;
    getline(cin, response);
    response = manualToLower(response);
    if (response == "y") {
        for (int i = originalIndexToRemove; i < categoryCount - 1; ++i) {
            categoryArray[i] = categoryArray[i + 1];
        }
        categoryCount--;
        try {
            ofstream outFile(categoriesFilename.c_str(), ios::out | ios::trunc);
            if (!outFile) throw runtime_error("Cannot open categories file for writing: " + categoriesFilename);
            for (int i = 0; i < categoryCount; ++i) {
                outFile << categoryArray[i] << endl;
            }
            outFile.close();
            setColor(GREEN); cout << "Category '" << categoryToDelete << "' deleted successfully.\n"; resetColor();
        } catch (const runtime_error& e) {
            cerr << "Error saving categories after deletion: " << e.what() << endl;
            setColor(YELLOW); cout << "Category was removed from memory but failed to save to file. Data might be inconsistent.\n"; resetColor();
        }
    } else {
        setColor(RED); cout << "Category deletion cancelled.\n"; resetColor();
    }
}
    void addProduct(const string& filename) {
        system("cls");
        setColor(LIGHT_BLUE); cout << "\n=== Add New Product ===\n"; resetColor();
        string name, price, category, stockInput;
        int stock;
        cout << "Enter product name: ";
        getline(cin, name);
        name.erase(0, name.find_first_not_of(" \t\n\r"));
        name.erase(name.find_last_not_of(" \t\n\r") + 1);
        if (name.empty()) {
            setColor(RED); cout << "Product name cannot be empty.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        if (binarySearch(name) != -1) {
            setColor(RED); cout << "Product name '" << name << "' already exists!\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        cout << "Enter price (e.g., RM 140.00): ";
        getline(cin, price);
        price.erase(0, price.find_first_not_of(" \t\n\r"));
        price.erase(price.find_last_not_of(" \t\n\r") + 1);
        if (!isValidPrice(price)) {
            setColor(RED); cout << "Invalid price format! Use format like RM 140.00 (must have '.xx').\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        cout << "\n";
        setColor(GREEN); cout << "=== Select a Category ===\n"; resetColor();
        if (categoryCount == 0) {
            setColor(RED); cout << "No categories exist. Please add a category first.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        int maxNumLength = (int)string("ID").length();
        int maxCategoryNameLength = (int)string("Category Name").length();
        for (int i = 0; i < categoryCount; ++i) {
            maxNumLength = manualMax(maxNumLength, (int)intToString(i + 1).length());
            maxCategoryNameLength = manualMax(maxCategoryNameLength, (int)categoryArray[i].length());
        }
        const int COLUMN_PADDING_SPACES = 2;
        int numColWidth = maxNumLength + COLUMN_PADDING_SPACES;
        int categoryNameColWidth = maxCategoryNameLength + COLUMN_PADDING_SPACES;
        string headerLine = "+" + string(numColWidth, '-') + "+" + string(categoryNameColWidth, '-') + "+";
        cout << headerLine << endl;
        cout << "|" << left << setw(numColWidth) << " ID"
             << "|" << left << setw(categoryNameColWidth) << " Category Name"
             << "|" << endl;
        cout << headerLine << endl;
        for (int i = 0; i < categoryCount; i++) {
            cout << "|" << left << setw(numColWidth) << (" " + intToString(i + 1));
            cout << "|" << left << setw(categoryNameColWidth) << (" " + categoryArray[i]);
            cout << "|" << endl;
        }
        cout << headerLine << endl;
        cout << "Enter category ID (1-" << categoryCount << ", or 0 to cancel): ";
        string categoryIdInput;
        int categoryId;
        getline(cin, categoryIdInput);
        try {
            size_t pos;
            categoryId = custom_stoi(categoryIdInput, &pos);
            string remaining = categoryIdInput.substr(pos);
            remaining.erase(0, remaining.find_first_not_of(" \t\n\r"));
            if (!remaining.empty()) {
                throw invalid_argument("Contains non-numeric characters");
            }
        } catch (...) {
            setColor(RED); cout << "Invalid input. Please enter a number.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        if (categoryId == 0) {
            setColor(RED); cout << "Product addition cancelled.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        if (categoryId < 1 || categoryId > categoryCount) {
            setColor(RED); cout << "Invalid category ID!\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        category = categoryArray[categoryId - 1];
        cout << "\nEnter stock: ";
        getline(cin, stockInput);
        stockInput.erase(0, stockInput.find_first_not_of(" \t\n\r"));
        stockInput.erase(stockInput.find_last_not_of(" \t\n\r") + 1);
        try {
            size_t pos;
            stock = custom_stoi(stockInput, &pos);
            string remaining = stockInput.substr(pos);
            remaining.erase(0, remaining.find_first_not_of(" \t\n\r"));
            if (!remaining.empty()) {
                throw invalid_argument("Contains non-numeric characters");
            }
            if (stock < 0) {
                setColor(RED); cout << "Stock cannot be negative!\n"; resetColor();
                cout << "\nPress any key to return..."; _getch(); return;
            }
            if (stock > numeric_limits<int>::max()) {
                setColor(RED); cout << "Stock value is too large.\n"; resetColor();
                cout << "\nPress any key to return..."; _getch(); return;
            }
        } catch (...) {
            setColor(RED); cout << "Invalid stock value! Please enter a whole number.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        try {
            if (productCount == productCapacity) {
                resizeProductsArray(productCapacity == 0 ? 10 : productCapacity * 2);
            }
            productsArray[productCount++] = Product(name, price, category, stock);
            insertionSort(productsArray, productCount, true, "name");
            saveToFile(filename);
            setColor(GREEN); cout << "Product '" << name << "' added successfully.\n"; resetColor();
        } catch (bad_alloc& e) {
            cerr << "Error: Memory allocation failed (Out of memory)\n";
            setColor(RED); cout << "Product addition failed due to memory error.\n"; resetColor();
        } catch (const runtime_error& e) {
            cerr << "Error saving product list after adding: " << e.what() << endl;
            setColor(RED); cout << "Product was added in memory but failed to save to file. Data might be inconsistent.\n"; resetColor();
        }
        cout << "\nPress any key to return..."; _getch();
    }
    void deleteProduct(const string& filename, const string& ordersFilename) {
    system("cls");
    setColor(LIGHT_BLUE); cout << "\n=== Delete Product ===\n"; resetColor();
    if (productCount == 0) {
        setColor(RED); cout << "No products to delete.\n"; resetColor();
        cout << "\nPress any key to return to menu..."; _getch(); return;
    }
    setColor(GREEN); cout << "=== Select Product to Delete ===\n"; resetColor();
    int maxNumLength = (int)string("No.").length();
    int maxNameLength = (int)string("Product Name").length();
    for (int i = 0; i < productCount; ++i) {
        maxNumLength = manualMax(maxNumLength, (int)intToString(i + 1).length());
        maxNameLength = manualMax(maxNameLength, (int)productsArray[i].name.length());
    }
    const int COLUMN_PADDING_SPACES = 2;
    int numColWidth = maxNumLength + COLUMN_PADDING_SPACES;
    int nameColWidth = maxNameLength + COLUMN_PADDING_SPACES;
    string headerLine = "+" + string(numColWidth, '-') + "+" + string(nameColWidth, '-') + "+";
    cout << headerLine << endl;
    cout << "|" << left << setw(numColWidth) << " No."
         << "|" << left << setw(nameColWidth) << " Product Name"
         << "|" << endl;
    cout << headerLine << endl;
    for (int i = 0; i < productCount; i++) {
        cout << "|" << left << setw(numColWidth) << (" " + intToString(i + 1));
        cout << "|" << left << setw(nameColWidth) << (" " + productsArray[i].name);
        cout << "|" << endl;
    }
    cout << headerLine << endl;
    cout << "Enter product ID to delete (1-" << productCount << "), or 0 to return: ";
    string input;
    int id;
    getline(cin, input);
    try {
        size_t pos;
        id = custom_stoi(input, &pos);
        string remaining = input.substr(pos);
        remaining.erase(0, remaining.find_first_not_of(" \t\n\r"));
        if (!remaining.empty()) {
            throw invalid_argument("Contains non-numeric characters");
        }
    } catch (...) {
        setColor(RED); cout << "Invalid input. Please enter a number.\n"; resetColor();
        return;
    }
    if (id == 0) {
        setColor(RED); cout << "Deletion cancelled.\n"; resetColor();
        return;
    }
    if (id < 1 || id > productCount) {
        setColor(RED); cout << "Invalid product ID.\n"; resetColor();
        return;
    }
    int originalIndexToDelete = id - 1;
    Product productToDelete = productsArray[originalIndexToDelete];
    int purchasedCount = 0;
    string* purchasedNames = getPurchasedProductNames(ordersFilename, purchasedCount);
    bool purchased = isProductPurchased(productToDelete.name, purchasedNames, purchasedCount);
    delete[] purchasedNames;
    if (purchased) {
        setColor(RED);
        cout << "This product cannot be deleted because it has been purchased by a customer.\n";
        resetColor();
        return;
    }
    setColor(YELLOW); cout << "Are you sure you want to delete '" << productToDelete.name << "'? (y/n): "; resetColor();
    string confirmation;
    getline(cin, confirmation);
    confirmation = manualToLower(confirmation);
    if (confirmation == "y") {
        for (int i = originalIndexToDelete; i < productCount - 1; ++i) {
            productsArray[i] = productsArray[i + 1];
        }
        productCount--;
        try {
            saveToFile(filename);
            setColor(GREEN); cout << "Product '" << productToDelete.name << "' deleted successfully.\n"; resetColor();
        } catch (const runtime_error& e) {
            cerr << "Error saving product list after deletion: " << e.what() << endl;
            setColor(YELLOW); cout << "Product was removed from memory but failed to save to file. Data might be inconsistent.\n"; resetColor();
        }
    } else {
        setColor(RED); cout << "Deletion cancelled.\n"; resetColor();
    }
}
    void manualReplace(string& str, char oldChar, char newChar) const {
        for (size_t i = 0; i < str.length(); ++i) {
            if (str[i] == oldChar) {
                str[i] = newChar;
            }
        }
    }
    void saveToFile(const string& filename) {
        try {
            ofstream outFile(filename.c_str(), ios::out | ios::trunc);
            if (!outFile) throw runtime_error("Cannot open product file for writing: " + filename);
            for (int i = 0; i < productCount; ++i) {
                string safeName = productsArray[i].name;
                manualReplace(safeName, ',', ';');
                string safePrice = productsArray[i].price;
                string safeCategory = productsArray[i].category;
                manualReplace(safeCategory, ',', ';');
                outFile << safeName << "," << safePrice << ","
                        << safeCategory << "," << productsArray[i].stock << "\n";
            }
            outFile.close();
        } catch (const runtime_error& e) {
            throw runtime_error("Error saving to file: " + string(e.what()));
        }
    }
    void loadFromFile(const string& filename) {
        delete[] productsArray;
        productsArray = NULL;
        productCount = 0;
        productCapacity = 0;
        ifstream inFile(filename.c_str());
        if (!inFile) {
            cout << "Product file not found: " + filename + ". Creating empty file.\n";
            ofstream outFile(filename.c_str());
            if (!outFile) {
                cerr << "Warning: Could not create empty product file: " + filename + "\n";
            }
            outFile.close();
            return;
        }
        string line;
        int lineNumber = 0;
        Product* tempProducts = new Product[10];
        int tempProductCount = 0;
        int tempProductCapacity = 10;
        while (getline(inFile, line)) {
            lineNumber++;
            size_t first = line.find_first_not_of(" \t\n\r");
            if (string::npos != first) {
                size_t last = line.find_last_not_of(" \t\n\r");
                line = line.substr(first, (last - first + 1));
            } else {
                line = "";
            }
            if (line.empty()) continue;
            stringstream ss(line);
            string tempName, tempPrice, tempCategory, tempStockStr;
            int tempStock = 0;
            bool lineError = false;
            if (getline(ss, tempName, ',') &&
                getline(ss, tempPrice, ',') &&
                getline(ss, tempCategory, ',') &&
                getline(ss, tempStockStr)) {
                string leftover;
                if (getline(ss, leftover)) {
                    cerr << "Warning: Skipping malformed line (too many fields) #" << lineNumber << " in " << filename << ": " << line << endl;
                    lineError = true;
                }
            } else {
                cerr << "Warning: Skipping malformed line (not enough fields) #" << lineNumber << " in " << filename << ": " << line << endl;
                lineError = true;
            }
            if (lineError) continue;
            size_t first_name = tempName.find_first_not_of(" \t\n\r");
            size_t last_name = tempName.find_last_not_of(" \t\n\r");
            if (string::npos != first_name) tempName = tempName.substr(first_name, (last_name - first_name + 1));
            else tempName = "";
            size_t first_price = tempPrice.find_first_not_of(" \t\n\r");
            size_t last_price = tempPrice.find_last_not_of(" \t\n\r");
            if (string::npos != first_price) tempPrice = tempPrice.substr(first_price, (last_price - first_price + 1));
            else tempPrice = "";
            size_t first_category = tempCategory.find_first_not_of(" \t\n\r");
            size_t last_category = tempCategory.find_last_not_of(" \t\n\r");
            if (string::npos != first_category) tempCategory = tempCategory.substr(first_category, (last_category - first_category + 1));
            else tempCategory = "";
            size_t first_stock = tempStockStr.find_first_not_of(" \t\n\r");
            size_t last_stock = tempStockStr.find_last_not_of(" \t\n\r");
            if (string::npos != first_stock) tempStockStr = tempStockStr.substr(first_stock, (last_stock - first_stock + 1));
            else tempStockStr = "";
            manualReplace(tempName, ';', ',');
            manualReplace(tempCategory, ';', ',');
            try {
                long stockLong = strtol(tempStockStr.c_str(), NULL, 10);
                if (stockLong < 0) {
                    cerr << "Warning: Negative stock value '" << tempStockStr << "' for '" << tempName << "' on line #" << lineNumber << " in " << filename << ". Setting to 0.\n";
                    tempStock = 0;
                } else if (stockLong > numeric_limits<int>::max()) {
                    cerr << "Warning: Stock value '" << tempStockStr << "' too large for '" << tempName << "' on line #" << lineNumber << " in " << filename << ". Setting to max int.\n";
                    tempStock = numeric_limits<int>::max();
                } else {
                    tempStock = static_cast<int>(stockLong);
                }
            } catch (...) {
                cerr << "Warning: Invalid stock format '" << tempStockStr << "' for product '" << tempName << "' on line #" << lineNumber << " in " << filename << ". Setting stock to 0.\n";
                tempStock = 0;
            }
            if (!isValidPrice(tempPrice)) {
                cerr << "Warning: Invalid price format '" << tempPrice << "' for product '" << tempName << "' on line #" << lineNumber << " in " << filename << ". Skipping product.\n";
                continue;
            }
            bool validCategory = false;
            string actualCategory = "Uncategorized";
            string lowerTempCategory = manualToLower(tempCategory);
            for (int i = 0; i < categoryCount; ++i) {
                if (manualToLower(categoryArray[i]) == lowerTempCategory) {
                    validCategory = true;
                    actualCategory = categoryArray[i];
                    break;
                }
            }
            if (!validCategory) {
                cerr << "Warning: Invalid category '" << tempCategory << "' for '" << tempName << "' on line #" << lineNumber << " in " << filename << ". Assigning to 'Uncategorized'.\n";
            }
            tempCategory = actualCategory;
            if (tempProductCount == tempProductCapacity) {
                int newCapacity = tempProductCapacity * 2;
                Product* newTempArray = new Product[newCapacity];
                for (int i = 0; i < tempProductCount; ++i) {
                    newTempArray[i] = tempProducts[i];
                }
                delete[] tempProducts;
                tempProducts = newTempArray;
                tempProductCapacity = newCapacity;
            }
            tempProducts[tempProductCount++] = Product(tempName, tempPrice, tempCategory, tempStock);
        }
        inFile.close();
        productsArray = tempProducts;
        productCount = tempProductCount;
        productCapacity = tempProductCapacity;
        insertionSort(productsArray, productCount, true, "name");
    }
    void displayProductById(int id, const Product* arr, int arrSize) const {
        if (arr == NULL || id < 1 || id > arrSize) {
            cerr << "Internal Error: Invalid product ID or array requested for display.\n";
            return;
        }
        const Product& product = arr[id - 1];
        setColor(LIGHT_BLUE); cout << "\n--- Product Details ---\n"; resetColor();
        setColor(GREEN); cout << "Product Name: " << product.name << endl; resetColor();
        setColor(LIGHT_BLUE); cout << "Category: " << product.category << endl; resetColor();
        setColor(GREEN); cout << "Price: " << product.price << endl; resetColor();
        setColor(LIGHT_BLUE); cout << "Stock: " << product.stock << endl; resetColor();
        setColor(LIGHT_BLUE); cout << "-----------------------\n"; resetColor();
    }
    void displayProductNamesByCategory(const string& category) const {
        Product* tempFilteredArray = NULL;
        int tempFilteredCount = 0;
        int tempFilteredCapacity = 0;
        string lowerCategory = manualToLower(category);
        for (int i = 0; i < productCount; ++i) {
            if (manualToLower(productsArray[i].category) == lowerCategory) {
                if (tempFilteredCount == tempFilteredCapacity) {
                    int newCapacity = (tempFilteredCapacity == 0) ? 10 : tempFilteredCapacity * 2;
                    Product* newArray = new Product[newCapacity];
                    for (int j = 0; j < tempFilteredCount; ++j) {
                        newArray[j] = tempFilteredArray[j];
                    }
                    delete[] tempFilteredArray;
                    tempFilteredArray = newArray;
                    tempFilteredCapacity = newCapacity;
                }
                tempFilteredArray[tempFilteredCount++] = productsArray[i];
            }
        }
        if (tempFilteredCount == 0) {
            setColor(RED); cout << "No products found in category: '" + category + "'.\n"; resetColor();
            cout << "\nPress any key to return to menu..."; _getch();
            delete[] tempFilteredArray;
            return;
        }
        insertionSort(tempFilteredArray, tempFilteredCount, true, "name");
        setColor(GREEN); cout << "=== Products in category '" + category + "' (sorted by name) ===\n"; resetColor();
        int maxNumLength = (int)string("No.").length();
        int maxProductNameLength = (int)string("Product Name").length();
        for (int i = 0; i < tempFilteredCount; ++i) {
            maxNumLength = manualMax(maxNumLength, (int)intToString(i + 1).length());
            maxProductNameLength = manualMax(maxProductNameLength, (int)tempFilteredArray[i].name.length());
        }
        const int COLUMN_PADDING_SPACES = 2;
        int numColWidth = maxNumLength + COLUMN_PADDING_SPACES;
        int productNameColWidth = maxProductNameLength + COLUMN_PADDING_SPACES;
        string headerLine = "+" + string(numColWidth, '-') + "+" + string(productNameColWidth, '-') + "+";
        cout << headerLine << endl;
        cout << "|" << left << setw(numColWidth) << " No."
             << "|" << left << setw(productNameColWidth) << " Product Name"
             << "|" << endl;
        cout << headerLine << endl;
        for (int i = 0; i < tempFilteredCount; ++i) {
            cout << "|" << left << setw(numColWidth) << (" " + intToString(i + 1));
            cout << "|" << left << setw(productNameColWidth) << (" " + tempFilteredArray[i].name);
            cout << "|" << endl;
        }
        cout << headerLine << endl;
        delete[] tempFilteredArray;
        cout << "\nPress any key to return to menu..."; _getch();
    }
    void searchProduct(const string& searchTerm) {
        system("cls");
        setColor(LIGHT_BLUE); cout << "\n=== Search Product ===\n"; resetColor();
        if (searchTerm.empty()) {
            setColor(RED); cout << "Search term cannot be empty.\n"; resetColor();
            cout << "\nPress any key to return to menu..."; _getch(); return;
        }
        if (productCount == 0) {
            setColor(RED); cout << "No products available to search.\n"; resetColor();
            cout << "\nPress any key to return to menu..."; _getch(); return;
        }
        Product* combinedResults = new Product[productCount];
        int combinedCount = 0;
        string lowerSearchTerm = manualToLower(searchTerm);
        int foundIndex = binarySearch(searchTerm);
        if (foundIndex != -1) {
            combinedResults[combinedCount++] = productsArray[foundIndex];
        }
        for (int i = 0; i < productCount; ++i) {
            if (manualContains(manualToLower(productsArray[i].name), lowerSearchTerm) ||
                manualContains(manualToLower(productsArray[i].category), lowerSearchTerm)) {
                bool duplicate = false;
                for (int j = 0; j < combinedCount; ++j) {
                    if (manualToLower(productsArray[i].name) == manualToLower(combinedResults[j].name)) {
                        duplicate = true;
                        break;
                    }
                }
                if (!duplicate) {
                    combinedResults[combinedCount++] = productsArray[i];
                }
            }
        }
        insertionSort(combinedResults, combinedCount, true, "name");
        if (combinedCount == 0) {
            setColor(RED); cout << "No products found matching '" << searchTerm << "' in name or category.\n"; resetColor();
        } else {
            setColor(GREEN); cout << "=== Search Results for '" << searchTerm << "' (sorted by name) ===\n"; resetColor();
            int maxNumLength = (int)string("No.").length();
            int maxProductNameLength = (int)string("Product Name").length();
            int maxCategoryLength = (int)string("Category").length();
            for (int i = 0; i < combinedCount; ++i) {
                maxNumLength = manualMax(maxNumLength, (int)intToString(i + 1).length());
                maxProductNameLength = manualMax(maxProductNameLength, (int)combinedResults[i].name.length());
                maxCategoryLength = manualMax(maxCategoryLength, (int)combinedResults[i].category.length());
            }
            const int COLUMN_PADDING_SPACES = 2;
            int numColWidth = maxNumLength + COLUMN_PADDING_SPACES;
            int productNameColWidth = maxProductNameLength + COLUMN_PADDING_SPACES;
            int categoryColWidth = maxCategoryLength + COLUMN_PADDING_SPACES;
            string headerLine = "+" + string(numColWidth, '-') + "+" + string(productNameColWidth, '-') + "+" + string(categoryColWidth, '-') + "+";
            cout << headerLine << endl;
            cout << "|" << left << setw(numColWidth) << " No."
                 << "|" << left << setw(productNameColWidth) << " Product Name"
                 << "|" << left << setw(categoryColWidth) << " Category"
                 << "|" << endl;
            cout << headerLine << endl;
            for (int i = 0; i < combinedCount; ++i) {
                cout << "|" << left << setw(numColWidth) << (" " + intToString(i + 1));
                cout << "|" << left << setw(productNameColWidth) << (" " + combinedResults[i].name);
                cout << "|" << left << setw(categoryColWidth) << (" " + combinedResults[i].category);
                cout << "|" << endl;
            }
            cout << headerLine << endl;
            cout << "Enter product ID (1-" << combinedCount << ") to view details, or 0 to return: ";
            string input;
            int id;
            getline(cin, input);
            try {
                size_t pos;
                id = custom_stoi(input, &pos);
                string remaining = input.substr(pos);
                remaining.erase(0, remaining.find_first_not_of(" \t\n\r"));
                if (!remaining.empty()) {
                    throw invalid_argument("Contains non-numeric characters");
                }
            } catch (...) {
                setColor(RED); cout << "Invalid input. Please enter a number.\n"; resetColor();
                delete[] combinedResults;
                cout << "\nPress any key to return to menu..."; _getch(); return;
            }
            if (id == 0) {
                setColor(RED); cout << "Returning to menu.\n"; resetColor();
            } else if (id >= 1 && id <= combinedCount) {
                system("cls");
                displayProductById(id, combinedResults, combinedCount);
                cout << "\nPress any key to return to menu..."; _getch();
            } else {
                setColor(RED); cout << "Invalid product ID.\n"; resetColor();
                cout << "\nPress any key to return to menu..."; _getch();
            }
        }
        delete[] combinedResults;
        cout << "\nPress any key to return to menu..."; _getch();
    }
    void sortAndDisplayCategories() {
        system("cls");
        setColor(LIGHT_BLUE); cout << "\n=== Product Categories (sorted alphabetically) ===\n"; resetColor();
        if (categoryCount == 0) {
            setColor(RED); cout << "No categories to display.\n"; resetColor();
            cout << "\nPress any key to return to menu..."; _getch(); return;
        }
        int maxNumLength = (int)string("No.").length();
        int maxCategoryNameLength = (int)string("Category Name").length();
        for (int i = 0; i < categoryCount; ++i) {
            maxNumLength = manualMax(maxNumLength, (int)intToString(i + 1).length());
            maxCategoryNameLength = manualMax(maxCategoryNameLength, (int)categoryArray[i].length());
        }
        const int COLUMN_PADDING_SPACES = 2;
        int numColWidth = maxNumLength + COLUMN_PADDING_SPACES;
        int categoryNameColWidth = maxCategoryNameLength + COLUMN_PADDING_SPACES;
        string headerLine = "+" + string(numColWidth, '-') + "+" + string(categoryNameColWidth, '-') + "+";
        cout << headerLine << endl;
        cout << "|" << left << setw(numColWidth) << " No."
             << "|" << left << setw(categoryNameColWidth) << " Category Name"
             << "|" << endl;
        cout << headerLine << endl;
        for (int i = 0; i < categoryCount; i++) {
            cout << "|" << left << setw(numColWidth) << (" " + intToString(i + 1));
            cout << "|" << left << setw(categoryNameColWidth) << (" " + categoryArray[i]);
            cout << "|" << endl;
        }
        cout << headerLine << endl;
        cout << "Enter category ID (1-" << categoryCount << ") to view products, or 0 to return: ";
        string input;
        int id;
        getline(cin, input);
        try {
            size_t pos;
            id = custom_stoi(input, &pos);
            string remaining = input.substr(pos);
            remaining.erase(0, remaining.find_first_not_of(" \t\n\r"));
            if (!remaining.empty()) {
                throw invalid_argument("Contains non-numeric characters");
            }
        } catch (...) {
            setColor(RED); cout << "Invalid input. Please enter a number.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        if (id == 0) {
            setColor(RED); cout << "Returning to menu.\n"; resetColor();
        } else if (id >= 1 && id <= categoryCount) {
            system("cls");
            displayProductNamesByCategory(categoryArray[id - 1]);
            cout << endl;
        } else {
            setColor(RED); cout << "Invalid category ID.\n"; resetColor();
            cout << "\nPress any key to return to menu..."; _getch();
        }
    }
    void sortAndDisplayProductNames() {
        system("cls");
        setColor(LIGHT_BLUE); cout << "\n=== Products (sorted by name) ===\n"; resetColor();
        if (productCount == 0) {
            setColor(RED); cout << "No products to display.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        int maxNumLength = (int)string("No.").length();
        int maxProductNameLength = (int)string("Product Name").length();
        int maxPriceLength = (int)string("Price").length();
        int maxStockLength = (int)string("Current Stock").length();
        for (int i = 0; i < productCount; ++i) {
            maxNumLength = manualMax(maxNumLength, (int)intToString(i + 1).length());
            maxProductNameLength = manualMax(maxProductNameLength, (int)productsArray[i].name.length());
            maxPriceLength = manualMax(maxPriceLength, (int)productsArray[i].price.length());
            maxStockLength = manualMax(maxStockLength, (int)intToString(productsArray[i].stock).length());
        }
        const int COLUMN_PADDING_SPACES = 2;
        const int STOCK_COLUMN_EXTRA_PAD = 2;
        int numColWidth = maxNumLength + COLUMN_PADDING_SPACES;
        int productNameColWidth = maxProductNameLength + COLUMN_PADDING_SPACES;
        int priceColWidth = maxPriceLength + COLUMN_PADDING_SPACES;
        int stockColWidth = manualMax((int)string("Current Stock").length(), maxStockLength) + COLUMN_PADDING_SPACES + STOCK_COLUMN_EXTRA_PAD;
        string headerLine = "+" + string(numColWidth, '-') + "+" + string(productNameColWidth, '-') + "+" + string(priceColWidth, '-') + "+" + string(stockColWidth, '-') + "+";
        cout << headerLine << endl;
        cout << "|" << left << setw(numColWidth) << " No."
             << "|" << left << setw(productNameColWidth) << " Product Name"
             << "|" << left << setw(priceColWidth) << " Price"
             << "|" << left << setw(stockColWidth) << " Current Stock"
             << "|" << endl;
        cout << headerLine << endl;
        for (int i = 0; i < productCount; i++) {
            cout << "|" << left << setw(numColWidth) << (" " + intToString(i + 1));
            cout << "|" << left << setw(productNameColWidth) << (" " + productsArray[i].name);
            cout << "|" << left << setw(priceColWidth) << (" " + productsArray[i].price);
            string stockStr = intToString(productsArray[i].stock);
            int paddingToLeft = stockColWidth - stockStr.length() -1;
            if (paddingToLeft < 0) paddingToLeft = 0;
            cout << "|" << string(paddingToLeft, ' ');
            setColor(LIGHT_BLUE); cout << stockStr; resetColor();
            cout << " |" << endl;
        }
        cout << headerLine << endl;
        cout << "Enter product ID (1-" << productCount << ") to view details, or 0 to return: ";
        string input;
        int id;
        getline(cin, input);
        try {
            size_t pos;
            id = custom_stoi(input, &pos);
            string remaining = input.substr(pos);
            remaining.erase(0, remaining.find_first_not_of(" \t\n\r"));
            if (!remaining.empty()) {
                throw invalid_argument("Contains non-numeric characters");
            }
        } catch (...) {
            setColor(RED); cout << "Invalid input. Please enter a number.\n"; resetColor();
            cout << "\nPress any key to return to menu..."; _getch(); return;
        }
        if (id == 0) {
            setColor(RED); cout << "Returning to menu.\n"; resetColor();
        } else if (id >= 1 && id <= productCount) {
            system("cls");
            displayProductById(id, productsArray, productCount);
            cout << "\nPress any key to return to menu..."; _getch();
        } else {
            setColor(RED); cout << "Invalid product ID.\n"; resetColor();
            cout << "\nPress any key to return to menu..."; _getch();
        }
    }
    int getProductStock(const string& productName) const {
        int index = binarySearch(productName);
        if (index != -1) {
            return productsArray[index].stock;
        }
        return -1;
    }
    bool updateProductStock(const string& productName, int quantityChange, const string& filename) {
        int index = binarySearch(productName);
        if (index != -1) {
            if (productsArray[index].stock + quantityChange < 0) {
                productsArray[index].stock = 0;
                setColor(YELLOW); cout << "Warning: Stock for '" << productsArray[index].name << "' cannot go below zero. Setting to 0.\n"; resetColor();
            } else if (productsArray[index].stock + quantityChange > numeric_limits<int>::max()) {
                 productsArray[index].stock = numeric_limits<int>::max();
                 setColor(YELLOW); cout << "Warning: Stock for '" << productsArray[index].name << "' cannot exceed maximum integer value. Setting to max int.\n"; resetColor();
            }
            else {
                productsArray[index].stock += quantityChange;
            }
            try {
                saveToFile(filename);
                return true;
            } catch (const runtime_error& e) {
                cerr << "Error saving product file after stock update: " << e.what() << endl;
                setColor(RED); cout << "Failed to save stock changes for '" << productName << "'. Data might be inconsistent.\n"; resetColor();
                return false;
            }
        }
        setColor(RED); cout << "Product '" << productName << "' not found for stock update.\n"; resetColor();
        return false;
    }
    void editProductStock(const string& productsFilename) {
        system("cls");
        setColor(LIGHT_BLUE); cout << "\n=== Edit Product Stock ===\n"; resetColor();
        if (productCount == 0) {
            setColor(RED); cout << "No products available to edit stock.\n"; resetColor();
            cout << "\nPress any key to return to menu..."; _getch(); return;
        }
        setColor(GREEN); cout << "=== Current Products (sorted by name) ===\n"; resetColor();
        int maxNumLength = (int)string("No.").length();
        int maxProductNameLength = (int)string("Product Name").length();
        int maxStockLength = (int)string("Current Stock").length();
        for (int i = 0; i < productCount; ++i) {
            maxNumLength = manualMax(maxNumLength, (int)intToString(i + 1).length());
            maxProductNameLength = manualMax(maxProductNameLength, (int)productsArray[i].name.length());
            maxStockLength = manualMax(maxStockLength, (int)intToString(productsArray[i].stock).length());
        }
        const int COLUMN_PADDING_SPACES = 2;
        const int STOCK_COLUMN_EXTRA_PAD = 2;
        int numColWidth = maxNumLength + COLUMN_PADDING_SPACES;
        int productNameColWidth = maxProductNameLength + COLUMN_PADDING_SPACES;
        int stockColWidth = manualMax((int)string("Current Stock").length(), maxStockLength) + COLUMN_PADDING_SPACES + STOCK_COLUMN_EXTRA_PAD;
        string headerLine = "+" + string(numColWidth, '-') + "+" + string(productNameColWidth, '-') + "+" + string(stockColWidth, '-') + "+";
        cout << headerLine << endl;
        cout << "|" << left << setw(numColWidth) << " No."
             << "|" << left << setw(productNameColWidth) << " Product Name"
             << "|" << left << setw(stockColWidth) << " Current Stock"
             << "|" << endl;
        cout << headerLine << endl;
        for (int i = 0; i < productCount; i++) {
            cout << "|" << left << setw(numColWidth) << (" " + intToString(i + 1));
            cout << "|" << left << setw(productNameColWidth) << (" " + productsArray[i].name);
            string stockStr = intToString(productsArray[i].stock);
            int paddingToLeft = stockColWidth - stockStr.length() -1;
            if (paddingToLeft < 0) paddingToLeft = 0;
            cout << "|" << string(paddingToLeft, ' ');
            setColor(LIGHT_BLUE); cout << stockStr; resetColor();
            cout << " |" << endl;
        }
        cout << headerLine << endl;
        cout << "Enter the number of the product to edit stock (1-" << productCount << ", or 0 to cancel): ";
        string input;
        int choice;
        getline(cin, input);
        try {
            size_t pos;
            choice = custom_stoi(input, &pos);
            string remaining = input.substr(pos);
            remaining.erase(0, remaining.find_first_not_of(" \t\n\r"));
            if (!remaining.empty()) {
                throw invalid_argument("Contains non-numeric characters");
            }
        } catch (...) {
            setColor(RED); cout << "Invalid input. Please enter a number.\n"; resetColor();
            cout << "\nPress any key to return to menu..."; _getch(); return;
        }
        if (choice == 0) {
            setColor(RED); cout << "Edit stock cancelled.\n"; resetColor();
            return;
        }
        if (choice < 1 || choice > productCount) {
            setColor(RED); cout << "Invalid product ID.\n"; resetColor();
            cout << "\nPress any key to return to menu..."; _getch(); return;
        }
        Product& selectedProduct = productsArray[choice - 1];
        cout << "Selected Product: " << selectedProduct.name << " (Current Stock: " << selectedProduct.stock << ")\n";
        cout << "Enter new stock quantity for '" << selectedProduct.name << "': ";
        string newStockInput;
        int newStock;
        getline(cin, newStockInput);
        newStockInput.erase(0, newStockInput.find_first_not_of(" \t\n\r"));
        newStockInput.erase(newStockInput.find_last_not_of(" \t\n\r") + 1);
        try {
            size_t pos;
            newStock = custom_stoi(newStockInput, &pos);
            string remaining = newStockInput.substr(pos);
            remaining.erase(0, remaining.find_first_not_of(" \t\n\r"));
            if (!remaining.empty()) {
                throw invalid_argument("Contains non-numeric characters");
            }
            if (newStock < 0) {
                setColor(RED); cout << "New stock cannot be negative!\n"; resetColor();
                cout << "\nPress any key to return to menu..."; _getch(); return;
            }
            if (newStock > numeric_limits<int>::max()) {
                setColor(RED); cout << "New stock value is too large.\n"; resetColor();
                cout << "\nPress any key to return to menu..."; _getch(); return;
            }
        } catch (...) {
            setColor(RED); cout << "Invalid stock quantity! Please enter a whole number.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        selectedProduct.stock = newStock;
        try {
            saveToFile(productsFilename);
            setColor(GREEN); cout << "Stock for '" << selectedProduct.name << "' updated to " << newStock << " successfully.\n"; resetColor();
        } catch (const runtime_error& e) {
            cerr << "Error saving product list after stock edit: " << e.what() << endl;
            setColor(RED); cout << "Stock was updated in memory but failed to save to file. Data might be inconsistent.\n"; resetColor();
        }
        cout << "\nPress any key to return to menu..."; _getch();
    }
    Product* getProductsArray() const {
        return productsArray;
    }
    int getProductCount() const {
        return productCount;
    }
};
struct OrderItem {
    string name;
    string priceAtPurchase;
    string category;
    int quantity;
    string timestamp;
    OrderItem(string n = "", string p = "", string c = "", int q = 0, string ts = "")
        : name(n), priceAtPurchase(p), category(c), quantity(q), timestamp(ts) {}
};
struct CustomerNode {
    OrderItem data;
    CustomerNode* next;
    CustomerNode(const OrderItem& item) : data(item), next(NULL) {}
};
class Customer {
private:
    CustomerNode* cartList;
    CustomerNode* orderHistory;
    int cartCount;
    int orderHistoryCount;
    string username;
    string password;
    bool isLoggedIn;
    ProductList* globalProductList;
    void clearList(CustomerNode*& head) {
        while (head) {
            CustomerNode* temp = head;
            head = head->next;
            delete temp;
        }
    }
    void insertAtEnd(CustomerNode*& head, const OrderItem& item) {
        CustomerNode* newNode = new CustomerNode(item);
        if (!head) {
            head = newNode;
            return;
        }
        CustomerNode* current = head;
        while (current->next) {
            current = current->next;
        }
        current->next = newNode;
    }
    void loadOrderHistory(const string& filename) {
        ifstream file(filename.c_str());
        if (!file) {
            return;
        }
        clearList(orderHistory);
        orderHistoryCount = 0;
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string tempUser, tempTime, tempName, tempQtyStr, tempPriceStr;
            if (getline(ss, tempUser, '\t') &&
                getline(ss, tempTime, '\t') &&
                getline(ss, tempName, '\t') &&
                getline(ss, tempQtyStr, '\t') &&
                getline(ss, tempPriceStr)) {
                if (manualToLower(tempUser) == manualToLower(username)) {
                    int tempQty;
                    try {
                        tempQty = custom_stoi(tempQtyStr);
                    } catch (...) {
                        cerr << "Warning: Invalid quantity in order history line: " << line << endl;
                        continue;
                    }
                    string tempCategory = "N/A";
                    int productIndex = globalProductList->binarySearch(tempName);
                    if (productIndex != -1) {
                         tempCategory = globalProductList->getProductsArray()[productIndex].category;
                    }
                    OrderItem item(tempName, tempPriceStr, tempCategory, tempQty, tempTime);
                    insertAtEnd(orderHistory, item);
                    orderHistoryCount++;
                }
            } else {
                 cerr << "Warning: Skipping malformed order history line: " << line << endl;
            }
        }
        file.close();
    }
public:
    Customer(ProductList* productListPtr)
        : cartList(NULL), orderHistory(NULL),
          cartCount(0), orderHistoryCount(0), isLoggedIn(false),
          globalProductList(productListPtr) {
        username = "";
        password = "";
    }
    ~Customer() {
        clearList(cartList);
        clearList(orderHistory);
    }
    bool registerCustomer(const string& filename) {
        system("cls");
        setColor(LIGHT_BLUE); cout << "\n=== Customer Registration ===\n"; resetColor();
        string newUsername, newPassword;
        cout << "Enter new username: ";
        getline(cin, newUsername);
        newUsername.erase(0, newUsername.find_first_not_of(" \t\n\r"));
        newUsername.erase(newUsername.find_last_not_of(" \t\n\r") + 1);
        if (newUsername.empty()) {
            setColor(RED); cout << "Username cannot be empty.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return false;
        }
        if (manualFind(newUsername, ',') != string::npos || manualFind(newUsername, '\t') != string::npos) {
            setColor(RED); cout << "Username cannot contain commas or tabs.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return false;
        }
        if (manualFileContains(filename, newUsername)) {
            setColor(RED); cout << "Username '" << newUsername << "' already exists (case-insensitive match).\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return false;
        }
        cout << "Enter password: ";
        newPassword = getPassword();
        if (newPassword.empty()) {
            setColor(RED); cout << "Password cannot be empty.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return false;
        }
        if (manualFind(newPassword, ',') != string::npos || manualFind(newPassword, '\t') != string::npos) {
            setColor(RED); cout << "Password cannot contain commas or tabs.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return false;
        }
        ofstream file(filename.c_str(), ios::app);
        if (!file) {
            setColor(RED); cout << "Failed to open registration file. Registration failed.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return false;
        }
        file << newUsername << "," << newPassword << "\n";
        file.close();
        setColor(GREEN); cout << "Registration successful for user '" << newUsername << "'!\n"; resetColor();
        cout << "\nPress any key to return..."; _getch();
        return true;
    }
    bool login(const string& filename) {
        system("cls");
        setColor(LIGHT_BLUE); cout << "\n=== Customer Login ===\n"; resetColor();
        int attempts = 3;
        while (attempts > 0) {
            cout << "Enter username: ";
            getline(cin, username);
            username.erase(0, username.find_first_not_of(" \t\n\r"));
            username.erase(username.find_last_not_of(" \t\n\r") + 1);
            if (username.empty()) {
                setColor(RED); cout << "Username cannot be empty.\n"; resetColor();
                attempts--;
                if (attempts > 0) {
                    cout << "Attempts remaining: " << attempts << "\n";
                    Sleep(0); system("cls"); setColor(LIGHT_BLUE); cout << "\n=== Customer Login ===\n"; resetColor();
                }
                continue;
            }
            cout << "Enter password: ";
            password = getPassword();
            if (password.empty()) {
                setColor(RED); cout << "Password cannot be empty.\n"; resetColor();
                attempts--;
                if (attempts > 0) {
                    cout << "Attempts remaining: " << attempts << "\n";
                    Sleep(0); system("cls"); setColor(LIGHT_BLUE); cout << "\n=== Customer Login ===\n"; resetColor();
                }
                continue;
            }
            ifstream file(filename.c_str());
            if (!file) {
                setColor(RED); cout << "Login file not found. Please register first.\n"; resetColor();
                cout << "\nPress any key to return..."; _getch(); return false;
            }
            string line;
            bool foundCredentials = false;
            while (getline(file, line)) {
                size_t commaPos = manualFind(line, ',');
                if (commaPos == string::npos) {
                    cerr << "Warning: Skipping malformed line in " << filename << " (no comma): " << line << endl;
                    continue;
                }
                string storedUsername = line.substr(0, commaPos);
                string storedPassword = line.substr(commaPos + 1);
                storedUsername.erase(0, storedUsername.find_first_not_of(" \t\n\r")); storedUsername.erase(storedUsername.find_last_not_of(" \t\n\r") + 1);
                storedPassword.erase(0, storedPassword.find_first_not_of(" \t\n\r")); storedPassword.erase(storedPassword.find_last_not_of(" \t\n\r") + 1);
                if (username == storedUsername && password == storedPassword) {
                    file.close();
                    isLoggedIn = true;
                    clearList(orderHistory);
                    loadOrderHistory("orders.txt");
                    setColor(GREEN); cout << "Login successful! Welcome, " << username << ".\n"; resetColor();
                    Sleep(1000);
                    return true;
                }
            }
            file.close();
            attempts--;
            setColor(RED); cout << "Invalid username or password.\n"; resetColor();
            if (attempts > 0) {
                cout << "Attempts remaining: " << attempts << "\n";
                Sleep(0); system("cls"); setColor(LIGHT_BLUE); cout << "\n=== Customer Login ===\n"; resetColor();
            } else {
                setColor(RED); cout << "Too many failed attempts. Returning to welcome menu.\n"; resetColor();
                Sleep(0); return false;
            }
        }
        return false;
    }
    void logout() {
        if (isLoggedIn) {
            isLoggedIn = false;
            username = "";
            password = "";
            clearList(cartList);
            cartCount = 0;
            setColor(RED); cout << "Customer logged out.\n"; resetColor();
            cout << "\nPress any key to return to welcome menu..."; _getch();
        } else {
            setColor(RED); cout << "Not logged in.\n"; resetColor();
        }
    }
    void searchProduct() {
        system("cls");
        setColor(LIGHT_BLUE); cout << "\n=== Search Product ===\n"; resetColor();
        if (!globalProductList || globalProductList->getProductCount() == 0) {
            setColor(RED); cout << "No products available to search.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        cout << "Enter part of Product Name or Category to search: ";
        string searchTerm;
        getline(cin, searchTerm);
        searchTerm.erase(0, searchTerm.find_first_not_of(" \t\n\r"));
        searchTerm.erase(searchTerm.find_last_not_of(" \t\n\r") + 1);
        if (searchTerm.empty()) {
            setColor(RED); cout << "No input provided for search.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        globalProductList->searchProduct(searchTerm);
    }
    void sortProducts() {
        system("cls");
        setColor(LIGHT_BLUE); cout << "\n=== Sort Products ===\n"; resetColor();
        if (!globalProductList || globalProductList->getProductCount() == 0) {
            setColor(RED); cout << "No products available to sort.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        cout << "Sort Options:\n";
        cout << "1. Sort by Category\n";
        cout << "2. Sort by Product Name\n";
        cout << "Enter choice (1-2, or 0 to cancel): ";
        string input;
        int choice;
        getline(cin, input);
        try {
            size_t pos;
            choice = custom_stoi(input, &pos);
            string remaining = input.substr(pos);
            remaining.erase(0, remaining.find_first_not_of(" \t\n\r"));
            if (!remaining.empty()) {
                throw invalid_argument("Contains non-numeric characters");
            }
        } catch (...) {
            setColor(RED); cout << "Invalid input. Please enter a number.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        if (choice == 0) {
            setColor(RED); cout << "Cancelled.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        if (choice == 1) {
            globalProductList->sortAndDisplayCategories();
        } else if (choice == 2) {
            globalProductList->sortAndDisplayProductNames();
        } else {
            setColor(RED); cout << "Invalid choice.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch();
        }
    }
    void addProductToCart() {
        system("cls");
        setColor(LIGHT_BLUE); cout << "\n=== Add Product to Cart ===\n"; resetColor();
        if (!isLoggedIn) {
            setColor(RED); cout << "Please login first to add products to cart.\n";
            cout << "\nPress any key to return..."; _getch(); return;
        }
        if (globalProductList->getProductCount() == 0) {
            setColor(RED); cout << "No products available to add.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        Product* availableProducts = globalProductList->getProductsArray();
        int arrSize = globalProductList->getProductCount();
        setColor(GREEN); cout << "=== Available Products ===\n"; resetColor();
        int maxNumLength = (int)string("No.").length();
        int maxNameLength = (int)string("Name").length();
        int maxPriceLength = (int)string("Price").length();
        int maxCategoryLength = (int)string("Category").length();
        int maxStockLength = (int)string("Stock").length();
        for (int i = 0; i < arrSize; i++) {
            maxNumLength = manualMax(maxNumLength, (int)intToString(i + 1).length());
            maxNameLength = manualMax(maxNameLength, (int)availableProducts[i].name.length());
            maxPriceLength = manualMax(maxPriceLength, (int)availableProducts[i].price.length());
            maxCategoryLength = manualMax(maxCategoryLength, (int)availableProducts[i].category.length());
            maxStockLength = manualMax(maxStockLength, (int)intToString(availableProducts[i].stock).length());
        }
        const int COLUMN_PADDING_SPACES = 2;
        const int STOCK_COLUMN_EXTRA_PAD = 2;
        int numColWidth = maxNumLength + COLUMN_PADDING_SPACES;
        int nameColWidth = maxNameLength + COLUMN_PADDING_SPACES;
        int priceColWidth = maxPriceLength + COLUMN_PADDING_SPACES;
        int categoryColWidth = maxCategoryLength + COLUMN_PADDING_SPACES;
        int stockColWidth = maxStockLength + COLUMN_PADDING_SPACES + STOCK_COLUMN_EXTRA_PAD;
        string headerLine = "+" + string(numColWidth, '-') + "+" + string(nameColWidth, '-') + "+" + string(priceColWidth, '-') + "+" + string(categoryColWidth, '-') + "+" + string(stockColWidth, '-') + "+";
        cout << headerLine << endl;
        cout << "|" << left << setw(numColWidth) << " No."
             << "|" << left << setw(nameColWidth) << " Name"
             << "|" << left << setw(priceColWidth) << " Price"
             << "|" << left << setw(categoryColWidth) << " Category"
             << "|" << left << setw(stockColWidth) << " Stock"
             << "|" << endl;
        cout << headerLine << endl;
        for (int i = 0; i < arrSize; ++i) {
            cout << "|" << left << setw(numColWidth) << (" " + intToString(i + 1));
            cout << "|" << left << setw(nameColWidth) << (" " + availableProducts[i].name);
            cout << "|" << left << setw(priceColWidth) << (" " + availableProducts[i].price);
            cout << "|" << left << setw(categoryColWidth) << (" " + availableProducts[i].category);
            string stockStr = intToString(availableProducts[i].stock);
            int paddingToLeft = stockColWidth - stockStr.length() -1;
            if (paddingToLeft < 0) paddingToLeft = 0;
            cout << "|" << string(paddingToLeft, ' ');
            setColor(LIGHT_BLUE); cout << stockStr; resetColor();
            cout << " |" << endl;
        }
        cout << headerLine << endl;
        cout << "\nEnter the number of the product to add to cart (1-" << arrSize << ", or 0 to cancel): ";
        string input;
        int choice;
        getline(cin, input);
        try {
            size_t pos;
            choice = custom_stoi(input, &pos);
            string remaining = input.substr(pos);
            remaining.erase(0, remaining.find_first_not_of(" \t\n\r"));
            if (!remaining.empty()) {
                throw invalid_argument("Contains non-numeric characters");
            }
        } catch (...) {
            setColor(RED); cout << "Invalid input. Please enter a number.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        if (choice == 0) {
            setColor(RED); cout << "Cancelled.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        if (choice < 1 || choice > arrSize) {
            setColor(RED); cout << "Product not found.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        const Product& selectedProduct = availableProducts[choice - 1];
        int q;
        cout << "Enter Quantity (max 5, current stock: " << selectedProduct.stock << "): ";
        string qtyInput;
        getline(cin, qtyInput);
        try {
            size_t pos;
            q = custom_stoi(qtyInput, &pos);
            string remaining = qtyInput.substr(pos);
            remaining.erase(0, remaining.find_first_not_of(" \t\n\r"));
            if (!remaining.empty()) {
                throw invalid_argument("Contains non-numeric characters");
            }
        } catch (...) {
            setColor(RED); cout << "Invalid quantity. Please enter a whole number.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        if (q <= 0 || q > 5) {
            setColor(RED); cout << "Invalid quantity. Please enter a number between 1 and 5.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        if (selectedProduct.stock < q) {
            setColor(RED); cout << "Insufficient stock. Only " << selectedProduct.stock << " available.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        OrderItem itemToAdd(
            selectedProduct.name,
            selectedProduct.price,
            selectedProduct.category,
            q,
            "");
        insertAtEnd(cartList, itemToAdd);
        cartCount++;
        setColor(GREEN); cout << q << " units of " << itemToAdd.name << " added to cart!\n"; resetColor();
        cout << "\nPress any key to return..."; _getch();
    }
    void displayCart() {
        system("cls");
        setColor(LIGHT_BLUE); cout << "\n=== Your Cart ===\n"; resetColor();
        if (!cartList) {
            setColor(RED); cout << "Cart is empty.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        int maxNumLength = (int)string("No.").length();
        int maxNameLength = (int)string("Name").length();
        int maxPriceLength = (int)string("Price").length();
        int maxCategoryLength = (int)string("Category").length();
        int maxQtyLength = (int)string("Qty.").length();
        CustomerNode* temp = cartList;
        while (temp) {
            maxNumLength = manualMax(maxNumLength, (int)intToString(cartCount).length());
            maxNameLength = manualMax(maxNameLength, (int)temp->data.name.length());
            maxPriceLength = manualMax(maxPriceLength, (int)temp->data.priceAtPurchase.length());
            maxCategoryLength = manualMax(maxCategoryLength, (int)temp->data.category.length());
            maxQtyLength = manualMax(maxQtyLength, (int)intToString(temp->data.quantity).length());
            temp = temp->next;
        }
        const int COLUMN_PADDING_SPACES = 2;
        const int QTY_COLUMN_EXTRA_PAD = 2;
        int numColWidth = maxNumLength + COLUMN_PADDING_SPACES;
        int nameColWidth = maxNameLength + COLUMN_PADDING_SPACES;
        int priceColWidth = maxPriceLength + COLUMN_PADDING_SPACES;
        int categoryColWidth = maxCategoryLength + COLUMN_PADDING_SPACES;
        int qtyColWidth = manualMax((int)string("Qty.").length(), maxQtyLength) + COLUMN_PADDING_SPACES + QTY_COLUMN_EXTRA_PAD;
        string headerLine = "+" + string(numColWidth, '-') + "+" + string(nameColWidth, '-') + "+" + string(priceColWidth, '-') + "+" + string(categoryColWidth, '-') + "+" + string(qtyColWidth, '-') + "+";
        cout << headerLine << endl;
        cout << "|" << left << setw(numColWidth) << " No."
             << "|" << left << setw(nameColWidth) << " Name"
             << "|" << left << setw(priceColWidth) << " Price"
             << "|" << left << setw(categoryColWidth) << " Category"
             << "|" << left << setw(qtyColWidth) << " Qty."
             << "|" << endl;
        cout << headerLine << endl;
        int i = 1;
        CustomerNode* displayCurrent = cartList;
        while (displayCurrent) {
            cout << "|" << left << setw(numColWidth) << (" " + intToString(i));
            cout << "|" << left << setw(nameColWidth) << (" " + displayCurrent->data.name);
            cout << "|" << left << setw(priceColWidth) << (" " + displayCurrent->data.priceAtPurchase);
            cout << "|" << left << setw(categoryColWidth) << (" " + displayCurrent->data.category);
            string qtyStr = intToString(displayCurrent->data.quantity);
            int paddingToLeft = qtyColWidth - qtyStr.length() -1;
            if (paddingToLeft < 0) paddingToLeft = 0;
            cout << "|" << string(paddingToLeft, ' ');
            setColor(LIGHT_BLUE); cout << qtyStr; resetColor();
            cout << " |" << endl;
            displayCurrent = displayCurrent->next;
            i++;
        }
        cout << headerLine << endl;
        cout << "\nPress any key to return..."; _getch();
    }
    void removeFromCart() {
        system("cls");
        setColor(LIGHT_BLUE); cout << "\n=== Remove from Cart ===\n"; resetColor();
        if (!isLoggedIn) {
            setColor(RED); cout << "Please login first.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        if (!cartList) {
            setColor(RED); cout << "Cart is empty. Nothing to remove.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        displayCart();
        cout << "\nEnter the number of the product to remove (1-" << cartCount << ", or 0 to cancel): ";
        string input;
        int choice;
        getline(cin, input);
        try {
            size_t pos;
            choice = custom_stoi(input, &pos);
            string remaining = input.substr(pos);
            remaining.erase(0, remaining.find_first_not_of(" \t\n\r"));
            if (!remaining.empty()) {
                throw invalid_argument("Contains non-numeric characters");
            }
        } catch (...) {
            setColor(RED); cout << "Invalid input. Please enter a number.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        if (choice == 0) {
            setColor(RED); cout << "Removal cancelled.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        if (choice < 1 || choice > cartCount) {
            setColor(RED); cout << "Product not found in cart.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        CustomerNode* current = cartList;
        CustomerNode* prev = NULL;
        for (int i = 1; i < choice && current; i++) {
            prev = current;
            current = current->next;
        }
        if (!current) {
            setColor(RED); cout << "Product not found in cart. (Internal error).\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        if (prev) {
            prev->next = current->next;
        } else {
            cartList = current->next;
        }
        delete current;
        cartCount--;
        setColor(GREEN); cout << "Product removed from cart successfully!\n"; resetColor();
        cout << "\nPress any key to return..."; _getch();
    }
    void updateCartQuantity() {
        system("cls");
        setColor(LIGHT_BLUE); cout << "\n=== Update Cart Quantity ===\n"; resetColor();
        if (!isLoggedIn) {
            setColor(RED); cout << "Please login first.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        if (!cartList) {
            setColor(RED); cout << "Cart is empty. Nothing to update.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        displayCart();
        cout << "\nEnter the number of the product to update quantity (1-" << cartCount << ", or 0 to cancel): ";
        string input;
        int choice;
        getline(cin, input);
        try {
            size_t pos;
            choice = custom_stoi(input, &pos);
            string remaining = input.substr(pos);
            remaining.erase(0, remaining.find_first_not_of(" \t\n\r"));
            if (!remaining.empty()) {
                throw invalid_argument("Contains non-numeric characters");
            }
        } catch (...) {
            setColor(RED); cout << "Invalid input. Please enter a number.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        if (choice == 0) {
            setColor(RED); cout << "Update cancelled.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        if (choice < 1 || choice > cartCount) {
            setColor(RED); cout << "Product not found in cart.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        CustomerNode* current = cartList;
        for (int i = 1; i < choice && current; i++) current = current->next;
        if (!current) {
            setColor(RED); cout << "Product not found in cart. (Internal error).\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        int newQty;
        cout << "Enter new Quantity for " << current->data.name << " (max 5): ";
        string qtyInput;
        getline(cin, qtyInput);
        try {
            size_t pos;
            newQty = custom_stoi(qtyInput, &pos);
            string remaining = qtyInput.substr(pos);
            remaining.erase(0, remaining.find_first_not_of(" \t\n\r"));
            if (!remaining.empty()) {
                throw invalid_argument("Contains non-numeric characters");
            }
        } catch (...) {
            setColor(RED); cout << "Invalid quantity. Please enter a whole number.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        if (newQty <= 0 || newQty > 5) {
            setColor(RED); cout << "Invalid quantity. Please enter a number between 1 and 5.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        int availableStock = globalProductList->getProductStock(current->data.name);
        if (availableStock == -1) {
            setColor(RED); cout << "Product '" << current->data.name << "' not found in main inventory. Cannot update quantity.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        if (availableStock < newQty) {
            setColor(RED); cout << "Insufficient stock. Only " << availableStock << " available for " << current->data.name << ".\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        current->data.quantity = newQty;
        setColor(GREEN); cout << "Quantity updated to " << newQty << " for " << current->data.name << "!\n"; resetColor();
        cout << "\nPress any key to return..."; _getch();
    }
    void order(const string& productsFilename) {
    system("cls");
    setColor(LIGHT_BLUE); cout << "\n=== Place Order ===\n"; resetColor();
    if (!isLoggedIn) {
        setColor(RED); cout << "Please login first to place an order.\n"; resetColor();
        cout << "\nPress any key to return..."; _getch(); return;
    }
    if (!cartList) {
        setColor(RED); cout << "Your cart is empty. Nothing to order.\n"; resetColor();
        cout << "\nPress any key to return..."; _getch(); return;
    }
    setColor(YELLOW); cout << "Items in your cart:\n"; resetColor();
    int maxNumLength = (int)string("No.").length();
    int maxNameLength = (int)string("Name").length();
    int maxPriceLength = (int)string("Price").length();
    int maxCategoryLength = (int)string("Category").length();
    int maxQtyLength = (int)string("Qty.").length();
    CustomerNode* tempNode = cartList;
    while (tempNode) {
        maxNumLength = manualMax(maxNumLength, (int)intToString(cartCount).length());
        maxNameLength = manualMax(maxNameLength, (int)tempNode->data.name.length());
        maxPriceLength = manualMax(maxPriceLength, (int)tempNode->data.priceAtPurchase.length());
        maxCategoryLength = manualMax(maxCategoryLength, (int)tempNode->data.category.length());
        maxQtyLength = manualMax(maxQtyLength, (int)intToString(tempNode->data.quantity).length());
        tempNode = tempNode->next;
    }
    const int COLUMN_PADDING_SPACES = 2;
    const int QTY_TOTAL_COLUMN_EXTRA_PAD = 2;
    int numColWidth = maxNumLength + COLUMN_PADDING_SPACES;
    int nameColWidth = maxNameLength + COLUMN_PADDING_SPACES;
    int priceColWidth = maxPriceLength + COLUMN_PADDING_SPACES;
    int categoryColWidth = maxCategoryLength + COLUMN_PADDING_SPACES;
    int qtyColWidth = manualMax((int)string("Qty.").length(), maxQtyLength) + COLUMN_PADDING_SPACES + QTY_TOTAL_COLUMN_EXTRA_PAD;
    string cartHeaderLine = "+" + string(numColWidth, '-') + "+" + string(nameColWidth, '-') + "+" + string(priceColWidth, '-') + "+" + string(categoryColWidth, '-') + "+" + string(qtyColWidth, '-') + "+";
    cout << cartHeaderLine << endl;
    cout << "|" << left << setw(numColWidth) << " No."
         << "|" << left << setw(nameColWidth) << " Name"
         << "|" << left << setw(priceColWidth) << " Price"
         << "|" << left << setw(categoryColWidth) << " Category"
         << "|" << left << setw(qtyColWidth) << " Qty."
         << "|" << endl;
    cout << cartHeaderLine << endl;
    int i = 1;
    CustomerNode* current = cartList;
    while (current) {
        cout << "|" << left << setw(numColWidth) << (" " + intToString(i));
        cout << "|" << left << setw(nameColWidth) << (" " + current->data.name);
        cout << "|" << left << setw(priceColWidth) << (" " + current->data.priceAtPurchase);
        cout << "|" << left << setw(categoryColWidth) << (" " + current->data.category);
        string qtyStr = intToString(current->data.quantity);
        int paddingToLeft = qtyColWidth - qtyStr.length() -1;
        if (paddingToLeft < 0) paddingToLeft = 0;
        cout << "|" << string(paddingToLeft, ' ');
        setColor(LIGHT_BLUE); cout << qtyStr; resetColor();
        cout << " |" << endl;
        current = current->next;
        i++;
    }
    cout << cartHeaderLine << endl;
    cout << "\nDo you want to confirm your order? (1 for Yes, 0 for No): ";
    string input;
    int choice;
    getline(cin, input);
    try {
        size_t pos;
        choice = custom_stoi(input, &pos);
        string remaining = input.substr(pos);
        remaining.erase(0, remaining.find_first_not_of(" \t\n\r"));
        if (!remaining.empty()) {
            throw invalid_argument("Contains non-numeric characters");
        }
    } catch (...) {
        setColor(RED); cout << "Invalid input. Please enter 0 or 1.\n"; resetColor();
        cout << "\nPress any key to return..."; _getch(); return;
    }
    if (choice == 1) {
        ofstream file("orders.txt", ios::app);
        if (!file) {
            setColor(RED); cout << "Failed to open orders file. Order could not be placed.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        time_t now = time(0);
        char time_buffer[50];
        strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d_%H:%M:%S", localtime(&now));
        string dt = time_buffer;
        float totalOrderAmount = 0.0;
        system("cls");
        setColor(LIGHT_BLUE); cout << "\n=== Order Invoice ===\n"; resetColor();
        cout << "Date: " << dt << endl;
        cout << "Customer: " << username << endl;
        cout << "\nItems Ordered:\n";
        maxNumLength = (int)string("No.").length();
        maxNameLength = (int)string("Name").length();
        int maxPriceColContentLength = (int)string("Price").length();
        maxQtyLength = (int)string("Qty").length();
        int maxItemTotalContentLength = (int)string("Total").length();
        current = cartList;
        while (current) {
            maxNumLength = manualMax(maxNumLength, (int)intToString(cartCount).length());
            maxNameLength = manualMax(maxNameLength, (int)current->data.name.length());
            string numPricePart = current->data.priceAtPurchase.substr(3);
            double priceVal = 0.0;
            try { priceVal = custom_stod(numPricePart); } catch(...) {}
            float itemTotal = priceVal * current->data.quantity;
            stringstream priceSS_len, totalSS_len;
            priceSS_len << "RM" << fixed << setprecision(2) << priceVal;
            totalSS_len << "RM" << fixed << setprecision(2) << itemTotal;
            maxPriceColContentLength = manualMax(maxPriceColContentLength, (int)priceSS_len.str().length());
            maxItemTotalContentLength = manualMax(maxItemTotalContentLength, (int)totalSS_len.str().length());
            maxQtyLength = manualMax(maxQtyLength, (int)intToString(current->data.quantity).length());
            current = current->next;
        }
        numColWidth = maxNumLength + COLUMN_PADDING_SPACES;
        nameColWidth = maxNameLength + COLUMN_PADDING_SPACES;
        priceColWidth = maxPriceColContentLength + COLUMN_PADDING_SPACES;
        qtyColWidth = manualMax((int)string("Qty").length(), maxQtyLength) + COLUMN_PADDING_SPACES + QTY_TOTAL_COLUMN_EXTRA_PAD;
        int itemTotalColWidth = manualMax((int)string("Total").length(), maxItemTotalContentLength) + COLUMN_PADDING_SPACES + QTY_TOTAL_COLUMN_EXTRA_PAD;
        string invoiceLine = "+" + string(numColWidth, '-') + "+" + string(nameColWidth, '-') + "+" + string(priceColWidth, '-') + "+" + string(qtyColWidth, '-') + "+" + string(itemTotalColWidth, '-') + "+";
        cout << invoiceLine << endl;
        cout << "|" << left << setw(numColWidth) << " No."
             << "|" << left << setw(nameColWidth) << " Name"
             << "|" << left << setw(priceColWidth) << " Price"
             << "|" << left << setw(qtyColWidth) << " Qty"
             << "|" << left << setw(itemTotalColWidth) << " Total"
             << "|" << endl;
        cout << invoiceLine << endl;
        current = cartList;
        i = 1;
        CustomerNode* nextCartItem = NULL;
        while (current) {
            nextCartItem = current->next;
            string numPricePart = current->data.priceAtPurchase.substr(3);
            double priceVal = 0.0;
            try { priceVal = custom_stod(numPricePart); } catch(...) {}
            float itemTotal = priceVal * current->data.quantity;
            totalOrderAmount += itemTotal;
            stringstream priceSS, totalSS;
            priceSS << "RM" << fixed << setprecision(2) << priceVal;
            totalSS << "RM" << fixed << setprecision(2) << itemTotal;
            cout << "|" << left << setw(numColWidth) << (" " + intToString(i));
            cout << "|" << left << setw(nameColWidth) << (" " + current->data.name);
            cout << "|" << left << setw(priceColWidth) << (" " + priceSS.str());
            string qtyStr = intToString(current->data.quantity);
            int qtyPaddingToLeft = qtyColWidth - qtyStr.length() -1;
            if (qtyPaddingToLeft < 0) qtyPaddingToLeft = 0;
            cout << "|" << string(qtyPaddingToLeft, ' ');
            setColor(LIGHT_BLUE); cout << qtyStr; resetColor();
            cout << " |";
            string totalStr = totalSS.str();
            int totalPaddingToLeft = itemTotalColWidth - totalStr.length() -1;
            if (totalPaddingToLeft < 0) totalPaddingToLeft = 0;
            cout << string(totalPaddingToLeft, ' ');
            setColor(LIGHT_BLUE); cout << totalStr; resetColor();
            cout << " |" << endl;
            file << username << "\t" << dt << "\t" << current->data.name << "\t"
                 << current->data.quantity << "\t" << current->data.priceAtPurchase << endl;
            globalProductList->updateProductStock(current->data.name, -current->data.quantity, productsFilename);
            OrderItem orderedItem = current->data;
            orderedItem.timestamp = dt;
            insertAtEnd(orderHistory, orderedItem);
            orderHistoryCount++;
            current = nextCartItem;
            i++;
        }
        cout << invoiceLine << endl;
        stringstream totalAmountSS;
        totalAmountSS << "RM" << fixed << setprecision(2) << totalOrderAmount;
        cout << "Total Amount: " << totalAmountSS.str() << endl;
        cout << invoiceLine << endl;
        file.close();
        clearList(cartList);
        cartCount = 0;
        setColor(GREEN); cout << "Order placed successfully! Thank you for your purchase.\n"; resetColor();
        cout << "\nPress any key to return..."; _getch();
    } else {
        setColor(RED); cout << "Order cancelled.\n"; resetColor();
        cout << "\nPress any key to return..."; _getch();
    }
}
    void viewOrderHistory() {
        system("cls");
        setColor(LIGHT_BLUE); cout << "\n=== Purchase Histories ===\n"; resetColor();
        if (!isLoggedIn) {
            setColor(RED); cout << "Please login first to view purchase history.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        if (!orderHistory) {
            setColor(RED); cout << "No purchase history available.\n"; resetColor();
            cout << "\nPress any key to return..."; _getch(); return;
        }
        CustomerNode* current = orderHistory;
        int maxNumLength = (int)string("No.").length();
        int maxNameLength = (int)string("Product").length();
        int maxQtyLength = (int)string("Qty").length();
        int maxPriceLength = (int)string("Price").length();
        int maxTimeLength = (int)string("Timestamp").length();
        CustomerNode* temp = orderHistory;
        while (temp) {
            maxNumLength = manualMax(maxNumLength, (int)intToString(orderHistoryCount).length());
            maxNameLength = manualMax(maxNameLength, (int)temp->data.name.length());
            maxQtyLength = manualMax(maxQtyLength, (int)intToString(temp->data.quantity).length());
            maxPriceLength = manualMax(maxPriceLength, (int)temp->data.priceAtPurchase.length());
            maxTimeLength = manualMax(maxTimeLength, (int)temp->data.timestamp.length());
            temp = temp->next;
        }
        const int COLUMN_PADDING_SPACES = 2;
        const int QTY_COL_EXTRA_PAD_HISTORY = 2;
        const int TIME_COL_EXTRA_PAD_HISTORY = 4;
        int numColWidth = maxNumLength + COLUMN_PADDING_SPACES;
        int nameColWidth = maxNameLength + COLUMN_PADDING_SPACES;
        int qtyColWidth = manualMax((int)string("Qty").length(), maxQtyLength) + COLUMN_PADDING_SPACES + QTY_COL_EXTRA_PAD_HISTORY;
        int priceColWidth = maxPriceLength + COLUMN_PADDING_SPACES;
        int timeColWidth = manualMax((int)string("Timestamp").length(), maxTimeLength) + COLUMN_PADDING_SPACES + TIME_COL_EXTRA_PAD_HISTORY;
        string headerLine = "+" + string(numColWidth, '-') + "+" + string(nameColWidth, '-') + "+" + string(priceColWidth, '-') + "+" + string(qtyColWidth, '-') + "+" + string(timeColWidth, '-') + "+";
        cout << headerLine << endl;
        cout << "|" << left << setw(numColWidth) << " No."
             << "|" << left << setw(nameColWidth) << " Product"
             << "|" << left << setw(priceColWidth) << " Price"
             << "|" << left << setw(qtyColWidth) << " Qty"
             << "|" << left << setw(timeColWidth) << " Timestamp"
             << "|" << endl;
        cout << headerLine << endl;
        int i = 1;
        current = orderHistory;
        while (current) {
            cout << "|" << left << setw(numColWidth) << (" " + intToString(i));
            cout << "|" << left << setw(nameColWidth) << (" " + current->data.name);
            cout << "|" << left << setw(priceColWidth) << (" " + current->data.priceAtPurchase);
            string qtyStr = intToString(current->data.quantity);
            int qtyPaddingToLeft = qtyColWidth - qtyStr.length() -1;
            if (qtyPaddingToLeft < 0) qtyPaddingToLeft = 0;
            cout << "|" << string(qtyPaddingToLeft, ' ');
            setColor(LIGHT_BLUE); cout << qtyStr; resetColor();
            cout << " |";
            cout << left << setw(timeColWidth) << (" " + current->data.timestamp);
            cout << "|" << endl;
            current = current->next;
            i++;
        }
        cout << headerLine << endl;
        cout << "\nPress any key to return..."; _getch();
    }
    bool isUserLoggedIn() const {
        return isLoggedIn;
    }
};
bool adminLogin(const string& staffFilename);
void adminRegister(const string& staffFilename);
void displayAdminMenu(ProductList& list, const string& categoriesFilename, const string& productsFilename, const string& ordersFilename);
void displayCustomerMenu(Customer& cust, const string& productsFilename);
int main() {
    ProductList globalProductList;
    Customer customerApp(&globalProductList);
    string categoriesFilename = "categories.txt";
    string productsFilename = "flowers.txt";
    string staffFilename = "staff.txt";
    string loginFilename = "login.txt";
    string ordersFilename = "orders.txt";
    while (true) {
        system("cls");
        setColor(LIGHT_BLUE); cout << "\n==== Welcome to Florish Shop ====\n"; resetColor();
        cout << "1. Admin Login/Registration\n";
        cout << "2. Customer Login/Registration\n";
        cout << "3. Exit Program\n";
        cout << "Enter your choice (1-3): ";
        string input;
        int initialChoice;
        getline(cin, input);
        try {
            size_t pos;
            initialChoice = custom_stoi(input, &pos);
            string remaining = input.substr(pos);
            remaining.erase(0, remaining.find_first_not_of(" \t\n\r"));
            if (!remaining.empty()) {
                throw invalid_argument("Contains non-numeric characters");
            }
        } catch (const invalid_argument&) {
            setColor(RED); cout << "Invalid input. Please enter a number from 1 to 3.\n"; resetColor();
            Sleep(100);
            continue;
        } catch (const out_of_range&) {
            setColor(RED); cout << "Input number is out of range. Please enter 1 to 3.\n"; resetColor();
            Sleep(100);
            continue;
        }
        switch (initialChoice) {
            case 1: {
                bool adminAccessGranted = false;
                while (!adminAccessGranted) {
                    system("cls");
                    setColor(LIGHT_BLUE); cout << "\n=== Admin Access ===\n"; resetColor();
                    cout << "1. Admin Register\n";
                    cout << "2. Admin Login\n";
                    cout << "3. Back to Main Menu\n";
                    cout << "Enter choice (1-3): ";
                    string adminInput;
                    int adminChoice;
                    getline(cin, adminInput);
                    try {
                        size_t pos;
                        adminChoice = custom_stoi(adminInput, &pos);
                        string remaining = adminInput.substr(pos);
                        remaining.erase(0, remaining.find_first_not_of(" \t\n\r"));
                        if (!remaining.empty()) throw invalid_argument("");
                    } catch (...) {
                        setColor(RED); cout << "Invalid input. Please enter a number from 1 to 3.\n"; resetColor();
                        Sleep(100);
                        continue;
                    }
                    switch (adminChoice) {
                        case 1:
                            adminRegister(staffFilename);
                            break;
                        case 2:
                            try {
                                setColor(YELLOW); cout << "Loading categories and products for validation...\n"; resetColor();
                                globalProductList.loadCategories(categoriesFilename);
                                globalProductList.loadFromFile(productsFilename);
                                Sleep(100);
                                if (adminLogin(staffFilename)) {
                                    adminAccessGranted = true;
                                    setColor(GREEN); cout << "Admin Login successful! Redirecting to Admin Menu...\n"; resetColor();
                                    showLoading();
                                    displayAdminMenu(globalProductList, categoriesFilename, productsFilename, ordersFilename);
                                }
                            } catch (const runtime_error& e) {
                                cerr << "Fatal Error during initial data loading for Admin: " << e.what() << endl;
                                setColor(RED); cout << "\nPress any key to exit program due to fatal error..."; resetColor();
                                _getch();
                                return 1;
                            }
                            break;
                        case 3:
                            adminAccessGranted = true;
                            break;
                        default:
                            setColor(RED); cout << "Invalid choice. Please enter 1, 2, or 3.\n"; resetColor();
                            Sleep(100);
                            break;
                    }
                }
                break;
            }
            case 2: {
                bool customerAccessGranted = false;
                while (!customerAccessGranted) {
                    system("cls");
                    setColor(LIGHT_BLUE); cout << "\n=== Customer Access ===\n"; resetColor();
                    cout << "1. Customer Register\n";
                    cout << "2. Customer Login\n";
                    cout << "3. Back to Main Menu\n";
                    cout << "Enter choice (1-3): ";
                    string customerInput;
                    int customerChoice;
                    getline(cin, customerInput);
                    try {
                        size_t pos;
                        customerChoice = custom_stoi(customerInput, &pos);
                        string remaining = customerInput.substr(pos);
                        remaining.erase(0, remaining.find_first_not_of(" \t\n\r"));
                        if (!remaining.empty()) throw invalid_argument("");
                    } catch (...) {
                        setColor(RED); cout << "Invalid input. Please enter a number from 1 to 3.\n"; resetColor();
                        Sleep(100);
                        continue;
                    }
                    switch (customerChoice) {
                        case 1:
                            customerApp.registerCustomer(loginFilename);
                            break;
                        case 2:
                            try {
                                setColor(YELLOW); cout << "Loading categories and products for customer access...\n"; resetColor();
                                globalProductList.loadCategories(categoriesFilename);
                                globalProductList.loadFromFile(productsFilename);
                                Sleep(100);
                                if (customerApp.login(loginFilename)) {
                                    customerAccessGranted = true;
                                    setColor(GREEN); cout << "Customer Login successful! Redirecting to Customer Menu...\n"; resetColor();
                                    showLoading();
                                    displayCustomerMenu(customerApp, productsFilename);
                                }
                            } catch (const runtime_error& e) {
                                cerr << "Fatal Error during initial data loading for Customer: " << e.what() << endl;
                                setColor(RED); cout << "\nPress any key to exit program due to fatal error..."; resetColor();
                                _getch();
                                return 1;
                            }
                            break;
                        case 3:
                            customerAccessGranted = true;
                            break;
                        default:
                            setColor(RED); cout << "Invalid choice. Please enter 1, 2, or 3.\n"; resetColor();
                            Sleep(100);
                            break;
                    }
                }
                break;
            }
            case 3:
                setColor(RED); cout << "Exiting Florish Shop program. Goodbye!\n"; resetColor();
                Sleep(100);
                return 0;
            default:
                setColor(RED); cout << "Invalid choice. Please enter 1, 2, or 3.\n"; resetColor();
                Sleep(100);
                break;
        }
    }
    return 0;
}
bool authenticateUser(const string& username, const string& password, const string& filename) {
    ifstream file(filename.c_str());
    if (!file) {
        cerr << "Error: Staff file not found or cannot be opened: " + filename + "\n";
        return false;
    }
    string line;
    while (getline(file, line)) {
        size_t first = line.find_first_not_of(" \t\n\r");
        if (string::npos != first) {
            size_t last = line.find_last_not_of(" \t\n\r");
            line = line.substr(first, (last - first + 1));
        } else {
            line = "";
        }
        if (line.empty()) continue;
        size_t commaPos = manualFind(line, ',');
        if (commaPos == string::npos) {
            cerr << "Warning: Skipping malformed line in " << filename << " (no comma): " << line << endl;
            continue;
        }
        string storedUsername = line.substr(0, commaPos);
        string storedPassword = line.substr(commaPos + 1);
        storedUsername.erase(0, storedUsername.find_first_not_of(" \t\n\r"));
        storedUsername.erase(storedUsername.find_last_not_of(" \t\n\r") + 1);
        storedPassword.erase(0, storedPassword.find_first_not_of(" \t\n\r"));
        storedPassword.erase(storedPassword.find_last_not_of(" \t\n\r") + 1);
        if (username == storedUsername && password == storedPassword) {
            file.close();
            return true;
        }
    }
    file.close();
    return false;
}
bool adminLogin(const string& staffFilename) {
    system("cls");
    string username, password;
    int attempts = 3;
    setColor(LIGHT_BLUE); cout << "\n=== Admin Login ===\n"; resetColor();
    while (attempts > 0) {
        cout << "Enter username: ";
        getline(cin, username);
        username.erase(0, username.find_first_not_of(" \t\n\r"));
        username.erase(username.find_last_not_of(" \t\n\r") + 1);
        if (username.empty()) {
            setColor(RED); cout << "Username cannot be empty.\n"; resetColor();
            attempts--;
            if (attempts > 0) {
                cout << "Attempts remaining: " << attempts << "\n";
                Sleep(0); system("cls"); setColor(LIGHT_BLUE); cout << "\n=== Admin Login ===\n"; resetColor();
            }
            continue;
        }
        cout << "Enter password: ";
        password = getPassword();
        if (password.empty()) {
            setColor(RED); cout << "Password cannot be empty.\n"; resetColor();
            attempts--;
            if (attempts > 0) {
                cout << "Attempts remaining: " << attempts << "\n";
                Sleep(0); system("cls"); setColor(LIGHT_BLUE); cout << "\n=== Admin Login ===\n"; resetColor();
            }
            continue;
        }
        if (authenticateUser(username, password, staffFilename)) {
            return true;
        } else {
            attempts--;
            setColor(RED); cout << "Invalid credentials.\n"; resetColor();
            if (attempts > 0) {
                cout << "Attempts remaining: " << attempts << "\n";
                Sleep(0); system("cls"); setColor(LIGHT_BLUE); cout << "\n=== Admin Login ===\n"; resetColor();
            }
        }
    }
    setColor(RED); cout << "Too many failed attempts. Returning to main menu.\n"; resetColor();
    Sleep(0);
    return false;
}
void adminRegister(const string& staffFilename) {
    system("cls");
    string username, password;
    setColor(LIGHT_BLUE); cout << "\n=== Admin Registration ===\n"; resetColor();
    bool fileExistsInitially = ifstream(staffFilename.c_str()).good();
    cout << "Enter desired username: ";
    getline(cin, username);
    username.erase(0, username.find_first_not_of(" \t\n\r"));
    username.erase(username.find_last_not_of(" \t\n\r") + 1);
    if (username.empty()) {
        setColor(RED); cout << "Username cannot be empty.\n"; resetColor();
        cout << "\nPress any key to return..."; _getch(); return;
    }
    if (manualFind(username, ',') != string::npos) {
        setColor(RED); cout << "Username cannot contain commas.\n"; resetColor();
        cout << "\nPress any key to return..."; _getch(); return;
    }
    if (fileExistsInitially && manualFileContains(staffFilename, username)) {
        setColor(RED); cout << "Username '" << username << "' already exists (case-insensitive match).\n"; resetColor();
        cout << "\nPress any key to return..."; _getch(); return;
    }
    cout << "Enter desired password: ";
    password = getPassword();
    if (password.empty()) {
        setColor(RED); cout << "Password cannot be empty.\n"; resetColor();
        cout << "\nPress any key to return..."; _getch(); return;
    }
    if (manualFind(password, ',') != string::npos) {
        setColor(RED); cout << "Password cannot contain commas.\n"; resetColor();
        cout << "\nPress any key to return..."; _getch(); return;
    }
    ofstream outFile(staffFilename.c_str(), ios::app);
    if (!outFile) {
        cerr << "Error: Could not open staff file for writing: " << staffFilename << endl;
        setColor(RED); cout << "Registration failed due to file error.\n"; resetColor();
    } else {
        outFile << username << "," << password << endl;
        outFile.close();
        setColor(GREEN); cout << "Admin user '" << username << "' registered successfully.\n"; resetColor();
    }
    cout << "\nPress any key to return..."; _getch();
}
void displayAdminMenu(ProductList& list, const string& categoriesFilename, const string& productsFilename, const string& ordersFilename) {
    bool adminLoggedIn = true;
    while (adminLoggedIn) {
        system("cls");
        setColor(LIGHT_BLUE); cout << "\n=== Admin Menu ===\n"; resetColor();
        cout << "1. View Categories (Sorted Alphabetically)\n";
        cout << "2. View Products (Sorted Alphabetically by Name)\n";
        cout << "3. Add New Category\n";
        cout << "4. Add New Product\n";
        cout << "5. Delete Category\n";
        cout << "6. Delete Product\n";
        cout << "7. Search Product (by Name or Category)\n";
        cout << "8. Search Category (by Name)\n";
        cout << "9. Edit Product Stock\n";
        cout << "10. Logout\n";
        cout << "Enter your choice (1-10): ";
        string input;
        int choice;
        getline(cin, input);
        try {
            size_t pos;
            choice = custom_stoi(input, &pos);
            string remaining = input.substr(pos);
            remaining.erase(0, remaining.find_first_not_of(" \t\n\r"));
            if (!remaining.empty()) {
                throw invalid_argument("Contains non-numeric characters");
            }
        } catch (const invalid_argument&) {
            setColor(RED); cout << "Invalid input. Please enter a number from 1 to 10.\n"; resetColor();
            Sleep(100);
            continue;
        } catch (const out_of_range&) {
            setColor(RED); cout << "Input number is out of range. Please enter 1 to 10.\n"; resetColor();
            Sleep(100);
            continue;
        }
        switch (choice) {
            case 1: list.sortAndDisplayCategories(); break;
            case 2: list.sortAndDisplayProductNames(); break;
            case 3: list.addCategory(categoriesFilename); break;
            case 4: list.addProduct(productsFilename); break;
            case 5: list.deleteCategory(categoriesFilename, productsFilename); cout << "\nPress any key to return to menu..."; _getch(); break;
            case 6: list.deleteProduct(productsFilename, ordersFilename); cout << "\nPress any key to return to menu..."; _getch(); break;
            case 7: {
                system("cls"); setColor(LIGHT_BLUE); cout << "\n=== Search Product ===\n"; resetColor();
                string searchTerm;
                cout << "Enter product name or category to search: ";
                getline(cin, searchTerm);
                searchTerm.erase(0, searchTerm.find_first_not_of(" \t\n\r"));
                searchTerm.erase(searchTerm.find_last_not_of(" \t\n\r") + 1);
                if (!searchTerm.empty()) {list.searchProduct(searchTerm);} else {setColor(RED); cout << "Search term cannot be empty.\n"; resetColor(); cout << "\nPress any key to return to menu..."; _getch();}
                break;
            }
            case 8: {
                system("cls"); setColor(LIGHT_BLUE); cout << "\n=== Search Category ===\n"; resetColor();
                string searchTerm;
                cout << "Enter category name to search: ";
                getline(cin, searchTerm);
                searchTerm.erase(0, searchTerm.find_first_not_of(" \t\n\r"));
                searchTerm.erase(searchTerm.find_last_not_of(" \t\n\r") + 1);
                if (!searchTerm.empty()) {list.searchCategory(searchTerm);} else {setColor(RED); cout << "Search term cannot be empty.\n"; resetColor(); cout << "\nPress any key to return to menu..."; _getch();}
                break;
            }
            case 9: list.editProductStock(productsFilename); break;
            case 10: adminLoggedIn = false; setColor(YELLOW); cout << "Admin logged out. Returning to main menu.\n"; resetColor(); Sleep(100); break;
            default: setColor(RED); cout << "Invalid choice. Please enter a number from 1 to 10.\n"; resetColor(); Sleep(100); break;
        }
    }
}
void displayCustomerMenu(Customer& cust, const string& productsFilename) {
    bool customerLoggedIn = true;
    while (customerLoggedIn) {
        system("cls");
        setColor(LIGHT_BLUE); cout << "\n=== Customer Menu ===\n"; resetColor();
        cout << "1. Add Product to Cart\n";
        cout << "2. Search Product\n";
        cout << "3. Sort Products (by Category or Name)\n";
        cout << "4. Remove from Cart\n";
        cout << "5. Update Cart Quantity\n";
        cout << "6. Place Order\n";
        cout << "7. View Order History\n";
        cout << "8. Logout\n";
        cout << "Enter your choice (1-8): ";
        string input;
        int choice;
        getline(cin, input);
        try {
            size_t pos;
            choice = custom_stoi(input, &pos);
            string remaining = input.substr(pos);
            remaining.erase(0, remaining.find_first_not_of(" \t\n\r"));
            if (!remaining.empty()) {
                throw invalid_argument("Contains non-numeric characters");
            }
        } catch (const invalid_argument&) {
            setColor(RED); cout << "Invalid input. Please enter a number from 1 to 8.\n"; resetColor();
            Sleep(100);
            continue;
        } catch (const out_of_range&) {
            setColor(RED); cout << "Input number is out of range. Please enter 1 to 8.\n"; resetColor();
            Sleep(100);
            continue;
        }
        switch (choice) {
            case 1: cust.addProductToCart(); break;
            case 2: cust.searchProduct(); break;
            case 3: cust.sortProducts(); break;
            case 4: cust.removeFromCart(); break;
            case 5: cust.updateCartQuantity(); break;
            case 6: cust.order(productsFilename); break;
            case 7: cust.viewOrderHistory(); break;
            case 8:
                customerLoggedIn = false;
                cust.logout();
                break;
            default:
                setColor(RED); cout << "Invalid choice. Please enter 1 to 8.\n"; resetColor();
                Sleep(100);
                break;
        }
    }
}
