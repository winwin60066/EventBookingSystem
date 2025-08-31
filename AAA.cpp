#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <cctype>
#include <fstream>
#include <ctime>
#include <vector>
using namespace std;

struct Date
{
    int day;
    int month;
    int year;

    bool operator==(const Date &other) const
    {
        return day == other.day && month == other.month && year == other.year;
    }
    string toString() const
    {
        stringstream ss;
        ss << year << "-" << setw(2) << setfill('0') << month
        << "-" << setw(2) << setfill('0') << day;
        return ss.str();
    }
};

struct Venue
{
    string venue;
    string avaEquipment;
    string size;
    string desc;
    vector<Date> datesBooked;
    double price;
};

struct Equipment
{
    int chairs;
    int tables;
    int booths;
    int projectors;
    int bins;
    int helpers;
    int tents;

    int &operator[](size_t index)
    {
        switch (index)
        {
        case 0:
            return chairs;
        case 1:
            return tables;
        case 2:
            return booths;
        case 3:
            return projectors;
        case 4:
            return bins;
        case 5:
            return helpers;
        case 6:
            return tents;
        default:
            throw std::out_of_range("Invalid index");
        }
    }
};

struct Event
{
    int id;
    string eventType;
    string eventName;
    Date date;
    string startTime;
    string endTime;
    string timeDuration;
    Venue venue;
    Equipment equipments;
    string eventDesc;
    double paid = 0.00;
};

void saveEvents(vector<Event>& events, int eventCount, const string &EVENTS_FILE);
void loadEventFromFile(vector<Event>& events, int &eventCount, int eventAvail[12 * 31][5], const string &EVENTS_FILE);
void mainMenu(vector<Event>& events, int &eventCount, int eventAvail[12 * 31][5], const string &EVENTS_FILE);

void displayEvents(vector<Event>& events, int eventCount)
{
    if (eventCount == 0)
    {
        cout << "No events available.\n";
        return;
    }

    cout << left    
         << setw(5) << "No"
         << setw(15) << "Event Type"
         << setw(15) << "Event ID"
         << setw(25) << "Event Name"
         << setw(15) << "Date"
         << setw(20) << "Duration"
         << setw(20) << "Venue"
         << setw(25) << "Description" << endl;

    cout << string(125, '-') << endl;

    for (int i = 0; i < eventCount; i++)
    {
        cout << left
             << setw(5) << i + 1
             << setw(15) << events[i].eventType
             << setw(15) << events[i].id
             << setw(25) << events[i].eventName
             << setw(15) << events[i].date.toString()
             << setw(20) << events[i].timeDuration
             << setw(20) << events[i].venue.venue
             << setw(25) << events[i].eventDesc << endl;
    }
}

void bookingOnDate(vector<Event>& events, int &eventCount, int eventAvail[12 * 31][5], const string &EVENTS_FILE)
{
    Event newEvent;
    char confirmation;
    string date;
    int year, month, day;

    do {
        cout << "\n------ Event Booking ------\n";
        int eventTypeChoice;
        cout << "\n[1] Seminar\n[2] Talk\n\nSelect event type: ";
        cin >> eventTypeChoice;

        if (eventTypeChoice == 1)
            newEvent.eventType = "Seminar";
        else if (eventTypeChoice == 2)
            newEvent.eventType = "Talk";
        else
            newEvent.eventType = "Other";

        cin.ignore();
        cout << "\nEnter your event name: ";
        getline(cin, newEvent.eventName);

        // Date validation loop
        while (true) {
            cout << "\nEnter date to check venue availability (yyyy-mm-dd): ";
            cin >> date;

            // Format check
            if (date.length() != 10 || date[4] != '-' || date[7] != '-') {
                cout << "\n[Invalid date format! Use yyyy-mm-dd]\n";
                continue;
            }

            // Digit check
            bool validDigits = true;
            for (int i = 0; i < date.size(); i++) {
                if (i == 4 || i == 7) continue; // skip dashes
                if (!isdigit(date[i])) {
                    validDigits = false;
                    break;
                }
            }
            if (!validDigits) {
                cout << "\n[Invalid date input! Please enter numbers]\n";
                continue;
            }

            try {
                year = stoi(date.substr(0, 4));
                month = stoi(date.substr(5, 2));
                day = stoi(date.substr(8, 2));
            } catch (...) {
                cout << "\n[Invalid numbers in date]\n";
                continue;
            }

            // Year range check
            time_t now = time(0);
            tm *ltm = localtime(&now);
            int currentYear = 1900 + ltm->tm_year;
            if (year < currentYear || year > currentYear + 5) {
                cout << "\n[Year out of allowed range! (" << currentYear << " - " << currentYear + 5 << ")]\n";
                continue;
            }

            // Leap year & month/day checks
            int monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
            if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
                monthDays[1] = 29;

            if (month < 1 || month > 12) {
                cout << "\n[Invalid month! Must be 1-12]\n";
                continue;
            }
            if (day < 1 || day > monthDays[month - 1]) {
                cout << "\n[Invalid day for this month]\n";
                continue;
            }

            // Save valid date
            newEvent.date = {day, month, year};
            break;
        }

        // Start time validation
        while (true) {
            cout << "\nEnter start time (hh:mm): ";
            cin >> newEvent.startTime;

            if (newEvent.startTime.length() != 5 || newEvent.startTime[2] != ':') {
                cout << "\n[Invalid time format! Please follow (hh:mm) and only enter numbers]\n";
                continue;
            }

            try {
                int startHours = stoi(newEvent.startTime.substr(0, 2));
                int startMinutes = stoi(newEvent.startTime.substr(3, 2));
                if (startHours < 0 || startHours > 23 || startMinutes < 0 || startMinutes > 59) {
                    cout << "\n[Invalid start time values!]\n";
                    continue;
                }
            } catch (...) {
                cout << "Invalid input! Please enter numbers only.\n";
                continue;
            }
            break; // valid start time
        }

        // End time validation
        while (true) {
            cout << "\nEnter end time (hh:mm): ";
            cin >> newEvent.endTime;

            if (newEvent.endTime.length() != 5 || newEvent.endTime[2] != ':') {
                cout << "\n[Invalid time format! Please follow (hh:mm) and only enter numbers]\n";
                continue;
            }

            try {
                int startHours = stoi(newEvent.startTime.substr(0, 2));
                int startMinutes = stoi(newEvent.startTime.substr(3, 2));
                int endHours = stoi(newEvent.endTime.substr(0, 2));
                int endMinutes = stoi(newEvent.endTime.substr(3, 2));

                if (endHours < 0 || endHours > 23 || endMinutes < 0 || endMinutes > 59) {
                    cout << "\n[Invalid end time values!]\n";
                    continue;
                }
                if (endHours < startHours || (endHours == startHours && endMinutes <= startMinutes)) {
                    cout << "\n[End time must be later than start time!]\n";
                    continue;
                }
            } catch (...) {
                cout << "Invalid input! Please enter numbers only.\n";
                continue;
            }
            break; // valid end time
        }

        // Venue selection
        int selectedVenue, venueIndex, dayIndex;
        while (true) {
            cout << "\n---------- Venues ----------\n[1] Hall 1\n[2] Hall 2\n[3] Hall 3\n[4] Hall 4\n[5] Hall 5\nSelect venue: ";
            cin >> selectedVenue;
            venueIndex = selectedVenue - 1;

            if (venueIndex < 0 || venueIndex > 4) {
                cout << "Invalid venue!\n";
                continue;
            }

            dayIndex = (newEvent.date.month - 1) * 31 + (newEvent.date.day - 1);
            if (eventAvail[dayIndex][venueIndex] == 1) {
                cout << "\n[Venue already booked! Please choose again]\n";
                continue;
            }

            eventAvail[dayIndex][venueIndex] = 1;
            newEvent.venue.venue = "Hall " + to_string(selectedVenue);
            break;
        }

        // Equipment selection
        cout << "\n---------- Equipment Selection ----------\n";
        string equipmentNames[] = {"Chairs", "Tables", "Booths", "Projectors", "Bins", "Helpers", "Tents"};

        for (int i = 0; i < 7; i++) {
            int quantity;
            while (true) {
                cout << "Enter number of " << equipmentNames[i] << ": ";
                cin >> quantity;
                
                // Check for input failure (non-numeric input)
                bool validDigits = true;
                if(!isdigit(quantity)){
                    validDigits = false;
                    break;
                }
                if(!validDigits){
                    cout << "\n[Invalid date input! Please enter numbers]\n";
                    continue;
                }
                
                // Check for negative numbers
                if (quantity < 0) {
                    cout << "[Please enter a valid number (0 or positive)]\n";
                    continue;
                }
                
                break;
            }
            newEvent.equipments[i] = quantity;
        }

        cin.ignore();
        cout << "\nEnter event description (if any): ";
        getline(cin, newEvent.eventDesc);

        newEvent.timeDuration = newEvent.startTime + " to " + newEvent.endTime;

        cout << "\n------ Event Summary ------\n";
        cout << "Event Type    : " << newEvent.eventType
             << "\nEvent Name    : " << newEvent.eventName
             << "\nDate          : " << newEvent.date.toString()
             << "\nTime Duration : " << newEvent.timeDuration
             << "\nVenue         : " << newEvent.venue.venue
             << "\nEquipments    : " << newEvent.equipments.chairs << " Chairs, "
             << newEvent.equipments.tables << " Tables, " << newEvent.equipments.booths << " Booths, "
             << newEvent.equipments.projectors << " Projectors, " << newEvent.equipments.bins << " Bins, "
             << newEvent.equipments.helpers << " Helpers, " << newEvent.equipments.tents << " Tents"
             << "\nDescription   : " << newEvent.eventDesc
             << endl;

        cout << "\n['y' - continue/ 'n' - re-enter all details]\nConfirm all details? (y/n): ";
        cin >> confirmation;
        confirmation = tolower(confirmation);

    } while (confirmation != 'y');

    newEvent.id = events.size() + 1;
    events.push_back(newEvent);
    eventCount = events.size();
    cout << "\n[Booking successful!]\n";

    saveEvents(events, eventCount, EVENTS_FILE);

    int option;
    cout << "Do you want to continue (y/n): ";
    cin >> option;
    option = tolower(option);

    do{
        if(option == 'y'){
            mainMenu(events, eventCount, eventAvail, EVENTS_FILE);
        }else(
            
        )
    }while(option!=0)

}


void checkAvailability(vector<Event>& events, int &eventCount, int eventAvail[12 * 31][5], const string &EVENTS_FILE)
{
    cout << "\n------ Check Venue Availability ------\n";
    string date;
    int year, month, day;

    while (true) {
        cout << "\nEnter date to check venue availability (yyyy-mm-dd): ";
        cin >> date;

        
        // First check overall length
        if (date.length() != 10) {
            cout << "\n[Invalid date format! Please follow (yyyy-mm-dd) and enter numbers only]\n";
            continue;
        }

        // Check the '-' positions
        if (date[4] != '-' || date[7] != '-') {
            cout << "\n[Invalid date format! Please follow (yyyy-mm-dd) and enter numbers only]\n";
            continue;
        }

        // Check that all other characters are digits
        bool validDigits = true;
        for (int i = 0; i < date.size(); i++) {
            if (i == 4 || i == 7) continue; // skip dashes
            if (!isdigit(date[i])) {
                validDigits = false;
                break;
            }
        }
        if (!validDigits) {
            cout << "\n[Invalid date input! Please enter numbers]\n";
            continue;
        }

        try {
            year = stoi(date.substr(0, 4));
            month = stoi(date.substr(5, 2));
            day = stoi(date.substr(8, 2));
        } catch (...) {
            cout << "Invalid date input! Please enter numbers.\n";
            continue;
        }


        // Get current time
        time_t now = time(0);
        tm *ltm = localtime(&now);
        int currentYear = 1900 + ltm->tm_year;

        if (year < currentYear || year > currentYear + 5) {
            cout << "\n[Year out of allowed range! (" << currentYear << " - " << currentYear + 5 << ") ]\n";
            continue;
        }

        // Handle leap years
        int monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
            monthDays[1] = 29; // February has 29 days in leap year
        }

        if (month < 1 || month > 12) {
            cout << "\n[Invalid month! Must be between 1-12]\n";
            continue;
        }

        if (day < 1 || day > monthDays[month - 1]) {
            cout << "\n[Invalid day for selected month]\n";
            continue;
        }

        // Calculate cumulative days from start of year
        int dayIndex = 0;
        for (int i = 0; i < month - 1; i++) {
            dayIndex += monthDays[i];
        }
        dayIndex += (day - 1);

        // Check if dayIndex is within bounds (0 to 365/366)
        int maxDays = 365;
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
            maxDays = 366;
        }

        if (dayIndex < 0 || dayIndex >= maxDays) {
            cout << "Date calculation error! Index out of bounds.\n";
            continue;
        }

        // Ensure we don't exceed array bounds
        if (dayIndex >= 12 * 31) {
            cout << "Date exceeds maximum supported date range.\n";
            continue;
        }

        break;
    }

    // Now, date is valid and you can use year, month, day, dayIndex
    int dayIndex = 0;
    int monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        monthDays[1] = 29;
    }
    for (int i = 0; i < month - 1; i++) {
        dayIndex += monthDays[i];
    }
    dayIndex += (day - 1);

    cout << "\n------ Venue Availability on " << date << " ------\n\n";
    cout << "+---------+--------------+\n";
    cout << "|  Venue  |    Status    |\n";
    cout << "+---------+--------------+\n";

    for (int i = 0; i < 5; i++) {
        cout << "| " << left << setw(7) << ("Hall " + to_string(i + 1)) << " | ";
        if (eventAvail[dayIndex][i] == 1) {
            cout << left << setw(12) << "Booked" << " |\n";
        } else {
            cout << left << setw(12) << "Available" << " |\n";
        }
}


    cout << "+---------+--------------+\n";

    cout << "\n[1] Back to Main Menu\n[2] Event booking\n[3] Display events\n[0] Exit\nSelect option: ";
    int option;
    cin >> option;

    do {
        switch (option) {
            case 1:
                mainMenu(events, eventCount, eventAvail, EVENTS_FILE);
                break;
            case 2:
                bookingOnDate(events, eventCount, eventAvail, EVENTS_FILE);
                break;
            case 3:
                displayEvents(events, eventCount);
                break;
            case 0:
                cout << "\nExiting program...\n";
                break;
            default:
                cout << "Invalid option! Please select again!\n";
        }
    } while (option != 0);
}

void saveEvents(vector<Event>& events, int eventCount, const string &EVENTS_FILE)
{
    ofstream outEventFile(EVENTS_FILE);
    if (!outEventFile)
    {
        cout << "Error saving events\n";
        return;
    }

    for (int i = 0; i < eventCount; i++)
    {
        outEventFile << events[i].id << "|"
                     << events[i].eventType << "|"
                     << events[i].eventName << "|"
                     << events[i].date.toString() << "|"
                     << events[i].timeDuration << "|"
                     << events[i].venue.venue << "|"
                     << "Chairs: " << events[i].equipments.chairs << ", "
                     << "Tables: " << events[i].equipments.tables << ", "
                     << "Booths: " << events[i].equipments.booths << ", "
                     << "Projectors: " << events[i].equipments.projectors << ", "
                     << "Bins: " << events[i].equipments.bins << ", "
                     << "Helpers: " << events[i].equipments.helpers << ", "
                     << "Tents: " << events[i].equipments.tents << "|"
                     << events[i].eventDesc << "\n";
    }
    outEventFile.close();
}

void loadEventFromFile(vector<Event>& events, int &eventCount, int eventAvail[12 * 31][5], const string &EVENTS_FILE)
{
    ifstream inEventFile(EVENTS_FILE);
    if (!inEventFile)
    {
        return; // no file yet
    }

    string line;
    eventCount = 0;

    while (getline(inEventFile, line))
    {
        if (line.empty()) continue;

        stringstream ss(line);
        string idStr, eventTypeStr, eventNameStr, dateStr, timeStr, venueStr, equipmentStr, descStr;

        // Get all fields separated by |
        if (!getline(ss, idStr, '|')) continue;
        if (!getline(ss, eventTypeStr, '|')) continue;
        if (!getline(ss, eventNameStr, '|')) continue;
        if (!getline(ss, dateStr, '|')) continue;
        if (!getline(ss, timeStr, '|')) continue;
        if (!getline(ss, venueStr, '|')) continue;
        if (!getline(ss, equipmentStr, '|')) continue;
        getline(ss, descStr); // Last field, no delimiter check needed

        Event evt;
        
        // Parse ID with error handling
        try {
            evt.id = stoi(idStr);
        } catch (...) {
            continue; // Skip this line if ID is invalid
        }
        
        evt.eventType = eventTypeStr;
        evt.eventName = eventNameStr;
        evt.timeDuration = timeStr;
        evt.venue.venue = venueStr;
        evt.eventDesc = descStr;

        // Parse date yyyy-mm-dd with error handling
        try {
            if (dateStr.length() >= 10) {
                int year = stoi(dateStr.substr(0, 4));
                int month = stoi(dateStr.substr(5, 2));
                int day = stoi(dateStr.substr(8, 2));
                evt.date = {day, month, year};
            } else {
                continue; // Skip if date format is wrong
            }
        } catch (...) {
            continue; // Skip this line if date parsing fails
        }

        // Parse equipment string "x,y,z,..." with error handling
        try {
            stringstream es(equipmentStr);
            string token;
            vector<int> eq;
            
            while (getline(es, token, ',')) {
                if (!token.empty()) {
                    eq.push_back(stoi(token));
                } else {
                    eq.push_back(0); // Default to 0 if empty
                }
            }

            // Ensure we have exactly 7 equipment values
            while (eq.size() < 7) {
                eq.push_back(0); // Fill missing values with 0
            }

            if (eq.size() >= 7) {
                evt.equipments.chairs = eq[0];
                evt.equipments.tables = eq[1];
                evt.equipments.booths = eq[2];
                evt.equipments.projectors = eq[3];
                evt.equipments.bins = eq[4];
                evt.equipments.helpers = eq[5];
                evt.equipments.tents = eq[6];
            }
        } catch (...) {
            // If equipment parsing fails, set all to 0
            evt.equipments.chairs = 0;
            evt.equipments.tables = 0;
            evt.equipments.booths = 0;
            evt.equipments.projectors = 0;
            evt.equipments.bins = 0;
            evt.equipments.helpers = 0;
            evt.equipments.tents = 0;
        }

        // Mark availability with error handling
        try {
            int dayIndex = (evt.date.month - 1) * 31 + (evt.date.day - 1);
            if (dayIndex >= 0 && dayIndex < 12 * 31) { // Check bounds
                if (venueStr.size() >= 5 && venueStr.substr(0,4) == "Hall") {
                    int hall = stoi(venueStr.substr(5)) - 1; // "Hall 1" -> get "1"
                    if (hall >= 0 && hall < 5) {
                        eventAvail[dayIndex][hall] = 1;
                    }
                }
            }
        } catch (...) {
            // Skip availability marking if it fails
        }

        events.push_back(evt);
    }
    
    eventCount = events.size();
    inEventFile.close();
}


void mainMenu(vector<Event>& events, int &eventCount, int eventAvail[12 * 31][5], const string &EVENTS_FILE)
{
    int option;
    do
    {
        cout << "\n------ Main Menu ------\n";
        cout << "[1] Check venue availability\n";
        cout << "[2] Event booking\n";
        cout << "[3] Display events\n";
        cout << "[0] Exit\n";
        cout << "\nSelect your option: ";
        cin >> option;

        switch (option)
        {
        case 1:
            checkAvailability(events, eventCount, eventAvail, EVENTS_FILE);
            break;
        case 2:
            bookingOnDate(events, eventCount, eventAvail, EVENTS_FILE);
            break;
        case 3:
            displayEvents(events, eventCount);
            break;
        case 0:
            cout << "\nExiting program...\n";
            break;
        default:
            cout << "Invalid option! Please select again!\n";
        }
    } while (option != 0);
}

int main()
{
    vector<Event> events;
    int eventCount = 0;
    int eventAvail[12 * 31][5] = {0};
    const string EVENTS_FILE = "events.txt";

    loadEventFromFile(events, eventCount, eventAvail, EVENTS_FILE);
    mainMenu(events, eventCount, eventAvail, EVENTS_FILE);
    return 0;
}
