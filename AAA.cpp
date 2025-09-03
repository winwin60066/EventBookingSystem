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

struct Organiser{
    string orgaName;
    string orgaPhoneNo;
    string orgaEmail;
};

struct Participant{
    string partiName;
    bool attended;
};

struct Complaint{
    int eventId;
    int complaintId;
    string complStatus; //pending, in progress, solved
    string complType; //like equipment, staff behaviour, other
    string reporterType; //staff or customer
    string complDesc;
};

struct Event
{
    string eventStatus; //TODO - Scheduled, Ongoing, Completed
    int eventId;
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
    int expectedPartiQty; //expected qty
    int actualPartiQty; // actual participant qty
    Organiser orgaName;
    Organiser orgaPhoneNo;
    Organiser orgaEmail;
    vector<Participant> participants;  
    vector<Complaint> complaints;  
};

// Function declarations - FIXED: Added missing declarations and corrected signatures
void saveEvents(vector<Event> &events, int eventCount, const string &EVENTS_FILE);
void loadEventFromFile(vector<Event> &events, int &eventCount, int eventAvail[12 * 31][5], const string &EVENTS_FILE);
void mainMenu(vector<Event> &events, int &eventCount, int eventAvail[12 * 31][5], const string &EVENTS_FILE, const string &ORGANISER_FILE, const string &COMPLAINTS_FILE);
void checkAvailability(vector<Event> &events, int &eventCount, int eventAvail[12 * 31][5], const string &EVENTS_FILE, const string &ORGANISER_FILE, const string &COMPLAINTS_FILE);
void eventBooking(vector<Event> &events, int &eventCount, int eventAvail[12 * 31][5], const string &EVENTS_FILE, const string &ORGANISER_FILE);
void searchEvents(vector<Event> &events, int eventCount);
void deleteEvent(vector<Event> &events, int &eventCount, int eventAvail[12 * 31][5], const string &EVENTS_FILE);
void saveOrganiser(vector<Event> &events, int eventCount, const string &ORGANISER_FILE);
void loadOrganiserFromFile(vector<Event> &events, const string &ORGANISER_FILE);
void updateEventStatus(vector<Event> &events, int eventCount, int eventAvail[12 * 31][5], const string &EVENTS_FILE, const string &ORGANISER_FILE, const string &COMPLAINTS_FILE);
void partiAttendance(vector<Event> &events, int eventCount);
void manageComplaint(vector<Event> &events, int eventCount, const string &COMPLAINTS_FILE);
void eventMonitoring(vector<Event> &events, int eventCount, int eventAvail[12 * 31][5], const string &EVENTS_FILE, const string &ORGANISER_FILE, const string &COMPLAINTS_FILE);
void saveComplaints(const vector<Event>& events, const string& COMPLAINTS_FILE);
void loadComplaints(vector<Event>& events, const string& COMPLAINTS_FILE);
void pressEnterToContinue();

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

void pressEnterToContinue(){
    cout << "\n[Press Enter to continue...]";
    cin.ignore();
    cin.get();
}

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

void manageComplaint(vector<Event> &events, int eventCount, const string &COMPLAINTS_FILE) {
    if (eventCount == 0) {
        cout << "No events available to manage complaints.\n";
        return;
    }

    int eventId;
    cout << "\n------ Complaint Management ------\n";
    cout << "Enter Event ID to manage complaints: ";
    cin >> eventId;

    // Find event
    int eventIndex = -1;
    for (int i = 0; i < eventCount; i++) {
        if (events[i].eventId == eventId) {
            eventIndex = i;
            break;
        }
    }

    if (eventIndex == -1) {
        cout << "Event not found!\n";
        return;
    }

    int choice;
    do {
        cout << "\nManaging complaints for Event: " << events[eventIndex].eventName << "\n";
        cout << "[1] Add Complaint\n";
        cout << "[2] View Complaints\n";
        cout << "[3] Update Complaint Status\n";
        cout << "[0] Back\n";
        cout << "Select option: ";
        cin >> choice;
        cin.ignore(); // clear buffer

        switch (choice) {
            case 1: {
                Complaint newComplaint;
                newComplaint.complaintId = events[eventIndex].complaints.size() + 1;
                cout << "Enter complaint description: ";
                getline(cin, newComplaint.complDesc);
                newComplaint.complStatus = "Pending";
                events[eventIndex].complaints.push_back(newComplaint);
                cout << "Complaint added successfully!\n";
                saveComplaints(events, COMPLAINTS_FILE);
                break;
            }

            case 2: {
                loadComplaints(events, COMPLAINTS_FILE);
                if (events[eventIndex].complaints.empty()) {
                    cout << "No complaints for this event.\n";
                } else {
                    cout << "\n--- Complaints (" << events[eventIndex].complaints.size() << ") ---\n";
                    for (auto &c : events[eventIndex].complaints) {
                        cout << "ID: " << c.complaintId 
                             << " | Description: " << c.complDesc
                             << " | Status: " << c.complStatus << endl;
                    }
                }
                break;
            }

            case 3: {
                if (events[eventIndex].complaints.empty()) {
                    cout << "No complaints to update.\n";
                } else {
                    int compId;
                    cout << "Enter Complaint ID to update: ";
                    cin >> compId;

                    bool found = false;
                    for (auto &c : events[eventIndex].complaints) {
                        if (c.complaintId == compId) {
                            found = true;
                            int opt;
                            cout << "Update Status:\n[1] Pending\n[2] In Progress\n[3] Solved\nSelect: ";
                            cin >> opt;
                            switch (opt) {
                                case 1: c.complStatus = "Pending"; break;
                                case 2: c.complStatus = "In Progress"; break;
                                case 3: c.complStatus = "Solved"; break;
                                default: cout << "Invalid option!\n"; continue;
                            }
                            cout << "Complaint status updated!\n";
                            break;
                        }
                    }
                    if (!found) {
                        cout << "Complaint not found.\n";
                    }
                }
                break;
            }

            case 0:
                cout << "Returning to event monitoring...\n";
                break;

            default:
                cout << "Invalid choice.\n";
        }
    } while (choice != 0);
}

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

void eventBooking(vector<Event> &events, int &eventCount, int eventAvail[12 * 31][5],const string &EVENTS_FILE, const string &ORGANISER_FILE)
{
    Event newEvent;
    char confirmation;
    string date;
    int year, month, day;

    do
    {
        cout << "\n------ Event Booking ------\n";
        int eventTypeChoice;
        cout << "\n[1] Seminar\n[2] Talk\n\nSelect event type: ";
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

        // Date validation loop
        while (true)
        {
            cout << "\nEnter event date (yyyy-mm-dd): ";
            cin >> date;

            // Format check
            if (date.length() != 10 || date[4] != '-' || date[7] != '-')
            {
                cout << "\n[Invalid date format! Use yyyy-mm-dd]\n";
                continue;
            }

            // Digit check
            bool validDigits = true;
            for (size_t i = 0; i < date.size(); i++)
            {
                if (i == 4 || i == 7)
                    continue; // skip dashes
                if (!isdigit(date[i]))
                {
                    validDigits = false;
                    break;
                }
            }
            if (!validDigits)
            {
                cout << "\n[Invalid date input! Please enter numbers]\n";
                continue;
            }

            try
            {
                year = stoi(date.substr(0, 4));
                month = stoi(date.substr(5, 2));
                day = stoi(date.substr(8, 2));
            }
            catch (...)
            {
                cout << "\n[Invalid numbers in date]\n";
                continue;
            }

            // Year range check
            time_t now = time(0);
            tm *ltm = localtime(&now);
            int currentYear = 1900 + ltm->tm_year;
            if (year < currentYear || year > currentYear + 5)
            {
                cout << "\n[Year out of allowed range! (" << currentYear << " - " << currentYear + 5 << ")]\n";
                continue;
            }

            // Leap year & month/day checks
            int monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
            if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
                monthDays[1] = 29;

            if (month < 1 || month > 12)
            {
                cout << "\n[Invalid month! Must be 1-12]\n";
                continue;
            }
            if (day < 1 || day > monthDays[month - 1])
            {
                cout << "\n[Invalid day for this month]\n";
                continue;
            }

            // Save valid date
            newEvent.date = {day, month, year};
            break;
        }

        // Start time validation
        while (true)
        {
            cout << "\nEnter start time (hh:mm): ";
            cin >> newEvent.startTime;

            if (newEvent.startTime.length() != 5 || newEvent.startTime[2] != ':')
            {
                cout << "\n[Invalid time format! Please follow (hh:mm) and only enter numbers]\n";
                continue;
            }

            try
            {
                int startHours = stoi(newEvent.startTime.substr(0, 2));
                int startMinutes = stoi(newEvent.startTime.substr(3, 2));
                if (startHours < 0 || startHours > 23 || startMinutes < 0 || startMinutes > 59)
                {
                    cout << "\n[Invalid start time values!]\n";
                    continue;
                }
            }
            catch (...)
            {
                cout << "Invalid input! Please enter numbers only.\n";
                continue;
            }
            break; // valid start time
        }

        // End time validation
        while (true)
        {
            cout << "\nEnter end time (hh:mm): ";
            cin >> newEvent.endTime;

            if (newEvent.endTime.length() != 5 || newEvent.endTime[2] != ':')
            {
                cout << "\n[Invalid time format! Please follow (hh:mm) and only enter numbers]\n";
                continue;
            }

            try
            {
                int startHours = stoi(newEvent.startTime.substr(0, 2));
                int startMinutes = stoi(newEvent.startTime.substr(3, 2));
                int endHours = stoi(newEvent.endTime.substr(0, 2));
                int endMinutes = stoi(newEvent.endTime.substr(3, 2));

                if (endHours < 0 || endHours > 23 || endMinutes < 0 || endMinutes > 59)
                {
                    cout << "\n[Invalid end time values!]\n";
                    continue;
                }
                if (endHours < startHours || (endHours == startHours && endMinutes <= startMinutes))
                {
                    cout << "\n[End time must be later than start time!]\n";
                    continue;
                }
            }
            catch (...)
            {
                cout << "Invalid input! Please enter numbers only.\n";
                continue;
            }
            break; // valid end time
        }

        // Venue selection
        int selectedVenue, venueIndex, dayIndex;
        while (true)
        {
            cout << "\n---------- Venues ----------\n[1] Hall 1\n[2] Hall 2\n[3] Hall 3\n[4] Hall 4\n[5] Hall 5\nSelect venue: ";
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
                cout << "\n[Venue already booked! Please choose again]\n";
                continue;
            }

            eventAvail[dayIndex][venueIndex] = 1;
            newEvent.venue.venue = "Hall " + to_string(selectedVenue);
            break;
        }

        // Equipment selection
        cout << "\n---------- Equipment Selection ----------\n";
        string equipmentNames[] = {"Chairs", "Tables", "Booths", "Projectors", "Bins", "Helpers", "Tents"};

        for (int i = 0; i < 7; i++)
        {
            int quantity;
            while (true)
            {
                cout << "Enter number of " << equipmentNames[i] << ": ";
                cin >> quantity;

                // Check for input failure (non-numeric input)
                if (cin.fail())
                {
                    cout << "[Please enter a valid number (0 or positive)]\n";
                    cin.clear();
                    cin.ignore(1000, '\n');
                    continue;
                }

                // Check for negative numbers
                if (quantity < 0)
                {
                    cout << "[Please enter a valid number (0 or positive)]\n";
                    continue;
                }

                break;
            }
            newEvent.equipments[i] = quantity;
        }

        cin.ignore();
        cout << "\nEnter event description (if any): ";
        getline(cin, newEvent.eventDesc);

        newEvent.timeDuration = newEvent.startTime + " to " + newEvent.endTime;

        cout << "\n------ Event Summary ------\n";
        cout << "Event Type    : " << newEvent.eventType
             << "\nEvent Name    : " << newEvent.eventName
             << "\nDate          : " << newEvent.date.toString()
             << "\nTime Duration : " << newEvent.timeDuration
             << "\nVenue         : " << newEvent.venue.venue
             << "\nEquipments    : " << newEvent.equipments.chairs << " Chairs, "
             << newEvent.equipments.tables << " Tables, " << newEvent.equipments.booths << " Booths, "
             << newEvent.equipments.projectors << " Projectors, " << newEvent.equipments.bins << " Bins, "
             << newEvent.equipments.helpers << " Helpers, " << newEvent.equipments.tents << " Tents"
             << "\nDescription   : " << newEvent.eventDesc
             << endl;

        cout << "\n['y' - continue/ 'n' - re-enter all details]\nConfirm all details? (y/n): ";
        cin >> confirmation;
        confirmation = tolower(confirmation);

    } while (confirmation != 'y');

    //enter organizer details
    do{
        cin.ignore(); // Clear input buffer
        string orgName, orgPhone, orgEmail;
        
        cout << "\n------ Organiser Details ------\n";
        cout << "Enter organiser name: ";
        getline(cin, orgName);
        
        // Phone validation
        while(true){
            cout << "Enter organiser phone number(e.g. 012-3456789): ";
            getline(cin, orgPhone);
            
            // Basic phone validation (xxx-xxxxxxx format)
            if(orgPhone.length() >= 11 && orgPhone[3] == '-'){
                bool validPhone = true;
                for(size_t i = 0; i < orgPhone.length(); i++){
                    if(i == 3) continue; // skip dash
                    if(!isdigit(orgPhone[i])){
                        validPhone = false;
                        break;
                    }
                }
                if(validPhone) break;
            }
            cout << "[Invalid phone format! Use xxx-xxxxxxx]\n";
        }
        
        // Email validation
        while(true){
            cout << "Enter organiser email(e.g. sample@gmail.com): ";
            cin >> orgEmail;
            
            // Basic email validation (contains @ and .)
            if(orgEmail.find('@') != string::npos && orgEmail.find('.') != string::npos){
                break;
            }
            cout << "[Invalid email format! Must contain @ and .]\n";
        }

        // Store organizer details
        newEvent.orgaName.orgaName = orgName;
        newEvent.orgaPhoneNo.orgaPhoneNo = orgPhone;
        newEvent.orgaEmail.orgaEmail = orgEmail;

        cout << "\n-------- Details Summary --------\n";
        cout << "Organiser name: " << orgName << "\n";
        cout << "Phone Number: " << orgPhone << "\n";
        cout << "Email: " << orgEmail << "\n";
                
        cout << "\n['y' - continue/ 'n' - re-enter all details]\nConfirm all details? (y/n): ";
        cin >> confirmation;
        confirmation = tolower(confirmation);
        
    }while(confirmation != 'y');

    newEvent.eventId = events.size() + 1;
    newEvent.eventStatus = "Scheduled";
    events.push_back(newEvent);
    eventCount = events.size();
    cout << "\n[Booking successful!]\n";
    

    saveEvents(events, eventCount, EVENTS_FILE);
    saveOrganiser(events, eventCount, ORGANISER_FILE);

    char option;
    do
    {
        cout << "\n[1] Manage and View Event\n[2] Back To Main Menu\nSelect Option: ";
        cin >> option;
        option = tolower(option);

        if (option == '1') // FIXED: Compare with character not integer
        {
            return; // FIXED: Let caller handle navigation
        }else if (option == '2')
        {
            return; // FIXED: Let caller handle navigation  
        }else
        {
            cout << "\n[Invalid option! Please try again!]\n";
        }
    } while (true);
}

void checkAvailability(vector<Event> &events, int &eventCount, int eventAvail[12 * 31][5], const string &EVENTS_FILE, const string &ORGANISER_FILE, const string &COMPLAINTS_FILE)
{
    cout << "\n------ Check Venue Availability ------\n";
    string date;
    int year, month, day;

    while (true)
    {
        cout << "\nEnter date to check venue availability (yyyy-mm-dd): ";
        cin >> date;

        // First check overall length
        if (date.length() != 10)
        {
            cout << "\n[Invalid date format! Please follow (yyyy-mm-dd) and enter numbers only]\n";
            continue;
        }

        // Check the '-' positions
        if (date[4] != '-' || date[7] != '-')
        {
            cout << "\n[Invalid date format! Please follow (yyyy-mm-dd) and enter numbers only]\n";
            continue;
        }

        // Check that all other characters are digits
        bool validDigits = true;
        for (size_t i = 0; i < date.size(); i++)
        {
            if (i == 4 || i == 7)
                continue; // skip dashes
            if (!isdigit(date[i]))
            {
                validDigits = false;
                break;
            }
        }
        if (!validDigits)
        {
            cout << "\n[Invalid date input! Please enter numbers]\n";
            continue;
        }

        try
        {
            year = stoi(date.substr(0, 4));
            month = stoi(date.substr(5, 2));
            day = stoi(date.substr(8, 2));
        }
        catch (...)
        {
            cout << "Invalid date input! Please enter numbers.\n";
            continue;
        }

        // Get current time
        time_t now = time(0);
        tm *ltm = localtime(&now);
        int currentYear = 1900 + ltm->tm_year;

        if (year < currentYear || year > currentYear + 5)
        {
            cout << "\n[Year out of allowed range! (" << currentYear << " - " << currentYear + 5 << ") ]\n";
            continue;
        }

        // Handle leap years
        int monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
        {
            monthDays[1] = 29;
        }

        if (month < 1 || month > 12)
        {
            cout << "\n[Invalid month! Must be between 1-12]\n";
            continue;
        }

        if (day < 1 || day > monthDays[month - 1])
        {
            cout << "\n[Invalid day for selected month]\n";
            continue;
        }

        break;
    }

    // Calculate day index
    int dayIndex = (month - 1) * 31 + (day - 1);

    cout << "\n------ Venue Availability on " << date << " ------\n\n";
    cout << "+---------+--------------+\n";
    cout << "|  Venue  |    Status    |\n";
    cout << "+---------+--------------+\n";

    for (int i = 0; i < 5; i++)
    {
        cout << "| " << left << setw(7) << ("Hall " + to_string(i + 1)) << " | ";
        if (eventAvail[dayIndex][i] == 1)
        {
            cout << left << setw(12) << "Booked" << " |\n";
        }
        else
        {
            cout << left << setw(12) << "Available" << " |\n";
        }
    }

    cout << "+---------+--------------+\n";

    cout << "\n[1] Back to Main Menu\n[2] Event booking\n[3] Display events\n[0] Exit\nSelect option: ";
    int option;
    cin >> option;

    switch (option)
    {
    case 1:
        return; // go back to main menu
    case 2:
        eventBooking(events, eventCount, eventAvail, EVENTS_FILE, ORGANISER_FILE);
        break;
    case 3:
        eventMonitoring(events, eventCount, eventAvail, EVENTS_FILE, ORGANISER_FILE, COMPLAINTS_FILE);
        break;
    case 0:
        cout << "\n[Exiting program...]\n";
        exit(0);
        break;
    default:
        cout << "Invalid option! Please select again!\n";
        break;
    }
}


void saveEvents(vector<Event> &events, int eventCount, const string &EVENTS_FILE)
{
    ofstream outEventFile(EVENTS_FILE);
    if (!outEventFile)
    {
        cout << "Error saving events\n";
        return;
    }

    for (int i = 0; i < eventCount; i++)
    {
        outEventFile << events[i].eventId << "|"
                     << events[i].eventStatus << "|"
                     << events[i].eventType << "|"
                     << events[i].eventName << "|"
                     << events[i].date.toString() << "|"
                     << events[i].timeDuration << "|"
                     << events[i].venue.venue << "|"
                     << events[i].equipments.chairs << ","
                     << events[i].equipments.tables << ","
                     << events[i].equipments.booths << ","
                     << events[i].equipments.projectors << ","
                     << events[i].equipments.bins << ","
                     << events[i].equipments.helpers << ","
                     << events[i].equipments.tents << "|"
                     << events[i].eventDesc << "|"
                     << events[i].orgaName.orgaName << "\n";
    }
    outEventFile.close();
}

void saveOrganiser(vector<Event> &events, int eventCount, const string &ORGANISER_FILE){
    ofstream outOrgFile(ORGANISER_FILE);
    if (!outOrgFile)
    {
        cout << "Error saving organiser details\n";
        return;
    }
    for(int i = 0; i < eventCount; i++){
        outOrgFile << events[i].eventId << "|"
               << events[i].orgaName.orgaName << "|"
               << events[i].orgaPhoneNo.orgaPhoneNo << "|"
               << events[i].orgaEmail.orgaEmail << "\n";
    }
    
    outOrgFile.close();
}

// Save all complaints for all events
void saveComplaints(const vector<Event>& events, const string& COMPLAINTS_FILE) {
    ofstream outFile(COMPLAINTS_FILE);
    if (!outFile) {
        cout << "Error saving complaints\n";
        return;
    }
    for (const auto& event : events) {
        for (const auto& c : event.complaints) {
            outFile << event.eventId << "|"
                    << c.complaintId << "|"
                    << c.complStatus << "|"
                    << c.complType << "|"
                    << c.reporterType << "|"
                    << c.complDesc << "\n";
        }
    }
    outFile.close();
}

// Load all complaints and assign to the correct event by eventId
void loadComplaints(vector<Event>& events, const string& COMPLAINTS_FILE) {
    ifstream inFile(COMPLAINTS_FILE);
    if (!inFile) {
        return; // No complaints file yet
    }
    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string eventIdStr, complaintIdStr, status, type, reporter, desc;
        if (!getline(ss, eventIdStr, '|')) continue;
        if (!getline(ss, complaintIdStr, '|')) continue;
        if (!getline(ss, status, '|')) continue;
        if (!getline(ss, type, '|')) continue;
        if (!getline(ss, reporter, '|')) continue;
        getline(ss, desc);

        try {
            int eventId = stoi(eventIdStr);
            int complaintId = stoi(complaintIdStr);
            // Find the event by eventId
            for (auto& event : events) {
                if (event.eventId == eventId) {
                    Complaint c;
                    c.eventId = eventId;
                    c.complaintId = complaintId;
                    c.complStatus = status;
                    c.complType = type;
                    c.reporterType = reporter;
                    c.complDesc = desc;
                    event.complaints.push_back(c);
                    break;
                }
            }
        } catch (...) {
            continue; // skip invalid lines
        }
    }
    inFile.close();
}

void loadEventFromFile(vector<Event> &events, int &eventCount, int eventAvail[12 * 31][5], const string &EVENTS_FILE)
{
    ifstream inEventFile(EVENTS_FILE);
    if (!inEventFile)
    {
        return; // no file yet
    }

    string line;
    eventCount = 0;

    while (getline(inEventFile, line))
    {
        if (line.empty())
            continue;

        stringstream ss(line);
        string idStr, eventStatusStr, eventTypeStr, eventNameStr, dateStr, timeStr, venueStr, equipmentStr, descStr, orgNameStr;

        // Get all fields separated by |
        if (!getline(ss, idStr, '|'))
            continue;
        if (!getline(ss, eventStatusStr, '|'))
            continue;
        if (!getline(ss, eventTypeStr, '|'))
            continue;
        if (!getline(ss, eventNameStr, '|'))
            continue;
        if (!getline(ss, dateStr, '|'))
            continue;
        if (!getline(ss, timeStr, '|'))
            continue;
        if (!getline(ss, venueStr, '|'))
            continue;
        if (!getline(ss, equipmentStr, '|'))
            continue;
        if (!getline(ss, descStr, '|'))
            continue;
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

        // Parse equipment string "x,y,z,..." with error handling
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


void loadOrganiserFromFile(vector<Event> &events, const string &ORGANISER_FILE){
    ifstream inOrgFile(ORGANISER_FILE);
    if (!inOrgFile)
    {
        return; // no organiser file yet
    }

    string line;
    while (getline(inOrgFile, line))
    {
        if (line.empty())
            continue;

        stringstream ss(line);
        string idStr, orgName, orgPhone, orgEmail;

        // Get all fields separated by |
        if (!getline(ss, idStr, '|'))
            continue;
        if (!getline(ss, orgName, '|'))
            continue;
        if (!getline(ss, orgPhone, '|'))
            continue;
        getline(ss, orgEmail);

        try
        {
            int eventId = stoi(idStr);
            
            // Find the corresponding event and update organiser details
            for (size_t i = 0; i < events.size(); i++)
            {
                if (events[i].eventId == eventId)
                {
                    events[i].orgaName.orgaName = orgName;
                    events[i].orgaPhoneNo.orgaPhoneNo = orgPhone;
                    events[i].orgaEmail.orgaEmail = orgEmail;
                    break;
                }
            }
        }
        catch (...)
        {
            continue; // Skip invalid entries
        }
    }
    
    inOrgFile.close();
}

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

int main()
{
    vector<Event> events;
    vector<Complaint> complaints;
    int eventCount = 0;
    int eventAvail[12 * 31][5] = {0};
    const string EVENTS_FILE = "events.txt";
    const string ORGANISER_FILE = "organiser.txt";
    const string COMPLAINTS_FILE = "complaints.txt";

    loadEventFromFile(events, eventCount, eventAvail, EVENTS_FILE);
    loadOrganiserFromFile(events, ORGANISER_FILE);
    loadComplaints(events, COMPLAINTS_FILE);
    mainMenu(events, eventCount, eventAvail, EVENTS_FILE, ORGANISER_FILE, COMPLAINTS_FILE);
    return 0;
}