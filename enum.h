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
	};
};

struct Venue
{
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

//TODO
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
    //
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

struct Organiser {
    string orgaName;
    string orgaPhoneNo;
    string orgaEmail;
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
};

struct Event {
    string eventStatus;
    int eventId; //or id
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
    Organiser orgaName;
    Organiser orgaPhoneNo;
    Organiser orgaEmail;
    vector<Participant> participants;
    vector<Complaints> complaints;
    User owner; //TODO not sure for what
};

struct Comment {
	string username;
	string comment;
};

// inline is to allow multiple modules to implement this
inline void printAll(initializer_list<string> list) {
	int counter = 1;
	for (const auto& b : list) {
		cout << "press " << counter << " for " << b << "\n";
		counter++;
	}
}

inline void pressEnter() {
	cout << "press enter to continue";
	cin.ignore(9999, '\n');
}
Screen loginScreen(User& currentUser, vector<User>& users);
Screen monitoringScreen(User& currentUser, vector<User>& users, vector<Event>& events, int& eventCount, int eventAvail[12*31][5], const string& EVENTS_FILE, const string& COMPLAINTS_FILE);
Screen bookingScreen(vector<Event> &events, vector<Venue> venues, User currentUser);



Screen newEventScreen(vector<Event>&, vector<Venue>, User);

void switchScreen();
void switchBack();
bool checkInt(int&);
bool checkValidDate(Event);
Event getDate(Event);
vector<Venue> getValidVenues(vector<Venue>, Date);
Venue getVenue(vector<Venue>&, Date);
Equipment getEquipment(Equipment);

Screen editEventScreen(vector<Event>&, vector<Venue>, int);

void searchEvents(vector<Event>& events, int eventCount);

Screen menu();


// main menu
void mainMenu(vector<Event>&, int&, int[12*31][5],
              const string&, const string&, const string&);

// event management
void eventBooking(vector<Event>&, int&, int[12*31][5],
                  const string&, const string&);
void checkAvailability(vector<Event>&, int&, int[12*31][5],
                       const string&, const string&, const string&);
void eventMonitoring(vector<Event>&, int, int[12*31][5],
                     const string&, const string&, const string&);
void searchEvents(vector<Event>&, int);
void deleteEvent(vector<Event>&, int&, int[12*31][5], const string&);
void updateEventStatus(vector<Event>&, int, int[12*31][5],
                       const string&, const string&, const string&);
void partiAttendance(vector<Event>&, int);

// complaints
void manageComplaint(vector<Event>&, int, const string&);
void saveComplaints(const vector<Event>&, const string&);
void loadComplaints(vector<Event>&, const string&);

// file io
void saveEvents(vector<Event>&, int, const string&);
void loadEventFromFile(vector<Event>&, int&, int[12*31][5], const string&);
void saveOrganiser(vector<Event>&, int, const string&);
void loadOrganiserFromFile(vector<Event>&, const string&);

// 
void pressEnterToContinue();
void displayEvent(vector<Event>&, int);
void displaySearchedEvents(vector<Event>&);

#endif
