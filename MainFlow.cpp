#include "enum.h"
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <sstream>

#define EVENTS_FILE "events.txt" //EVENT_FILE = "Events.txt"
#define COMPLAINTS_FILE "complaints.txt"
#define VENUE_FILE "Venues.txt"
#define USER_FILE "Users.txt"
using namespace std;

//TODO: init(load file)

int main(){
    Screen currentScreen = LoginRegister;
    vector<Event> events;
    vector<Venue> venues;
	vector<User> users;
    

    //TODO: init(load file)

    //TODO eventCount change to currentEvent maybe
    int eventCount = 0;
    int eventAvail[12*31][5] = {0};

    /*
    venues.push_back(
		Venue{"venue","ava equipment","size","desc"}
	);
    */

    User currentUser;

    //TODO: init(load file)

    while(currentScreen != exiting){
        switch(currentScreen){
            case LoginRegister:{
                currentScreen = loginScreen(currentUser, users);
                break;
            }
            //TODO event booking
            case EventBooking:{
                currentScreen = bookingScreen(currentUser, users);break;
            } 
            //TODO event management(event monitoring)
            case EventMonitoring:
            {
                currentScreen = monitoringScreen(currentUser, users, events, eventCount, eventAvail, EVENTS_FILE, COMPLAINTS_FILE);break;
            } 
            /*
            case adPage: {
				currentScreen = ad();
				break;
			}
            case home: {
				// temp to see if the thing works
				currentScreen = mainMenu();
				if (currentScreen == LoginRegister) {
					currentUser = User{};
				}
				break;
			}
            case editEvent: {
				currentScreen = editEventScreen(events, venues, selectedEvent.id);
				saveEvents(events);
				break;
			}
            */

        }
    }

}