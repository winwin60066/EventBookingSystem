#include "enum.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>
#include <ctime>
#include <cctype>
#define EVENTS_FILE "events.txt"
#define COMPLAINTS_FILE "complaints.txt"
#define VENUE_FILE "Venues.txt"
#define USER_FILE "Users.txt"
#define ORGANISER_FILE "organiser.txt"
using namespace std;

// Forward declarations
bool checkInt(int&);
void pressEnter();
void printAll(const vector<string>& items);
Screen monitoringScreen(User& currentUser, vector<User>& users, vector<Event>& events, int& eventCount, int eventAvail[12*31][5]);
Screen mainMenu();

// Function declarations
Venue getVenue(vector<Venue>& venues, Date date);
Event getDate(Event event);
vector<Venue> getValidVenues(vector<Venue> venues, Date date);
Equipment getEquipment(Equipment equipment);

// Screen switching functions
void switchScreen() {
    cout << "\033[?1049h";
}

void switchBack() {
    cout << "\033[?1049l";
}

//bookingScreen
Screen bookingScreen(vector<Event> &events, int &eventCount, int eventAvail[12 * 31][5], Equipment &equipment, User& currentUser, vector<User>& users) {
    Event newEvent;
    char confirmation;
    string selfType;

    do {
        cout << "\n------ Event Booking ------\n";
        int eventTypeChoice;
        cout << "\n[1] Seminar\n[2] Talk\n[3] Conference\n[4] Others(self type)\nSelect event type: ";
        cin >> eventTypeChoice;

        if (eventTypeChoice == 1)
            newEvent.eventType = "Seminar";
        else if (eventTypeChoice == 2)
            newEvent.eventType = "Talk";
        else if (eventTypeChoice == 3)
            newEvent.eventType = "Conference";
        else if (eventTypeChoice == 4) {
            cout << "Enter other type: ";
            cin >> selfType;
            newEvent.eventType = selfType;
        } else {
            cout << "Invalid choice! Please try again.\n";
            continue;
        }
            
        cin.ignore();
        while (newEvent.eventName.empty()) {
            cout << "Insert event name: ";
            getline(cin, newEvent.eventName);
            if (newEvent.eventName.empty()) {
                cout << "[Event name cannot be empty. Please try again!]\n";
            }
        }

        // Date validation loop
        bool validDate = false;
        while (!validDate) {
            cout << "\n[Enter Event Date]\n";
            cout << "Day: ";
            if (!checkInt(newEvent.date.day)) {
                cout << "Invalid day.\n";
                continue;
            }
            cout << "Month: ";
            if (!checkInt(newEvent.date.month)) {
                cout << "Invalid month.\n";
                continue;
            }
            cout << "Year: ";
            if (!checkInt(newEvent.date.year)) {
                cout << "Invalid year.\n";
                continue;
            }

            // Validate date
            int monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
            if ((newEvent.date.year % 4 == 0 && newEvent.date.year % 100 != 0) || (newEvent.date.year % 400 == 0)) {
                monthDays[1] = 29;
            }
            if (newEvent.date.month < 1 || newEvent.date.month > 12 || newEvent.date.day < 1 || newEvent.date.day > monthDays[newEvent.date.month - 1]) {
                cout << "Invalid date! Please try again.\n";
            } else {
                validDate = true;
            }
        }

        // Get time, venue, equipment, etc. (assume truncated code is here, add stubs if needed)
        // Example: newEvent.startTime = ...;
        // newEvent.venue = getVenue(...);
        // newEvent.equipments = getEquipment(...);
        // newEvent.organiser.orgaName = ...;
        // etc.

        cout << "Enter event description: ";
        getline(cin, newEvent.eventDesc);

        // Set defaults
        newEvent.eventStatus = "Scheduled";
        newEvent.expectedPartiQty = 0; // Prompt if needed
        newEvent.actualPartiQty = 0;
        newEvent.paid = 0.0;

        // Assign ownership
        newEvent.owner = currentUser;

        // Add to events
        newEvent.eventId = events.size() + 1;
        events.push_back(newEvent);
        currentUser.owned.push_back(newEvent.eventId);

        // Update users vector with updated currentUser
        for (auto& u : users) {
            if (u.name == currentUser.name) {
                u.owned = currentUser.owned;
                break;
            }
        }

        eventCount = events.size();
        saveEvents(events, eventCount);
        saveUsers(users);

        cout << "Book another event? (y/n): ";
        cin >> confirmation;
        cin.ignore();
    } while (tolower(confirmation) == 'y');

    return MainMenu; // Or appropriate screen
}

//getVenue (stub)
Venue getVenue(vector<Venue>& venues, Date date) {
    // Implement logic to select venue
    return venues[0]; // Stub
}

//getDate (stub)
Event getDate(Event event) {
    // Implement
    return event;
}

//getValidVenues (stub)
vector<Venue> getValidVenues(vector<Venue> venues, Date date) {
    // Implement
    return venues;
}

//getEquipment
Equipment getEquipment(Equipment equipment) {
    string equipNames[] = {"chairs", "tables", "booths", "projectors", "bins", "helpers", "tents"};
    int equipLength = sizeof(equipNames) / sizeof(equipNames[0]);

    switchScreen();
    system("cls");
    int input = 1;
    int equipNum = 0;
    while (input != 0) {
        cout << "Are there any equipments that you would like to borrow?\n\n";
        cout << "You have currently borrowed: \n";
        for (int i = 0; i < equipLength; i++) {
            cout << left << setw(11) << equipNames[i] << " : " << equipment[i] << "\n";
        }
        cout << "\n";
        printAll({ "chairs", "tables", "booths", "projectors", "bins", "helpers", "tents" });
        cout << "press 0 when done\n";
        cout << "Select equipment (1-" << equipLength << "): ";
        if (!checkInt(input)) {
            cout << "please insert a valid number\n";
            pressEnter();
            continue;
        }
        if (input == 0) break;
        if (input < 1 || input > equipLength) {
            cout << "please insert 1 - " << equipLength << "\n";
            pressEnter();
            continue;
        }
        // get number of equipment
        while (true) {
            cout << "How many " << equipNames[input - 1] << " would you like to borrow: ";
            if (!checkInt(equipNum)) {
                cout << "please insert a valid number\n";
                pressEnter();
            } else if (equipNum < 0) {
                cout << "Invalid number of equipment borrowed\n\n";
                pressEnter();
            } else {
                switch(input - 1) {
                    case 0: equipment.chairs = equipNum; break;
                    case 1: equipment.tables = equipNum; break;
                    case 2: equipment.booths = equipNum; break;
                    case 3: equipment.projectors = equipNum; break;
                    case 4: equipment.bins = equipNum; break;
                    case 5: equipment.helpers = equipNum; break;
                    case 6: equipment.tents = equipNum; break;
                }
                break;
            }
        }
        equipNum = 0;
        system("cls");
    }
    system("cls");
    switchBack();
    return equipment;
}

//checkInt
bool checkInt(int& num) {
    if (cin >> num) {
        return true;
    }
    cin.clear();
    cin.ignore(9999, '\n');
    return false;
}