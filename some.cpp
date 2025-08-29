#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <cctype>
using namespace std;

//struct for event
struct Event{
    string eventName;
    string date; //yyyy-mm-dd
    string startTime; //hh:mm
    string endTime;
    string timeDuration; //start time + end time
    string venue;
    string equipment[5]; //logistics
};



void displayEvents(Event events[], int eventCount) {
    cout << left << setw(5) << "No" << setw(25) << "Event Name" << setw(15) << "Date" << setw(20) << "Venue" << endl;

    cout << string(75, '-') << endl; // separator line

    for (int i = 0; i < eventCount; i++) {
        cout << left << setw(5) << i + 1 << setw(25) << events[i].eventName << setw(15) << events[i].date << setw(20) << events[i].venue << endl;
    }

    eventMonitoring(events, eventCount);
}

//---------------------------event monitoring---------------------------
void eventMonitoring(Event events[], int eventCount){
    Event events[100];
    int eventCount = 0;

    cout << "\n------ Event Summary ------\n";
    if(eventCount == 0){
        cout << "No events available.\n";
        return;
    }

    //display all event
    displayEvents(events, eventCount);

    

}



//---------------------------event booking on dates and logistics---------------------------



void bookingOnDate(Event events[], int &eventCount, int eventAvail[12*31][5]) {
        Event newEvent;
        char confirmation;

        do {
            // Event name
            cin.ignore();
            cout << "Enter your event name: ";
            getline(cin, newEvent.eventName);

            // Event date
            int day, month;
            while (true) {
                cout << "Enter your event date (yyyy-mm-dd): ";
                cin >> newEvent.date;

                if (newEvent.date.length() != 10 || newEvent.date[4] != '-' || newEvent.date[7] != '-') {
                    cout << "Follow the format yyyy-mm-dd!\n";
                    continue;
                }

                bool valid = true;
                for (int i = 0; i < 10; i++) {
                    if (i == 4 || i == 7) continue;
                    if (!isdigit(newEvent.date[i])) {
                        valid = false;
                        break;
                    }
                }
                if (!valid) {
                    cout << "Invalid date digits! Try again.\n";
                    continue;
                }

                month = stoi(newEvent.date.substr(5,2));
                day   = stoi(newEvent.date.substr(8,2));
                int monthDays[] = {31,28,31,30,31,30,31,31,30,31,30,31};
                if (month < 1 || month > 12 || day < 1 || day > monthDays[month-1]) {
                    cout << "Invalid day for selected month! Please try again!\n";
                    continue;
                }
                break;
            }

            // Event time
            while (true) {
                cout << "Enter start time (hh:mm): ";
                cin >> newEvent.startTime;
                cout << "Enter end time (hh:mm): ";
                cin >> newEvent.endTime;

                if (newEvent.startTime.length() != 5 || newEvent.startTime[2] != ':' ||
                    newEvent.endTime.length() != 5 || newEvent.endTime[2] != ':') {
                    cout << "Invalid time format!\n";
                    continue;
                }

                int sh = stoi(newEvent.startTime.substr(0,2));
                int sm = stoi(newEvent.startTime.substr(3,2));
                int eh = stoi(newEvent.endTime.substr(0,2));
                int em = stoi(newEvent.endTime.substr(3,2));

                if (sh < 0 || sh > 23 || eh < 0 || eh > 23 || sm < 0 || sm > 59 || em < 0 || em > 59) {
                    cout << "Invalid hours or minutes!\n";
                    continue;
                }

                if (eh < sh || (eh == sh && em <= sm)) {
                    cout << "End time must be later than start time!\n";
                    continue;
                }
                break;
            }

            // Venue selection
            int selectedVenue, venueIndex, dayIndex;
            while (true) {
                cout << "Select venue (1-5):\n1. Hall1\n2. Hall2\n3. Hall3\n4. Hall4\n5. Hall5\n";
                cin >> selectedVenue;

                venueIndex = selectedVenue - 1;
                if (venueIndex < 0 || venueIndex > 4) {
                    cout << "Invalid venue number!\n";
                    continue;
                }

                dayIndex = (month-1)*31 + (day-1);
                if (eventAvail[dayIndex][venueIndex] == 1) {
                    cout << "Venue already booked on this date! Choose another.\n";
                    continue;
                }

                eventAvail[dayIndex][venueIndex] = 1;
                newEvent.venue = "Hall" + to_string(selectedVenue);
                cout << "Venue booked successfully!\n";
                break;
            }

            // Time duration
            newEvent.timeDuration = newEvent.startTime + " to " + newEvent.endTime;

            // Show confirmation
            cout << "\n=== Event Confirmation ===\n";
            cout << "Name: " << newEvent.eventName << "\nDate: " << newEvent.date 
                << "\nTime: " << newEvent.timeDuration << "\nVenue: " << newEvent.venue << endl;

            cout << "Confirm all details? (y/n): ";
            cin >> confirmation;
            confirmation = tolower(confirmation);

        } while (confirmation != 'y');

        events[eventCount++] = newEvent;
        cout << "Booking successful!\n";
    }

void mainMenu(Event events[], int &eventCount, int eventAvail[12*31][5]) {
    int option;
    do {
        cout << "\n=== Main Menu ===\n";
        cout << "1. Check availability\n";
        cout << "2. Event booking on dates & logistics\n";
        cout << "3. Event monitoring\n";
        cout << "4. Payment history\n";
        cout << "5. Event reporting\n";
        cout << "0. Exit\n";
        cout << "Select an option: ";
        cin >> option;

        switch(option) {
            case 1:
                cout << "Check availability (function not implemented yet)\n";
                break;
            case 2:
                bookingOnDate(events, eventCount, eventAvail);
                break;
            case 3:
                eventMonitoring(events, eventCount);
                break;
            case 4:
                cout << "Payment history (function not implemented yet)\n";
                break;
            case 5:
                cout << "Event reporting (function not implemented yet)\n";
                break;
            case 0:
                cout << "Exiting menu.\n";
                break;
            default:
                cout << "Invalid option! Try again.\n";
        }
    } while(option != 0);
}


int main(){
    int eventCount = 0; //number of booked events
    int eventAvail[12*31][5] = {0}; // initialize 2D array

    //mainMenu(events, eventCount, eventAvail);


    return 0;
}