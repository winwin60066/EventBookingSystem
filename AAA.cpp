#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <cctype>
#include <fstream>
#include <ctime>
#include <vector>
using namespace std;

struct Date
{
    int day;
    int month;
    int year;

    bool operator==(const Date &other) const
    {
        return day == other.day && month == other.month && year == other.year;
    }
    string toString() const
    {
        stringstream ss;
        ss << year << "-" << setw(2) << setfill('0') << month
        << "-" << setw(2) << setfill('0') << day;
        return ss.str();
    }
};

struct Venue
{
    string venue;
    string avaEquipment;
    string size;
    string desc;
    vector<Date> datesBooked;
    double price;
};

struct Equipment
{
    int chairs;
    int tables;
    int booths;
    int projectors;
    int bins;
    int helpers;
    int tents;

    int &operator[](size_t index)
    {
        switch (index)
        {
        case 0:
            return chairs;
        case 1:
            return tables;
        case 2:
            return booths;
        case 3:
            return projectors;
        case 4:
            return bins;
        case 5:
            return helpers;
        case 6:
            return tents;
        default:
            throw std::out_of_range("Invalid index");
        }
    }
};

struct Event
{
    int id;
    string eventType;
    string eventName;
    Date date;
    string startTime;
    string endTime;
    string timeDuration;
    Venue venue;
    Equipment equipments;
    string eventDesc;
    double paid = 0.00;
};

void saveEvents(vector<Event>& events, int eventCount, const string &EVENTS_FILE);
void loadEventFromFile(vector<Event>& events, int &eventCount, int eventAvail[12 * 31][5], const string &EVENTS_FILE);
void mainMenu(vector<Event>& events, int &eventCount, int eventAvail[12 * 31][5], const string &EVENTS_FILE);

void displayEvents(vector<Event>& events, int eventCount)
{
    if (eventCount == 0)
    {
        cout << "No events available.\n";
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

    for (int i = 0; i < eventCount; i++)
    {
        cout << left
             << setw(5) << i + 1
             << setw(15) << events[i].eventType
             << setw(15) << events[i].id
             << setw(25) << events[i].eventName
             << setw(15) << events[i].date.toString()
             << setw(20) << events[i].timeDuration
             << setw(20) << events[i].venue.venue
             << setw(25) << events[i].eventDesc << endl;
    }
}



void bookingOnDate(vector<Event>& events, int &eventCount, int eventAvail[12 * 31][5], const string &EVENTS_FILE)
{
    Event newEvent;
    char confirmation;

    do
    {
        cout << "\n------ Event Booking ------\n";
        int eventTypeChoice;
        cout << "Event Type: \n[1] Seminar\n[2] Talk\nSelect event type: ";
        cin >> eventTypeChoice;

        if (eventTypeChoice == 1)
            newEvent.eventType = "Seminar";
        else if (eventTypeChoice == 2)
            newEvent.eventType = "Talk";
        else
            newEvent.eventType = "Other";

        cin.ignore(); 
        cout << "\nEnter your event name: ";
        getline(cin, newEvent.eventName);

        while (true)
        {
            cout << "Enter your event date (yyyy-mm-dd): ";
            string dateInput;
            cin >> dateInput;

            int year, month, day;
            char dash1, dash2;
            stringstream ss(dateInput);
            if (!(ss >> year >> dash1 >> month >> dash2 >> day) || dash1 != '-' || dash2 != '-')
            {
                cout << "Invalid format!\n";
                continue;
            }

            int monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
            if (month < 1 || month > 12 || day < 1 || day > monthDays[month - 1])
            {
                cout << "Invalid day for selected month!\n";
                continue;
            }

            newEvent.date = {day, month, year};
            break;
        }

        while (true)
        {
            cout << "Enter start time (hh:mm): ";
            cin >> newEvent.startTime;
            cout << "Enter end time (hh:mm): ";
            cin >> newEvent.endTime;

            if (newEvent.startTime.length() != 5 || newEvent.startTime[2] != ':' ||
                newEvent.endTime.length() != 5 || newEvent.endTime[2] != ':')
            {
                cout << "Invalid time format!\n";
                continue;
            }

            int startHours = stoi(newEvent.startTime.substr(0, 2));
            int startMinutes = stoi(newEvent.startTime.substr(3, 2));
            int endHours = stoi(newEvent.endTime.substr(0, 2));
            int endMinutes = stoi(newEvent.endTime.substr(3, 2));

            if (startHours < 0 || startHours > 23 || endHours < 0 || endHours > 23 ||
                startMinutes < 0 || startMinutes > 59 || endMinutes < 0 || endMinutes > 59)
            {
                cout << "Invalid hours/minutes!\n";
                continue;
            }
            if (endHours < startHours || (endHours == startHours && endMinutes <= startMinutes))
            {
                cout << "End time must be later than start time!\n";
                continue;
            }
            break;
        }

        int selectedVenue, venueIndex, dayIndex;
        while (true)
        {
            cout << "Venue Options:\n[1] Hall 1\n[2] Hall 2\n[3] Hall 3\n[4] Hall 4\n[5] Hall 5\nSelect venue: ";
            cin >> selectedVenue;
            venueIndex = selectedVenue - 1;

            if (venueIndex < 0 || venueIndex > 4)
            {
                cout << "Invalid venue!\n";
                continue;
            }

            dayIndex = (newEvent.date.month - 1) * 31 + (newEvent.date.day - 1);
            if (eventAvail[dayIndex][venueIndex] == 1)
            {
                cout << "Venue already booked! Please choose again.\n";
                continue;
            }

            eventAvail[dayIndex][venueIndex] = 1;
            newEvent.venue.venue = "Hall " + to_string(selectedVenue);
            break;
        }

        cin.ignore();
        cout << "Enter event description (if any): ";
        getline(cin, newEvent.eventDesc);

        newEvent.timeDuration = newEvent.startTime + " to " + newEvent.endTime;

        cout << "\n------ Event Summary ------\n";
        cout << "Event Type    : " << newEvent.eventType
             << "\nEvent Name    : " << newEvent.eventName
             << "\nDate          : " << newEvent.date.toString()
             << "\nTime Duration : " << newEvent.timeDuration
             << "\nVenue         : " << newEvent.venue.venue 
             << "\nDescription   : " << newEvent.eventDesc
             << endl;

        cout << "\n['y' - continue/ 'n' - re-enter all details]\nConfirm all details? (y/n): ";
        cin >> confirmation;
        confirmation = tolower(confirmation);

    } while (confirmation != 'y');

    newEvent.id = eventCount + 1;
    events[eventCount++] = newEvent;
    cout << "Booking successful!\n";

    saveEvents(events, eventCount, EVENTS_FILE);

    int option;
    cout << "[1] Back to Main Menu\n[0] Exit";
    cin >> option;
    switch(option){
        case 1:
            mainMenu(events, eventCount, eventAvail, EVENTS_FILE);
            break;
        case 0:
            cout << "\nExiting program...\n";
            break;
        default:
            cout << "Invalid option! Please select again!\n";
    }
}

void checkAvailability(vector<Event>& events, int &eventCount, int eventAvail[12 * 31][5], const string &EVENTS_FILE)
{
    string date;
    cout << "------ Check Venue Availability ------\n";
    cout << "\nEnter date to check venue availability (yyyy-mm-dd): ";
    cin >> date;

    if (date.length() != 10 || date[4] != '-' || date[7] != '-')
    {
        cout << "Invalid date format! Use yyyy-mm-dd.\n";
        return;
    }

    int year = stoi(date.substr(0, 4));
    int month = stoi(date.substr(5, 2));
    int day = stoi(date.substr(8, 2));

    // Get current time
    time_t now = time(0);
    tm *ltm = localtime(&now);
    int currentYear = 1900 + ltm->tm_year;

    if (year < currentYear || year > currentYear + 5)
    {
        cout << "Year out of allowed range! (" << currentYear << " - " << currentYear + 5 << ")\n";
        return;
    }

    // Handle leap years
    int monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        monthDays[1] = 29; // February has 29 days in leap year
    }

    if (month < 1 || month > 12)
    {
        cout << "Invalid month! Must be between 1-12.\n";
        return;
    }

    if (day < 1 || day > monthDays[month - 1])
    {
        cout << "Invalid day for selected month!\n";
        return;
    }

    // Calculate cumulative days from start of year
    int dayIndex = 0;
    for (int i = 0; i < month - 1; i++) {
        dayIndex += monthDays[i];
    }
    dayIndex += (day - 1);

    // Check if dayIndex is within bounds (0 to 365/366)
    int maxDays = 365;
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        maxDays = 366;
    }
    
    if (dayIndex < 0 || dayIndex >= maxDays) {
        cout << "Date calculation error! Index out of bounds.\n";
        return;
    }

    // Ensure we don't exceed array bounds
    if (dayIndex >= 12 * 31) {
        cout << "Date exceeds maximum supported date range.\n";
        return;
    }

    cout << "\n------ Venue Availability on " << date << " ------\n";
    cout << "+---------+--------------+\n";
    cout << "|  Venue  |    Status    |\n";
    cout << "+---------+--------------+\n";

    for (int i = 0; i < 5; i++)
    {
        cout << "| Hall " << setw(2) << setfill(' ') << (i + 1) << " | ";
        if (eventAvail[dayIndex][i] == 1)
        {
            cout << left << setw(12) << "Booked" << " |\n";
        }
        else
        {
            cout << left << setw(1) << "Available" << " |\n";
        }
    }

    cout << "+---------+--------------+\n";

    cout << "\n[1] Back to Main Menu\n[2] Event booking\n[3] Display events\n[0] Exit\nSelect option: ";
    int option;
    cin >> option;

    do{
        switch(option){
            case 1:
                mainMenu(events, eventCount, eventAvail, EVENTS_FILE);
                break;
            case 2:
                bookingOnDate(events,eventCount, eventAvail, EVENTS_FILE);
                break;
            case 3:
                displayEvents(events,eventCount);
                break;
            case 0:
                cout << "\nExiting program...\n";
                break;
            default:
                cout << "Invalid option! Please select again!\n";
        }
    }while(option != 0);
}


void saveEvents(vector<Event>& events, int eventCount, const string &EVENTS_FILE)
{
    ofstream outEventFile(EVENTS_FILE);
    if (!outEventFile)
    {
        cout << "Error saving events\n";
        return;
    }

    for (int i = 0; i < eventCount; i++)
    {
        outEventFile << events[i].id << "|"                // first save id
                     << events[i].eventType << "|"         // then eventType
                     << events[i].eventName << "|"
                     << events[i].date.toString() << "|"
                     << events[i].timeDuration << "|"
                     << events[i].venue.venue << "|"
                     << events[i].eventDesc << "\n";
    }
    outEventFile.close();
}

void loadEventFromFile(vector<Event>& events, int &eventCount, int eventAvail[12 * 31][5], const string &EVENTS_FILE)
{
    ifstream inEventFile(EVENTS_FILE);
    if (!inEventFile)
    {
        return; // no file yet, skip
    }

    string line;
    eventCount = 0;

    while (getline(inEventFile, line))
    {
        if (line.empty()) continue;

        stringstream ss(line);
        string idStr, eventTypeStr, eventNameStr, dateStr, timeStr, venueStr, descStr;

        getline(ss, idStr, '|');
        getline(ss, eventTypeStr, '|');
        getline(ss, eventNameStr, '|');
        getline(ss, dateStr, '|');
        getline(ss, timeStr, '|');
        getline(ss, venueStr, '|');
        getline(ss, descStr);

        events[eventCount].id = stoi(idStr);
        events[eventCount].eventType = eventTypeStr;
        events[eventCount].eventName = eventNameStr;
        events[eventCount].timeDuration = timeStr;
        events[eventCount].venue.venue = venueStr;
        events[eventCount].eventDesc = descStr;

        // parse date yyyy-mm-dd
        int year = stoi(dateStr.substr(0, 4));
        int month = stoi(dateStr.substr(5, 2));
        int day = stoi(dateStr.substr(8, 2));
        events[eventCount].date = {day, month, year};

        // mark availability
        int dayIndex = (month - 1) * 31 + (day - 1);
        if (venueStr.size() >= 5 && venueStr.substr(0,4) == "Hall")
        {
            int hall = stoi(venueStr.substr(4)) - 1;
            if (hall >= 0 && hall < 5)
            {
                eventAvail[dayIndex][hall] = 1;
            }
        }

        eventCount++;
    }

    inEventFile.close();
}


void mainMenu(vector<Event>& events, int &eventCount, int eventAvail[12 * 31][5], const string &EVENTS_FILE)
{
    int option;
    do
    {
        cout << "\n------ Main Menu ------\n";
        cout << "[1] Check venue availability\n";
        cout << "[2] Event booking\n";
        cout << "[3] Display events\n";
        cout << "[0] Exit\n";
        cout << "\nSelect your option: ";
        cin >> option;

        switch (option)
        {
        case 1:
            checkAvailability(events, eventCount, eventAvail, EVENTS_FILE);
            break;
        case 2:
            bookingOnDate(events, eventCount, eventAvail, EVENTS_FILE);
            break;
        case 3:
            displayEvents(events, eventCount);
            break;
        case 0:
            cout << "\nExiting program...\n";
            break;
        default:
            cout << "Invalid option! Please select again!\n";
        }
    } while (option != 0);
}

int main()
{
    vector<Event> events;
    int eventCount = 0;
    int eventAvail[12 * 31][5] = {0};
    const string EVENTS_FILE = "events.txt";

    loadEventFromFile(events, eventCount, eventAvail, EVENTS_FILE);
    mainMenu(events, eventCount, eventAvail, EVENTS_FILE);
    return 0;
}
