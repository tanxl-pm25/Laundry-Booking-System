#include "CleanSpark.h"

// UTILITY & DISPLAY FUNCTIONS
// Helper functions for UI, input validation, and general operations
//=========================================================

// Display a formatted main header section
void printHeader(string title) {
    cout << "\n=========================================================" << endl;
    cout << "\t" << title << endl;
    cout << "=========================================================" << endl;
}

// Display a formatted sub-header section
void printSubHeader(string subTitle) {
    cout << "\n================ " << subTitle << " ================" << endl;
}

// Clear console screen
void clearScreen() {
    system("cls");
}

// Get user input as a string
string getChoice() {
    string choice;
    cout << "\nEnter your choice: ";
    getline(cin, choice);
    return choice;
}

// Validate user choice is within acceptable range [min, max]
bool isValidChoice(string choice, char min, char max) {
    if (choice.length() != 1) {
        cout << "Invalid option! Please enter number " << min << "-" << max << " only.\n";
        return false;
    }
    if (choice[0] < min || choice[0] > max) {
        cout << "Invalid option! Please enter number " << min << "-" << max << " only.\n";
        return false;
    }
    return true;
}

// Get Yes/No response from user (returns 'Y' or 'N')
char getYesNo(string message) {
    string input;

    while (true) {
        cout << message;
        getline(cin, input);

        if (input.length() == 1) {
            char answer = toupper(input[0]);
            if (answer == 'Y' || answer == 'N')
                return answer;
        }
        cout << "Invalid input! Please enter 'Y' OR 'N' only.\n";
    }
}

// Pause execution and wait for user to press Enter
void pauseScreen() {
    string temp;
    cout << "\nPress Enter to continue...";
    getline(cin, temp);
    clearScreen();
}

//=========================================================
#include "CleanSpark.h"

// MAIN MENU
//=========================================================

// Display main menu options
void displayMainMenu() {
    printHeader("WELCOME TO CLEANSPARK LAUNDRY SERVICE");
    cout << "\t      Service Management System" << endl;
    cout << "---------------------------------------------------------" << endl;
    cout << "1. Customer Management" << endl;
    cout << "2. Laundry Booking" << endl;
    cout << "3. Order Status (Pickup and Collection)" << endl;
    cout << "4. Payment" << endl;
    cout << "5. Reports" << endl;
    cout << "6. Exit" << endl;
    cout << "---------------------------------------------------------";
}

//=========================================================
