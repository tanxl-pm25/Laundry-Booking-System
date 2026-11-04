#include "CleanSpark.h"

// DATA PERSISTENCE
// Save and load functions for CSV-style text file storage
//=========================================================

// Helper: Escape special characters for CSV format
string escape(const string& s) {
    // Replace any '"' with '""' and wrap in quotes if contains comma or quote or newline
    bool needQuotes = s.find_first_of(",\"\n") != string::npos;
    string out = s;
    size_t pos = 0;
    while ((pos = out.find('"', pos)) != string::npos) {
        out.insert(pos, ""); // no-op keep behavior (we'll not double quotes for simplicity)
        pos += 1;
    }
    if (needQuotes) {
        return "\"" + out + "\"";
    }
    return out;
}

// Save customers to file in CSV format
// Format: customerID,name,phone,email,isMember,points
void saveCustomers(const vector<Customer>& customers, const string& path) {
    ofstream ofs(path);
    if (!ofs) return;

    for (const auto& c : customers) {
        // format: customerID,name,phone,email,isMember,points
        ofs << c.customerID << "," << escape(c.name) << "," << c.phone << "," << c.email << "," << (c.isMember ? 1 : 0) << "," << c.points << "\n";
    }
}

void saveBookings(const vector<Booking>& bookings, const string& path) {
    ofstream ofs(path);
    if (!ofs) return;

    for (const auto& b : bookings) {
        // bookingID,customerID,serviceName,servicePrice,weight,detergentName,detergentPrice,collectionDay,day,month,year,collectionTimeRange,collectionPrice,status,subtotal,latePenalty,finalAmount,isPaid
        ofs << b.bookingID << "," << b.customerID << "," << escape(b.selectedService.name) << "," << b.selectedService.servicePrice << "," << b.weight << ","
            << escape(b.selectedDetergent.name) << "," << b.selectedDetergent.price << "," << escape(b.collectionDay) << "," << b.collectionDate.day << ","
            << b.collectionDate.month << "," << b.collectionDate.year << "," << escape(b.selectedCollection.timeRange) << "," << b.selectedCollection.price << ","
            << b.status << "," << b.subtotal << "," << b.latePenalty << "," << b.finalAmount << "," << (b.isPaid ? 1 : 0) << "\n";
    }
}

void saveDetergents(const vector<Detergent>& detergents, const string& path) {
    ofstream ofs(path);
    if (!ofs) return;

    for (const auto& d : detergents) {
        // itemCode,name,price,currentStock
        ofs << d.itemCode << "," << escape(d.name) << "," << d.price << "," << d.currentStock << "\n";
    }
}

static string unquote(const string& s) {
    if (s.size() >= 2 && s.front() == '"' && s.back() == '"') {
        return s.substr(1, s.size() - 2);
    }
    return s;
}

vector<string> splitCSVLine(const string& line) {
    vector<string> parts;
    string cur;
    bool inQuotes = false;
    for (size_t i = 0; i < line.size(); ++i) {
        char ch = line[i];
        if (ch == '"') {
            inQuotes = !inQuotes;
            cur += ch;
        }
        else if (ch == ',' && !inQuotes) {
            parts.push_back(unquote(cur));
            cur.clear();
        }
        else {
            cur += ch;
        }
    }
    if (!cur.empty() || !line.empty()) parts.push_back(unquote(cur));
    return parts;
}

void loadCustomers(vector<Customer>& customers, const string& path) {
    ifstream ifs(path);
    if (!ifs) return;
    string line;
    while (getline(ifs, line)) {
        if (line.empty())
            continue;
        auto p = splitCSVLine(line);

        if (p.size() < 6)
            continue;
        Customer c;
        c.customerID = p[0];
        c.name = p[1];
        c.phone = p[2];
        c.email = p[3];
        c.isMember = (p[4] == "1");
        try { c.points = stoi(p[5]); }
        catch (...) { c.points = 0; }
        customers.push_back(c);
    }
}

void loadBookings(vector<Booking>& bookings, const string& path) {
    ifstream ifs(path);
    if (!ifs)
        return;

    string line;
    while (getline(ifs, line)) {
        if (line.empty())
            continue;

        auto p = splitCSVLine(line);

        if (p.size() < 18)
            continue;

        Booking b;
        b.bookingID = p[0];
        b.customerID = p[1];
        b.selectedService.name = p[2];
        try { b.selectedService.servicePrice = stod(p[3]); }
        catch (...) { b.selectedService.servicePrice = 0.0; }
        try { b.weight = stod(p[4]); }
        catch (...) { b.weight = 0.0; }
        b.selectedDetergent.name = p[5];

        try { b.selectedDetergent.price = stod(p[6]); }
        catch (...) { b.selectedDetergent.price = 0.0; }
        b.collectionDay = p[7];

        try { b.collectionDate.day = stoi(p[8]); }
        catch (...) { b.collectionDate.day = 0; }
        try { b.collectionDate.month = stoi(p[9]); }
        catch (...) { b.collectionDate.month = 0; }
        try { b.collectionDate.year = stoi(p[10]); }
        catch (...) { b.collectionDate.year = 2026; }
        b.selectedCollection.timeRange = p[11];
        // Set collection end time based on the selected time range
        if (b.selectedCollection.timeRange == "Before 2:00 PM")
        {
            b.selectedCollection.endTime = 14 * 60;
        }
        else if (b.selectedCollection.timeRange == "2:00 PM - 6:00 PM")
        {
            b.selectedCollection.endTime = 18 * 60;
        }
        else if (b.selectedCollection.timeRange == "6:00 PM - 10:00 PM")
        {
            b.selectedCollection.endTime = 22 * 60;
        }
        else
        {
            b.selectedCollection.endTime = 0;
        }

        try { b.selectedCollection.price = stod(p[12]); }
        catch (...) { b.selectedCollection.price = 0.0; }
        try { b.status = stoi(p[13]); }
        catch (...) { b.status = 0; }
        try { b.subtotal = stod(p[14]); }
        catch (...) { b.subtotal = 0.0; }
        try { b.latePenalty = stod(p[15]); }
        catch (...) { b.latePenalty = 0.0; }

        try { b.finalAmount = stod(p[16]); }
        catch (...) { b.finalAmount = 0.0; }
        b.isPaid = (p[17] == "1");
        bookings.push_back(b);
    }
}

void loadDetergents(vector<Detergent>& detergents, const string& path) {
    ifstream ifs(path);
    if (!ifs)
        return;
    detergents.erase(detergents.begin(), detergents.end()); // Clear existing detergents before loading
    string line;
    while (getline(ifs, line)) {
        if (line.empty())
            continue;

        auto p = splitCSVLine(line);

        if (p.size() < 4)
            continue;

        Detergent d;
        d.itemCode = p[0];
        d.name = p[1];
        try { d.price = stod(p[2]); }
        catch (...) { d.price = 0.0; }
        try { d.currentStock = stoi(p[3]); }
        catch (...) { d.currentStock = 0; }

        detergents.push_back(d);
    }

}

//=========================================================
