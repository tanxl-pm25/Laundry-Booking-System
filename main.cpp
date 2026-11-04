#include "CleanSpark.h"

// MAIN PROGRAM ENTRY POINT
// Initializes system, loads persisted data, and runs main menu loop
//=========================================================

// Entry point: Initialize vectors, load data, and start main menu
int main() {
    // Initialize customer records vector
    vector<Customer> customers;

    // Initialize booking records vector
    vector<Booking> bookings;

    // Initialize detergent inventory with default items
    // Format: itemCode, name, price, currentStock
    vector<Detergent> detergents = {
        {"D01", "Normal", 0.00, 10},
        {"D02", "Softener", 2.00, 5},
        {"D03", "Baby Detergent", 5.00, 0}
    };

    // Initialize available laundry services with pricing
    // Format: name, pricePerKg
    vector<Service> services = {
        {"Wash", 4.00},
        {"Wash + Dry", 6.00},
        {"Wash + Iron", 7.00},
        {"Full Service", 8.00}
    };

    // Initialize available collection time slots
    // Format: timeRange, price, endTimeInMinutes
    vector<CollectionOption> collectionOptions = {
        {"Before 2:00 PM", 10.00, 14 * 60},
        {"2:00 PM - 6:00 PM", 5.00, 18 * 60},
        {"6:00 PM - 10:00 PM", 0.00, 22 * 60}
    };

    // Load persisted data from files
    loadCustomers(customers, "customers.txt");
    loadBookings(bookings, "bookings.txt");
    loadDetergents(detergents, "detergents.txt");

    // Main application loop
    string mainChoice = "0";

    do {
        displayMainMenu();
        mainChoice = getChoice();

        if (!isValidChoice(mainChoice, '1', '6')) {
            continue;
        }

        // Process menu selection
        switch (mainChoice[0]) {
        case '1':
            clearScreen();
            CustomerManagement(customers, bookings);
            break;
        case '2':
            clearScreen();
            LaundryBooking(customers, bookings, services, detergents, collectionOptions);
            break;
        case '3':
            clearScreen();
            updateOrderStatus(bookings, customers);
            break;
        case '4':
            clearScreen();
            payment(bookings, customers);
            break;
        case '5':
            clearScreen();
            reportsMenu(customers, bookings, services, detergents);
            break;
        case '6':
            // Exit message
            cout << "\n============================================" << endl;
            cout << "Thank You For Using" << endl;
            cout << "CleanSpark Laundry Service System" << endl;
            cout << "Exiting system..." << endl;
            cout << "============================================" << endl;
            break;
        }

    } while (mainChoice != "6");

    // Save all data to files before exiting
    saveCustomers(customers, "customers.txt");
    saveBookings(bookings, "bookings.txt");
    saveDetergents(detergents, "detergents.txt");

    return 0;
}