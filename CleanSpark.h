#ifndef CLEANSPARK_H
#define CLEANSPARK_H

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <sstream>
#include <regex>
#include <ctime>
#include <limits>
#include <fstream>
#include <cctype>
#include <cstdlib>
#include <stdexcept>

using namespace std;

//=========================================================
// DATA STRUCTURES - shared by all modules
//=========================================================
struct Customer {
    string customerID;
    string name;
    string phone;
    string email;
    bool isMember = false;
    int points = 0;
};

struct Date {
    int day = 0;
    int month = 0;
    int year = 2026;
};

struct Service {
    string name;
    double servicePrice = 0.0;
};

struct Detergent {
    string itemCode;
    string name;
    double price = 0.0;
    int currentStock = 0;
};

struct CollectionOption {
    string timeRange;
    double price = 0.0;
    int endTime = 0;
};

struct Booking {
    string bookingID;
    string customerID;
    Service selectedService;
    double weight = 0.0;
    Detergent selectedDetergent;
    string collectionDay;
    Date collectionDate;
    CollectionOption selectedCollection;
    int status = 0;
    double subtotal = 0.0;
    double latePenalty = 0.0;
    double finalAmount = 0.0;
    bool isPaid = false;
};

//=========================================================
// SHARED FUNCTION DECLARATIONS
//=========================================================
// Utility / Main Menu
void printHeader(string title);
void printSubHeader(string subTitle);
void clearScreen();
string getChoice();
bool isValidChoice(string choice, char min, char max);
char getYesNo(string message);
void pauseScreen();
void displayMainMenu();

// Customer Management
string generateCustomerID(const vector<Customer>& customers);
string formatName(string name);
string toUpperCase(string text);
bool IDMatch(string input, string storedID);
int findCustomerIndex(const vector<Customer>& customers, const string& keyword);
void customerManagementMenu();
void displayCustomer(const Customer& c);
string getName();
bool phoneExists(const vector<Customer>& customers, string phone);
string getPhone(const vector<Customer>& customers);
bool emailExists(const vector<Customer>& customers, string email);
bool validEmail(string email);
string getEmail(const vector<Customer>& customers);
string generateReceiptID();
bool processMembershipPayment(double& amountPaid, double& change);
void addCustomer(vector<Customer>& customers);
void searchCustomer(const vector<Customer>& customers);
void editCustomer(vector<Customer>& customers);
void deleteCustomer(vector<Customer>& customers);
void viewCustomerHistory(const vector<Customer>& customers, const vector<Booking>& bookings);
void CustomerManagement(vector<Customer>& customers, const vector<Booking>& bookings);

// Laundry Booking
string generateBookingID(const vector<Booking>& bookings);
double getWeight();
int getCurrentTimeInMinutes();
bool isLeapYear(int year);
bool isValidDate(int day, int month, int year);
bool isAtLeastTwoDaysLater(int day, int month, int year);
void getOtherCollectionDate(Booking& newOrder);
int findBookingCustomer(const vector<Customer>& customers, Booking& newOrder, const vector<Booking>& bookings);
bool selectService(Booking& newOrder, const vector<Service>& services);
int selectDetergent(Booking& newOrder, const vector<Detergent>& detergents);
void selectCollectionDay(Booking& newOrder);
void selectCollectionTime(Booking& newOrder, const vector<CollectionOption>& collectionOptions);
string displayBookingSummary(Booking& newOrder, const Customer& customer);
void LaundryBooking(const vector<Customer>& customers, vector<Booking>& bookings, const vector<Service>& services, vector<Detergent>& detergents, const vector<CollectionOption>& collectionOptions);

// Order Status
extern string statusName[5];
int findBookingIndex(const vector<Booking>& bookings, const string& bookingID);
void displayBookingInfo(const Booking& booking, const Customer& customer);
double calculateLatePenalty(const Booking& booking, int& minutesLate);
void displayStatusOptions(const Booking& booking);
bool updateBookingStatus(Booking& booking);
void updateOrderStatus(vector<Booking>& bookings, const vector<Customer>& customers);

// Payment
int findBooking(const vector<Booking>& bookings, const string& bookingID);
bool checkBookingStatus(const Booking& booking);
int findCustomer(const vector<Customer>& customers, const string& customerID);
void displayPaymentProcess(const Booking& booking, const Customer& customer, double subtotal, double discount, double totalAfterDiscount, double sst, double totalBeforePoints);
double redeemPoints(Customer& customer, double totalBeforePoints, int& pointsRedeemed);
void displayPaymentTotal(double totalBeforePoints, double pointsDiscount, double latePenalty, double grandTotal);
double getPaymentAmount(double grandTotal);
void generateReceipt(const Booking& booking, const Customer& customer, double subtotal, double discount, double sst, double pointsDiscount, int pointsRedeemed, int pointsEarned, double grandTotal, double amountPaid, double change);
void payment(vector<Booking>& bookings, vector<Customer>& customers);

// Reports
string formatRM(double amount);
int findCustomerByID(const vector<Customer>& customers, const string& id);
bool parseDate(const string& input, Date& out);
bool sameDate(const Date& a, const Date& b);
Date tmToDate(const tm& t);
string currentDateTime();
int weekdayOf(const Date& date);
bool askExportReport();
void exportReportIfRequested(const string& reportContent, const string& defaultName);
void dailyReport(const vector<Customer>& customers, const vector<Booking>& bookings);
void weeklyReport(const vector<Customer>& customers, const vector<Booking>& bookings);
void monthlyReport(const vector<Customer>& customers, const vector<Booking>& bookings);
void yearlyReport(const vector<Customer>& customers, const vector<Booking>& bookings);
void displayUncollectedOrders(const vector<Customer>& customers, const vector<Booking>& bookings);
string getDetergentStatus(const Detergent& detergent);
int findDetergentIndexByCode(const vector<Detergent>& detergents, const string& itemCode);
void restockDetergent(vector<Detergent>& detergents);
void inventoryStatusReport(vector<Detergent>& detergents);
void reportsMenu(vector<Customer>& customers, vector<Booking>& bookings, vector<Service>& services, vector<Detergent>& detergents);

// Data Persistence
string escape(const string& s);
void saveCustomers(const vector<Customer>& customers, const string& path);
void saveBookings(const vector<Booking>& bookings, const string& path);
void saveDetergents(const vector<Detergent>& detergents, const string& path);

vector<string> splitCSVLine(const string& line);
void loadCustomers(vector<Customer>& customers, const string& path);
void loadBookings(vector<Booking>& bookings, const string& path);
void loadDetergents(vector<Detergent>& detergents, const string& path);

#endif
