#include "enum.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
using namespace std;


//pressEnterToContinue
void pressEnterToContinue(){
    cout << "\n[Press Enter to continue...]";
    cin.ignore();
    cin.get();
}


//displayEvent
void displayEvent(vector<Event> &events, int eventCount){
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
             << setw(15) << events[i].eventId
             << setw(25) << events[i].eventName
             << setw(15) << events[i].date.toString()
             << setw(20) << events[i].timeDuration
             << setw(20) << events[i].venue.venue
             << setw(25) << events[i].eventDesc << endl;
    }
}


//displaySearchedEvents
void displaySearchedEvents(vector<Event> &filteredEvents)
{
    if (filteredEvents.empty())
    {
        cout << "No events found.\n";
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

    for (size_t i = 0; i < filteredEvents.size(); i++)
    {
        cout << left
             << setw(5) << i + 1
             << setw(15) << filteredEvents[i].eventType
             << setw(15) << filteredEvents[i].eventId
             << setw(25) << filteredEvents[i].eventName
             << setw(15) << filteredEvents[i].date.toString()
             << setw(20) << filteredEvents[i].timeDuration
             << setw(20) << filteredEvents[i].venue.venue
             << setw(25) << filteredEvents[i].eventDesc << endl;
    }
}
