#include "enum.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
using namespace std;



//eventMonitoring
void eventMonitoring(vector<Event> &events, int eventCount, int eventAvail[12 * 31][5], const string &EVENTS_FILE, const string &ORGANISER_FILE, const string &COMPLAINTS_FILE)
{
    if (eventCount == 0)
    {
        cout << "\n[No events available]\n";
        pressEnterToContinue();
        return; // FIXED: Changed from recursive call to return
    }

    // Main loop only for actions
    while (true)
    {
        int option;
        cout << "\n----- Event Monitoring -----\n";
        cout << "\n[1] Display all event list\n[2] Search event\n[3] Delete event\n[4] Update/Manage event status\n[5] Check/Manage Participants Attendance\n[6] Add/View Complaint\n[7] Generate Report\n[8] Back to Main Menu\nSelect option: ";
        cin >> option;
        cin.ignore(); // clear newline

        switch (option)
        {
        case 1:
            displayEvent(events, eventCount);
            pressEnterToContinue();
            break;
        case 2:
            searchEvents(events, eventCount);
            break;
        case 3:
            deleteEvent(events, eventCount, eventAvail, EVENTS_FILE);
            break;
        case 4:
            updateEventStatus(events, eventCount, eventAvail, EVENTS_FILE, ORGANISER_FILE, COMPLAINTS_FILE);
            break;
        case 5:
            partiAttendance(events, eventCount);
            break;
        case 6: 
            manageComplaint(events, eventCount, COMPLAINTS_FILE);
            break;
        case 7:
            cout << "generate report";
            break;
        case 8: 
            return; // FIXED: Changed from recursive call to return
        default:
            cout << "Invalid input! Try again.\n";
            break;
        }
    }
}


//searchEvents
void searchEvents(vector<Event> &events, int eventCount)
{
    vector<Event> filteredEvents;

    int choice;
    cout << "\nSearch event by:\n";
    cout << "[1] Event Type\n[2] Event ID\n[3] Event Name\n[4] Date (yyyy-mm-dd)\n[5] Venue\n";
    cout << "Select option: ";
    cin >> choice;
    cin.ignore(); // clear newline

    string keyword, searchEventType;

    if (choice == 1)
    {
        int option;
        cout << "[1] Seminar\n[2] Talk\nSelect event type: ";
        cin >> option;
        cin.ignore();
        if (option == 1)
            searchEventType = "Seminar";
        else if (option == 2)
            searchEventType = "Talk";
        else
        {
            cout << "Invalid option!\n";
            return;
        }
    }
    else
    {
        cout << "Enter keyword: ";
        getline(cin, keyword);
    }

    for (int i = 0; i < eventCount; i++)
    {
        bool match = false;
        if (choice == 1)
            match = (events[i].eventType == searchEventType);
        else if (choice == 2) {
            // FIXED: Added proper error handling for string to int conversion
            try {
                match = (events[i].eventId == stoi(keyword));
            } catch (...) {
                cout << "Invalid event ID format!\n";
                return;
            }
        }
        else if (choice == 3)
            // FIXED: Corrected search logic
            match = (events[i].eventName.find(keyword) != string::npos);
        else if (choice == 4)
            match = (events[i].date.toString() == keyword);
        else if (choice == 5)
            // FIXED: Corrected search logic
            match = (events[i].venue.venue.find(keyword) != string::npos);

        if (match)
            filteredEvents.push_back(events[i]);
    }

    if (filteredEvents.empty())
    {
        cout << "No events found.\n";
    }
    else
    {
        displaySearchedEvents(filteredEvents);
    }
    pressEnterToContinue();
}


//deleteEvent
void deleteEvent(vector<Event> &events, int &eventCount, int eventAvail[12 * 31][5], const string &EVENTS_FILE)
{
    if (eventCount == 0)
    {
        cout << "No events available to delete.\n";
        return;
    }

    displayEvent(events, eventCount); // Show events first

    int index;
    cout << "\nEnter the event number to delete (0 to cancel): ";
    cin >> index;

    if (index == 0)
    {
        cout << "\n[Delete cancelled]\n";
        return;
    }

    if (index < 1 || index > eventCount)
    {
        cout << "Invalid event number.\n";
        return;
    }

    // Ask for confirmation
    char confirm;
    cout << "Are you sure you want to delete event \""
         << events[index - 1].eventName << "\"? (y/n): ";
    cin >> confirm;

    if (tolower(confirm) == 'y')
    {
        // Free up the venue availability
        Event &eventToDelete = events[index - 1];
        int dayIndex = (eventToDelete.date.month - 1) * 31 + (eventToDelete.date.day - 1);
        string venueStr = eventToDelete.venue.venue;

        if (venueStr.size() >= 6 && venueStr.substr(0, 4) == "Hall")
        {
            try
            {
                int hall = stoi(venueStr.substr(5)) - 1; // "Hall 1" -> get "1"
                if (hall >= 0 && hall < 5 && dayIndex >= 0 && dayIndex < 12 * 31)
                {
                    eventAvail[dayIndex][hall] = 0; // Free the venue
                }
            }
            catch (...)
            {
                // Ignore if venue parsing fails
            }
        }

        events.erase(events.begin() + (index - 1));
        eventCount--;

        // Update IDs to be sequential
        for (int i = 0; i < eventCount; i++)
        {
            events[i].eventId = i + 1;
        }

        cout << "Event deleted successfully.\n";
        saveEvents(events, eventCount, EVENTS_FILE); // Save after deletion
    }
    else
    {
        cout << "Delete cancelled.\n";
    }
}


//updateEventStatus
void updateEventStatus(vector<Event> &events, int eventCount, int eventAvail[12 * 31][5], const string &EVENTS_FILE, const string &ORGANISER_FILE, const string &COMPLAINTS_FILE) {
    int updateStatus;
    cout << "\n------ Update Event Status ------\n";
    cout << "Enter event ID to update status or [55] to search event: ";
    cin >> updateStatus;

    if (updateStatus == 55) {
        searchEvents(events, eventCount);
        return; // after search, exit this function
    }

    bool found = false;
    for (int i = 0; i < eventCount; i++) {
        if (events[i].eventId == updateStatus) {
            int option;
            do {
                cout << "\n----- Update Status -----\n"
                     << "[1] Ongoing\n"
                     << "[2] Completed\n"
                     << "[3] Cancelled\n"
                     << "Select option: ";
                cin >> option;

                switch (option) {
                    case 1:
                        events[i].eventStatus = "Ongoing";
                        break;
                    case 2: 
                        events[i].eventStatus = "Completed";
                        break;
                    case 3:
                        events[i].eventStatus = "Cancelled";
                        break;
                    default:
                        cout << "\nInvalid option. Please try again.\n";
                }
            } while (option < 1 || option > 3);

            cout << "\n[Status Updated Successfully!]\n";
            saveEvents(events, eventCount, EVENTS_FILE);
            pressEnterToContinue();
            // FIXED: Removed recursive call
            found = true;
            break;
        }
    }

    if (!found) {
        cout << "Event ID not found.\n";
    }
}


//partiAttendance
void partiAttendance(vector<Event> &events, int eventCount) {
    cout << "\n----- Participant Attendance Management ------\n";
    cout << "[1] Check Participant Attendance\n";
    cout << "[2] Update Participant Attendance\n";
    cout << "Select option: ";
    
    int option;
    cin >> option;
    cin.ignore(); // clear newline left by cin

    switch (option) {
    case 1: {
        string partiName;
        cout << "\n----- Check Participant Attendance -----\n";
        cout << "Enter participant name: ";
        getline(cin, partiName);

        bool found = false;
        for (int i = 0; i < eventCount; i++) {
            for (size_t j = 0; j < events[i].participants.size(); j++) { 
                if (events[i].participants[j].partiName == partiName) {
                    cout << "Participant " << partiName 
                         << " found in Event: " << events[i].eventName 
                         << " | Attendance: " 
                         << (events[i].participants[j].attended ? "Present" : "Absent")
                         << endl;
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
        cout << "\n----- Update Participant Attendance -----\n";
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
