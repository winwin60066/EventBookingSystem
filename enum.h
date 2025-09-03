#ifndef SCREENS_H
#define SCREENS_H

#include <vector>
#include <string>
#include <iostream>
#include <sstream>

// ===== 数据结构 =====
struct Date {
    int day, month, year;
    bool operator==(const Date& other) const {
        return day == other.day && month == other.month && year == other.year;
    }
    std::string toString() const {
        std::stringstream ss;
        ss << year << "-" << (month<10?"0":"") << month << "-" << (day<10?"0":"") << day;
        return ss.str();
    }
};

struct Venue {
    std::string venue;
    std::string avaEquipment;
    std::string size;
    std::string desc;
    std::vector<Date> datesBooked;
    double price;
};

struct Equipment {
    int chairs, tables, booths, projectors, bins, helpers, tents;

    int& operator[](size_t index) {
        switch (index) {
            case 0: return chairs;
            case 1: return tables;
            case 2: return booths;
            case 3: return projectors;
            case 4: return bins;
            case 5: return helpers;
            case 6: return tents;
            default: throw std::out_of_range("Invalid index");
        }
    }
};

struct Organiser {
    std::string orgaName;
    std::string orgaPhoneNo;
    std::string orgaEmail;
};

struct Participant {
    std::string partiName;
    bool attended;
};

struct Complaint {
    int eventId;
    int complaintId;
    std::string complStatus;
    std::string complType;
    std::string reporterType;
    std::string complDesc;
};

struct Event {
    std::string eventStatus;
    int eventId;
    std::string eventType;
    std::string eventName;
    Date date;
    std::string startTime, endTime, timeDuration;
    Venue venue;
    Equipment equipments;
    std::string eventDesc;
    double paid = 0.00;
    int expectedPartiQty;
    int actualPartiQty;
    Organiser orgaName;
    Organiser orgaPhoneNo;
    Organiser orgaEmail;
    std::vector<Participant> participants;
    std::vector<Complaint> complaints;
};

// ====== 函数声明 ======
// main menu
void mainMenu(std::vector<Event>&, int&, int[12*31][5],
              const std::string&, const std::string&, const std::string&);

// event management
void eventBooking(std::vector<Event>&, int&, int[12*31][5],
                  const std::string&, const std::string&);
void checkAvailability(std::vector<Event>&, int&, int[12*31][5],
                       const std::string&, const std::string&, const std::string&);
void eventMonitoring(std::vector<Event>&, int, int[12*31][5],
                     const std::string&, const std::string&, const std::string&);
void searchEvents(std::vector<Event>&, int);
void deleteEvent(std::vector<Event>&, int&, int[12*31][5], const std::string&);
void updateEventStatus(std::vector<Event>&, int, int[12*31][5],
                       const std::string&, const std::string&, const std::string&);
void partiAttendance(std::vector<Event>&, int);

// complaints
void manageComplaint(std::vector<Event>&, int, const std::string&);
void saveComplaints(const std::vector<Event>&, const std::string&);
void loadComplaints(std::vector<Event>&, const std::string&);

// file io
void saveEvents(std::vector<Event>&, int, const std::string&);
void loadEventFromFile(std::vector<Event>&, int&, int[12*31][5], const std::string&);
void saveOrganiser(std::vector<Event>&, int, const std::string&);
void loadOrganiserFromFile(std::vector<Event>&, const std::string&);

// utils
void pressEnterToContinue();
void displayEvent(std::vector<Event>&, int);
void displaySearchedEvents(std::vector<Event>&);

#endif
