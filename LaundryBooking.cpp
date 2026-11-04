#include "CleanSpark.h"

// LAUNDRY BOOKING SYSTEM
// Core booking functions with service, detergent, collection selection
//=========================================================

// Helper: Generate unique booking ID
// Returns ID format: Bnnnn (e.g., B0001, B0002)
string generateBookingID(const vector<Booking>& bookings) {
    int maxID = 0;
    string id = "B";

    for (const Booking& b : bookings) {
        int number = stoi(b.bookingID.substr(1));

        if (number > maxID)
            maxID = number;
    }

    maxID++;

    id += string(4 - to_string(maxID).length(), '0');
    id += to_string(maxID);

    return id;
}

// Get and validate laundry weight from user input
// Returns weight in kilograms (must be > 0)
double getWeight() {
    string input;
    double weight;

    while (true) {
        cout << "Enter Weight (kg): ";
        getline(cin, input);

        try {
            size_t pos;
            weight = stod(input, &pos);

            if (pos != input.length() || weight <= 0) {
                cout << "\nInvalid weight! Please enter a value greater than 0.\n";
                continue;
            }
            return weight;
        }
        catch (...) {
            cout << "\nInvalid weight! Please enter numeric value only.\n";
        }
    }
}

// Get current time in minutes since midnight
int getCurrentTimeInMinutes() {
    time_t now = time(0);
    tm localTime;
    localtime_s(&localTime, &now);

    return localTime.tm_hour * 60 + localTime.tm_min;
}

// Check if year is a leap year
bool isLeapYear(int year) {
    return (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
}

// Validate if date (day, month, year) is valid
bool isValidDate(int day, int month, int year) {
    if (month < 1 || month > 12)
        return false;
    if (day < 1)
        return false;

    int daysInMonth;

    switch (month) {
    case 1: case 3: case 5: case 7: case 8: case 10: case 12:
        daysInMonth = 31;
        break;
    case 4: case 6: case 9: case 11:
        daysInMonth = 30;
        break;
    case 2:
        daysInMonth = isLeapYear(year) ? 29 : 28;
        break;
    default:
        return false;
    }

    return day <= daysInMonth;
}

// Validate if selected date is at least 2 days from today
bool isAtLeastTwoDaysLater(int day, int month, int year) {
    // Get today's date
    time_t now = time(0);
    tm currentDate;
    localtime_s(&currentDate, &now);

    // Set current date to midnight
    currentDate.tm_hour = 0;
    currentDate.tm_min = 0;
    currentDate.tm_sec = 0;

    // Add 2 days
    currentDate.tm_mday += 2;
    time_t minimumDate = mktime(&currentDate);

    // Create selected date
    tm selectedDate = {};
    selectedDate.tm_mday = day;
    selectedDate.tm_mon = month - 1;
    selectedDate.tm_year = year - 1900;
    selectedDate.tm_hour = 0;
    selectedDate.tm_min = 0;
    selectedDate.tm_sec = 0;

    time_t selectedTime = mktime(&selectedDate);
    return selectedTime >= minimumDate;
}

// Get and validate collection date for "Other date" option
void getOtherCollectionDate(Booking& newOrder) {
    string input;
    char slash;

    while (true) {
        cout << "\nEnter Collection Date (DD/MM): ";
        getline(cin, input);

        stringstream ss(input);

        if (!(ss >> newOrder.collectionDate.day >> slash >> newOrder.collectionDate.month) || slash != '/') {
            cout << ">>> Invalid format! Please use DD/MM.\n";
            continue;
        }

        string extra;

        if (ss >> extra) {
            cout << ">>> Invalid format! Please use DD/MM.\n";
            continue;
        }

        if (!isValidDate(newOrder.collectionDate.day, newOrder.collectionDate.month, newOrder.collectionDate.year)) {
            cout << ">>> Invalid date! The selected date does not exist.\n";
            continue;
        }

        // Check whether it is at least two days later
        if (!isAtLeastTwoDaysLater(
            newOrder.collectionDate.day,
            newOrder.collectionDate.month,
            newOrder.collectionDate.year)) {

            cout << ">>> Other Date must be at least two days from today. Please select Today or Tomorrow instead.\n";
            continue;
        }

        break;
    }
}

//=========================================================
// Find Customer For Booking
//=========================================================
int findBookingCustomer(const vector<Customer>& customers, Booking& newOrder, const vector<Booking>& bookings) {
    string customerID;

    cout << "\nEnter Customer ID : ";
    getline(cin, customerID);

    int customerIndex = findCustomerIndex(customers, customerID);

    if (customerIndex == -1) {
        cout << ">>> Error: Customer not found!\n";
        pauseScreen();
        return -1;
    }

    cout << ">>> Welcome back, " << customers[customerIndex].name << "! (Current Points: " << customers[customerIndex].points << ")\n";

    newOrder.bookingID = generateBookingID(bookings);
    newOrder.customerID = customers[customerIndex].customerID;

    cout << "\nBooking ID : " << newOrder.bookingID << " (Auto-generated)\n";

    return customerIndex;
}

//========================================
// Select Service
//========================================
bool selectService(Booking& newOrder, const vector<Service>& services) {
    string choice;
    int serviceIndex;

    cout << "\n==================";
    cout << "\n  Select Service";
    cout << "\n==================\n";
    cout << "1. Wash             (RM 4.00/kg)\n";
    cout << "2. Wash + Dry       (RM 6.00/kg)\n";
    cout << "3. Wash + Iron      (RM 7.00/kg)\n";
    cout << "4. Full Service     (RM 8.00/kg)\n";
    cout << "5. Back\n";

    while (true) {
        choice = getChoice();

        if (!isValidChoice(choice, '1', '5'))
            continue;

        if (choice == "5") {
            clearScreen();
            return false;
        }

        serviceIndex = choice[0] - '1';

        newOrder.selectedService = services[serviceIndex];

        //========================================
        // Enter Weight
        //========================================
        newOrder.weight = getWeight();

        stringstream weightText;
        weightText << newOrder.weight;

        cout << "\nSelected Service: " << newOrder.selectedService.name
            << " (RM " << fixed << setprecision(2) << newOrder.selectedService.servicePrice << "/kg)";
        cout << "\nSelected Weight: " << weightText.str() << " kg\n";

        char confirm = getYesNo("Confirm this service? (Y/N): ");

        if (confirm == 'Y') {
            clearScreen();
            return true;
        }

        cout << "\nPlease select the service again.\n";
    }
}

//=========================================================
// Select Detergent
//=========================================================
int selectDetergent(Booking& newOrder, const vector<Detergent>& detergents) {
    string choice;
    int detergentIndex;

    cout << "\n====================";
    cout << "\n  Choose Detergent";
    cout << "\n====================\n";
    cout << "1. Normal            (Free)\n";
    cout << "2. Softener          (+ RM 2.00)\n";
    cout << "3. Baby Detergent    (+ RM 5.00)\n";

    while (true) {
        choice = getChoice();

        if (!isValidChoice(choice, '1', '3'))
            continue;

        detergentIndex = choice[0] - '1';

        // Check current stock
        if (detergents[detergentIndex].currentStock == 0) {
            cout << ">>> Error: Detergent is currently out of stock! "
                << "Please choose another.\n";
            continue;
        }

        newOrder.selectedDetergent = detergents[detergentIndex];

        cout << "\nSelected Detergent: " << newOrder.selectedDetergent.name;

        if (newOrder.selectedDetergent.price == 0.00) {
            cout << " (Free)\n";
        }
        else {
            cout << " (RM " << fixed << setprecision(2) << newOrder.selectedDetergent.price << ")\n";
        }
        char confirm = getYesNo("Confirm this detergent? (Y/N): ");

        if (confirm == 'Y') {
            clearScreen();
            return detergentIndex;
        }

        cout << "\nPlease select the detergent again.\n";
    }
}

//========================================
// Collection Day
//========================================
void selectCollectionDay(Booking& newOrder) {
    string choice;
    int collectionIndex;
    int currentTime = getCurrentTimeInMinutes();
    bool todayAvailable = currentTime < 22 * 60;

    cout << "\n==================";
    cout << "\n  Collection Day";
    cout << "\n==================\n";
    cout << "1. Today";
    if (!todayAvailable) {
        cout << " [Unavailable]";
    }
    cout << endl;
    cout << "2. Tomorrow\n";
    cout << "3. Other date";

    while (true) {

        choice = getChoice();

        if (!isValidChoice(choice, '1', '3'))
            continue;

        collectionIndex = choice[0] - '1';

        if (collectionIndex == 0 && !todayAvailable) {
            cout << ">>> Today is no longer available for collection. "
                << "Please select Tomorrow or Other Date.\n";
            continue;
        }

        //========================================
        // Today
        //========================================
        if (collectionIndex == 0)
        {
            newOrder.collectionDay = "Today";

            // Get today's date
            time_t now = time(0);
            tm localTime;

            localtime_s(&localTime, &now);

            newOrder.collectionDate.day = localTime.tm_mday;

            newOrder.collectionDate.month = localTime.tm_mon + 1;

            newOrder.collectionDate.year = 2026;
        }

        //========================================
        // Tomorrow
        //========================================
        else if (collectionIndex == 1)
        {
            newOrder.collectionDay = "Tomorrow";

            // Get tomorrow's date
            time_t now = time(0);
            tm localTime;

            localtime_s(&localTime, &now);

            localTime.tm_mday += 1;

            // Let mktime handle month/year changes
            mktime(&localTime);

            newOrder.collectionDate.day = localTime.tm_mday;

            newOrder.collectionDate.month = localTime.tm_mon + 1;

            newOrder.collectionDate.year = 2026;
        }

        //========================================
        // Other Date
        //========================================
        else {
            newOrder.collectionDay = "Other date";
            getOtherCollectionDate(newOrder);
        }

        cout << "\nSelected Day: ";

        if (newOrder.collectionDay == "Other date") {
            cout << right << setfill('0') << setw(2) << newOrder.collectionDate.day << "/"
                << setw(2) << newOrder.collectionDate.month << "/" << newOrder.collectionDate.year
                << setfill(' ') << left << "\n";
        }
        else {
            cout << newOrder.collectionDay << "\n";
        }

        char confirm = getYesNo("Confirm this option? (Y/N): ");

        if (confirm == 'Y') {
            clearScreen();
            return;
        }

        cout << "\nPlease select the day again.\n";
    }
}

//========================================
// Collection Time
//========================================
void selectCollectionTime(Booking& newOrder, const vector<CollectionOption>& collectionOptions) {
    string choice;
    int collectionTimeIndex;

    int currentTime = getCurrentTimeInMinutes();

    cout << "\n====================";
    cout << "\n  Collection Time";
    cout << "\n====================\n";

    // Display options only once
    for (int i = 0; i < 3; i++) {
        cout << i + 1 << ". " << left << setw(22) << collectionOptions[i].timeRange;

        if (newOrder.collectionDay == "Today") {
            if (collectionOptions[i].price == 0) {
                cout << "(Free)";
            }
            else {
                cout << "(+ RM " << fixed << setprecision(2) << collectionOptions[i].price << ")";
            }
            if (currentTime >= collectionOptions[i].endTime) {
                cout << " [Unavailable]";
            }
        }
        else {
            cout << "(Free)";
        }
        cout << endl;
    }

    while (true) {
        choice = getChoice();

        if (!isValidChoice(choice, '1', '3'))
            continue;

        collectionTimeIndex = choice[0] - '1';

        // Check unavailable option
        if (newOrder.collectionDay == "Today" && currentTime >= collectionOptions[collectionTimeIndex].endTime) {
            cout << ">>> This collection time is no longer available. Please choose another.\n";
            continue;
        }

        newOrder.selectedCollection = collectionOptions[collectionTimeIndex];

        // Tomorrow / Other Date = Free
        if (newOrder.collectionDay != "Today") {
            newOrder.selectedCollection.price = 0.00;
        }
        cout << "\nSelected Time: " << newOrder.selectedCollection.timeRange;

        if (newOrder.selectedCollection.price == 0.00) {
            cout << " (Free)\n";
        }
        else {
            cout << " (RM " << fixed << setprecision(2) << newOrder.selectedCollection.price << ")\n";
        }

        char confirm = getYesNo("Confirm this time? (Y/N): ");

        if (confirm == 'Y') {
            clearScreen();
            return;
        }
        cout << "\nPlease select the time again.\n";
    }
}

//========================================
// Display Price Summary
//========================================
string displayBookingSummary(Booking& newOrder, const Customer& customer) {
    //========================================
    // Calculate subtotal
    //========================================
    double serviceTotal = newOrder.selectedService.servicePrice * newOrder.weight;
    newOrder.subtotal = serviceTotal + newOrder.selectedDetergent.price + newOrder.selectedCollection.price;

    //========================================
    // Price Summary
    //========================================
    cout << fixed << setprecision(2);
    cout << "\n=================================================";
    cout << "\n                  PRICE SUMMARY                  ";
    cout << "\n=================================================\n";

    cout << left << setw(12) << "Booking ID" << ": " << newOrder.bookingID << endl;
    cout << left << setw(12) << "Customer" << ": " << customer.name << " (" << newOrder.customerID << ")" << endl;
    cout << "-------------------------------------------------\n";

    stringstream weightText;
    weightText << newOrder.weight;

    cout << left << setw(12) << "Service" << ": "
        << setw(25) << (newOrder.selectedService.name + " (" + weightText.str() + "kg)")
        << "|RM " << serviceTotal << endl;

    cout << left << setw(12) << "Detergent" << ": " << setw(25) << newOrder.selectedDetergent.name
        << "|RM " << newOrder.selectedDetergent.price << endl;

    //========================================
    // Prepare Collection Date
    //========================================
    stringstream dateText;

    dateText << right << setfill('0') << setw(2) << newOrder.collectionDate.day << "/" << setw(2)
        << newOrder.collectionDate.month << "/" << newOrder.collectionDate.year;

    string date = dateText.str();

    // Collection Day
    cout << left << setfill(' ') << setw(12) << "Collection" << ": " << setw(25) << date
        << "|RM " << newOrder.selectedCollection.price << endl;

    string collectionText = " (" + newOrder.selectedCollection.timeRange + ")";
    cout << setw(13) << " " << setw(26) << collectionText << "|" << endl;


    cout << "-------------------------------------------------\n";

    cout << left << setw(12) << "Subtotal" << " "
        << setw(26) << " " << "|RM " << newOrder.subtotal << endl;

    //========================================
    // Display Penalty Information
    //========================================
    cout << "\n>>> Note: A penalty of up to RM10.00 may apply if you collect your laundry more than 30 minutes after the selected time.\n";
    cout << "\nLate Collection Penalty Schedule:\n";
    cout << "Late by 0 - 30 minutes         : RM 0.00\n";
    cout << "Late by 31 - 60 minutes        : RM 2.00\n";
    cout << "Late by 61 - 120 minutes       : RM 5.00\n";
    cout << "Late by 121 - 180 minutes      : RM 8.00\n";
    cout << "Late by more than 180 minutes  : RM 10.00\n";

    return date;
}

// MAIN LAUNDRY BOOKING FUNCTION
// Handles complete booking workflow: customer selection, service, detergent, collection time
void LaundryBooking(const vector<Customer>& customers, vector<Booking>& bookings, const vector<Service>& services, vector<Detergent>& detergents, const vector<CollectionOption>& collectionOptions) {
    Booking newOrder;
    printSubHeader("LAUNDRY BOOKING");

    // Customer
    int customerIndex = findBookingCustomer(customers, newOrder, bookings);

    if (customerIndex == -1)
        return;

    // Service
    if (!selectService(newOrder, services))
        return;

    // Detergent
    int detergentIndex = selectDetergent(newOrder, detergents);

    // Collection Day
    selectCollectionDay(newOrder);

    // Collection Time
    selectCollectionTime(newOrder, collectionOptions);

    // Price Summary
    string date = displayBookingSummary(newOrder, customers[customerIndex]);

    //========================================
    // Booking Confirmation
    //========================================
    char confirm = getYesNo("\nConfirm booking? (Y/N): ");

    if (confirm == 'Y') {
        // Decrease detergent stock
        detergents[detergentIndex].currentStock--;
        // Add booking to records
        bookings.push_back(newOrder);

        cout << "\nBooking created successfully!\n";
        cout << "Collection Day  : " << date << endl;
        cout << "Collection Time : " << newOrder.selectedCollection.timeRange << endl;
    }
    else {
        cout << "\n>>> Booking cancelled.\n";
    }

    pauseScreen();
}

//=========================================================
