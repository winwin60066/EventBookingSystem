#include "enum.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>
#include <ctime>
#include <cctype>
using namespace std;

// Forward declarations
bool checkInt(int&);
void pressEnter();
void printAll(const vector<string>& items);
void saveEvents(vector<Event>& events, int eventCount, const string& filename);
Screen EventMonitoring(vector<Event>& events, int& eventCount, int eventAvail[12*31][5], const string& EVENTS_FILE, const string& COMPLAINTS_FILE, Equipment equipment);
Screen MainMenu();

// Function declarations
Venue getVenue(vector<Venue>& venues, Date date);
Event getDate(Event event);
vector<Venue> getValidVenues(vector<Venue> venues, Date date);
Equipment getEquipment(Equipment equipment);

// Screen switching functions
void switchScreen() {
    cout << "\033[?1049h";
}

void switchBack() {
    cout << "\033[?1049l";
}

//eventBooking function
Screen bookingScreen(vector<Event> &events, int &eventCount, int eventAvail[12 * 31][5], const string &EVENTS_FILE ,const string &COMPLAINTS_FILE, Equipment &equipment)  
{
    Event newEvent;
    char confirmation;
    string selfType;

    do
    {
        cout << "\n------ Event Booking ------\n";
        int eventTypeChoice;
        cout << "\n[1] Seminar\n[2] Talk\n[3] Conference\n[4] Others(self type)\nSelect event type: ";
        cin >> eventTypeChoice;

        if (eventTypeChoice == 1)
            newEvent.eventType = "Seminar";
        else if (eventTypeChoice == 2)
            newEvent.eventType = "Talk";
        else if(eventTypeChoice == 3)
            newEvent.eventType = "Conference";
        else if(eventTypeChoice == 4){
            cout << "Enter other type: ";
            cin >> selfType;
            newEvent.eventType = selfType;
        }
        else {
            cout << "Invalid choice! Please try again.\n";
            continue;
        }
            
        cin.ignore();
        while(newEvent.eventName == "") {
            cout << "Insert event name: ";
            getline(cin, newEvent.eventName);
            if (newEvent.eventName == "") {
                cout << "[Event name cannot be empty. Please try again!]\n";
            }
        }

        // Date validation loop
        bool repeat = false;
        while (true)
        {
            //day
            do{
                cout << "\n[Enter Event Date]\n";
                cout << "Day: ";
                cin >> newEvent.date.day;
                if(!checkInt(newEvent.date.day)){
                    cout << "\nPlease insert a number\n";
                    repeat = true;
                }else{
                    repeat = false;
                }
            }while(repeat);

            //month
            do{
                cout << "\nMonth: ";
                cin >> newEvent.date.month;
                if(!checkInt(newEvent.date.month)){
                    cout << "\nPlease insert a number\n";
                    repeat = true;
                }else{
                    repeat = false;
                }
            }while(repeat);

            //year
            do{
                cout << "\nYear: ";
                cin >> newEvent.date.year;
                if(!checkInt(newEvent.date.year)){
                    cout << "\nPlease insert a number\n";
                    repeat = true;
                }else{
                    repeat = false;
                }
            }while(repeat);

            // Year range check
            time_t now = time(0);
            tm *ltm = localtime(&now);
            int currentYear = 1900 + ltm->tm_year;
            if (newEvent.date.year < currentYear || newEvent.date.year > currentYear + 5)
            {
                cout << "\n[Year out of allowed range! (" << currentYear << " - " << currentYear + 5 << ")]\n";
                continue;
            }

            // Leap year & month/day checks
            int monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
            if ((newEvent.date.year % 4 == 0 && newEvent.date.year % 100 != 0) || (newEvent.date.year % 400 == 0))
                monthDays[1] = 29;

            if (newEvent.date.month < 1 || newEvent.date.month > 12)
            {
                cout << "\n[Invalid month! Must be 1-12]\n";
                continue;
            }
            if (newEvent.date.day < 1 || newEvent.date.day > monthDays[newEvent.date.month - 1])
            {
                cout << "\n[Invalid day for this month]\n";
                continue;
            }

            break; // Valid date
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

        //entry fee
        while (true) {
            cout << "\nEntry fee for each participant(Non-adjustable) : RM ";
            if (cin >> newEvent.entryFee) {  // Changed from event.entryFee to newEvent.entryFee
                // if successful then break
                cin.ignore(9999, '\n');
                break;
            }
            else {
                cin.clear();
                cin.ignore(9999, '\n');
                cout << "[Entry fee cannot be empty! Please try again!]";
            }
        }

        // Get equipment
        Equipment selectedEquipment = getEquipment(equipment);
        newEvent.equipments = selectedEquipment;

        cin.ignore();
        while (newEvent.eventDesc == "") {  // Changed from event.eventDesc to newEvent.eventDesc
            cout << "\nInsert event Description: ";
            getline(cin, newEvent.eventDesc);  // Changed from event.eventDesc to newEvent.eventDesc
            if (newEvent.eventDesc == "") {
                cout << "[Event Description cannot be empty. Please try again!]\n";
            }
        }

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

    int option;
    do
    {
        cout << "\n[1] Manage and View Event\n[2] Back To Main Menu\nSelect Option: ";
        cin >> option;

        switch(option){
            case 1: return EventMonitoring(events, eventCount, eventAvail, EVENTS_FILE, COMPLAINTS_FILE, equipment);
            case 2: return MainMenu();
            default: cout << "Invalid option! Please try again!"; 
        }

    } while (true);
}

//checkAvailability
void checkAvailability(vector<Event> &events, int &eventCount, int eventAvail[12 * 31][5],const string &EVENTS_FILE, const string &COMPLAINTS_FILE)
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
        Equipment tempEquipment; // Create a temporary equipment object
        eventBooking(events, eventCount, eventAvail, EVENTS_FILE, COMPLAINTS_FILE, tempEquipment);
        break;
    case 3:
        Equipment equipment;
        EventMonitoring(events, eventCount, eventAvail, EVENTS_FILE, COMPLAINTS_FILE, equipment);
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

Equipment getEquipment(Equipment equipment) {
    string equipNames[] = {"chairs", "tables", "booths", "projectors", "bins", "helpers", "tents"};
    int equipLength = sizeof(equipNames) / sizeof(equipNames[0]);

    switchScreen();
    system("cls");
    int input = 1;
    int equipNum = 0;
    while (input != 0) {
        cout << "Are there any equipments that you would like to borrow?\n\n";
        cout << "You have currently borrowed: \n";
        for (int i = 0; i < equipLength; i++) {
            cout << left << setw(11) << equipNames[i] << " : " << equipment[i] << "\n";
        }
        cout << "\n";
        printAll({ "chairs", "tables", "booths", "projectors", "bins", "helpers", "tents" });
        cout << "press 0 when done\n";
        cout << "Select equipment (1-" << equipLength << "): ";
        if (!checkInt(input)) {
            cout << "please insert a valid number\n";
            pressEnter();
            continue;
        }
        if (input == 0) break;
        if (input < 1 || input > equipLength) {
            cout << "please insert 1 - " << equipLength << "\n";
            pressEnter();
            continue;
        }
        // get number of equipment
        while (true) {
            cout << "How many " << equipNames[input - 1] << " would you like to borrow: ";
            if (!checkInt(equipNum)) {
                cout << "please insert a valid number\n";
                pressEnter();
            }
            else if (equipNum < 0) {
                cout << "Invalid number of equipment borrowed\n\n";
                pressEnter();
            }
            else {
                // Access equipment members properly
                switch(input - 1) {
                    case 0: equipment.chairs = equipNum; break;
                    case 1: equipment.tables = equipNum; break;
                    case 2: equipment.booths = equipNum; break;
                    case 3: equipment.projectors = equipNum; break;
                    case 4: equipment.bins = equipNum; break;
                    case 5: equipment.helpers = equipNum; break;
                    case 6: equipment.tents = equipNum; break;
                }
                break;
            }
        }
        equipNum = 0;
        system("cls");
    }
    system("cls");
    switchBack();
    return equipment;
}

