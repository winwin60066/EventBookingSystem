/*
*  the enum is defined in another file so it's value is consistant in every other file
*  the .h is so that it can be consistant and not redefined in other files to keep value consistant
*/
#ifndef SCREENS_H
#define SCREENS_H
#include <vector>
#include <string>
#include <iostream>

struct Date {
	int day;
	int month;
	int year;

	// operator == to check if booked or not
	bool operator==(const Date& other) const {
		return day == other.day && month == other.month && year == other.year;
	}
};

struct Venue {
	std::string venue;
	std::string avaEquipment;
	std::string size;
	std::string desc;
	std::vector<Date> datesBooked;
	double price;

	bool operator==(const Venue& other) const {
		return venue == other.venue && avaEquipment == other.avaEquipment && size == other.size;
	}
};

enum Screen {
	login,
	home,
	adPage,
	viewEvent,
	newEvent,
	payment,
	comments,
	exiting,
	editEvent
};

struct Equipment {
	int chairs;
	int tables;
	int booths;
	int projectors;
	int bins;
	int helpers;
	int tents;

	// allow to save like an array (using [] symbol)
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

struct Event {
	int id;
	std::string eventType;
	Date date;
	Venue venue;
	Equipment equipments;
	std::string eventName;
	std::string eventDesc;
	double paid = 0.00;
	double entryFee;
	User owner;
};

struct User {
	std::string name;
	std::string password;
	std::vector<Event> reservation;
};

struct Comment {
	std::string username;
	std::string comment;
};

// inline is to allow multiple modules to implement this
inline void printAll(std::initializer_list<std::string> list) {
	int counter = 1;
	for (const auto& b : list) {
		std::cout << "press " << counter << " for " << b << "\n";
		counter++;
	}
}

inline void pressEnter() {
	std::cout << "press enter to continue";
	std::cin.ignore(9999, '\n');
}

Screen loginScreen();
Screen newEventScreen(std::vector<Event>&, std::vector<Venue>, User);

void switchScreen();
void switchBack();
bool checkInt(int&);
bool checkValidDate(Event);
Event getDate(Event);
std::vector<Venue> getValidVenues(std::vector<Venue>, Date);
Venue getVenue(std::vector<Venue>&, Date);
Equipment getEquipment(Equipment);

Screen editEventScreen(std::vector<Event>&, std::vector<Venue>, int);

#endif