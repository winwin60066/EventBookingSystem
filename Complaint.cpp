#include "enum.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
using namespace std;


//manageComplaint
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


//saveComplaints
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


//loadComplaints
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
