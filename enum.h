#ifndef SCREENS_H
#define SCREENS_H

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
using namespace std;

struct Date {
    int day, month, year;
    bool operator==(const Date& other) const {
        return day == other.day && month == other.month && year == other.year;
    }
    string toString() const {
        return to_string(day) + "-" + to_string(month) + "-" + to_string(year);
    }
};

struct Venue {
    string venue;
    string avaEquipment;
    string size;
    string desc;
    vector<Date> datesBooked;
    double price;

    bool operator==(const Venue& other) const {
        return venue == other.venue && avaEquipment == other.avaEquipment && size == other.size;
    }
};

enum Screen {
    LoginRegister,
    home,
    adPage,
    viewEvent,
    newEvent,
    payment,
    comments,
    exiting,
    editEvent,
    Complaint,
    EventBooking,
    MainMenu,
    EventMonitoring
};

struct Equipment {
    int chairs, tables, booths, projectors, bins, helpers, tents;

    int& operator[](size_t index) {
        switch (index) {
            case 0: return chairs;
            case 1: return tables;
            case 2: return booths;
            case 3: return projectors;
            case 4: return bins;
            case 5: return helpers;
            case 6: return tents;
            default: throw out_of_range("Invalid index");
        }
    }
};

struct Participant {
    string partiName;
    bool attended;
};

struct Complaints {
    int eventId;
    int complaintId;
    string complStatus;
    string complType;
    string reporterType;
    string complDesc;
};

struct User {
    string name;
    string password;
    vector<int> reservationID;
    vector<int> owned; // track owned event IDs
};

struct Event {
    string eventStatus;
    int eventId;
    string eventType;
    string eventName;
    Date date;
    string startTime, endTime, timeDuration;
    Venue venue;
    Equipment equipments;
    string eventDesc;
    double paid = 0.00;
    double entryFee = 0.00;
    int expectedPartiQty;
    int actualPartiQty;
    vector<Participant> participants;
    vector<Complaints> complaints;
    User owner;
};

struct Comment {
    string username;
    string comment;
};

// inline printAll
inline void printAll(initializer_list<string> list) {
    int counter = 1;
    for (const auto& item : list) {
        cout << "[" << counter++ << "] " << item << endl;
    }
}

// Screen functions
Screen loginScreen(User& currentUser, vector<User>& users);
Screen bookingScreen(vector<Event>& events, int& eventCount, int eventAvail[12*31][5], Equipment& equipment, User& currentUser, vector<User>& users);
Screen monitoringScreen(User& currentUser, vector<User>& users, vector<Event>& events, 
                        int& eventCount, int eventAvail[12*31][5]);

//Screen exiting();

// Loading functions
vector<Venue> loadVenues();
vector<User> loadUsers(const vector<Event>& events);
void loadEvents(vector<Event>& events, int& eventCount, int eventAvail[12*31][5]);

// Main menu
Screen mainMenuScreen(vector<Event>& events, int& eventCount, int eventAvail[12*31][5]);

// Event management
void checkAvailability(vector<Event>&, int&, int[12*31][5]);
void searchEvents(vector<Event>&, int);
void deleteEvent(vector<Event>&, int&, int[12*31][5]);
void updateEventStatus(vector<Event>&, int, int[12*31][5]);
void partiAttendance(vector<Event>&, int);

// Complaints
void manageComplaint(vector<Event>&, int);
void saveComplaints(const vector<Event>&);
void loadComplaints(vector<Event>&);

// File IO
void saveEvents(vector<Event>&, int&);
void saveUsers(const vector<User>&);
// Others
void pressEnter();
void displayEvent(vector<Event>&, int);
void displaySearchedEvents(vector<Event>&);
bool checkInt(int&);


#endif