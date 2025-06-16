#include <iostream>
#include <fstream> // For file operations
#include <string>
#include <stdexcept> // For runtime_error, invalid_argument, out_of_range
// Removed: #include <vector>
#include <conio.h> // For _getch
#include <windows.h> // For Sleep, GetStdHandle, GetConsoleScreenBufferInfo
// Removed: #include <algorithm> // For transform, find_if, replace
#include <cctype> // For tolower, isdigit (will use manual tolower, but isdigit is basic)
#include <cstdlib> // For system("cls"), system("pause")
#include <limits> // For numeric_limits
#include <sstream> // For parsing strings (allowed for I/O)
#include <numeric> // For numeric_limits (for max())
#include <iomanip> // For output formatting (optional, but good for price)


using namespace std;

// Struct for Product
struct Product {
    string name;
    string price; // Store price as string (e.g., "RM 140.00")
    string category;
    int stock;
};

// Struct for Linked List Node
struct ProductNode {
    Product data;
    ProductNode* next;
    ProductNode(string n, string p, string c, int s) : data{n, p, c, s}, next(nullptr) {}
    // No need for copy/assignment ops for this simple case, default ones are fine
};

// Manual toLower function (replaces std::toLower used with transform)
string manualToLower(const string& str) {
    string lower = str;
    for (size_t i = 0; i < lower.length(); ++i) { // Use size_t for string index
        lower[i] = ::tolower(lower[i]); // Use global ::tolower from cctype
    }
    return lower;
}

// Manual find character in string (simplified, returns index or npos)
size_t manualFind(const string& str, char ch, size_t start_pos = 0) {
    for (size_t i = start_pos; i < str.length(); ++i) {
        if (str[i] == ch) {
            return i;
        }
    }
    return string::npos;
}

// Manual search for string in file (for checking existing users)
bool manualFileContains(const string& filename, const string& search_term) {
    ifstream file(filename);
    if (!file) {
        // File not found, cannot contain the term
        return false;
    }
    string line;
    string lower_search_term = manualToLower(search_term);
    while (getline(file, line)) {
         line.erase(0, line.find_first_not_of(" \t\n\r"));
         line.erase(line.find_last_not_of(" \t\n\r") + 1);
         if (!line.empty()) {
             // Extract username part assuming format is "username,password"
             size_t commaPos = manualFind(line, ',');
             string storedUsername = (commaPos == string::npos) ? line : line.substr(0, commaPos);
              storedUsername.erase(0, storedUsername.find_first_not_of(" \t\n\r"));
              storedUsername.erase(storedUsername.find_last_not_of(" \t\n\r") + 1);

             if (manualToLower(storedUsername) == lower_search_term) {
                 file.close();
                 return true; // Found a match
             }
         }
    }
    file.close();
    return false; // No match found
}


// Class to manage products using a Linked List and categories using a dynamic array
class ProductList {
private:
    ProductNode* head;
    int size; // Product count

    // Replaced std::vector<string> categories
    string* categoryArray;
    int categoryCount;
    int categoryCapacity;

    // Helper for dynamic category array resizing
    void resizeCategoryArray(int newCapacity) {
        if (newCapacity < categoryCount) {
             // Handle shrinking carefully if needed, or just disallow shrinking below count
             // For simplicity here, we assume resizing is only for growth
             return;
        }
        if (newCapacity == 0) newCapacity = 1; // Always keep at least capacity 1

        string* newArray = new string[newCapacity];
        // Manually copy elements
        for (int i = 0; i < categoryCount; ++i) {
            newArray[i] = categoryArray[i];
        }

        delete[] categoryArray; // Free old memory
        categoryArray = newArray;
        categoryCapacity = newCapacity;
    }

public:
    // Constructor
    ProductList() : head(nullptr), size(0), categoryArray(nullptr), categoryCount(0), categoryCapacity(0) {}

    // Destructor to free allocated memory
    ~ProductList() {
        ProductNode* current = head;
        while (current != nullptr) {
            ProductNode* next = current->next;
            delete current;
            current = next;
        }
        head = nullptr; // Ensure head is null after deleting all nodes
        size = 0;

        // Free category array memory
        delete[] categoryArray;
        categoryArray = nullptr;
        categoryCount = 0;
        categoryCapacity = 0;
    }

    // Prevent copying and assignment (optional but good practice for classes managing resources)
    ProductList(const ProductList&) = delete;
    ProductList& operator=(const ProductList&) = delete;

    // Validate price format (e.g., "RM 140.00")
    bool isValidPrice(const string& price) const { // Made const
        // Check for "RM " prefix (case-insensitive check)
        if (price.length() < 4 || manualToLower(price.substr(0, 3)) != "rm ") return false;

        string numPart = price.substr(3); // Get the part after "RM "

        // Check if the numeric part contains only digits and at most one decimal point
        bool hasDecimal = false;
        bool hasDigit = false;
        size_t decimalPos = string::npos;

        for (size_t i = 0; i < numPart.length(); ++i) {
            char c = numPart[i];
            if (isdigit(c)) {
                hasDigit = true;
            } else if (c == '.') {
                if (hasDecimal) return false; // More than one decimal point
                hasDecimal = true;
                decimalPos = i;
            } else {
                return false; // Invalid character
            }
        }

        if (!hasDigit) return false; // Must have at least one digit

        // Price must have a decimal point and exactly two digits after it, e.g., "RM 140.00" not "RM 140" or "RM 140.0"
        if (!hasDecimal || numPart.length() - 1 - decimalPos != 2) {
            return false;
        }

        // Attempt to convert to double to catch floating-point errors - assumes stod is allowed
        try {
            size_t pos;
            stod(numPart, &pos); // Use stod for parsing numeric part
            // Check if the entire numeric part was consumed by stod
            return pos == numPart.length();

        } catch (...) {
            return false; // conversion failed
        }
    }

    // Check if a file exists
    bool fileExists(const string& filename) const { // Made const
        ifstream file(filename);
        return file.good();
    }

     // Insertion sort for categories (case-insensitive) - DEFINITION STARTS HERE
    void insertionSortCategories(string* arr, int n, bool ascending) {
        if (arr == nullptr || n <= 1) return; // Add checks

        for (int i = 1; i < n; i++) {
            string key = arr[i];
            string lowerKey = manualToLower(key);
            int j = i - 1;
            while (j >= 0) {
                bool comparison;
                if (ascending) {
                    comparison = (manualToLower(arr[j]) > lowerKey);
                } else {
                    comparison = (manualToLower(arr[j]) < lowerKey);
                }

                if (comparison) {
                    arr[j + 1] = arr[j];
                    j--;
                } else {
                    break;
                }
            }
            arr[j + 1] = key;
        }
    } // DEFINITION ENDS HERE


    // Load categories from file
    void loadCategories(const string& filename) {
        // Clear existing categories
        delete[] categoryArray;
        categoryArray = nullptr;
        categoryCount = 0;
        categoryCapacity = 0;


        ifstream inFile(filename);

        if (!inFile) {
             cerr << "Categories file not found: " << filename << ". Creating default.\n";
             ofstream outFile(filename);
             if (outFile) {
                 outFile << "Uncategorized\n"; // Use a more generic default
                 outFile.close();
                 cout << "Default category 'Uncategorized' created.\n";
                 // Now add it to the array
                 categoryCapacity = 10; // Initial capacity
                 categoryArray = new string[categoryCapacity];
                 categoryArray[0] = "Uncategorized";
                 categoryCount = 1;
             } else {
                 cerr << "Error: Cannot create default categories file: " << filename << endl;
                 throw runtime_error("Initialization failed: Cannot create categories file.");
             }
        } else {
            string line;
             // Read lines to populate array, resizing dynamically
             categoryCapacity = 10; // Initial capacity
             categoryArray = new string[categoryCapacity];
             categoryCount = 0;

             while (getline(inFile, line)) {
                // Trim leading/trailing whitespace manually
                 size_t first = line.find_first_not_of(" \t\n\r");
                if (string::npos == first) {
                    line = ""; // line is all whitespace
                } else {
                    size_t last = line.find_last_not_of(" \t\n\r");
                    line = line.substr(first, (last - first + 1));
                }


                if (!line.empty()) {
                     // Add to dynamic array - resize if needed
                     if (categoryCount == categoryCapacity) {
                         resizeCategoryArray(categoryCapacity * 2); // Double capacity
                     }
                     categoryArray[categoryCount++] = line;
                }
            }
            inFile.close();

             if (categoryCount == 0) { // If file existed but was empty after trimming
                cout << "Warning: Categories file was empty or contained only whitespace. Adding a default category.\n";
                // Re-allocate if needed, ensure size is at least 1
                 delete[] categoryArray;
                 categoryCapacity = 10; // Reset initial capacity
                 categoryArray = new string[categoryCapacity];
                 categoryArray[0] = "Uncategorized";
                 categoryCount = 1;

                 // Optional: Save the default category back to the file
                 ofstream outFile(filename);
                 if (outFile) {
                     outFile << "Uncategorized\n";
                     outFile.close();
                 } else {
                     cerr << "Error saving default category to " << filename << endl;
                 }
            }
        }

        // Always sort categories after loading or modifying
        insertionSortCategories(categoryArray, categoryCount, true); // Now calls the array version

        // Ensure "Uncategorized" exists if not loaded (should have been added above, but double-check)
        bool uncategorizedExists = false;
        for (int i = 0; i < categoryCount; ++i) {
            if (manualToLower(categoryArray[i]) == "uncategorized") {
                uncategorizedExists = true;
                break;
            }
        }
        if (!uncategorizedExists) {
             cout << "Warning: 'Uncategorized' category not found after initial load/create. Adding it.\n";
             // Add Uncategorized - handle resize
             if (categoryCount == categoryCapacity) {
                 resizeCategoryArray(categoryCapacity == 0 ? 1 : categoryCapacity * 2);
             }
             categoryArray[categoryCount++] = "Uncategorized";
             insertionSortCategories(categoryArray, categoryCount, true); // Resort
              // Optional: Save the default category back to the file
             ofstream outFile(filename);
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


    // Add new category to categories.txt
    void addCategory(const string& filename) {
        system("cls");
        string newCategory;
        cout << "Enter new category: ";
        // cin.ignore is handled in displayMenu before calling this function

        getline(cin, newCategory);
        newCategory.erase(0, newCategory.find_first_not_of(" \t\n\r"));
        newCategory.erase(newCategory.find_last_not_of(" \t\n\r") + 1);

        if (newCategory.empty()) {
            cout << "Category name cannot be empty.\n";
            return;
        }
         // Manual search for duplicate (case-insensitive)
         bool exists = false;
        for (int i = 0; i < categoryCount; ++i) {
            if (manualToLower(categoryArray[i]) == manualToLower(newCategory)) {
                exists = true;
                break;
            }
        }
        if (exists) {
            cout << "Category '" << newCategory << "' already exists (case-insensitive match).\n";
            return;
        }

        // Add to dynamic array - resize if needed
        if (categoryCount == categoryCapacity) {
             resizeCategoryArray(categoryCapacity == 0 ? 10 : categoryCapacity * 2); // Initial or double capacity
        }
        categoryArray[categoryCount++] = newCategory;

        insertionSortCategories(categoryArray, categoryCount, true); // Sort the array

        try {
            ofstream outFile(filename, ios::out | ios::trunc); // Overwrite the file with sorted list
            if (!outFile) throw runtime_error("Cannot open categories file for writing: " + filename);
            for (int i = 0; i < categoryCount; ++i) {
                outFile << categoryArray[i] << endl;
            }
            outFile.close();
            cout << "Category '" << newCategory << "' added successfully.\n";
        } catch (const runtime_error& e) {
            cerr << "Error saving categories: " << e.what() << endl;
             // If save fails, attempt to remove the category from memory to keep consistent
             // Manual removal from array
             int addedIndex = -1;
             string lowerNewCat = manualToLower(newCategory);
             for(int i=0; i<categoryCount; ++i) {
                 if(manualToLower(categoryArray[i]) == lowerNewCat) {
                     addedIndex = i;
                     break;
                 }
             }

             if(addedIndex != -1) {
                 for(int i=addedIndex; i<categoryCount-1; ++i) categoryArray[i] = categoryArray[i+1];
                 categoryCount--;
                  // Attempt to shrink if needed after failure
                   if (categoryCount > 0 && categoryCapacity / categoryCount >= 2) {
                        resizeCategoryArray(categoryCapacity / 2);
                   } else if (categoryCount == 0 && categoryCapacity > 1) {
                         resizeCategoryArray(1);
                   }
             }
             cout << "Warning: Category was added in memory but failed to save to file. Data might be inconsistent.\n";
        }
    }

    // Delete category from categories.txt and handle associated products
    void deleteCategory(const string& categoriesFilename, const string& productsFilename) {
        system("cls");
        if (categoryCount == 0) {
            cout << "No categories to delete.\n";
            return;
        }

        // Find "Uncategorized" index manually
        int uncategorizedIndex = -1;
        for(int i = 0; i < categoryCount; ++i) {
            if (manualToLower(categoryArray[i]) == "uncategorized") {
                uncategorizedIndex = i;
                break;
            }
        }

        cout << "=== Select Category to Delete ===\n";
        int displayIndex = 1;
        // Use temporary dynamic arrays to store deletable options and their original indices
        // Allocate maximum possible size initially
        string* deletableCategories = new string[categoryCount];
        int* deletableIndices = new int[categoryCount];
        int deletableCount = 0;

        for (int i = 0; i < categoryCount; ++i) {
            if (i != uncategorizedIndex) { // Don't list Uncategorized as deletable
                 cout << displayIndex << ". " << categoryArray[i] << endl;
                 deletableCategories[deletableCount] = categoryArray[i];
                 deletableIndices[deletableCount] = i; // Store the original index
                 deletableCount++;
                 displayIndex++;
            } else {
                 cout << "(Cannot delete) " << categoryArray[i] << endl;
            }
        }

        if (deletableCount == 0) {
            cout << "-----------------------------------\n";
            cout << "No other categories available to delete.\n";
             // Clean up temp arrays regardless of input type
             delete[] deletableCategories;
             delete[] deletableIndices;
             cout << "\nPress any key to return to menu...";
            _getch();
            return;
        }

         cout << "-----------------------------------\n";
        cout << "Enter category ID (1-" << deletableCount << ") to delete, or 0 to return: ";

        string input;
        int id;
        // cin.ignore is handled in displayMenu before calling this function
        getline(cin, input);
        try {
            size_t pos;
            id = stoi(input, &pos);
            string remaining = input.substr(pos);
             remaining.erase(0, remaining.find_first_not_of(" \t\n\r"));
             if (!remaining.empty()) {
                 throw invalid_argument("Contains non-numeric characters");
             }
        } catch (...) { // Catch invalid_argument, out_of_range etc.
            cout << "Invalid input. Please enter a number.\n";
             // Clean up temp arrays before returning
             delete[] deletableCategories;
             delete[] deletableIndices;
             cout << "\nPress any key to return to menu...";
            _getch();
            return;
        }

        if (id == 0) {
            cout << "Returning to menu.\n";
             // Clean up temp arrays before returning
             delete[] deletableCategories;
             delete[] deletableIndices;
             Sleep(500);
            return;
        }
        if (id < 1 || id > deletableCount) {
            cout << "Invalid category ID.\n";
             // Clean up temp arrays before returning
             delete[] deletableCategories;
             delete[] deletableIndices;
             cout << "\nPress any key to return to menu...";
            _getch();
            return;
        }

        // Get the category name and its original index from the temporary list
        string categoryToDelete = deletableCategories[id - 1];
        int originalIndexToRemove = deletableIndices[id - 1];

        delete[] deletableCategories; // Clean up temp arrays
        delete[] deletableIndices;


        // Count products in this category manually (Replaces the vector<Product> usage)
        int productsInCategoryCount = 0;
        ProductNode* current = head;
        string lowerCategoryToDelete = manualToLower(categoryToDelete);
        while(current != nullptr) {
            if (manualToLower(current->data.category) == lowerCategoryToDelete) {
                productsInCategoryCount++;
            }
            current = current->next;
        }

        cout << "\nCategory '" << categoryToDelete << "' contains " << productsInCategoryCount << " product(s).\n";
        cout << "Are you sure you want to delete this category and reassign its products to 'Uncategorized'? (y/n): ";
        string response;
        getline(cin, response);
        response = manualToLower(response);

        if (response == "y") {
            // Reassign products in the linked list
            ProductNode* node = head;
            while(node != nullptr) {
                 if (manualToLower(node->data.category) == lowerCategoryToDelete) {
                     node->data.category = "Uncategorized"; // Reassign
                 }
                 node = node->next;
            }
            // Save product list after reassigning
            try {
                 saveToFile(productsFilename);
                 cout << productsInCategoryCount << " product(s) reassigned to 'Uncategorized'.\n";
            } catch (const runtime_error& e) {
                 cerr << "Error saving products after reassigning: " << e.what() << endl;
                 cout << "Category deletion aborted due to product save failure.\n";
                 return; // Abort category deletion if products can't be saved
            }

            // Remove category from the dynamic array
            // Shift elements to fill the gap
            for (int i = originalIndexToRemove; i < categoryCount - 1; ++i) {
                categoryArray[i] = categoryArray[i+1];
            }
            categoryCount--; // Decrement count

            // Optional: Shrink array if significantly empty (not strictly required, but good practice)
             if (categoryCount > 0 && categoryCapacity / categoryCount >= 2) { // Shrink if less than half full (and not about to become empty)
                 resizeCategoryArray(categoryCapacity / 2);
             } else if (categoryCount == 0 && categoryCapacity > 1) { // If empty, shrink to capacity 1
                  resizeCategoryArray(1);
             }


            // Save categories list
            try {
                ofstream outFile(categoriesFilename, ios::out | ios::trunc); // Overwrite the file
                if (!outFile) throw runtime_error("Cannot open categories file for writing: " + categoriesFilename);
                for (int i = 0; i < categoryCount; ++i) {
                    outFile << categoryArray[i] << endl;
                }
                outFile.close();
                cout << "Category '" << categoryToDelete << "' deleted successfully.\n";
            } catch (const runtime_error& e) {
                cerr << "Error saving categories after deletion: " << e.what() << endl;
                 cout << "Category was removed from memory but failed to save to file.\n";
            }
        } else {
            cout << "Category deletion cancelled.\n";
        }
    }


    void addProduct(const string& filename) {
        system("cls");
        string name, price, category, stockInput;
        int stock;
        // cin.ignore is handled in displayMenu before calling this function

        cout << "Enter product name: ";
        getline(cin, name);
        name.erase(0, name.find_first_not_of(" \t\n\r"));
        name.erase(name.find_last_not_of(" \t\n\r") + 1);
        if (name.empty()) {
             cout << "Product name cannot be empty.\n";
             return;
        }

        ProductNode* current = head;
        while (current != nullptr) {
            if (manualToLower(current->data.name) == manualToLower(name)) {
                cout << "Product name '" << name << "' already exists!\n";
                return;
            }
            current = current->next;
        }

        cout << "Enter price (e.g., RM 140.00): ";
        getline(cin, price);
        price.erase(0, price.find_first_not_of(" \t\n\r"));
        price.erase(price.find_last_not_of(" \t\n\r") + 1);
        if (!isValidPrice(price)) {
            cout << "Invalid price format! Use format like RM 140.00 (must have '.xx').\n";
            return;
        }

        cout << "Enter category: ";
        getline(cin, category);
        category.erase(0, category.find_first_not_of(" \t\n\r"));
        category.erase(category.find_last_not_of(" \t\n\r") + 1);
        if (category.empty()) {
             cout << "Category cannot be empty.\n";
             return;
        }

        bool validCategory = false;
        string actualCategory = ""; // Store the correctly cased category name
        string lowerCategory = manualToLower(category);
        for (int i = 0; i < categoryCount; ++i) { // Iterate through categoryArray
            if (manualToLower(categoryArray[i]) == lowerCategory) {
                validCategory = true;
                actualCategory = categoryArray[i]; // Use stored case for consistency
                break;
            }
        }
        if (!validCategory) {
            cout << "Invalid category '" << category << "'! Please use one from the existing list (Categories are case-insensitive for matching, but stored case is used).\n";
            return;
        }
        category = actualCategory; // Use the correctly cased category from the list

        cout << "Enter stock: ";
        getline(cin, stockInput);
        stockInput.erase(0, stockInput.find_first_not_of(" \t\n\r"));
        stockInput.erase(stockInput.find_last_not_of(" \t\n\r") + 1);

        try {
            size_t pos;
            long stockLong = stol(stockInput, &pos); // Assuming stol is allowed
             if (pos != stockInput.length()) { // Check if entire string was consumed (no trailing junk)
                 throw invalid_argument("Contains non-numeric characters");
             }
            if (stockLong < 0) {
                cout << "Stock cannot be negative!\n";
                return;
            }
            // Check if stock value fits in an int
            if (stockLong > numeric_limits<int>::max()) { // Assuming numeric_limits allowed
                 cout << "Stock value is too large.\n";
                 return;
            }
            stock = static_cast<int>(stockLong); // Cast to int

        } catch (const invalid_argument& e) {
            cout << "Invalid stock value! Please enter a whole number.\n";
            return;
        } catch (const out_of_range& e) { // Assuming out_of_range allowed
             cout << "Stock value is out of the valid number range.\n";
             return;
        } catch (...) { // Catch any other potential exceptions
             cerr << "An unexpected error occurred processing stock input.\n";
             return;
        }

        try {
            ProductNode* newNode = new ProductNode(name, price, category, stock);
            newNode->next = head; // Add to the beginning of the list
            head = newNode;
            size++;
            saveToFile(filename);
            cout << "Product '" << name << "' added successfully.\n";
        } catch (bad_alloc& e) { // Assuming bad_alloc allowed
            cerr << "Error: Memory allocation failed (Out of memory)\n";
        } catch (const runtime_error& e) { // Catch saveToFile errors (Assuming runtime_error allowed)
             cerr << "Error saving product list after adding: " << e.what() << endl;
        }
    }

    // Delete product by name
    void deleteProduct(const string& filename) {
         system("cls");
         if (head == nullptr) {
             cout << "No products to delete.\n";
             return;
         }

         string productName;
         cout << "Enter name of product to delete: ";
         // cin.ignore is handled in displayMenu before calling this function

         getline(cin, productName);

         productName.erase(0, productName.find_first_not_of(" \t\n\r"));
         productName.erase(productName.find_last_not_of(" \t\n\r") + 1);

         if (productName.empty()) {
             cout << "Product name cannot be empty.\n";
             return;
         }

         // Find the product in the linked list manually
         ProductNode* current = head;
         ProductNode* previous = nullptr;
         bool found = false;

         string lowerProductName = manualToLower(productName);

         while(current != nullptr) {
             if (manualToLower(current->data.name) == lowerProductName) {
                 found = true;
                 break; // Found the node to delete
             }
             previous = current;
             current = current->next;
         }

         if (!found) {
             cout << "Product '" << productName << "' not found.\n";
             return;
         }

         // Ask for confirmation using the found product's actual name (correct casing)
         cout << "Are you sure you want to delete '" << current->data.name << "'? (y/n): ";
         string confirmation;
         getline(cin, confirmation);
         confirmation = manualToLower(confirmation);

         if (confirmation != "y") {
             cout << "Deletion cancelled.\n";
             return;
         }

         // Delete the node
         if (previous == nullptr) { // Deleting the head node
             head = current->next;
         } else { // Deleting a non-head node
             previous->next = current->next;
         }

         delete current; // Free the memory
         size--; // Decrement size

         // Save the updated list to file
         try {
             saveToFile(filename);
             cout << "Product '" << productName << "' deleted successfully.\n";
         } catch (const runtime_error& e) {
             cerr << "Error saving product list after deletion: " << e.what() << endl;
              cout << "Product was removed from memory but failed to save to file.\n";
         }
    }


    // Add new product (for loading from file) - adds to the end now for preserving file order upon initial load
    void addProductNode(string name, string price, string category, int stock) {
        try {
            ProductNode* newNode = new ProductNode(name, price, category, stock);
             if (head == nullptr) {
                 head = newNode;
             } else {
                 ProductNode* temp = head;
                 while(temp->next != nullptr) {
                     temp = temp->next;
                 }
                 temp->next = newNode;
             }
            size++;
        } catch (bad_alloc& e) {
            cerr << "Memory allocation failed while loading product '" << name << "'. Skipping.\n";
        }
    }

    // Manually replace character in a string
    void manualReplace(string& str, char oldChar, char newChar) const {
        for (size_t i = 0; i < str.length(); ++i) { // Use size_t for string index
            if (str[i] == oldChar) {
                str[i] = newChar;
            }
        }
    }

    // Save products to file
    void saveToFile(const string& filename) {
        try {
            ofstream outFile(filename, ios::out | ios::trunc); // Overwrite the file
            if (!outFile) throw runtime_error("Cannot open product file for writing: " + filename);
            ProductNode* current = head;
            while (current != nullptr) {
                // Replace problematic characters like commas and newlines in fields if necessary
                // Manual replacement
                string safeName = current->data.name;
                manualReplace(safeName, ',', ';');

                string safePrice = current->data.price; // Price format should be consistent "RM X.XX"
                string safeCategory = current->data.category;
                 manualReplace(safeCategory, ',', ';');

                outFile << safeName << "," << safePrice << ","
                        << safeCategory << "," << current->data.stock << endl;
                current = current->next;
            }
            outFile.close();
        } catch (const runtime_error& e) {
            throw runtime_error("Error saving to file: " + string(e.what()));
        }
    }

        void loadFromFile(const string& filename) {
         // Clear existing list before loading
         ProductNode* current = head;
        while (current != nullptr) {
            ProductNode* next = current->next;
            delete current;
            current = next;
        }
        head = nullptr;
        size = 0;

        ifstream inFile(filename);
        if (!inFile) {
            cout << "Product file not found: " + filename + ". Creating empty file.\n";
            ofstream outFile(filename); // Creates the file
            if (!outFile) {
                 cerr << "Warning: Could not create empty product file: " + filename + "\n";
            }
             outFile.close();
             return; // No data to load from a new empty file
        }

        string line;
        int lineNumber = 0;
        while (getline(inFile, line)) {
            lineNumber++;
            // Trim leading/trailing whitespace from the line manually
            size_t first = line.find_first_not_of(" \t\n\r");
            if (string::npos == first) {
                line = ""; // line is all whitespace
            } else {
                size_t last = line.find_last_not_of(" \t\n\r");
                line = line.substr(first, (last - first + 1));
            }


            if (line.empty()) continue; // Skip empty or whitespace-only lines

            stringstream ss(line); // Assuming stringstream is allowed
            string tempName, tempPrice, tempCategory, tempStockStr;
            int tempStock = 0;
            bool lineError = false;

            // Use getline with stringstream to parse CSV fields
            if (getline(ss, tempName, ',') &&
                getline(ss, tempPrice, ',') &&
                getline(ss, tempCategory, ',') &&
                getline(ss, tempStockStr))
            {
                // Check if there's any leftover data in the stringstream (more than 4 fields)
                string leftover;
                if (getline(ss, leftover)) {
                     cerr << "Warning: Skipping malformed line (too many fields) #" << lineNumber << " in " << filename << ": " << line << endl;
                     lineError = true;
                }

            } else {
                 cerr << "Warning: Skipping malformed line (not enough fields) #" << lineNumber << " in " << filename << ": " << line << endl;
                 lineError = true;
            }

            if (lineError) continue; // Skip to next line if parsing failed

            // Trim whitespace from parsed fields *after* extraction manually
            size_t first_name = tempName.find_first_not_of(" \t\n\r"); size_t last_name = tempName.find_last_not_of(" \t\n\r");
            if (string::npos != first_name) tempName = tempName.substr(first_name, (last_name - first_name + 1)); else tempName = "";
            size_t first_price = tempPrice.find_first_not_of(" \t\n\r"); size_t last_price = tempPrice.find_last_not_of(" \t\n\r");
            if (string::npos != first_price) tempPrice = tempPrice.substr(first_price, (last_price - first_price + 1)); else tempPrice = "";
            size_t first_category = tempCategory.find_first_not_of(" \t\n\r"); size_t last_category = tempCategory.find_last_not_of(" \t\n\r");
            if (string::npos != first_category) tempCategory = tempCategory.substr(first_category, (last_category - first_category + 1)); else tempCategory = "";
            size_t first_stock = tempStockStr.find_first_not_of(" \t\n\r"); size_t last_stock = tempStockStr.find_last_not_of(" \t\n\r");
            if (string::npos != first_stock) tempStockStr = tempStockStr.substr(first_stock, (last_stock - first_stock + 1)); else tempStockStr = "";


             // Replace semicolons back to commas manually
             manualReplace(tempName, ';', ',');
             manualReplace(tempCategory, ';', ',');


            // Validate and parse Stock (Assuming stol is allowed for string to long conversion)
            try {
                size_t stockPos;
                long stockLong = stol(tempStockStr, &stockPos); // Assuming stol is allowed
                 if (stockPos != tempStockStr.length()) { // Check if entire string was consumed
                     throw invalid_argument("Contains non-numeric characters or trailing data"); // Assuming invalid_argument allowed
                 }
                if (stockLong < 0) {
                     cerr << "Warning: Negative stock value '" << tempStockStr << "' for '" << tempName << "' on line #" << lineNumber << " in " << filename << ". Setting to 0.\n";
                     tempStock = 0;
                } else if (stockLong > numeric_limits<int>::max()) { // Assuming numeric_limits allowed
                     cerr << "Warning: Stock value '" << tempStockStr << "' too large for '" << tempName << "' on line #" << lineNumber << " in " << filename << ". Setting to max int.\n";
                     tempStock = numeric_limits<int>::max();
                }
                else {
                    tempStock = static_cast<int>(stockLong); // Cast to int
                }
            } catch (const invalid_argument& e) { // Assuming invalid_argument allowed
                cerr << "Warning: Invalid stock format '" << tempStockStr << "' for product '" << tempName << "' on line #" << lineNumber << " in " << filename << ". Setting stock to 0. Details: " << e.what() << endl;
                tempStock = 0; // Set stock to 0 on error
            } catch (const out_of_range& e) { // Assuming out_of_range allowed
                 cerr << "Warning: Stock value '" << tempStockStr << "' out of range for '" << tempName << "' on line #" << lineNumber << " in " << filename << ". Setting stock to max int. Details: " << e.what() << endl;
                 tempStock = numeric_limits<int>::max(); // Set stock to max on error
            } catch (...) {
                 cerr << "Warning: Unexpected error parsing stock '" << tempStockStr << "' for '" << tempName << "' on line #" << lineNumber << " in " << filename << ". Setting stock to 0.\n";
                 tempStock = 0; // Set stock to 0 on other errors
            }


            // Validate Price
            if (!isValidPrice(tempPrice)) {
                // cerr << "Warning: Invalid price format '" << tempPrice << "' for '" << tempName << "' on line #" << lineNumber << " in " << filename << ". Skipping product.\n"; // --- COMMENTED OUT THIS LINE ---
                continue; // Skip product if price is invalid
            }
             // Validate Category using manual search
            bool validCategory = false;
            string actualCategory = "Uncategorized"; // Default to Uncategorized if not found
            string lowerTempCategory = manualToLower(tempCategory);

            for (int i = 0; i < categoryCount; ++i) {
                if (manualToLower(categoryArray[i]) == lowerTempCategory) {
                    validCategory = true;
                    actualCategory = categoryArray[i]; // Use stored case from categories list
                    break;
                }
            }
             if (!validCategory) {
                 cerr << "Warning: Invalid category '" << tempCategory << "' for '" << tempName << "' on line #" << lineNumber << " in " << filename << ". Assigning to 'Uncategorized'.\n";
             }
            tempCategory = actualCategory; // Use validated/defaulted category name


            // Add product to the linked list
            addProductNode(tempName, tempPrice, tempCategory, tempStock);
        }
        inFile.close();
    }
// ... (rest of the ProductList class and global functions remain the same) ...

    // Convert Linked List to dynamic array for sorting and display
    Product* convertListToArray(int& outSize) const {
        outSize = size;
        if (size == 0) {
            return nullptr;
        }
        Product* arr = new Product[size]; // Allocate memory
        ProductNode* current = head;
        int index = 0;
        while (current != nullptr && index < size) { // Added index check for safety
            arr[index++] = current->data; // Copy Product data
            current = current->next;
        }
        return arr; // Caller must delete[] this array
    }

    // Insertion Sort for products (MARKED AS CONST, operates on raw array)
    void insertionSort(Product* arr, int n, bool ascending, string sortBy) const {
        if (arr == nullptr || n <= 1) return; // Add checks

        sortBy = manualToLower(sortBy);

        for (int i = 1; i < n; i++) {
            Product key = arr[i]; // Copy the whole struct
            int j = i - 1;

            if (sortBy == "category") {
                while (j >= 0) {
                    bool comparison;
                    if (ascending) {
                         comparison = (manualToLower(arr[j].category) > manualToLower(key.category));
                    } else {
                         comparison = (manualToLower(arr[j].category) < manualToLower(key.category));
                    }
                    if (comparison) {
                         arr[j + 1] = arr[j];
                         j--;
                    } else {
                         break;
                    }
                }
            } else if (sortBy == "name") {
                 while (j >= 0) {
                    bool comparison;
                    if (ascending) {
                         comparison = (manualToLower(arr[j].name) > manualToLower(key.name));
                    } else {
                         comparison = (manualToLower(arr[j].name) < manualToLower(key.name));
                    }
                    if (comparison) {
                         arr[j + 1] = arr[j];
                         j--;
                    } else {
                         break;
                    }
                }
            }
             // Note: Price sorting is omitted as per implied requirement

            arr[j + 1] = key; // Place the copied struct
        }
    }

    // Display product details from an array
    void displayProductById(int id, const Product* arr, int arrSize) const { // Made const, accepts array
        if (arr == nullptr || id < 1 || id > arrSize) {
            cerr << "Internal Error: Invalid product ID or array requested for display.\n";
            return;
        }
        const Product& product = arr[id - 1];
        cout << "\n--- Product Details ---\n";
        cout << "Product Name: " << product.name << endl;
        cout << "Category: " << product.category << endl;
        cout << "Price: " << product.price << endl; // Price is already in "RM X.XX" format
        cout << "Stock: " << product.stock << endl;
        cout << "-----------------------\n";
    }

    // Display product names by category from the linked list
    void displayProductNamesByCategory(const string& category) const { // Made const
        // Manually filter products into a temporary dynamic array
        Product* tempFilteredArray = nullptr;
        int tempFilteredCount = 0;
        int tempFilteredCapacity = 0;

        ProductNode* current = head;
        string lowerCategory = manualToLower(category);

        while(current != nullptr) {
            if (manualToLower(current->data.category) == lowerCategory) {
                 // Add to temporary array, resize if needed manually
                 if (tempFilteredCount == tempFilteredCapacity) {
                     int newCapacity = (tempFilteredCapacity == 0) ? 10 : tempFilteredCapacity * 2; // Arbitrary growth factor
                     Product* newArray = new Product[newCapacity]; // Allocate new memory
                     // Manually copy existing elements
                     for (int i = 0; i < tempFilteredCount; ++i) {
                         newArray[i] = tempFilteredArray[i];
                     }
                     delete[] tempFilteredArray; // Free old memory
                     tempFilteredArray = newArray;
                     tempFilteredCapacity = newCapacity;
                 }
                tempFilteredArray[tempFilteredCount++] = current->data; // Copy Product data
            }
            current = current->next;
        }

        if (tempFilteredCount == 0) {
            cout << "No products found in category: '" + category + "'.\n"; // String concatenation
            delete[] tempFilteredArray; // Clean up temp array memory
            return;
        }

        // Sort filtered products by name using insertion sort on the temporary array
        insertionSort(tempFilteredArray, tempFilteredCount, true, "name"); // Valid call now

        // Display product names from the sorted temporary array
         cout << "=== Products in category '" + category + "' (sorted by name) ===\n"; // String concatenation
        for (int i = 0; i < tempFilteredCount; ++i) {
             cout << (i + 1) << ". " << tempFilteredArray[i].name << endl;
        }
        cout << "---------------------------------------------\n";

        delete[] tempFilteredArray; // Clean up temporary array memory after use
    }


    // Sort and display categories (no longer needs external sorting call)
    void sortAndDisplayCategories() {
        system("cls");
        if (categoryCount == 0) {
            cout << "No categories to display.\n";
            cout << "\nPress any key to return to menu...";
            _getch();
            return;
        }
        // Category array is kept sorted by load and add operations (calls insertionSortCategories internally)

        cout << "=== Product Categories (sorted alphabetically) ===\n";
        for (int i = 0; i < categoryCount; i++) {
            cout << (i + 1) << ". " << categoryArray[i] << endl; // Use categoryArray
        }
        cout << "------------------------------------------------\n";
        cout << "Enter category ID (1-" << categoryCount << ") to view products, or 0 to return: "; // Use categoryCount

        string input;
        int id;
        // cin.ignore is handled in displayMenu before calling this function
        getline(cin, input);
        try {
            size_t pos;
            id = stoi(input, &pos);
             string remaining = input.substr(pos);
             remaining.erase(0, remaining.find_first_not_of(" \t\n\r"));
             if (!remaining.empty()) {
                 throw invalid_argument("Contains non-numeric characters");
            }
        } catch (...) { // Catch invalid_argument, out_of_range etc.
            cout << "Invalid input. Please enter a number.\n";
            // _getch handled by displayMenu
            return;
        }

        if (id == 0) {
             cout << "Returning to menu.\n";
             Sleep(500);
        } else if (id >= 1 && id <= categoryCount) {
            system("cls"); // Clear screen before displaying product names for the category
            displayProductNamesByCategory(categoryArray[id - 1]); // Pass category name from array (Valid call now)
             cout << endl;
            // _getch handled by displayMenu
        } else {
            cout << "Invalid category ID.\n";
             // _getch handled by displayMenu
        }
    }

    // Sort and display product names with IDs
    void sortAndDisplayProductNames() {
        system("cls");
        int arrSize;
        Product* arr = convertListToArray(arrSize); // Get products as dynamic array

        if (arr == nullptr || arrSize == 0) {
            cout << "No products to display.\n";
            // No array to delete if size is 0 or arr is nullptr
            cout << "\nPress any key to return to menu...";
            _getch();
            return;
        }
        // Sort products by name using insertion sort on the array
        insertionSort(arr, arrSize, true, "name"); // Valid call now

        cout << "=== Products (sorted by name) ===\n";
        for (int i = 0; i < arrSize; i++) { // Iterate through array
            cout << (i + 1) << ". " << arr[i].name << endl;
        }
         cout << "-----------------------------------\n";
        cout << "Enter product ID (1-" << arrSize << ") to view details, or 0 to return: ";
        string input;
        int id;
        // cin.ignore is handled in displayMenu before calling this function
        getline(cin, input);
        try {
             size_t pos;
            id = stoi(input, &pos);
             string remaining = input.substr(pos);
             remaining.erase(0, remaining.find_first_not_of(" \t\n\r"));
             if (!remaining.empty()) {
                 throw invalid_argument("Contains non-numeric characters");
             }
        } catch (...) { // Catch invalid_argument, out_of_range etc.
            cout << "Invalid input. Please enter a number.\n";
            delete[] arr; // Clean up array memory before returning
            // _getch handled by displayMenu
            return; // Exit function after invalid input
        }

        if (id == 0) {
             cout << "Returning to menu.\n";
             Sleep(500);
        } else if (id >= 1 && id <= arrSize) {
            system("cls"); // Clear screen for the second page
            // Display details for the selected product from the sorted array
             displayProductById(id, arr, arrSize); // Valid call now
            // _getch handled by displayMenu
        } else {
            cout << "Invalid product ID.\n";
            // _getch handled by displayMenu
        }

        delete[] arr; // Clean up array memory after use
    }

}; // End of ProductList class

// --- Global Helper Functions (Outside the class) ---
// Assuming these standard C++ headers/functions are allowed for basic utilities

string getPassword() {
    string password;
    char ch;
    cout << flush;
    // Read characters until Enter (13) or Newline (10)
    while ((ch = _getch()) != 13 && ch != 10) {
        if (ch == 8) { // Backspace key
            if (!password.empty()) {
                password.pop_back();
                // Move cursor back, print space, move cursor back again
                cout << "\b \b" << flush;
            }
        } else if (ch >= 32 && ch <= 126) { // Printable characters (ASCII)
             password += ch;
             cout << '*' << flush;
        }
         // Ignore other control characters like arrow keys, etc.
    }
    cout << endl; // Print a newline after the user presses Enter
    return password;
}

// Helper to get console window dimensions (Assuming Windows.h functions allowed)
void getConsoleSize(int& width, int& height) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    } else {
        width = 80; // Default if unable to get info
        height = 25; // Default
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
    int vertical_padding = (console_height > 1) ? (console_height / 2) - 1 : 0; // Center vertically over 1 line

    // Move cursor down for vertical padding
    for (int i = 0; i < vertical_padding; ++i) {
        cout << "\n";
    }

    // Print leading padding and prefix
    cout << string(horizontal_padding, ' ') + prefix << flush; // String concatenation

    int total_steps = barWidth;
    int duration_ms = 2000; // Total animation duration (2 seconds)
    int sleep_ms_per_step = (total_steps > 0) ? duration_ms / total_steps : 0;
    if (sleep_ms_per_step == 0 && total_steps > 0) sleep_ms_per_step = 1; // Ensure some delay if duration is short

    for (int i = 0; i <= total_steps; ++i) { // Include 100% step
        // Print the filled part
        cout << string(i, '\xDB');
        // Print the empty part
        cout << string(total_steps - i, '\xB0');
        // Print the suffix
        cout << suffix << flush;

        // Move cursor back to the start of the bar
        cout << "\r" + string(horizontal_padding, ' ') + prefix << flush; // String concatenation

        if (i < total_steps) { // Don't sleep after the last step
             Sleep(sleep_ms_per_step);
        }
    }
    // After loop, print the full bar one last time and add a newline
     cout << string(total_steps, '\xDB') + suffix << endl; // String concatenation
}


bool authenticateUser(const string& username, const string& password, const string& filename) {
    ifstream file(filename);
    if (!file) {
        // This case should ideally be handled before calling, but check anyway
        cerr << "Error: Staff file not found or cannot be opened: " + filename + "\n"; // String concatenation
        return false;
    }

    string line;
    while (getline(file, line)) {
        // Trim leading/trailing whitespace manually
        size_t first = line.find_first_not_of(" \t\n\r");
        if (string::npos == first) {
            line = ""; // line is all whitespace
        } else {
            size_t last = line.find_last_not_of(" \t\n\r");
            line = line.substr(first, (last - first + 1));
        }

        if (line.empty()) continue;

        size_t commaPos = manualFind(line, ','); // Use manualFind
        if (commaPos == string::npos) {
            cerr << "Warning: Skipping malformed line in " << filename << " (no comma): " << line << endl;
            continue;
        }

        string storedUsername = line.substr(0, commaPos);
        string storedPassword = line.substr(commaPos + 1);

        // Trim username and password manually
        size_t first_u = storedUsername.find_first_not_of(" \t\n\r"); size_t last_u = storedUsername.find_last_not_of(" \t\n\r");
        if (string::npos != first_u) storedUsername = storedUsername.substr(first_u, (last_u - first_u + 1)); else storedUsername = "";
        size_t first_p = storedPassword.find_first_not_of(" \t\n\r"); size_t last_p = storedPassword.find_last_not_of(" \t\n\r");
        if (string::npos != first_p) storedPassword = storedPassword.substr(first_p, (last_p - first_p + 1)); else storedPassword = "";


        if (username == storedUsername && password == storedPassword) {
            file.close();
            return true; // Match found
        }
    }
    file.close();
    return false; // No match found
}

// Admin login function
bool adminLogin(const string& staffFilename) {
    system("cls");
    string username, password;
    int attempts = 3;

    cout << "=== Admin Login ===\n";

    while (attempts > 0) {
        cout << "Enter username: ";
        getline(cin, username); // Use getline for username too to handle spaces if needed
        username.erase(0, username.find_first_not_of(" \t\n\r"));
        username.erase(username.find_last_not_of(" \t\n\r") + 1);

         if (username.empty()) {
             cout << "Username cannot be empty.\n";
             attempts--;
             if (attempts > 0) {
                 cout << "Attempts remaining: " << attempts << "\n";
                 Sleep(1500);
                 system("cls");
                 cout << "=== Admin Login ===\n";
             }
             continue; // Ask for username/password again
         }

        cout << "Enter password: ";
        password = getPassword(); // getPassword() already handles endl

         if (password.empty()) {
             cout << "Password cannot be empty.\n";
             attempts--;
             if (attempts > 0) {
                 cout << "Attempts remaining: " << attempts << "\n";
                 Sleep(1500);
                 system("cls");
                 cout << "=== Admin Login ===\n";
             }
             continue; // Ask for username/password again
         }

        if (authenticateUser(username, password, staffFilename)) {
            return true;
        } else {
            attempts--;
            cout << "Invalid credentials.\n";
            if (attempts > 0) {
                 cout << "Attempts remaining: " << attempts << "\n";
                 Sleep(1500);
                 system("cls");
                 cout << "=== Admin Login ===\n";
            }
        }
    }
    cout << "Too many failed attempts. Exiting.\n";
    Sleep(2000);
    return false; // Login failed after attempts
}

// Admin register function
void adminRegister(const string& staffFilename) {
    system("cls");
    string username, password;

    cout << "=== Admin Registration ===\n";

     // Check if the staff file exists, if not, this is the first user, no need to check for existing
    bool fileExistsInitially = ifstream(staffFilename).good();


    cout << "Enter desired username: ";
    getline(cin, username);
    username.erase(0, username.find_first_not_of(" \t\n\r"));
    username.erase(username.find_last_not_of(" \t\n\r") + 1);

    if (username.empty()) {
        cout << "Username cannot be empty.\n";
        cout << "\nPress any key to return...";
        _getch();
        return;
    }

    // Check for comma in username (CSV delimiter)
    if (manualFind(username, ',') != string::npos) {
        cout << "Username cannot contain commas.\n";
         cout << "\nPress any key to return...";
        _getch();
        return;
    }

    // Check if username already exists IF the file existed initially
    if (fileExistsInitially && manualFileContains(staffFilename, username)) {
        cout << "Username '" << username << "' already exists (case-insensitive match).\n";
         cout << "\nPress any key to return...";
        _getch();
        return;
    }

    cout << "Enter desired password: ";
    password = getPassword(); // getPassword() handles newline

    if (password.empty()) {
        cout << "Password cannot be empty.\n";
         cout << "\nPress any key to return...";
        _getch();
        return;
    }
     // Check for comma in password
     if (manualFind(password, ',') != string::npos) {
         cout << "Password cannot contain commas.\n";
          cout << "\nPress any key to return...";
         _getch();
         return;
     }


    // Append user to file
    ofstream outFile(staffFilename, ios::app); // Open in append mode
    if (!outFile) {
        cerr << "Error: Could not open staff file for writing: " << staffFilename << endl;
        cout << "Registration failed due to file error.\n";
    } else {
        outFile << username << "," << password << endl;
        outFile.close();
        cout << "User '" << username << "' registered successfully.\n";
    }

    cout << "\nPress any key to return...";
    _getch();
}


// Main menu
void displayMenu(ProductList& list, const string& categoriesFilename, const string& productsFilename) {
    while (true) {
        system("cls");
        cout << "\n=== Admin Menu ===\n";
        cout << "1. Sort by Category\n"; // Combined sorting and viewing
        cout << "2. Sort By Product\n";         // Combined sorting and viewing
        cout << "3. Add Category\n";
        cout << "4. Add Product\n";
        cout << "5. Delete Category\n";
        cout << "6. Delete Product\n";
        cout << "7. Logout\n"; // Changed Exit to Logout
        cout << "Enter choice (1-7): ";
        string input;
        int choice;

        getline(cin, input);

        try {
            size_t pos;
            choice = stoi(input, &pos);
             string remaining = input.substr(pos);
             remaining.erase(0, remaining.find_first_not_of(" \t\n\r"));
             if (!remaining.empty()) {
                 throw invalid_argument("Contains non-numeric characters");
             }

        } catch (const invalid_argument& e) {
            cout << "Invalid input. Please enter a number from 1 to 7.\n";
            Sleep(1500);
            continue;
        } catch (const out_of_range& e) {
             cout << "Input number is out of valid range. Please enter a number from 1 to 7.\n";
             Sleep(1500);
            continue;
        } catch (...) { // Catch any other unexpected errors during conversion
             cout << "An unexpected error occurred while reading your choice.\n";
             Sleep(1500);
            continue;
        }

        // Now handle the valid integer choice
        switch (choice) {
            case 1: // View Categories & Products by Category
                list.sortAndDisplayCategories();
                cout << "\nPress any key to return to menu...";
                _getch();
                break;
            case 2: // View All Products & Details
                list.sortAndDisplayProductNames();
                cout << "\nPress any key to return to menu...";
                _getch();
                break;
            case 3: // Add Category
                list.addCategory(categoriesFilename);
                cout << "\nPress any key to return to menu...";
                _getch();
                break;
            case 4: // Add Product
                list.addProduct(productsFilename);
                cout << "\nPress any key to return to menu...";
                _getch();
                break;
            case 5: // Delete Category
                list.deleteCategory(categoriesFilename, productsFilename);
                cout << "\nPress any key to return to menu...";
                _getch();
                break;
            case 6: // Delete Product
                list.deleteProduct(productsFilename);
                cout << "\nPress any key to return to menu...";
                _getch();
                break;
            case 7: // Logout
                cout << "Logging out.\n";
                Sleep(1000);
                return; // Exit displayMenu, returning to the initial menu loop in main
            default:
                cout << "Invalid choice. Please enter a number from 1 to 7.\n";
                Sleep(1500);
                // Loop continues, clearing the screen and showing the menu again
        }
    }
}

int main() {
    // system("chcp 65001 > nul"); // Uncomment if you need UTF-8 support on Windows console

    ProductList list; // Create ProductList instance
    string categoriesFilename = "D:\\TDS4223\\Project\\categories.txt";
    string productsFilename = "D:\\TDS4223\\Project\\flowers.txt";
    string staffFilename = "D:\\TDS4223\\Project\\staff.txt"; // Filename for staff credentials

    // --- Initial Menu Loop (Register or Login) ---
    bool loggedIn = false;
    while (!loggedIn) {
        system("cls");
        cout << "\n=== Welcome ===\n";
        cout << "1. Admin Register\n";
        cout << "2. Admin Login\n";
        cout << "3. Exit Program\n"; // Added explicit exit option
        cout << "Enter choice (1-3): ";
        string input;
        int initialChoice;

        getline(cin, input);

        try {
            size_t pos;
            initialChoice = stoi(input, &pos);
             string remaining = input.substr(pos);
             remaining.erase(0, remaining.find_first_not_of(" \t\n\r"));
             if (!remaining.empty()) {
                 throw invalid_argument("Contains non-numeric characters");
             }

        } catch (...) {
            cout << "Invalid input. Please enter a number from 1 to 3.\n";
            Sleep(1500);
            continue; // Loop back to initial menu
        }

        switch (initialChoice) {
            case 1: // Register
                adminRegister(staffFilename); // Calls the registration function
                // After registration, loop back to the initial menu
                break;
            case 2: // Login
                // Attempt to load categories before login, as loadCategories handles file creation
                try {
                     cout << "Loading categories for validation...\n";
                     list.loadCategories(categoriesFilename);
                     Sleep(500);
                     if (adminLogin(staffFilename)) { // Call the login function
                         loggedIn = true; // Set flag if login is successful
                     } else {
                         // adminLogin already prints messages and handles attempts/failure exit
                         // If adminLogin returns false, it means attempts were exhausted.
                         // The function exits main() in that case, so no need for break here.
                         return 1; // Exit main if login failed after attempts
                     }
                } catch (const runtime_error& e) {
                     cerr << "Fatal Error during category loading: " << e.what() << endl;
                     cout << "\nPress any key to exit...";
                     _getch();
                     return 1; // Exit program if categories cannot be loaded before login
                }
                break; // Break switch case (if login was successful)
            case 3: // Exit Program
                cout << "Exiting program.\n";
                Sleep(1000);
                return 0; // Exit main
            default:
                cout << "Invalid choice. Please enter 1, 2, or 3.\n";
                Sleep(1500);
                // Loop back to initial menu
        }
    }
    // --- End Initial Menu Loop (Logged in) ---


    // If we reach here, login was successful
    try {
        // We loaded categories before login. Now load products.
        system("cls"); // Clear the login screen
        cout << "Login successful!\n";
        showLoading(); // Show centered loading bar

        list.loadFromFile(productsFilename);
        Sleep(500); // Short pause after loading products before menu appears

        // Display main admin menu
        displayMenu(list, categoriesFilename, productsFilename); // Pass filenames

    } catch (const runtime_error& e) {
        cerr << "Fatal Runtime Error: " << e.what() << endl;
        cout << "\nPress any key to exit...";
        _getch();
        return 1;
    } catch (const exception& e) {
         cerr << "An unexpected standard error occurred: " << e.what() << endl;
        cout << "\nPress any key to exit...";
        _getch();
         return 1;
    } catch (...) { // Catch any unknown exception
         cerr << "An unknown fatal error occurred.\n";
        cout << "\nPress any key to exit...";
        _getch();
         return 1;
    }

    return 0;
}
