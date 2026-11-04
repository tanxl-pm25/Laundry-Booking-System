#include "CleanSpark.h"

// PAYMENT PROCESSING
// Functions for processing payments, calculating discounts, and generating receipts
//=========================================================

// Find Booking
// Returns booking index if found, -1 if not found
int findBooking(const vector<Booking>& bookings, const string& bookingID)
{
    for (int i = 0; i < bookings.size(); i++)
    {
        if (IDMatch(bookingID, bookings[i].bookingID))
            return i;
    }

    return -1;
}

// Check Booking Status
bool checkBookingStatus(const Booking& booking)
{
    if (booking.status != 4)
    {
        cout << "\n>>> Payment cannot be made yet.\n";
        cout << ">>> Current Status : "
            << statusName[booking.status] << endl;
        cout << ">>> Please wait until the booking is collected.\n";

        pauseScreen();
        return false;
    }

    if (booking.isPaid)
    {
        cout << "\n>>> This booking has already been paid.\n";
        cout << ">>> Payment cannot be made again.\n";

        pauseScreen();
        return false;
    }

    return true;
}

// Find Customer
// Returns customer index if found, -1 if not found
int findCustomer(const vector<Customer>& customers,
    const string& customerID)
{
    for (int i = 0; i < customers.size(); i++)
    {
        if (IDMatch(customerID, customers[i].customerID))
            return i;
    }

    return -1;
}

// Payment Process
void displayPaymentProcess(
    const Booking& booking,
    const Customer& customer,
    double subtotal,
    double discount,
    double totalAfterDiscount,
    double sst,
    double totalBeforePoints)
{
    double serviceTotal = booking.selectedService.servicePrice * booking.weight;
    cout << fixed << setprecision(2);

    cout << "\n=========================================================\n";
    cout << "                   PAYMENT PROCESS\n";
    cout << "=========================================================\n";

    cout << left << setw(30) << "Booking ID" << ": " << booking.bookingID << endl;
    cout << left << setw(30) << "Customer" << ": " << customer.name << " (" << customer.customerID << ")" << endl;
    cout << "\n---------------------------------------------------------\n";

    // SERVICE DETAILS
    cout << left << setw(30) << "Service" << ": " << booking.selectedService.name << endl;
    cout << left << setw(30) << "Weight" << ": " << booking.weight << " kg" << endl;
    cout << left << setw(30) << "Service Fee" << ": RM " << serviceTotal << endl;

    // DETERGENT DETAILS
    cout << left << setw(30) << "Detergent" << ": " << booking.selectedDetergent.name << endl;
    cout << left << setw(30) << "Detergent Fee" << ": RM " << booking.selectedDetergent.price << endl;
    cout << "\n---------------------------------------------------------\n";

    // SUBTOTAL
    cout << left << setw(30) << "Subtotal" << ": RM " << subtotal << endl;

    // MEMBER STATUS
    if (customer.isMember)
    {
        cout << left << setw(30) << "Member Status" << ": Yes (10% off subtotal)" << endl;
        cout << left << setw(30) << "Member Discount" << ": - RM " << discount << endl;
        // AFTER DISCOUNT
        cout << left << setw(30) << "Subtotal After Discount" << ": RM " << totalAfterDiscount << endl;
    }
    else
    {
        cout << left << setw(30) << "Member Status" << ": No" << endl;
    }



    // SST
    cout << left << setw(30) << "SST (6%)" << ": RM " << sst << endl;

    // TOTAL BEFORE POINTS
    cout << left << setw(30) << "Total Before Points" << ": RM " << totalBeforePoints << endl;
}

// Redeem Loyalty Points
double redeemPoints(
    Customer& customer,
    double totalBeforePoints,
    int& pointsRedeemed)
{
    pointsRedeemed = 0;

    // Non-member cannot redeem points
    if (!customer.isMember)
    {
        return 0.00;
    }

    cout << "\n>>> Redemption Rate: 100 Points = RM 5.00\n";
    cout << ">>> Points can only be redeemed in multiples of 100.\n";
    cout << ">>> You have " << customer.points << " Points.\n";

    // Transaction amount too low
    if (totalBeforePoints < 5.00)
    {
        cout << "\n>>> Points redemption is unavailable.\n";
        cout << ">>> Minimum transaction amount is RM 5.00.\n";

        return 0.00;
    }

    // Not enough points
    if (customer.points < 100)
    {
        cout << "\n>>> Points redemption is unavailable.\n";
        cout << ">>> You need at least 100 points to redeem.\n";

        return 0.00;
    }

    char redeem;

    // Ask whether customer wants to redeem
    while (true)
    {
        cout << "\nRedeem points? (Y/N): ";
        cin >> redeem;
        cin.ignore();

        if (redeem == 'Y' ||
            redeem == 'y' ||
            redeem == 'N' ||
            redeem == 'n')
        {
            break;
        }

        cout << ">>> Invalid input! Please enter Y or N only.\n";
    }

    // Customer does not want to redeem
    if (redeem == 'N' || redeem == 'n')
    {
        return 0.00;
    }

    // Enter points
    while (true)
    {
        cout << "\nEnter points to redeem: ";
        cin >> pointsRedeemed;
        cin.ignore();

        // Check positive number
        if (pointsRedeemed <= 0)
        {
            cout << ">>> Invalid! Please enter a positive number.\n";
            continue;
        }

        // Check multiple of 100
        if (pointsRedeemed % 100 != 0)
        {
            cout << ">>> Invalid! Points must be redeemed in multiples of 100.\n";

            continue;
        }

        // Check available points
        if (pointsRedeemed > customer.points)
        {
            cout << ">>> You do not have enough points.\n";
            cout << ">>> Available Points: " << customer.points << endl;

            continue;
        }

        // Calculate requested discount
        double requestedDiscount =
            (pointsRedeemed / 100) * 5.00;

        // Discount cannot exceed transaction amount
        if (requestedDiscount >= totalBeforePoints)
        {
            cout << ">>> Invalid! Points discount cannot exceed the transaction amount.\n";

            continue;
        }

        break;
    }

    // Calculate points discount
    double pointsDiscount =
        (pointsRedeemed / 100) * 5.00;

    // Deduct points
    customer.points -= pointsRedeemed;

    cout << left << setw(30) << "Points Redeemed" << ": - RM " << pointsDiscount
        << " (" << pointsRedeemed << " pts deducted)" << endl;

    return pointsDiscount;
}

// Display Payment Total
void displayPaymentTotal(
    double totalBeforePoints,
    double pointsDiscount,
    double latePenalty,
    double grandTotal)
{
    cout << "\n---------------------------------------------------------\n";

    // Points discount
    if (pointsDiscount > 0.00)
    {
        cout << left << setw(30) << "Points Discount" << ": - RM " << pointsDiscount << endl;
    }

    // Late penalty
    if (latePenalty > 0.00)
    {
        cout << left << setw(30) << "Late Collection Penalty" << ": RM " << latePenalty << endl;
    }
    else
    {
        cout << left << setw(30) << "Late Collection Penalty" << ": Free" << endl;
    }

    cout << "\n---------------------------------------------------------\n";
    cout << left << setw(30) << "GRAND TOTAL" << ": RM " << grandTotal << endl;
    cout << "---------------------------------------------------------\n";
}

// Get Payment Amount
double getPaymentAmount(double grandTotal)
{
    double amountPaid;

    while (true)
    {
        cout << "\nAmount Paid : RM ";
        cin >> amountPaid;
        cin.ignore();

        // Invalid input
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(1000, '\n');

            cout << ">>> Invalid input! Please enter a valid amount.\n";

            continue;
        }

        // Insufficient payment
        if (amountPaid < grandTotal)
        {
            cout << ">>> Insufficient payment!\n";
            cout << ">>> Please enter RM " << fixed << setprecision(2) << grandTotal << " or more.\n";

            continue;
        }

        return amountPaid;
    }
}

// Generate Receipt
//=========================================================
void generateReceipt(
    const Booking& booking,
    const Customer& customer,
    double subtotal,
    double discount,
    double sst,
    double pointsDiscount,
    int pointsRedeemed,
    int pointsEarned,
    double grandTotal,
    double amountPaid,
    double change)
{
    string receiptID = generateReceiptID();
    double serviceTotal = booking.selectedService.servicePrice * booking.weight;

    // Current date
    time_t now = time(0);

    tm currentTime;
    localtime_s(&currentTime, &now);

    int day = currentTime.tm_mday;
    int month = currentTime.tm_mon + 1;
    int year = currentTime.tm_year + 1900;

    cout << "\n====================================================\n";
    cout << "         CLEANSPARK LAUNDRY Service System\n";
    cout << "====================================================\n";

    cout << left << setw(25) << "Receipt No" << ": " << receiptID << endl;
    cout << left << setw(25) << "Booking ID" << ": " << booking.bookingID << endl;


    cout << left << setw(25) << "Date" << ": " << setfill('0') << right
        << setw(2) << day << "/" << setw(2) << month << "/"
        << year << setfill(' ') << endl;

    // CUSTOMER
    cout << left << setw(25) << "Customer" << ": " << customer.name << " (" << customer.customerID << ")" << endl;

    // SERVICE
    cout << left << setw(25) << "Service" << ": " << booking.selectedService.name << " (" << booking.weight << " kg)" << endl;

    cout << left << setw(25) << "Service Fee" << ": RM " << serviceTotal << endl;

    // DETERGENT
    cout << left << setw(25) << "Detergent" << ": " << booking.selectedDetergent.name << endl;
    cout << left << setw(25) << "Detergent Fee" << ": RM " << booking.selectedDetergent.price << endl;

    // COLLECTION
    cout << left << setw(25) << "Collection" << ": " << booking.collectionDay << endl;
    cout << left << setw(25) << "Collection Time" << ": " << booking.selectedCollection.timeRange << endl;


    // PAYMENT BREAKDOWN
    cout << "\n----------------------------------------------------\n";
    cout << left << setw(25) << "Subtotal" << ": RM " << subtotal << endl;
    cout << left << setw(25) << "SST (6%)" << ": RM " << sst << endl;

    if (customer.isMember)
    {
        cout << left << setw(25) << "Discount" << ": - RM " << discount << endl;
        cout << left << setw(25) << "Pts Offer" << ": - RM " << pointsDiscount << endl;
    }


    // LATE PENALTY
    if (booking.latePenalty > 0.00)
    {
        cout << left << setw(25) << "Late Penalty" << ": RM " << booking.latePenalty << endl;
    }
    else
    {
        cout << left << setw(25) << "Late Penalty" << ": Free\n";
    }

    cout << "----------------------------------------------------\n";

    // FINAL AMOUNT
    cout << left << setw(25) << "Total" << ": RM " << grandTotal << endl;
    cout << left << setw(25) << "Paid" << ": RM " << amountPaid << endl;
    cout << left << setw(25) << "Change" << ": RM " << change << endl;

    // POINTS
    if (customer.isMember)
    {
        cout << "\n";
        cout << left << setw(25) << "Points Redeemed" << ": " << pointsRedeemed << " pts" << endl;
        cout << left << setw(25) << "Points Earned" << ": " << pointsEarned << " pts" << endl;
        cout << left << setw(25) << "Points Balance" << ": " << customer.points << " pts" << endl;
    }

    cout << "====================================================\n";

    cout << "\nThank You!\n";

}


// Main payment processing function
// Handles: payment amount validation, discount application, SST calculation, 
// points redemption, and receipt generation
void payment(vector<Booking>& bookings, vector<Customer>& customers)
{
    printSubHeader("Payment");
    string bookingID;

    // GET BOOKING ID
    cout << "\nEnter Booking ID (0 to Back): ";
    getline(cin, bookingID);
    clearScreen();

    if (bookingID == "0")
    {
        clearScreen();
        return;
    }

    // Find Booking
    int index = findBookingIndex(bookings, bookingID);
    if (index == -1)
    {
        cout << "\n>>> Error: Booking not found!\n";
        pauseScreen();
        return;
    }

    // check booking status
    if (!checkBookingStatus(bookings[index]))
    {
        return;
    }

    // FIND CUSTOMER
    int customerIndex = findCustomer(customers, bookings[index].customerID);

    if (customerIndex == -1)
    {
        cout << "\n>>> Error: Customer information not found!\n";
        pauseScreen();
        return;
    }

    Booking& booking = bookings[index];
    Customer& customer = customers[customerIndex];

    // Payment Calculation
    double subtotal = booking.subtotal;

    // Member discount
    double discount = 0.00;

    if (customer.isMember)
    {
        discount = subtotal * 0.10;
    }

    // Total after member discount
    double totalAfterDiscount = subtotal - discount;

    // SST 6%
    double sst = totalAfterDiscount * 0.06;

    // Total before points
    double totalBeforePoints = totalAfterDiscount + sst;

    // Payment Process
    displayPaymentProcess(booking, customer, subtotal, discount,
        totalAfterDiscount, sst, totalBeforePoints);

    //Redeem Points
    int pointsRedeemed = 0;
    double pointsDiscount = redeemPoints(customer, totalBeforePoints, pointsRedeemed);

    // Late Collection Penalty
    double latePenalty = booking.latePenalty;

    // Grand Total
    double grandTotal = totalBeforePoints - pointsDiscount + latePenalty;
    displayPaymentTotal(totalBeforePoints, pointsDiscount, latePenalty, grandTotal);

    //Payment
    double amountPaid = getPaymentAmount(grandTotal);

    // Change
    double change = amountPaid - grandTotal;
    cout << "Change : RM " << fixed << setprecision(2) << change << endl;

    // Earn point
    int pointsEarned = 0;

    // Only Premium Members earn points
    if (customer.isMember)
    {
        pointsEarned = static_cast<int>(grandTotal);
        customer.points += pointsEarned;
    }

    // save Payment
    booking.finalAmount = grandTotal;
    booking.isPaid = true;

    cout << "\n>>> Payment Successful!\n";

    if (customer.isMember)
    {
        cout << ">>> Earned " << pointsEarned << " Points for this transaction!\n";
        cout << ">>> RM 1 = 1 point\n";
    }

    // Generate Receipt
    cout << "\nPress Enter to view receipt...";
    cin.get();

    clearScreen();

    generateReceipt(booking, customer, subtotal, discount, sst,
        pointsDiscount, pointsRedeemed, pointsEarned,
        grandTotal, amountPaid, change);


    pauseScreen();
}

//=========================================================
