#include "enum.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
using namespace std;


//eventBooking
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


//checkAvailability
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
