#include "CleanSpark.h"

// REPORTING SYSTEM
// Functions for generating sales reports, inventory status, and uncollected orders
//=========================================================

// Helper: Format amount as RM currency string
string formatRM(double amount) {
    stringstream ss;
    ss << "RM " << fixed << setprecision(2) << amount;
    return ss.str();
}

// Find customer by ID
// Returns: index if found, -1 if not found
int findCustomerByID(const vector<Customer>& customers, const string& id) {
    for (int i = 0; i < customers.size(); ++i) {
        if (IDMatch(id, customers[i].customerID))
            return i;
    }
    return -1;
}

// Choose date helpers
bool parseDate(const string& input, Date& out) {
    // Expect DD/MM/YYYY
    int d, m, y; char s1, s2;
    stringstream ss(input);
    if (!(ss >> d >> s1 >> m >> s2 >> y))
        return false;

    if (s1 != '/' || s2 != '/')
        return false;

    if (!isValidDate(d, m, y))
        return false;

    out.day = d;
    out.month = m;
    out.year = y;
    return true;
}

// Check same day
bool sameDate(const Date& a, const Date& b) {
    return a.day == b.day && a.month == b.month && a.year == b.year;
}

// Convert tm to Date (assumes year 1900 offset)
Date tmToDate(const tm& t) {
    Date d;
    d.day = t.tm_mday;
    d.month = t.tm_mon + 1;
    d.year = t.tm_year + 1900;
    return d;
}

// Return current system date/time as string DD/MM/YYYY HH:MM:SS
string currentDateTime() {
    time_t now = time(0);
    tm t;
    localtime_s(&t, &now);
    stringstream ss;
    ss << setfill('0') << setw(2) << t.tm_mday << "/" << setw(2) << t.tm_mon + 1 << "/" << t.tm_year + 1900 << " " << setw(2) << t.tm_hour << ":" << setw(2) << t.tm_min << ":" << setw(2) << t.tm_sec;
    return ss.str();
}

// NOTE: `currentDateTime()` returns the local system time formatted for
// inclusion in report headers. It is used by all report functions so the
// printed report shows when it was generated.

// Get weekday 0=Sun..6=Sat for a given Date
int weekdayOf(const Date& date) {
    tm tt = {};
    tt.tm_mday = date.day;
    tt.tm_mon = date.month - 1;
    tt.tm_year = date.year - 1900;
    mktime(&tt);
    return tt.tm_wday;
}

bool askExportReport() {
    while (true) {
        char choice = getYesNo("\nDo you want to export this report to a text file? (Y/N): ");


        if (choice == 'Y')
            return true;
        if (choice == 'N')
            return false;
    }

    cout << "Invalid input! Please enter 'Y' or 'N' only.\n";
}


void exportReportIfRequested(const string& reportContent, const string& defaultName) {
    if (!askExportReport())
        return;

    cout << "\nEnter filename to save report (press Enter to use '" << defaultName << "'): ";
    string fname;
    getline(cin, fname);
    if (fname.empty())
        fname = defaultName;
    if (fname.find('.') == string::npos)
        fname += ".txt";

    ofstream ofs(fname);
    if (ofs)
    {
        ofs << reportContent;
        ofs.close();
        cout << "Report saved to " << fname << "\n";
    }
    else
    {
        cout << "Failed to save report to " << fname << "\n";
    }
}

//==================== REPORT FUNCTIONS ====================

// DAILY REPORT
// - Prompts user for a date (DD/MM/YYYY).
// - Aggregates only completed and paid bookings (status==4 && isPaid==true).
// - Builds the report into a string buffer, prints it to console, then
//   prompts the user to save the report as a text file. Default filename
//   is `report_daily_D_M_Y.txt` and the file is written to the program's
//   current working directory.
void dailyReport(const vector<Customer>& customers, const vector<Booking>& bookings)
{
    clearScreen();
    cout << "Enter date (DD/MM/YYYY) (0 to back): ";

    string input;
    getline(cin, input);

    if (input == "0")
        return;
    Date d;
    if (!parseDate(input, d))
    {
        cout << "Invalid date format." << "\n";
        pauseScreen();
        return;
    }


    int completed = 0;
    double gross = 0.0, discounts = 0.0, sst = 0.0, late = 0.0, pointsRedeemed = 0.0;

    for (const auto& b : bookings)
    {
        if (!b.isPaid)
            continue;
        if (!sameDate(b.collectionDate, d))
            continue;
        if (b.status != 4)
            continue;
        completed++;
        gross += b.subtotal;
        int ci = findCustomerByID(customers, b.customerID);
        bool member = (ci != -1 && customers[ci].isMember);
        double disc = member ? (b.subtotal * 0.10) : 0.0;
        discounts += disc;
        double after = b.subtotal - disc;
        sst += after * 0.06;
        late += b.latePenalty;
    }

    double net = gross - discounts - sst + late - pointsRedeemed;

    // build report text
    ostringstream report;
    report << "=========================================================\n";
    report << "\tDAILY SALES REPORT\n";
    report << "=========================================================\n";
    report << "Generated on: " << currentDateTime() << "\n\n";
    report << "Report Date : " << setfill('0') << setw(2) << d.day << "/" << setw(2) << d.month << "/" << d.year << setfill(' ') << "\n";
    report << "Total Completed Orders : " << completed << "\n";
    report << left << setw(30) << "Gross Sales" << ": " << formatRM(gross) << "\n";
    report << left << setw(30) << "Total Discounts" << ": - " << formatRM(discounts) << "\n";
    report << left << setw(30) << "SST Collected" << ": " << formatRM(sst) << "\n";
    report << left << setw(30) << "Late Penalties" << ": " << formatRM(late) << "\n";
    report << left << setw(30) << "Points Redeemed (est)" << ": " << formatRM(pointsRedeemed) << "\n";
    report << left << setw(30) << "Net Revenue" << ": " << formatRM(net) << "\n";
    if (completed == 0) report << "\nNo completed orders found for this period." << "\n";

    // output to console
    cout << report.str();

    string defaultName = string("report_daily_") + to_string(d.day) + string("_") + to_string(d.month) + string("_") + to_string(d.year) + string(".txt");
    exportReportIfRequested(report.str(), defaultName);

    pauseScreen();
}

// WEEKLY REPORT
// - Prompts for a date inside the week; computes Monday as week start.
// - Aggregates completed/paid bookings for each day of that week.
// - Outputs summary statistics and a simple text bar for each day.
// - Prompts to save the report to a text file. Default filename is
//   `report_weekly_D_M_Y.txt` where D/M/Y is the Monday of the week.
void weeklyReport(const vector<Customer>& customers, const vector<Booking>& bookings)
{
    clearScreen();
    cout << "Enter a date within the week (DD/MM/YYYY) (0 to back): ";

    string input;
    getline(cin, input);

    if (input == "0")
        return;
    Date d;
    if (!parseDate(input, d))
    {
        cout << "Invalid date format." << "\n";
        pauseScreen();
        return;
    }

    tm tt = {};
    tt.tm_mday = d.day;
    tt.tm_mon = d.month - 1;
    tt.tm_year = d.year - 1900;
    mktime(&tt);
    int wday = tt.tm_wday; // 0 sun..6 sat
    int daysToMon = (wday == 0) ? 6 : (wday - 1);
    tt.tm_mday -= daysToMon;
    mktime(&tt);
    Date monday = tmToDate(tt);

    int completed = 0;
    double gross = 0, discounts = 0, sst = 0, late = 0, pointsRedeemed = 0;
    vector<int> dailyCount(7, 0);

    for (int i = 0; i < 7; i++)
    {
        tm cur = tt;
        cur.tm_mday = tt.tm_mday + i;
        mktime(&cur);
        Date day = tmToDate(cur);
        for (const auto& b : bookings)
        {
            if (!b.isPaid || b.status != 4)
                continue;
            if (sameDate(b.collectionDate, day))
            {
                dailyCount[i]++;
                completed++;
                gross += b.subtotal;
                int ci = findCustomerByID(customers, b.customerID);
                bool member = (ci != -1 && customers[ci].isMember);
                double disc = member ? (b.subtotal * 0.10) : 0.0;
                discounts += disc;
                double after = b.subtotal - disc;
                sst += after * 0.06;
                late += b.latePenalty;
            }
        }
    }

    double avgPerDay = (gross == 0 || completed == 0) ? 0.0 : gross / 7.0;
    // build report text
    ostringstream report;
    report << "=========================================================\n";
    report << "\tWEEKLY SALES REPORT\n";
    report << "=========================================================\n";
    report << "Generated on: " << currentDateTime() << "\n\n";
    report << "Week Period Starting : " << setfill('0') << setw(2) << monday.day << "/" << setw(2) << monday.month << "/" << monday.year << setfill(' ') << "\n";
    report << "Total Completed Orders : " << completed << "\n";
    report << left << setw(30) << "Total Weekly Revenue" << ": " << formatRM(gross) << "\n";
    report << left << setw(30) << "SST Collected" << ": " << formatRM(sst) << "\n";
    report << left << setw(30) << "Total Discounts" << ": - " << formatRM(discounts) << "\n";
    report << left << setw(30) << "Late Penalties" << ": " << formatRM(late) << "\n";
    report << left << setw(30) << "Average Revenue Per Day" << ": " << formatRM(avgPerDay) << "\n";

    report << "\nBookings per day:\n";
    const string names[7] = { "SUNDAY", "MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY" };
    for (int i = 0; i < 7; i++) {
        report << left << setw(12) << names[(i + tt.tm_wday) % 7];
        report << "|" << setw(2) << " ";
        for (int b = 0; b < dailyCount[i]; b++)
        {
            report << '#';
        }
        report << " " << dailyCount[i] << "\n";
    }

    if (completed == 0) report << "\nNo completed orders found for this period." << "\n";

    cout << report.str();

    string defaultName = string("report_weekly_") + to_string(monday.day) + string("_") + to_string(monday.month) + string("_") + to_string(monday.year) + string(".txt");
    exportReportIfRequested(report.str(), defaultName);

    pauseScreen();
}

// MONTHLY REPORT
// - Prompts for month/year (MM/YYYY).
// - Aggregates completed/paid bookings by day of month.
// - Prints totals and a day-by-day bar chart ("#" per booking).
// - Prompts to save the report to a text file (default: `report_monthly_MM_YYYY.txt`).
void monthlyReport(const vector<Customer>& customers, const vector<Booking>& bookings)
{
    clearScreen();
    cout << "Enter month and year (MM/YYYY) (0 to back): ";

    string input;
    getline(cin, input);
    if (input == "0")
        return;

    int m, y;
    char c;
    stringstream ss(input);
    if (!(ss >> m >> c >> y) || c != '/')
    {
        cout << "Invalid format." << "\n";
        pauseScreen();
        return;
    }
    if (m < 1 || m > 12)
    {
        cout << "Invalid month." << "\n";
        pauseScreen();
        return;
    }

    int daysInMonth = 31;
    switch (m)
    {
    case 4:
    case 6:
    case 9:
    case 11:
        daysInMonth = 30;
        break;
    case 2:
        daysInMonth = isLeapYear(y) ? 29 : 28;
        break;
    }

    int totalBookings = 0;
    double gross = 0.0;
    vector<int> perDay(daysInMonth + 1, 0);
    for (const auto& b : bookings)
    {
        if (!b.isPaid || b.status != 4)
            continue;
        if (b.collectionDate.month == m && b.collectionDate.year == y)
        {
            totalBookings++;
            gross += b.subtotal;
            if (b.collectionDate.day >= 1 && b.collectionDate.day <= daysInMonth)
                perDay[b.collectionDate.day]++;
        }
    }

    double avgSpendingPerCustomer = (totalBookings == 0) ? 0.0 : gross / totalBookings;
    // build report
    ostringstream report;
    report << "=========================================================\n";
    report << "\tMONTHLY SALES REPORT\n";
    report << "=========================================================\n";
    report << "Generated on: " << currentDateTime() << "\n\n";
    report << "Month/Year : " << right << setfill('0') << setw(2) << m << "/" << y << setfill(' ') << "\n";
    report << left << setw(30) << "Total Monthly Revenue" << ": " << formatRM(gross) << "\n";
    report << left << setw(30) << "Total Bookings" << ": " << totalBookings << "\n";
    report << left << setw(30) << "Average Spending per Booking" << ": " << formatRM(avgSpendingPerCustomer) << "\n";

    report << "\nBookings by day:\n";
    for (int day = 1; day <= daysInMonth; ++day)
    {
        report << "Day " << right << setfill('0') << setw(2) << day << setfill(' ') << " | ";
        int cnt = perDay[day];
        for (int b = 0; b < cnt; b++) report << '#';
        report << " " << cnt << "\n";
    }

    if (totalBookings == 0) report << "\nNo completed orders found for this period." << "\n";

    cout << report.str();

    string defaultName = string("report_monthly_") + (m < 10 ? "0" : "") + to_string(m) + string("_") + to_string(y) + string(".txt");
    exportReportIfRequested(report.str(), defaultName);

    pauseScreen();
}

// YEARLY REPORT
// - Prompts for year (YYYY).
// - Aggregates completed/paid bookings by month.
// - Prints totals and a month-by-month bar chart ("#" per booking).
// - Prompts to save the report to a text file (default: `report_yearly_YYYY.txt`).
void yearlyReport(const vector<Customer>& customers, const vector<Booking>& bookings)
{
    clearScreen();
    cout << "Enter year (YYYY) (0 to back): ";

    string input;
    getline(cin, input);

    if (input == "0")
        return;
    int y;
    try
    {
        y = stoi(input);
    }
    catch (...)
    {
        cout << "Invalid year." << "\n";
        pauseScreen();
        return;
    }

    int totalBookings = 0;
    double gross = 0.0;
    vector<int> perMonth(13, 0);
    for (const auto& b : bookings)
    {
        if (!b.isPaid || b.status != 4)
            continue;
        if (b.collectionDate.year == y)
        {
            totalBookings++;
            gross += b.subtotal;
            if (b.collectionDate.month >= 1 && b.collectionDate.month <= 12)
                perMonth[b.collectionDate.month]++;
        }
    }

    double avgSpendingPerCustomer = (totalBookings == 0) ? 0.0 : gross / totalBookings;
    // build report
    ostringstream report;
    report << "=========================================================\n";
    report << "\tYEARLY SALES REPORT\n";
    report << "=========================================================\n";
    report << "Generated on: " << currentDateTime() << "\n\n";
    report << "Year : " << y << "\n";
    report << left << setw(30) << "Total Yearly Revenue" << ": " << formatRM(gross) << "\n";
    report << left << setw(30) << "Total Bookings" << ": " << totalBookings << "\n";
    report << left << setw(30) << "Average Spending per Booking" << ": " << formatRM(avgSpendingPerCustomer) << "\n";

    const string months[13] = { "", "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };
    report << "\nBookings by month:\n";
    for (int m = 1; m <= 12; m++)
    {
        report << left << setw(10) << months[m] << " | ";
        int cnt = perMonth[m];
        for (int b = 0; b < cnt; b++) report << '#';
        report << " " << cnt << "\n";
    }

    if (totalBookings == 0) report << "\nNo completed orders found for this period." << "\n";

    cout << report.str();

    string defaultName = string("report_yearly_") + to_string(y) + string(".txt");
    exportReportIfRequested(report.str(), defaultName);

    pauseScreen();
}

void displayUncollectedOrders(const vector<Customer>& customers, const vector<Booking>& bookings)
{
    clearScreen();
    ostringstream report;
    report << "=========================================================\n";
    report << "\tUNCOLLECTED ORDERS\n";
    report << "=========================================================\n\n";

    vector<Booking> pendingBookings;
    pendingBookings.reserve(bookings.size());
    for (const Booking& booking : bookings)
    {
        if (booking.status != 4)
        {
            pendingBookings.push_back(booking);
        }
    }

    if (pendingBookings.empty())
    {
        report << "No uncollected orders found.\n";
        cout << report.str();
        exportReportIfRequested(report.str(), "report_uncollected_orders.txt");
        pauseScreen();
        return;
    }

    report << left << setw(11) << "Booking ID"
        << setw(27) << "Customer Name"
        << setw(15) << "Phone"
        << "Status\n";
    report << setfill('-') << setw(10) << "" << " "
        << setw(26) << "" << " "
        << setw(14) << "" << " "
        << setw(18) << "" << "\n";
    report << setfill(' ');

    int totalUncollected = 0;
    for (const Booking& booking : pendingBookings)
    {
        string customerName = "Unknown";
        string phone = "N/A";

        int customerIndex = findCustomerByID(customers, booking.customerID);
        if (customerIndex != -1)
        {
            customerName = customers[customerIndex].name;
            phone = customers[customerIndex].phone;
        }

        report << left << setw(12) << booking.bookingID
            << setw(26) << customerName
            << setw(15) << phone
            << statusName[booking.status] << "\n";

        totalUncollected++;
    }

    report << "\nTotal Uncollected Orders : " << totalUncollected << "\n";

    cout << report.str();
    exportReportIfRequested(report.str(), "report_uncollected_orders.txt");
    pauseScreen();
}

string getDetergentStatus(const Detergent& detergent) {
    if (detergent.currentStock == 0)
        return "OUT OF STOCK!";
    if (detergent.currentStock < 5)
        return "LOW STOCK!";
    return "Healthy";
}

int findDetergentIndexByCode(const vector<Detergent>& detergents, const string& itemCode) {
    string normalized = itemCode;
    for (char& ch : normalized) {
        ch = toupper(ch);
    }

    for (int i = 0; i < detergents.size(); ++i) {
        string current = detergents[i].itemCode;
        for (char& ch : current) {
            ch = toupper(ch);
        }
        if (current == normalized) {
            return i;
        }
    }
    return -1;
}

void restockDetergent(vector<Detergent>& detergents)
{
    printSubHeader("RESTOCK DETERGENT");

    cout << "Enter Item Code to restock (e.g., D01): ";
    string itemCode;
    getline(cin, itemCode);

    int index = findDetergentIndexByCode(detergents, itemCode);
    if (index == -1) {
        cout << "\n>>> Error: Invalid item code.\n";
        pauseScreen();
        return;
    }

    Detergent& selected = detergents[index];
    cout << "\n>>> Selected: " << selected.name << " (Current Stock: " << selected.currentStock << ")\n";

    int quantity = 0;
    while (true) {
        cout << "Enter stock quantity to add: ";
        string input;
        getline(cin, input);

        try {
            quantity = stoi(input);
            if (quantity <= 0) {
                throw invalid_argument("invalid");
            }
            break;
        }
        catch (...) {
            cout << ">>> Invalid quantity. Please enter a positive number.\n";
        }
    }

    selected.currentStock += quantity;
    cout << "\n>>> Update Successful!\n";
    cout << ">>> " << selected.name << " new stock: " << selected.currentStock << "\n";
    pauseScreen();
}

void inventoryStatusReport(vector<Detergent>& detergents)
{
    string choice;

    do {
        clearScreen();

        ostringstream report;
        report << "=========================================================\n";
        report << "\tINVENTORY STATUS\n";
        report << "=========================================================\n";
        report << left << setw(5) << "ID"
            << setw(22) << "Item Name"
            << setw(18) << "Current Stock"
            << "Status\n";
        report << "-------------------------------------------------------\n";

        for (const Detergent& detergent : detergents) {
            report << left << setw(5) << detergent.itemCode
                << setw(22) << detergent.name
                << setw(18) << detergent.currentStock
                << getDetergentStatus(detergent) << "\n";
        }

        report << "-------------------------------------------------------\n\n";
        cout << report.str();


        exportReportIfRequested(report.str(), "report_inventory_status.txt");


        cout << "Options:\n";
        cout << "1. Restock Item\n";
        cout << "2. Back to Report Menu\n\n";
        cout << "Enter Choice: ";
        getline(cin, choice);

        if (choice == "1") {
            restockDetergent(detergents);
        }
        else if (choice == "2") {
            clearScreen();
            return;
        }
        else {
            cout << "\n>>> Invalid choice. Please enter 1 or 2.\n";
            pauseScreen();
        }

    } while (true);
}


//==================== REPORTS MENU ====================
void reportsMenu(vector<Customer>& customers, vector<Booking>& bookings, vector<Service>& services, vector<Detergent>& detergents)
{
    string choice;
    string choice2;
    do
    {
        printSubHeader("REPORTS");
        cout << "1. Sales Report\n";
        cout << "2. Uncollected Order\n";
        cout << "3. Inventory Status\n";
        cout << "4. Back\n";

        choice = getChoice();
        if (!isValidChoice(choice, '1', '4'))
            continue;

        switch (choice[0])
        {
        case '1':
            clearScreen();
            printSubHeader("REPORTS");
            cout << "1. Daily Sales Report\n";
            cout << "2. Weekly Sales Report\n";
            cout << "3. Monthly Sales Report\n";
            cout << "4. Yearly Sales Report\n";
            cout << "5. Back\n";
            choice2 = getChoice();
            if (!isValidChoice(choice2, '1', '5'))
                continue;
            switch (choice2[0])
            {
            case '1':
                dailyReport(customers, bookings);
                break;
            case '2':
                weeklyReport(customers, bookings);
                break;
            case '3':
                monthlyReport(customers, bookings);
                break;
            case '4':
                yearlyReport(customers, bookings);
                break;
            case '5':
                clearScreen();
                break;
            }
            break;
        case '2':
            displayUncollectedOrders(customers, bookings);
            break;
        case '3':
            inventoryStatusReport(detergents);
            break;
        case '4':
            clearScreen();
            return;
        }

    } while (true);
}

//=========================================================
