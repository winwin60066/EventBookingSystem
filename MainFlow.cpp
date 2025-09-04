#include "enum.h"
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <vector>
#include <algorithm> // for transform

#define EVENTS_FILE "events.txt"
#define COMPLAINTS_FILE "complaints.txt"
#define VENUE_FILE "Venues.txt"
#define USER_FILE "Users.txt"
using namespace std;

// Forward declarations for all screen functions
Screen loginScreen(User& currentUser, vector<User>& users);
Screen bookingScreen(vector<Event>& events, int& eventCount, int eventAvail[12*31][5], 
                    const string& EVENTS_FILE, const string& COMPLAINTS_FILE, Equipment& equipment);
Screen monitoringScreen(User& currentUser, vector<User>& users, vector<Event>& events, int& eventCount, int eventAvail[12*31][5], const string& EVENTS_FILE, const string& COMPLAINTS_FILE);
Screen mainMenu();
Screen exiting();

// Loading functions declarations (fixed signatures)
vector<User> loadEvents(vector<Event>& events, int& eventCount, int eventAvail[12*31][5], const string& filename);
vector<User> loadUsers(vector<User>& users, const string& filename, const vector<Event>& events);
vector<User> loadVenues(vector<Venue>& venues, const string& filename);

int main(){
    Screen currentScreen = LoginRegister;
    vector<Event> events;
    vector<Venue> venues;
    vector<User> users;
    
    // Initialize equipment
    Equipment equipment = {0, 0, 0, 0, 0, 0, 0}; // Initialize all to 0

    // Initialize event availability array
    int eventAvail[12*31][5] = {0};
    int eventCount = 0;

    // Load data from files
    loadEvents(events, eventCount, eventAvail, EVENTS_FILE);
    venues = loadVenues(); // This function returns a vector
    users = loadUsers(events); // This function returns a vector and takes events parameter

    User currentUser;

    while(currentScreen != exiting){
        switch(currentScreen){
            case LoginRegister:{
                currentScreen = loginScreen(currentUser, users);
                break;
            }
            case EventBooking:{
                currentScreen = bookingScreen(events, eventCount, eventAvail, EVENTS_FILE, COMPLAINTS_FILE, equipment);
                break;
            } 
            case EventMonitoring:
            {
                currentScreen = monitoringScreen(currentUser, users, events, eventCount, eventAvail, EVENTS_FILE, COMPLAINTS_FILE);
                break;
            } 
            case MainMenu: {
                currentScreen = mainMenu();
                if (currentScreen == LoginRegister) {
                    currentUser = User{}; // Reset user if logging out
                }
                break;
            }
            default: {
                cout << "Invalid screen state! Returning to login.\n";
                currentScreen = LoginRegister;
                break;
            }
        }
    }

    // Save data before exiting
    saveEvents(events, eventCount, EVENTS_FILE);
    saveUsers(users, USER_FILE);
    
    cout << "Program exited successfully.\n";
    return 0;
}

// Implementation of loadEvents (renamed from loadEventFromFile to match declaration)
vector<User> loadEvents(vector<Event>& events, int& eventCount, int eventAvail[12 * 31][5], const string& filename)
{
    vector<User> users;
    ifstream inEventFile(filename);
    if (!inEventFile)
    {
        return; // no file yet
    }

    string line;
    eventCount = 0;
    events.clear(); // Clear existing events

    while (getline(inEventFile, line))
    {
        if (line.empty())
            continue;

        stringstream ss(line);
        string idStr, eventStatusStr, eventTypeStr, eventNameStr, dateStr, timeStr, venueStr, equipmentStr, descStr, orgNameStr;

        // Get all fields separated by |
        if (!getline(ss, idStr, '|')) continue;
        if (!getline(ss, eventStatusStr, '|')) continue;
        if (!getline(ss, eventTypeStr, '|')) continue;
        if (!getline(ss, eventNameStr, '|')) continue;
        if (!getline(ss, dateStr, '|')) continue;
        if (!getline(ss, timeStr, '|')) continue;
        if (!getline(ss, venueStr, '|')) continue;
        if (!getline(ss, equipmentStr, '|')) continue;
        if (!getline(ss, descStr, '|')) continue;
        getline(ss, orgNameStr);

        Event evt;

        // Parse ID with error handling
        try
        {
            evt.eventId = stoi(idStr);
        }
        catch (...)
        {
            continue;
        }

        evt.eventStatus = eventStatusStr;
        evt.eventType = eventTypeStr;
        evt.eventName = eventNameStr;
        evt.timeDuration = timeStr;
        evt.venue.venue = venueStr;
        evt.eventDesc = descStr;
        evt.orgaName.orgaName = orgNameStr;

        // Parse date yyyy-mm-dd with error handling
        try
        {
            if (dateStr.length() >= 10)
            {
                int year = stoi(dateStr.substr(0, 4));
                int month = stoi(dateStr.substr(5, 2));
                int day = stoi(dateStr.substr(8, 2));
                evt.date = {day, month, year};
            }
            else
            {
                continue;
            }
        }
        catch (...)
        {
            continue;
        }

        try
        {
            stringstream es(equipmentStr);
            string token;
            vector<int> eq;

            while (getline(es, token, ','))
            {
                if (!token.empty())
                {
                    eq.push_back(stoi(token));
                }
                else
                {
                    eq.push_back(0);
                }
            }

            // Ensure we have exactly 7 equipment values
            while (eq.size() < 7)
            {
                eq.push_back(0);
            }

            if (eq.size() >= 7)
            {
                evt.equipments.chairs = eq[0];
                evt.equipments.tables = eq[1];
                evt.equipments.booths = eq[2];
                evt.equipments.projectors = eq[3];
                evt.equipments.bins = eq[4];
                evt.equipments.helpers = eq[5];
                evt.equipments.tents = eq[6];
            }
        }
        catch (...)
        {
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
        try
        {
            int dayIndex = (evt.date.month - 1) * 31 + (evt.date.day - 1);
            if (dayIndex >= 0 && dayIndex < 12 * 31)
            {
                if (venueStr.size() >= 6 && venueStr.substr(0, 4) == "Hall")
                {
                    int hall = stoi(venueStr.substr(5)) - 1; // "Hall 1" -> get "1"
                    if (hall >= 0 && hall < 5)
                    {
                        eventAvail[dayIndex][hall] = 1;
                    }
                }
            }
        }
        catch (...)
        {
            // Skip availability marking if it fails
        }

        events.push_back(evt);
    }

    eventCount = events.size();
    inEventFile.close();
}

// Implementation of loadVenues (returns vector<Venue>)
vector<Venue> loadVenues() {
    vector<Venue> venues;
    ifstream input(VENUE_FILE);

    if (!input) {
        // Return default venues if file doesn't exist
        venues.push_back(Venue{"Hall 1", "Chairs, Tables, Projectors", "Large", "Main hall with capacity for 200 people"});
        venues.push_back(Venue{"Hall 2", "Chairs, Tables", "Medium", "Conference room for 50 people"});
        venues.push_back(Venue{"Hall 3", "Chairs, Booths", "Small", "Meeting room for 20 people"});
        return venues;
    }

    string line;
    while (getline(input, line)) {
        if (line.empty()) continue;
        
        stringstream ss(line);
        string name, avaEquipment, size, desc;
        vector<Date> datesBooked;
        double price;

        if (!getline(ss, name, '|')) continue;
        if (!getline(ss, avaEquipment, '|')) continue;
        if (!getline(ss, size, '|')) continue;
        if (!getline(ss, desc, '|')) continue;
        
        // Read dates (simplified parsing)
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
                    } catch (...) {
                        // Skip invalid dates
                    }
                }
            }
        }

        // Read price
        string priceStr;
        if (getline(ss, priceStr)) {
            try {
                price = stod(priceStr);
            } catch (...) {
                price = 0.0;
            }
        } else {
            price = 0.0;
        }

        venues.push_back(Venue{name, avaEquipment, size, desc, datesBooked, price});
    }

    return venues;
}

// Implementation of loadUsers (returns vector<User>)
vector<User> loadUsers(const vector<Event>& events) {
    vector<User> users;
    ifstream input(USER_FILE);

    if (!input) {
        return users; // Return empty vector if file doesn't exist
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
        
        // Read reservations
        string resStr;
        if (getline(ss, resStr, '|')) {
            stringstream resStream(resStr);
            string token;
            while (getline(resStream, token, ',')) {
                try {
                    reservation.push_back(stoi(token));
                } catch (...) {
                    // Skip invalid IDs
                }
            }
        }

        // Read owned events
        string ownStr;
        if (getline(ss, ownStr)) {
            stringstream ownStream(ownStr);
            string token;
            while (getline(ownStream, token, ',')) {
                try {
                    owned.push_back(stoi(token));
                } catch (...) {
                    // Skip invalid IDs
                }
            }
        }

        users.push_back(User{name, password, reservation, owned});
    }

    return users;
}