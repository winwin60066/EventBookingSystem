#include "enum.h"
#include <iostream>
#include <fstream>
#include <string>
#define EVENTS_FILE "events.txt"
#define COMPLAINTS_FILE "complaints.txt"
#define VENUE_FILE "Venues.txt"
#define USER_FILE "Users.txt"

using namespace std;

//signUp
void signUp(vector<User>& users) {
    User user;

    cout << "\n===================================== Sign Up =====================================\n";
    cout << "Enter a username: ";
    cin >> user.name;
    cout << "Enter a password: ";
    cin >> user.password;

    bool found = false;
    for (const User& u : users) {
        if (u.name == user.name) {
            found = true;
            break;
        }
    }

    if (!found) {
        users.push_back(user);
        saveUsers(users);
        cout << "Sign up successful! You can now log in.\n\n";
    } else {
        cout << "Your username is already taken.\n\n";
    }
}

//userlogin
void userlogin(User& currentUser, vector<User>& users) {
    string username, password;
    bool found = false;

    cout << "\n======================================= Login ======================================\n";
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    for (User& user : users) { // Reference to update if needed
        if (user.name == username && user.password == password) {
            found = true;
            currentUser = user;
            break;
        }
    }

    if (found) {
        cout << "Login successful! Welcome, " << username << ".\n\n";
    } else {
        cout << "Invalid username or password. Try again.\n\n";
    }
}

//loginScreen
Screen loginScreen(User& currentUser, vector<User>& users) {
    currentUser = User{};
    
    int choice;

    while (true) {
        // ASCII art (assume full; truncated for brevity)
        cout << "ASCII ART HERE\n";
        cout << "\n=====================  Welcome to Seminar Registration System  =====================\n";
        cout << "[1] Sign Up\n";
        cout << "[2] Login\n";
        cout << "[3] Exit\n";
        cout << "Select your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                signUp(users);
                break;
            case 2:
                userlogin(currentUser, users);
                if (!currentUser.name.empty()) {
                    cin.ignore(9999, '\n');
                    return home;
                }
                break;
            case 3:
                cout << "Exiting...\n";
                return exiting;
            default:
                cout << "Invalid choice! Try again.\n";
        }
        cout << "press enter to continue";
        cin.ignore(9999, '\n');
        cin.ignore(9999, '\n');
    }
}