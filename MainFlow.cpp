#include "enum.h"
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <vector>
#include <algorithm> 
#include <iomanip>

#define EVENTS_FILE "events.txt"
#define COMPLAINTS_FILE "complaints.txt"
#define VENUE_FILE "Venues.txt"
#define USER_FILE "Users.txt"
#define ORGANISER_FILE "organiser.txt"
using namespace std;

// Forward declarations
Screen loginScreen(User& currentUser, vector<User>& users);
Screen bookingScreen(vector<Event>& events, int& eventCount, int eventAvail[12*31][5], Equipment& equipment, User& currentUser, vector<User>& users);
Screen monitoringScreen(User& currentUser, vector<User>& users, vector<Event>& events, int& eventCount, int eventAvail[12*31][5]);
Screen mainMenuScreen(vector<Event>& events, int& eventCount, int eventAvail[12*31][5]);

// Loading functions
void loadEvents(vector<Event>& events, int& eventCount, int eventAvail[12*31][5]);
vector<User> loadUsers(const vector<Event>& events);
vector<Venue> loadVenues();

int main() {
    Screen currentScreen = LoginRegister;
    vector<Event> events;
    vector<Venue> venues;
    vector<User> users;
    
    Equipment equipment = {0, 0, 0, 0, 0, 0, 0};

    int eventAvail[12*31][5] = {0};
    int eventCount = 0;

    // Load data
    loadEvents(events, eventCount, eventAvail);
    loadComplaints(events);
    venues = loadVenues();
    users = loadUsers(events);

    User currentUser;

    while (currentScreen != exiting) {
        switch (currentScreen) {
            case LoginRegister: {
                currentScreen = loginScreen(currentUser, users);
                break;
            }
            case EventBooking: {
                if (currentUser.name.empty()) {
                    cout << "Error: No user logged in. Returning to login.\n";
                    currentScreen = LoginRegister;
                    break;
                }
                currentScreen = bookingScreen(events, eventCount, eventAvail, equipment, currentUser, users);
                break;
            } 
            case EventMonitoring: {
                if (currentUser.name.empty()) {
                    cout << "Error: No user logged in. Returning to login.\n";
                    currentScreen = LoginRegister;
                    break;
                }
                currentScreen = monitoringScreen(currentUser, users, events, eventCount, eventAvail);
                break;
            } 
            case MainMenu: {
                currentScreen = mainMenuScreen(events, eventCount, eventAvail);
                if (currentScreen == LoginRegister) {
                    currentUser = User{};
                }
                break;
            }
            case home: { // Explicit transition from home to MainMenu
                currentScreen = MainMenu;
                break;
            }
            default: {
                cout << "Invalid screen. Returning to main menu.\n";
                currentScreen = MainMenu;
                break;
            }
        }
    }

    // Save all before exit
    saveEvents(events, eventCount);
    saveUsers(users);
    saveComplaints(events);
    saveOrganiser(events, eventCount);

    // Print the exit message here (always executed on exit)
    cout << "Exiting program...\n";

    return 0;
}

//loadEvents (void)
void loadEvents(vector<Event>& events, int& eventCount, int eventAvail[12*31][5]) {
    ifstream inEventFile(EVENTS_FILE);
    if (!inEventFile) {
        cout << "Error: Could not open event file '" << EVENTS_FILE << "' for reading.\n";
        return;
    }

    string line;
    eventCount = 0;

    while (getline(inEventFile, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string idStr, eventStatusStr, eventTypeStr, eventNameStr, dateStr, timeStr, venueStr, entryFeeStr, equipmentStr, descStr, orgNameStr;

        if (!getline(ss, idStr, '|')) continue;
        if (!getline(ss, eventStatusStr, '|')) continue;
        if (!getline(ss, eventTypeStr, '|')) continue;
        if (!getline(ss, eventNameStr, '|')) continue;
        if (!getline(ss, dateStr, '|')) continue;
        if (!getline(ss, timeStr, '|')) continue;
        if (!getline(ss, venueStr, '|')) continue;
        if (!getline(ss, entryFeeStr, '|')) continue;
        if (!getline(ss, equipmentStr, '|')) continue;
        if (!getline(ss, descStr, '|')) continue;
        getline(ss, orgNameStr);

        Event evt;
        try {
            evt.eventId = stoi(idStr);
            evt.eventStatus = eventStatusStr;
            evt.eventType = eventTypeStr;
            evt.eventName = eventNameStr;
            // Parse date
            if (dateStr.length() == 10 && dateStr[4] == '-' && dateStr[7] == '-') {
                int year = stoi(dateStr.substr(0, 4));
                int month = stoi(dateStr.substr(5, 2));
                int day = stoi(dateStr.substr(8, 2));
                int monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
                if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) monthDays[1] = 29;
                if (month >= 1 && month <= 12 && day >= 1 && day <= monthDays[month - 1]) {
                    evt.date = {day, month, year};
                } else continue;
            } else continue;
            evt.timeDuration = timeStr;
            evt.venue.venue = venueStr;
            evt.entryFee = stod(entryFeeStr);
            // Parse equipment (assume comma-separated)
            stringstream equipSS(equipmentStr);
            string token;
            getline(equipSS, token, ','); evt.equipments.chairs = stoi(token);
            getline(equipSS, token, ','); evt.equipments.tables = stoi(token);
            getline(equipSS, token, ','); evt.equipments.booths = stoi(token);
            getline(equipSS, token, ','); evt.equipments.projectors = stoi(token);
            getline(equipSS, token, ','); evt.equipments.bins = stoi(token);
            getline(equipSS, token, ','); evt.equipments.helpers = stoi(token);
            getline(equipSS, token, ','); evt.equipments.tents = stoi(token);
            evt.eventDesc = descStr;
            evt.organiser.orgaName = orgNameStr; // Adjust if more fields

            // Mark availability
            int dayIndex = (evt.date.month - 1) * 31 + (evt.date.day - 1);
            string venueStrLocal = venueStr;
            if (venueStrLocal.size() >= 6 && venueStrLocal.substr(0, 4) == "Hall") {
                int hall = stoi(venueStrLocal.substr(5)) - 1;
                if (hall >= 0 && hall < 5) {
                    eventAvail[dayIndex][hall] = 1;
                }
            }

            events.push_back(evt);
        } catch (...) {
            continue;
        }
    }
    eventCount = events.size();
    inEventFile.close();
}

//loadVenues
vector<Venue> loadVenues() {
    vector<Venue> venues;
    ifstream input(VENUE_FILE);
    if (!input) {
        cout << "Warning: Could not open venue file '" << VENUE_FILE << "'.\n";
        return venues;
    }

    string line;
    while (getline(input, line)) {
        if (line.empty()) continue;
        
        stringstream ss(line);
        string name, avaEquipment, size, desc;
        vector<Date> datesBooked;
        double price = 0.0;

        if (!getline(ss, name, '|')) continue;
        if (!getline(ss, avaEquipment, '|')) continue;
        if (!getline(ss, size, '|')) continue;
        if (!getline(ss, desc, '|')) continue;
        
        string datesStr;
        if (getline(ss, datesStr, '|')) {
            stringstream datesStream(datesStr);
            string dateToken;
            while (getline(datesStream, dateToken, ',')) {
                if (dateToken.length() >= 10) {
                    try {
                        int year = stoi(dateToken.substr(0, 4));
                        int month = stoi(dateToken.substr(5, 2));
                        int day = stoi(dateToken.substr(8, 2));
                        datesBooked.push_back({day, month, year});
                    } catch (...) {}
                }
            }
        }

        string priceStr;
        if (getline(ss, priceStr)) {
            try {
                price = stod(priceStr);
            } catch (...) {}
        }

        venues.push_back(Venue{name, avaEquipment, size, desc, datesBooked, price});
    }
    input.close();
    return venues;
}

//loadUsers
vector<User> loadUsers(const vector<Event>& events) {
    vector<User> users;
    ifstream input(USER_FILE);

    if (!input) {
        cout << "Warning: Could not open user file '" << USER_FILE << "'.\n";
        return users;
    }

    string line;
    while (getline(input, line)) {
        if (line.empty()) continue;
        
        stringstream ss(line);
        string name, password;
        vector<int> reservation;
        vector<int> owned;

        if (!getline(ss, name, '|')) continue;
        if (!getline(ss, password, '|')) continue;
        
        string resStr;
        if (getline(ss, resStr, '|')) {
            stringstream resStream(resStr);
            string token;
            while (getline(resStream, token, ',')) {
                try {
                    reservation.push_back(stoi(token));
                } catch (...) {}
            }
        }

        string ownStr;
        if (getline(ss, ownStr)) {
            stringstream ownStream(ownStr);
            string token;
            while (getline(ownStream, token, ',')) {
                try {
                    owned.push_back(stoi(token));
                } catch (...) {}
            }
        }

        users.push_back(User{name, password, reservation, owned});
    }
    input.close();
    return users;
}
/*
//mainMenu implementation
Screen mainMenu() {
    system("cls");
    cout << "\n------ Main Menu ------\n";
    printAll({"Check venue availability", "Event booking", "Manage Events", "Exit"});
    int option;
    cin >> option;
    cin.ignore();

    switch (option) {
        case 1:
            cout << "Check availability - Stub\n";
            pressEnter();
            return MainMenu;
        case 2:
            return EventBooking;
        case 3:
            return EventMonitoring;
        case 4:
            return exiting;
        default:
            cout << "Invalid option! Please select again!\n";
            return mainMenuScreen();
    }
}
    */

void checkAvailability(vector<Event> &events, int &eventCount, int eventAvail[12 * 31][5])
{
    cout << "\n------ Check Venue Availability ------\n";
    string date;
    int year, month, day;

    while (true)
    {
        cout << "\nEnter date to check venue availability (yyyy-mm-dd): ";
        cin >> date;

        // First check overall length
        if (date.length() != 10)
        {
            cout << "\n[Invalid date format! Please follow (yyyy-mm-dd) and enter numbers only]\n";
            continue;
        }

        // Check the '-' positions
        if (date[4] != '-' || date[7] != '-')
        {
            cout << "\n[Invalid date format! Please follow (yyyy-mm-dd) and enter numbers only]\n";
            continue;
        }

        // Check that all other characters are digits
        bool validDigits = true;
        for (int i = 0; i < date.size(); i++)
        {
            if (i == 4 || i == 7)
                continue; // skip dashes
            if (!isdigit(date[i]))
            {
                validDigits = false;
                break;
            }
        }
        if (!validDigits)
        {
            cout << "\n[Invalid date input! Please enter numbers]\n";
            continue;
        }

        try
        {
            year = stoi(date.substr(0, 4));
            month = stoi(date.substr(5, 2));
            day = stoi(date.substr(8, 2));
        }
        catch (...)
        {
            cout << "Invalid date input! Please enter numbers.\n";
            continue;
        }

        // Get current time
        time_t now = time(0);
        tm *ltm = localtime(&now);
        int currentYear = 1900 + ltm->tm_year;

        if (year < currentYear || year > currentYear + 5)
        {
            cout << "\n[Year out of allowed range! (" << currentYear << " - " << currentYear + 5 << ") ]\n";
            continue;
        }

        // Handle leap years
        int monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
        {
            monthDays[1] = 29;
        }

        if (month < 1 || month > 12)
        {
            cout << "\n[Invalid month! Must be between 1-12]\n";
            continue;
        }

        if (day < 1 || day > monthDays[month - 1])
        {
            cout << "\n[Invalid day for selected month]\n";
            continue;
        }

        break;
    }

    // Calculate day index
    int dayIndex = (month - 1) * 31 + (day - 1);

    cout << "\n------ Venue Availability on " << date << " ------\n\n";
    cout << "+---------+--------------+\n";
    cout << "|  Venue  |    Status    |\n";
    cout << "+---------+--------------+\n";

    for (int i = 0; i < 5; i++)
    {
        cout << "| " << left << setw(7) << ("Hall " + to_string(i + 1)) << " | ";
        if (eventAvail[dayIndex][i] == 1)
        {
            cout << left << setw(12) << "Booked" << " |\n";
        }
        else
        {
            cout << left << setw(12) << "Available" << " |\n";
        }
    }

    cout << "+---------+--------------+\n";

    cout << "\n[1] Back to Main Menu\n[2] Event booking\n[3] Display events\n[0] Exit\nSelect option: ";
    int option;
    cin >> option;

    switch (option)
    {
    case 1:
        return; // go back to main menu
    case 2:
        //eventBooking(events, eventCount, eventAvail, EVENTS_FILE, ORGANISER_FILE);
        break;
    case 3:
        //eventMonitoring(events, eventCount, eventAvail, EVENTS_FILE, ORGANISER_FILE, COMPLAINTS_FILE);
        break;
    case 0:
        cout << "\n[Exiting program...]\n";
        exit(0);
        break;
    default:
        cout << "Invalid option! Please select again!\n";
        break;
    }
}
