#include "enum.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <regex>
#include <string>
#define EVENTS_FILE "events.txt"
#define COMPLAINTS_FILE "complaints.txt"
#define VENUE_FILE "Venues.txt"
#define USER_FILE "Users.txt"
#define ORGANISER_FILE "organiser.txt"
using namespace std;

void displayEvent(vector<Event> &events, int eventCount);
void pressEnterToContinue();
void displaySearchedEvents(vector<Event> &filteredEvents);
void manageComplaint(vector<Event> &events, int eventCount);


//searchEvents
void searchEvents(vector<Event> &events, int eventCount) {
    vector<Event> filteredEvents;

    int choice;
    cout << "\nSearch event by:\n";
    cout << "[1] Event Type\n[2] Event ID\n[3] Event Name\n[4] Date (yyyy-mm-dd)\n[5] Venue\n";
    cout << "Select option: ";
    cin >> choice;
    cin.ignore(); // clear newline

    string keyword, searchEventType, otherType;

    if (choice == 1) {
        int option;
        cout << "[1] Seminar\n[2] Talk\n[3] Conference\n[4] Other\nSelect event type: ";
        cin >> option;
        cin.ignore();
        if (option == 1)
            searchEventType = "Seminar";
        else if (option == 2)
            searchEventType = "Talk";
        else if (option == 3)
            searchEventType = "Conference";
        else if (option == 4) {
            cout << "Enter other type: ";
            getline(cin, otherType);
            searchEventType = otherType;
        } else {
            cout << "Invalid option!\n";
            return;
        }
    } else if (choice >= 2 && choice <= 5) {
        cout << "Enter keyword: ";
        getline(cin, keyword);
    } else {
        cout << "Invalid option!\n";
        return;
    }

    // Helper function for case-insensitive comparison
    auto caseInsensitiveFind = [](const string& text, const string& pattern) {
    if (pattern.empty()) return false;
    return regex_search(text, regex(pattern, regex_constants::icase));
    };

    for (int i = 0; i < eventCount; i++) {
        bool match = false;
        
        if (choice == 1) {
            match = caseInsensitiveFind(events[i].eventType, searchEventType);
        } else if (choice == 2) {
            match = caseInsensitiveFind(to_string(events[i].eventId), keyword);
        } else if (choice == 3) {
            match = caseInsensitiveFind(events[i].eventName, keyword);
        } else if (choice == 4) {
            match = caseInsensitiveFind(events[i].date.toString(), keyword);
        } else if (choice == 5) {
            match = caseInsensitiveFind(events[i].venue.venue, keyword);
        }
        
        if (match) {
            filteredEvents.push_back(events[i]);
        }
    }

    displaySearchedEvents(filteredEvents);
}

// Stub for updateEventStatus (implement logic as needed)
void updateEventStatus(vector<Event>& events, int eventCount, int eventAvail[12*31][5]) {
    // Example stub: Update status for an event
    int eventId;
    cout << "Enter Event ID to update status: ";
    cin >> eventId;
    for (int i = 0; i < eventCount; i++) {
        if (events[i].eventId == eventId) {
            cout << "Enter new status: ";
            cin >> events[i].eventStatus;
            saveEvents(events, eventCount);
            return;
        }
    }
    cout << "Event not found.\n";
}

// Stub for deleteEvent
void deleteEvent(vector<Event>& events, int& eventCount, int eventAvail[12*31][5]) {
    int eventId;
    cout << "Enter Event ID to delete: ";
    cin >> eventId;
    for (auto it = events.begin(); it != events.end(); ++it) {
        if (it->eventId == eventId) {
            // Update availability
            int dayIndex = (it->date.month - 1) * 31 + (it->date.day - 1);
            string venueStr = it->venue.venue;
            if (venueStr.size() >= 6 && venueStr.substr(0, 4) == "Hall") {
                int hall = stoi(venueStr.substr(5)) - 1;
                if (hall >= 0 && hall < 5) {
                    eventAvail[dayIndex][hall] = 0;
                }
            }
            events.erase(it);
            eventCount = events.size();
            saveEvents(events, eventCount);
            return;
        }
    }
    cout << "Event not found.\n";
}

//partiAttendance (completed based on partial code)
void partiAttendance(vector<Event>& events, int eventCount) {
    int option;
    cout << "\n----- Participant Attendance -----\n";
    cout << "[1] Check Attendance\n[2] Update Attendance\nSelect option: ";
    cin >> option;
    cin.ignore();

    switch (option) {
        case 1: {
            string partiName;
            cout << "Enter participant name: ";
            getline(cin, partiName);

            bool found = false;
            for (int i = 0; i < eventCount; i++) {
                for (size_t j = 0; j < events[i].participants.size(); j++) {
                    if (events[i].participants[j].partiName == partiName) {
                        cout << "Event: " << events[i].eventName << " - Status: " 
                             << (events[i].participants[j].attended ? "Present" : "Absent") << endl;
                        found = true;
                    }
                }
            }
            if (!found) {
                cout << "Participant not found in any event.\n";
            }
            break;
        }
        case 2: {
            string partiName;
            cout << "Enter participant name: ";
            getline(cin, partiName);

            bool updated = false;
            for (int i = 0; i < eventCount; i++) {
                for (size_t j = 0; j < events[i].participants.size(); j++) { 
                    if (events[i].participants[j].partiName == partiName) {
                        cout << "Mark as present? (y/n): ";
                        char ch;
                        cin >> ch;
                        events[i].participants[j].attended = (tolower(ch) == 'y');
                        cout << "\n[Attendance updated]\n";
                        updated = true;
                    }
                }
            }
            if (!updated) {
                cout << "Participant not found.\n";
            }
            break;
        }
        default:
            cout << "Invalid option!\n";
    }
}

//monitoringScreen
Screen monitoringScreen(User& currentUser, vector<User>& users, vector<Event>& events, int& eventCount, int eventAvail[12*31][5]) {
    system("cls");
    while (true) {
        cout << "\n---------- Event Management ----------\n";
        printAll({"Display My Events","Search Event","Update/Manage Event Status","Check/Manage Participants Attendance","Add/View Complaints from Participants","Delete Event","Generate Report","Back to Main Menu","Exit"});
        int input;
        cin >> input;
        cin.ignore();

        switch(input) {
            case 1: {
                vector<Event> ownedEvents;
                for (int i = 0; i < eventCount; i++) {
                    if (find(currentUser.owned.begin(), currentUser.owned.end(), events[i].eventId) != currentUser.owned.end()) {
                        ownedEvents.push_back(events[i]);
                    }
                }
                displaySearchedEvents(ownedEvents);
                pressEnter();
                break;
            }
            case 2:
                searchEvents(events, eventCount);
                pressEnter();
                break;
            case 3:
                updateEventStatus(events, eventCount, eventAvail);
                pressEnter();
                break;
            case 4:
                partiAttendance(events, eventCount);
                pressEnter();
                break;
            case 5:
                manageComplaint(events, eventCount);
                pressEnter();
                break;
            case 6:
                deleteEvent(events, eventCount, eventAvail);
                pressEnter();
                break;
            case 7:
                cout << "Generate Report - Stub implementation\n"; // Add real report logic if needed
                pressEnter();
                break;
            case 8: 
                return MainMenu;
            case 9: return exiting;
            default:
                cout << "Invalid option! Please try again!\n";
                pressEnter();
                break;
        }
        saveEvents(events, eventCount); // Save after changes
        saveUsers(users); // Save users if needed
    }
    return EventMonitoring; // Should not reach here
}