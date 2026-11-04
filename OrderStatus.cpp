#include "CleanSpark.h"

// ORDER STATUS & TRACKING
// Functions for updating and tracking booking status and late penalties
//=========================================================

// Booking status names for display
string statusName[5] = {
    "Waiting Pickup",
    "Washing",
    "Drying",
    "Ready For Collection",
    "Collected"
};

// Find booking by ID
// Returns: index if found, -1 if not found
int findBookingIndex(const vector<Booking>& bookings, const string& bookingID) {
    // Find booking using Booking ID
    for (int i = 0; i < bookings.size(); i++) {
        if (IDMatch(bookingID, bookings[i].bookingID))
            return i;
    }
    return -1;  // Booking not found
}

// Display Booking Information
void displayBookingInfo(const Booking& booking, const Customer& customer)
{
    cout << "\nBooking ID      : " << booking.bookingID << endl;
    cout << "Customer        : " << customer.name << " (" << customer.customerID << ")" << endl;
    cout << "\nCurrent Status  : " << statusName[booking.status] << endl;
}

// Calculate late collection penalty based on collection time
// Returns: penalty amount in RM
// Penalties: 0-30 min = free, 31-60 min = RM 2, 61-120 min = RM 5, etc.
double calculateLatePenalty(const Booking& booking, int& minutesLate)
{
    time_t now = time(nullptr);
    tm currentTime;

    localtime_s(&currentTime, &now);

    // Collection date and time
    tm collectionDateTime = {};
    collectionDateTime.tm_year = booking.collectionDate.year - 1900;
    collectionDateTime.tm_mon = booking.collectionDate.month - 1;
    collectionDateTime.tm_mday = booking.collectionDate.day;

    // endTime is stored in minutes
    // Example: 14 * 60 = 840
    int endMinutes = booking.selectedCollection.endTime;

    collectionDateTime.tm_hour = endMinutes / 60;
    collectionDateTime.tm_min = endMinutes % 60;
    collectionDateTime.tm_sec = 0;

    // Convert collection date/time to time_t
    time_t collectionTimestamp = mktime(&collectionDateTime);

    // Convert current date/time to time_t
    time_t currentTimestamp = mktime(&currentTime);

    // Check if customer is not late
    if (currentTimestamp <= collectionTimestamp)
    {
        minutesLate = 0;
        return 0.00;
    }

    // Calculate minutes late
    minutesLate =
        static_cast<int>(difftime(currentTimestamp, collectionTimestamp) / 60);

    // Calculate penalty
    if (minutesLate <= 30)
    {
        return 0.00;
    }
    else if (minutesLate <= 60)
    {
        return 2.00;
    }
    else if (minutesLate <= 120)
    {
        return 5.00;
    }
    else if (minutesLate <= 180)
    {
        return 8.00;
    }
    else
    {
        return 10.00;
    }
}

// Display Status Option
void displayStatusOptions(const Booking& booking)
{
    cout << "\nUpdate Booking Status: \n";

    if (booking.status != 0)
        cout << "1. Waiting Pickup\n";

    if (booking.status != 1)
        cout << "2. Washing\n";

    if (booking.status != 2)
        cout << "3. Drying\n";

    if (booking.status != 3)
        cout << "4. Ready For Collection\n";

    if (booking.status != 4)
        cout << "5. Collected\n";

    cout << "6. Back\n";
}

// Update Booking Status
bool updateBookingStatus(Booking& booking)
{
    displayStatusOptions(booking);

    string choice;

    while (true)
    {
        cout << "\nEnter Choice : ";
        getline(cin, choice);

        // Back
        if (choice == "6")
        {
            clearScreen();
            return false;
        }

        // Invalid choice
        if (choice != "1" &&
            choice != "2" &&
            choice != "3" &&
            choice != "4" &&
            choice != "5")
        {
            cout << "Invalid Choice! Please enter 1 - 6 only.\n";
            continue;
        }

        // Prevent selecting current status
        if ((choice == "1" && booking.status == 0) ||
            (choice == "2" && booking.status == 1) ||
            (choice == "3" && booking.status == 2) ||
            (choice == "4" && booking.status == 3) ||
            (choice == "5" && booking.status == 4))
        {
            cout << "Invalid Choice! Current status is already selected.\n";
            continue;
        }

        // Confirm status update
        char confirm =
            getYesNo("Confirm this status update? (Y/N): ");

        if (confirm == 'N')
        {
            cout << ">>> Update cancelled. Please select another status.\n";
            continue;
        }

        // Update status
        switch (choice[0])
        {
        case '1':
            booking.status = 0;
            break;

        case '2':
            booking.status = 1;
            break;

        case '3':
            booking.status = 2;
            break;

        case '4':
            booking.status = 3;
            break;

        case '5':
        {
            booking.status = 4;

            int minutesLate = 0;

            booking.latePenalty =
                calculateLatePenalty(booking, minutesLate);

            cout << "\n";

            if (booking.latePenalty == 0.00)
            {
                cout << "Collection Status       : On Time\n";
                cout << "Late Collection Penalty : Free\n";
            }
            else
            {
                cout << "Collection Status       : Late\n";
                cout << "Minutes Late            : "
                    << minutesLate << " minutes\n";

                cout << "Late Collection Penalty : RM "
                    << fixed << setprecision(2)
                    << booking.latePenalty << endl;
            }

            break;
        }
        }

        return true;
    }
}

void updateOrderStatus(vector<Booking>& bookings, const vector<Customer>& customers)
{
    printSubHeader("PICKUP & COLLECTION");

    string bookingID;

    cout << "\nEnter Booking ID : ";
    getline(cin, bookingID);

    // Find Booking
    int index = findBookingIndex(bookings, bookingID);

    if (index == -1)
    {
        cout << ">>> Error: Booking not found.\n";
        pauseScreen();
        return;
    }

    //Find customer
    int customerIndex = findCustomerIndex(customers, bookings[index].customerID);

    if (customerIndex == -1)
    {
        cout << ">>> Error: Customer not found!\n";
        pauseScreen();
        return;
    }

    // Display Booking
    displayBookingInfo(bookings[index], customers[customerIndex]);


    // Collected is the final status
    if (bookings[index].status == 4)
    {
        cout << "\n>>> This booking has already been collected.\n";
        cout << ">>> No further status update is allowed.\n";

        pauseScreen();
        return;
    }

    // update status
    bool updated = updateBookingStatus(bookings[index]);
    if (!updated)
        return;

    // Success Message
    cout << "\n>>> Booking Updated Successfully!\n";
    cout << ">>> Current Status : " << statusName[bookings[index].status] << endl;

    pauseScreen();
}

//=========================================================
