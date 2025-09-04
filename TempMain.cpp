#include "enum.h"
#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

Screen menu() {
	system("cls");
	cout << "temp menu...\n";
	printAll({ "Search event", "new Event", "login","exit" });
	int input;
	cin >> input;
	cin.ignore(9999, '\n');

	switch (input) {
	case 1: {
		return viewEvent;
	}
	case 2: return newEvent;
	case 3: return login;
	case 4: return exiting;
	default:
		cout << "Invalid choice! Returning to menu.\n";
		return menu();
	}
}