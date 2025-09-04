#include "enum.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Function used to sign up a new user
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

    if(!found){
        users.push_back(user);
        cout << "Sign up successful! You can now log in.\n\n";
    }
    else {
        cout << "Your username is already taken.\n\n";
    }
}

// Function used to log in an existing user
void userlogin(User& currentUser,  vector<User>& users) {
    string username, password, u, p;
    bool found = false;

    cout << "\n======================================= Login ======================================\n";
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    for (User user : users) {
        if (user.name == username && user.password == password) {
            found = true;
            currentUser = user;
        }
    }

    if (found) {
        cout << "Login successful! Welcome, " << username << ".\n\n";
    }
    else {
        cout << "Invalid username or password. Try again.\n\n";
    }
}

Screen loginScreen(User& currentUser, vector<User>& users) {

    currentUser = User{};
    
        int choice;

        while (true) {
            cout << "  /$$$$$$                          /$$                                                                 \n";
            cout << " /$$__  $$                        |__/                                                                 \n";
            cout << "| $$  \\__/  /$$$$$$  /$$$$$$/$$$$  /$$ /$$$$$$$   /$$$$$$   /$$$$$$                                   \n";
            cout << "|  $$$$$$  /$$__  $$| $$_  $$_  $$| $$| $$__  $$ |____  $$ /$$__  $$                                  \n";
            cout << " \\____  $$| $$$$$$$$| $$ \\ $$ \\ $$| $$| $$  \\ $$  /$$$$$$$| $$  \\__/                                  \n";
            cout << " /$$  \\ $$| $$_____/| $$ | $$ | $$| $$| $$  | $$ /$$__  $$| $$                                        \n";
            cout << "|  $$$$$$/|  $$$$$$$| $$ | $$ | $$| $$| $$  | $$|  $$$$$$$| $$                                        \n";
            cout << " \\______/  \\_______/|__/ |__/ |__/|__/|__/  |__/ \\_______/|__/                                        \n";
            cout << "                                                                                                      \n";
            cout << "                                                                                                      \n";
            cout << " /$$$$$$$                      /$$             /$$                          /$$     /$$              \n";
            cout << "| $$__  $$                    |__/            | $$                         | $$    |__/              \n";
            cout << "| $$  \\ $$  /$$$$$$   /$$$$$$  /$$  /$$$$$$$ /$$$$$$    /$$$$$$  /$$$$$$  /$$$$$$   /$$  /$$$$$$  /$$$$$$$ \n";
            cout << "| $$$$$$$/ /$$__  $$ /$$__  $$| $$ /$$_____/|_  $$_/   /$$__  $$|____  $$|_  $$_/  | $$ /$$__  $$| $$__  $$\n";
            cout << "| $$__  $$| $$$$$$$$| $$  \\ $$| $$|  $$$$$$   | $$    | $$  \\__/ /$$$$$$$  | $$    | $$| $$  \\ $$| $$  \\ $$\n";
            cout << "| $$  \\ $$| $$_____/| $$  | $$| $$ \\____  $$  | $$ /$$| $$      /$$__  $$  | $$ /$$| $$| $$  | $$| $$  | $$\n";
            cout << "| $$  | $$|  $$$$$$$|  $$$$$$$| $$ /$$$$$$$/  |  $$$$/| $$     |  $$$$$$$  |  $$$$/| $$|  $$$$$$/| $$  | $$\n";
            cout << "|__/  |__/ \\_______/ \\____  $$|__/|_______/    \\___/  |__/      \\_______/   \\___/  |__/ \\______/ |__/  |__/\n";
            cout << "                     /$$  \\ $$                                                                       \n";
            cout << "                    |  $$$$$$/                                                                       \n";
            cout << "                     \\______/                                                                        \n";
            cout << "  /$$$$$$                        /$$                                                                  \n";
            cout << " /$$__  $$                      | $$                                                                  \n";
            cout << "| $$  \\__/ /$$   /$$  /$$$$$$$ /$$$$$$    /$$$$$$  /$$$$$$/$$$$                                       \n";
            cout << "|  $$$$$$ | $$  | $$ /$$_____/|_  $$_/   /$$__  $$| $$_  $$_  $$                                      \n";
            cout << " \\____  $$| $$  | $$|  $$$$$$   | $$    | $$$$$$$$| $$ \\ $$ \\ $$                                      \n";
            cout << " /$$  \\ $$| $$  | $$ \\____  $$  | $$ /$$| $$_____/| $$ | $$ | $$                                      \n";
            cout << "|  $$$$$$/|  $$$$$$$ /$$$$$$$/  |  $$$$/|  $$$$$$$| $$ | $$ | $$                                      \n";
            cout << " \\______/  \\____  $$|_______/    \\___/   \\_______/|__/ |__/ |__/                                      \n";
            cout << "           /$$  | $$                                                                                \n";
            cout << "          |  $$$$$$/                                                                                \n";
            cout << "           \\______/                                                                                 \n";
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