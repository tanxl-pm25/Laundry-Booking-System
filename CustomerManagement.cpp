#include "CleanSpark.h"

// CUSTOMER MANAGEMENT
// Functions for managing customer profiles, records, and history
//=========================================================

// Helper: Generate unique customer ID based on existing customers
// Returns ID format: Cnnnn (e.g., C0001, C0002)
string generateCustomerID(const vector<Customer>& customers) {
    int maxID = 0;      //Store the largest cust num
    string id = "C";

    //Check every customer in vector
    for (const Customer& c : customers) { //declare as const to avoid modification on ID

        //Remove the first char ('C') and convert the rest to int
        int number = stoi(c.customerID.substr(1));

        //update the largest cust num
        if (number > maxID)
            maxID = number;
    }

    maxID++;   //generate next cust num

    id += string(4 - to_string(maxID).length(), '0');   //make the ID 4 digit by adding zero
    id += to_string(maxID); //append the customer number

    return id;

}

// Format customer name: proper capitalization and handle extra whitespace
// Example: "ali  AHMAD" -> "Ali Ahmad"
string formatName(string name) {
    stringstream ss(name);     // Extract words and ignore extra whitespace
    string word;
    string result = "";

    while (ss >> word) {
        // Capitalize first letter of the word
        word[0] = toupper(word[0]);

        // Convert remaining letters to lowercase
        for (int i = 1; i < word.length(); i++) {
            word[i] = tolower(word[i]);
        }

        // Append formatted word to result with space
        result += word + " ";
    }

    // Remove extra space at end
    if (!result.empty()) {
        result.pop_back();
    }

    return result;
}

// Convert string to uppercase
string toUpperCase(string text) {
    for (char& c : text) {
        c = toupper(c);
    }
    return text;
}

// Case-insensitive ID comparison
bool IDMatch(string input, string storedID) {
    return toUpperCase(input) == toUpperCase(storedID);
}

// Find customer by ID, phone, or email
// Returns: index if found, -1 if not found
int findCustomerIndex(const vector<Customer>& customers, const string& keyword) {
    for (int i = 0; i < customers.size(); i++) {
        if (IDMatch(keyword, customers[i].customerID))
            return i;
        if (customers[i].phone == keyword)
            return i;
        if (customers[i].email == keyword)
            return i;
    }
    return -1;  // Customer not found
}

// Display customer management menu
void customerManagementMenu() {
    printSubHeader("CUSTOMER MANAGEMENT");
    cout << "1. Add New Customer\n";
    cout << "2. Search Customer\n";
    cout << "3. Edit Customer\n";
    cout << "4. Delete Customer\n";
    cout << "5. View Customer Laundry History\n";
    cout << "6. Back\n";
}

// Display customer details in formatted table
void displayCustomer(const Customer& c) {
    cout << "\n========== CUSTOMER DETAILS ==========\n";
    cout << "Customer ID : " << c.customerID << endl;
    cout << "Name        : " << c.name << endl;
    cout << "Phone       : " << c.phone << endl;
    cout << "Email       : " << c.email << endl;
    cout << "Member      : " << (c.isMember ? "Yes" : "No") << endl;
    cout << "Points      : " << c.points << endl;
}

// INPUT & VALIDATION FUNCTIONS

// Get and validate customer name input
// Requirements: 3-50 characters, alphabets and spaces only
string getName() {
    string name;
    bool valid;

    do {
        valid = true;
        cout << "Name\t\t: ";
        getline(cin, name);

        if (name.empty()) {
            valid = false;
            cout << "Invalid input! Please enter your name.\n";
            continue;
        }

        if (name.length() < 3 || name.length() > 50) {
            valid = false;
            cout << "Invalid length! Name must be between 3 and 50 characters.\n";
            continue;
        }

        // Only allow alphabet and space characters
        for (char c : name) {
            if (!isalpha(c) && c != ' ') {
                valid = false;
                cout << "Invalid name! Please enter alphabet only.\n";
                break;
            }
        }

    } while (!valid);

    // Format name before storing (capitalize first letter)
    return formatName(name);
}

// Check if phone number already exists in customer database
bool phoneExists(const vector<Customer>& customers, string phone) {
    for (const Customer& c : customers) {
        if (c.phone == phone) {
            return true;
        }
    }
    return false;
}

// Get and validate phone number input
// Requirements: 10-11 digits, must start with '01', no duplicates
string getPhone(const vector<Customer>& customers) {
    string phone;
    bool valid;

    do {
        valid = true;
        cout << "Phone Number\t: ";
        getline(cin, phone);

        if (phone.empty()) {
            valid = false;
            cout << "Invalid input! Phone number cannot be empty.\n";
            continue;
        }

        // Only allow numeric characters
        for (char c : phone) {
            if (!isdigit(c)) {
                valid = false;
                cout << "Invalid phone number! The phone number must be numeric only.\n";
                break;
            }
        }

        // Check phone length (10-11 digits)
        if (phone.length() < 10 || phone.length() > 11) {
            valid = false;
            cout << "Invalid phone number! The phone number must be exactly 10 or 11 digits long.\n";
        }

        // Phone must start with "01"
        if (phone.substr(0, 2) != "01") {
            valid = false;
            cout << "Invalid phone number! The phone number should start with '01'.\n";
        }

        // Check for duplicate phone number
        if (phoneExists(customers, phone)) {
            valid = false;
            cout << "Phone number already exists. Please try again.\n";
        }

    } while (!valid);

    return phone;
}

// Check if email already exists in customer database
bool emailExists(const vector<Customer>& customers, string email) {
    for (const Customer& c : customers) {
        if (c.email == email) {
            return true;
        }
    }
    return false;
}

// Validate email format using regex pattern
bool validEmail(string email) {
    regex pattern(R"((\w+)(\.\w+)*@(\w+\.)+\w+)");
    return regex_match(email, pattern);
}

// Get and validate email input
// Requirements: Valid email format, no duplicates
string getEmail(const vector<Customer>& customers) {
    string email;
    bool valid;

    do {
        valid = true;
        cout << "Email\t\t: ";
        getline(cin, email);

        // Validate email format
        valid = validEmail(email);
        if (!valid) {
            cout << "Please enter a valid email address (e.g., name@example.com).\n";
        }

        // Check for duplicate email
        if (emailExists(customers, email)) {
            valid = false;
            cout << "Email already exists. Please check and try again.\n";
        }

    } while (!valid);

    return email;
}

string generateReceiptID()
{
    static int receiptNumber = 1001;

    string receiptID = "R" + to_string(receiptNumber);

    receiptNumber++;

    return receiptID;
}
bool processMembershipPayment(double& amountPaid, double& change)
{
    const double membershipFee = 20.00;

    while (true)
    {
        cout << "\nMembership Fee : RM "
            << fixed << setprecision(2)
            << membershipFee << endl;

        cout << "Amount Paid    : RM ";
        cin >> amountPaid;

        // Check invalid input
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(1000, '\n');

            cout << ">>> Invalid amount! Please enter a valid number.\n";
            continue;
        }

        cin.ignore(1000, '\n');

        // Check insufficient payment
        if (amountPaid < membershipFee)
        {
            cout << ">>> Insufficient payment!\n";
            cout << ">>> Please enter RM "
                << fixed << setprecision(2)
                << membershipFee << " or more.\n";

            continue;
        }

        break;
    }

    change = amountPaid - membershipFee;
    return true;
}

//=========================================================
// CUSTOMER CRUD OPERATIONS
// Functions for Add, Search, Edit, Delete, View customer data
//=========================================================

// Add new customer to file with optional membership
void addCustomer(vector<Customer>& customers) {
    Customer newCust;

    printSubHeader("ADD CUSTOMER");
    newCust.customerID = generateCustomerID(customers);
    newCust.name = getName();
    newCust.phone = getPhone(customers);
    newCust.email = getEmail(customers);

    cout << "\n>>> Premium Member Benefits:\n";
    cout << ">>> 10% discount\n";
    cout << ">>> RM1 = 1 point\n";
    cout << ">>> 100 points = RM5\n";

    char option = getYesNo("\nRegister as Premium Member (Y/N): ");

    if (option == 'Y') {
        cout << "Membership Fee\t: RM 20.00\n";
        cout << "\nProceed to payment...\n";

        // MEMBERSHIP PAYMENT
        double amountPaid;
        double change;

        processMembershipPayment(amountPaid, change);

        // payment succeccful
        cout << "\n>>> Membership Payment Successful!\n";
        cout << "\nAmount Paid : RM " << fixed << setprecision(2) << amountPaid << endl;
        cout << "Change      : RM " << change << endl;
        cout << "\nPress Enter to view membership receipt...";
        cin.get();

        clearScreen();
        // MEMBERSHIP INFORMATION
        newCust.isMember = true;
        newCust.points = 50;

        // MEMBERSHIP RECEIPT
        string receiptID = generateReceiptID();
        double membershipFee = 20.00;

        cout << "\n====================================================\n";
        cout << "              CLEANSPARK LAUNDRY\n";
        cout << "              PREMIUM MEMBERSHIP\n";
        cout << "====================================================\n";

        cout << left << setw(18) << "Receipt No" << ": " << receiptID << endl;
        cout << left << setw(18) << "Customer Name" << ": " << newCust.name << " (" << newCust.customerID << ")\n";
        cout << left << setw(18) << "Payment Type" << ": Premium Membership\n";
        cout << "----------------------------------------------------\n";

        cout << left << setw(18) << "Membership Fee" << ": RM " << setw(8)
            << membershipFee << endl;
        cout << left << setw(18) << "Amount Paid" << ": RM " << setw(8)
            << amountPaid << endl;
        cout << left << setw(18) << "Change" << ": RM " << setw(8)
            << change << endl;

        cout << "----------------------------------------------------\n";

        cout << left << setw(18) << "Membership" << ": Premium\n";
        cout << left << setw(18) << "Welcome Bonus" << ": + 50 Points\n";
        cout << left << setw(18) << "Points Balance" << ": " << newCust.points << " pts\n";
        cout << "====================================================\n";
    }
    else {
        newCust.isMember = false;
        newCust.points = 0;
    }

    customers.push_back(newCust);  // Add new customer to database
    cout << "\nCustomer added successfully!\n";
    displayCustomer(newCust);
    pauseScreen();
}

// Search customer by ID, phone, or email
void searchCustomer(const vector<Customer>& customers) {
    string keyword;

    printSubHeader("SEARCH CUSTOMER");
    cout << "\nEnter Customer ID/Phone/Email to search: ";
    getline(cin, keyword);

    int index = findCustomerIndex(customers, keyword);

    if (index == -1) {
        cout << ">>> Customer not found.\n\n";
        pauseScreen();
        return;
    }
    else {
        Customer c = customers[index];
        cout << ">>> Customer found.\n\n";

        //display search result if exists
        cout << "Customer ID\t:" << c.customerID << endl;
        cout << "Name\t\t:" << c.name << endl;
        cout << "Phone\t\t:" << c.phone << endl;
        cout << "Email\t\t:" << c.email << endl;
        cout << "Member Status\t:";
        if (c.isMember)
            cout << "Premium\n";
        else
            cout << "Normal\n";
        cout << "Points\t\t:" << c.points << endl;
    }

    pauseScreen();
}

// Edit existing customer information (name, phone, email)
void editCustomer(vector<Customer>& customers) {
    string keyword;

    printSubHeader("EDIT CUSTOMER");
    cout << "\nEnter Customer ID/Phone/Email to edit: ";
    getline(cin, keyword);

    int index = findCustomerIndex(customers, keyword);

    if (index == -1) {
        cout << ">>> Customer not found.\n\n";
        return;
    }
    else {
        cout << ">>> Customer Found: " << customers[index].name << " (" << customers[index].phone << ")\n";
    }

    string choice;
    do {
        cout << "\nWhat do you want to update?\n";
        cout << "1. Name\n";
        cout << "2. Phone Number\n";
        cout << "3. Email\n";
        cout << "4. Cancel\n";

        choice = getChoice();
        if (!isValidChoice(choice, '1', '4')) {
            continue;
        }

        switch (choice[0]) {
        case '1':
            cout << "Current Name\t: " << customers[index].name << endl;
            customers[index].name = getName();
            cout << ">>> Name updated successfully!\n";
            break;
        case '2':
            cout << "Current Phone\t: " << customers[index].phone << endl;
            customers[index].phone = getPhone(customers);
            cout << ">>> Phone updated successfully!\n";
            break;
        case '3':
            cout << "Current Email\t: " << customers[index].email << endl;
            customers[index].email = getEmail(customers);
            cout << ">>> Email updated successfully!\n";
            break;
        case '4':
            clearScreen();
            break;
        }

    } while (choice != "4");
}

// Delete customer from database
void deleteCustomer(vector<Customer>& customers) {
    string keyword;

    printSubHeader("DELETE CUSTOMER");
    cout << "\nEnter Customer ID/Phone/Email to search: ";
    getline(cin, keyword);

    if (keyword == "0")
        return;

    int index = findCustomerIndex(customers, keyword);
    if (index == -1) {
        cout << ">>> Customer not found.\n\n";
    }
    else {
        cout << ">>> Customer Found: " << customers[index].name << " (" << customers[index].phone << ")\n";
        char option = getYesNo("Are you sure you want to delete this customer record? (Y/N): ");
        if (option == 'Y') {
            customers.erase(customers.begin() + index);
            cout << ">>> Customer deleted successfully!\n";
            pauseScreen();
        }
        else {
            cout << ">>> Deletion cancelled.\n";
            pauseScreen();
        }
    }
}

// View customer laundry history
void viewCustomerHistory(const vector<Customer>& customers, const vector<Booking>& bookings) {
    string keyword;
    printSubHeader("CUSTOMER LAUNDRY HISTORY");
    cout << "\nEnter Customer ID/Phone/Email to search: ";
    getline(cin, keyword);

    int index = findCustomerIndex(customers, keyword);
    if (index == -1) {
        cout << ">>> Customer not found.\n";
        pauseScreen();
        return;
    }

    //========================================
    // Check whether history exists
    //========================================
    bool found = false;
    for (const Booking& booking : bookings) {
        if (IDMatch(booking.customerID,
            customers[index].customerID)) {
            found = true;
            break;
        }
    }

    // No history -> stop here
    if (!found) {
        cout << ">>> History for " << customers[index].name << " (" << customers[index].phone << ")\n\n";
        cout << ">>> No laundry history found for this customer.\n";
        pauseScreen();
        return;
    }

    //========================================
    // History found
    //========================================
    clearScreen();
    // Display title again after clearing screen
    cout << string(75, '=') << endl;
    cout << setw(25) << " " << "CUSTOMER LAUNDRY HISTORY" << endl;
    cout << string(75, '=') << endl;
    cout << ">>> History for " << customers[index].name << " (" << customers[index].phone << ")\n\n";

    cout << left << setw(14) << "Date" << setw(14) << "Booking ID" << setw(17) << "Service" << setw(15) << "Total (RM)" << "Status" << endl;
    cout << string(75, '-') << endl;

    double totalSpent = 0.00;

    //========================================
    // Display Customer History
    //========================================
    for (const Booking& booking : bookings) {
        // Only display bookings that belong to this customer
        if (!IDMatch(booking.customerID,
            customers[index].customerID)) {
            continue;
        }
        found = true;

        //========================================
        // Prepare Date
        //========================================
        stringstream dateText;
        dateText << right << setfill('0') << setw(2) << booking.collectionDate.day << "/" << setw(2) << booking.collectionDate.month << "/" << booking.collectionDate.year;
        string date = dateText.str();

        //========================================
        // Prepare Status
        //========================================
        string status;
        switch (booking.status) {
        case 0:
            status = "Waiting Pickup";
            break;
        case 1:
            status = "Washing";
            break;
        case 2:
            status = "Drying";
            break;
        case 3:
            status = "Ready For Collection";
            break;
        case 4:
            status = "Collected";
            break;
        default:
            status = "Unknown";
        }

        //========================================
        // Display History Row
        //========================================
        cout << left << setfill(' ') << setw(14) << date << setw(14) << booking.bookingID << setw(17) << booking.selectedService.name;
        if (booking.isPaid) {
            cout << setw(15) << fixed << setprecision(2) << booking.finalAmount;
            totalSpent += booking.finalAmount;
        }
        else {
            cout << setw(15) << "Pending";
        }
        cout << status << endl;
    }
    cout << endl << string(75, '-') << endl;
    cout << left << setw(14) << "Total Spent:" << "RM " << fixed << setprecision(2) << totalSpent << endl;
    pauseScreen();

}

// Main customer management menu loop
void CustomerManagement(vector<Customer>& customers, const vector<Booking>& bookings) {
    string choice = "0";

    do {
        customerManagementMenu();
        choice = getChoice();

        if (!isValidChoice(choice, '1', '6')) {
            continue;
        }

        switch (choice[0]) {
        case '1':
            addCustomer(customers);
            break;
        case '2':
            searchCustomer(customers);
            break;
        case '3':
            editCustomer(customers);
            break;
        case '4':
            deleteCustomer(customers);
            break;
        case '5':
            viewCustomerHistory(customers, bookings);
            break;
        case '6':
            clearScreen();
            return;
        }

    } while (choice != "6");

}

//=========================================================
