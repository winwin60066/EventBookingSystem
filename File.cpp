#include "enum.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
using namespace std;


//saveEvents
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
                     << events[i].entryFee << "|"
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


//loadEventFromFile
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


//saveOrganiser
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



//loadOrganiserFromFile
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
