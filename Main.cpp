#include "enum.h"
#include <iostream>
using namespace std;

void mainMenu(vector<Event> &events, int &eventCount, int eventAvail[12 * 31][5], const string &EVENTS_FILE, const string &ORGANISER_FILE, const string &COMPLAINTS_FILE)
{
    int option;
    do
    {
        cout << "\n------ Main Menu ------\n";
        cout << "[1] Check venue availability\n";
        cout << "[2] Event booking\n";
        cout << "[3] Display events and event management\n";
        cout << "[0] Exit\n";
        cout << "\nSelect your option: ";
        cin >> option;

        switch (option)
        {
        case 1:
            checkAvailability(events, eventCount, eventAvail, EVENTS_FILE, ORGANISER_FILE, COMPLAINTS_FILE);
            break;
        case 2:
            eventBooking(events, eventCount, eventAvail, EVENTS_FILE, ORGANISER_FILE);
            break;
        case 3:
            eventMonitoring(events, eventCount, eventAvail, EVENTS_FILE, ORGANISER_FILE, COMPLAINTS_FILE);
            break;
        case 0:
            cout << "\n[Exiting program...]\n";
            break;
        default:
            cout << "Invalid option! Please select again!\n";
        }
    } while (option != 0);
}

int main() {
    vector<Event> events;
    int eventCount = 0;
    int eventAvail[12*31][5] = {0};

    const string EVENTS_FILE = "events.txt";
    const string ORGANISER_FILE = "organiser.txt";
    const string COMPLAINTS_FILE = "complaints.txt";

    loadEventFromFile(events, eventCount, eventAvail, EVENTS_FILE);
    loadOrganiserFromFile(events, ORGANISER_FILE);
    loadComplaints(events, COMPLAINTS_FILE);

    mainMenu(events, eventCount, eventAvail, EVENTS_FILE, ORGANISER_FILE, COMPLAINTS_FILE);
    return 0;
}
