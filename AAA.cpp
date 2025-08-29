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

void saveEvents(Event events[], int eventCount, const string &EVENTS_FILE);
void loadEventFromFile(Event events[], int &eventCount, int eventAvail[12 * 31][5], const string &EVENTS_FILE);

void displayEvents(Event events[], int eventCount)
{
    if (eventCount == 0)
    {
        cout << "No events available.\n";
        return;
    }

    cout << left
         << setw(5) << "No"
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
             << setw(15) << events[i].id
             << setw(25) << events[i].eventName
             << setw(15) << events[i].date.toString()
             << setw(20) << events[i].timeDuration
             << setw(20) << events[i].venue.venue
             << setw(25) << events[i].eventDesc << endl;
    }
}

void checkAvailability(int eventAvail[12 * 31][5])
{
    system("cls");
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

    // get current time
    time_t now = time(0);
    tm *ltm = localtime(&now);
    int currentYear = 1900 + ltm->tm_year;

        if (year < currentYear || year > currentYear + 5)
        {
            cout << "Year out of allowed range! (" << currentYear << " - " << currentYear + 5 << ")\n";
            return;
        }

        int monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        if (month < 1 || month > 12 || day < 1 || day > monthDays[month - 1])
        {
            cout << "Invalid day for selected month!\n";
            return;
        }

        int dayIndex = (month - 1) * 31 + (day - 1);

        cout << "\n------ Venue Availability on " << date << " ------\n";
        cout << "+---------+--------------+\n";
        cout << "|  Venue  |    Status    |\n";
        cout << "+---------+--------------+\n";

        for (int i = 0; i < 5; i++)
        {
            cout << "| Hall " << setw(2) << setfill(' ') << (i + 1) << "  | ";
            if (eventAvail[dayIndex][i] == 1)
            {
                cout << left << setw(12) << "Booked" << "|\n";
            }
            else
            {
                cout << left << setw(12) << "Available" << "|\n";
            }
        }

    cout << "+---------+--------------+\n";
}

void bookingOnDate(Event events[], int &eventCount, int eventAvail[12 * 31][5], const string &EVENTS_FILE)
{
    Event newEvent;
    char confirmation;

    do
    {
        cin.ignore();
        cout << "\n------ Event Booking ------\n";
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

            if (startHours < 0 || startHours > 23 || endHours < 0 || endHours > 23 || startMinutes < 0 || startMinutes > 59 || endMinutes < 0 || endMinutes > 59)
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
            cout << "Venue:\n1. Hall1\n2. Hall2\n3. Hall3\n4. Hall4\n5. Hall5\nSelect venue: ";
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
            newEvent.venue.venue = "Hall" + to_string(selectedVenue);
            cout << "Venue booked successfully!\n";
            break;
        }

        newEvent.timeDuration = newEvent.startTime + " to " + newEvent.endTime;

        cout << "\n------ Event Summary ------\n";
        cout << "Name: " << newEvent.eventName
             << "\nDate: " << newEvent.date.toString()
             << "\nTime: " << newEvent.timeDuration
             << "\nVenue: " << newEvent.venue.venue << endl;

        cout << "['y' - continue/ 'n' - re-enter all details]\nConfirm all details? (y/n): ";
        cin >> confirmation;
        confirmation = tolower(confirmation);

    } while (confirmation != 'y');

    newEvent.id = eventCount + 1;
    events[eventCount++] = newEvent;
    cout << "Booking successful!\n";

    saveEvents(events, eventCount, EVENTS_FILE);
}

void saveEvents(Event events[], int eventCount, const string &EVENTS_FILE)
{
    ofstream outEventFile(EVENTS_FILE);
    if (!outEventFile)
    {
        cout << "Error saving events\n";
        return;
    }

    for (int i = 0; i < eventCount; i++)
    {
        outEventFile << events[i].id << "|"
                     << events[i].eventName << "|"
                     << events[i].date.toString() << "|"
                     << events[i].startTime << "|"
                     << events[i].endTime << "|"
                     << events[i].timeDuration << "|"
                     << events[i].venue.venue << "|"
                     << events[i].eventDesc << "\n";
    }
    outEventFile.close();
}

void loadEventFromFile(Event events[], int &eventCount, int eventAvail[12 * 31][5], const string &EVENTS_FILE)
{
    ifstream inEventFile(EVENTS_FILE);
    if (!inEventFile)
    {
        return;
    }

    string line;
    eventCount = 0;

    while (getline(inEventFile, line))
    {
        stringstream ss(line);
        string idStr, dateStr;
        getline(ss, idStr, '|');
        events[eventCount].id = stoi(idStr);

        getline(ss, events[eventCount].eventName, '|');
        getline(ss, dateStr, '|');
        getline(ss, events[eventCount].startTime, '|');
        getline(ss, events[eventCount].endTime, '|');
        getline(ss, events[eventCount].timeDuration, '|');
        getline(ss, events[eventCount].venue.venue, '|');
        getline(ss, events[eventCount].eventDesc, '|');

        int year = stoi(dateStr.substr(0, 4));
        int month = stoi(dateStr.substr(5, 2));
        int day = stoi(dateStr.substr(8, 2));
        events[eventCount].date = {day, month, year};

        int dayIndex = (month - 1) * 31 + (day - 1);
        int hall = stoi(events[eventCount].venue.venue.substr(4)) - 1;
        eventAvail[dayIndex][hall] = 1;

        eventCount++;
    }

    inEventFile.close();
}

void mainMenu(Event events[], int &eventCount, int eventAvail[12 * 31][5], const string &EVENTS_FILE)
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
            checkAvailability(eventAvail);
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
    Event events[100];
    int eventCount = 0;
    int eventAvail[12 * 31][5] = {0};
    const string EVENTS_FILE = "events.txt";

    loadEventFromFile(events, eventCount, eventAvail, EVENTS_FILE);
    mainMenu(events, eventCount, eventAvail, EVENTS_FILE);
    return 0;
}
