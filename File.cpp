#include "enum.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#define EVENTS_FILE "events.txt"
#define COMPLAINTS_FILE "complaints.txt"
#define VENUE_FILE "Venues.txt"
#define USER_FILE "Users.txt"
using namespace std;

//saveEvents
void saveEvents(vector<Event> &events, int eventCount) {
    ofstream outEventFile(EVENTS_FILE);
    if (!outEventFile) {
        cout << "Error saving events\n";
        return;
    }

    for (int i = 0; i < eventCount; i++) {
        outEventFile << events[i].eventId << "|"
                    //total participant
                    //attended participant
                     << events[i].eventStatus << "|"
                     << events[i].eventType << "|"
                     << events[i].eventName << "|"
                     << events[i].date.toString() << "|"
                     << events[i].timeDuration << "|"
                     << events[i].venue.venue << "|"
                     << events[i].entryFee << "|"
                     << events[i].equipments.chairs << ","
                     << events[i].equipments.tables << ","
                     << events[i].equipments.booths << ","
                     << events[i].equipments.projectors << ","
                     << events[i].equipments.bins << ","
                     << events[i].equipments.helpers << ","
                     << events[i].equipments.tents << "|"
                     << events[i].eventDesc << "\n";
    }
    outEventFile.close();
}

//saveUsers
void saveUsers(const vector<User>& users) {
    ofstream outFile(USER_FILE);
    if (!outFile) {
        cout << "Error opening user file for writing.\n";
        return;
    }
    for (const auto& user : users) {
        outFile << user.name << "|" << user.password << "|";
        for (size_t i = 0; i < user.reservationID.size(); ++i) {
            outFile << user.reservationID[i];
            if (i < user.reservationID.size() - 1) outFile << ",";
        }
        outFile << "|";
        for (size_t i = 0; i < user.owned.size(); ++i) {
            outFile << user.owned[i];
            if (i < user.owned.size() - 1) outFile << ",";
        }
        outFile << "\n";
    }
    outFile.close();
}
