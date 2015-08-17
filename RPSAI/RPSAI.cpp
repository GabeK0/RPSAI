#include "stdafx.h"
#include <iostream>
#include <vector>
#include <time.h>
#include <stdlib.h>
#include <cstdlib>
#include <string>
#include <Windows.h>
#include <fstream>
#include <algorithm>


using namespace std;

void print(vector<int> x) {
	for (int i = 0; i < x.size(); i++)
		cout << x[i];
}

void save(vector<int> db) {
	ofstream file("data.txt");
	for (int i = 0; i < db.size(); i++)
		file << db[i];
}

void read(vector<int> &db, string name) {

	string line;
	if (name.length() < 4) {
		cout << "Appending .txt . . ." << endl;
		name.insert(name.length(), ".txt");
	} else if (name[name.length() - 1] != 't' && name[name.length() - 2] != 'x' && name[name.length() - 3] != 't' && name[name.length() - 4] != '.') {
		cout << "Appending .txt . . ." << endl;
		name.insert(name.length(), ".txt");
	}
	ifstream file(name);

	while (getline(file, line)) {
		for (int i = 0; i < line.length(); i++)
			db.push_back(line[i] - 48);
	}
}

int compare(vector<int> a, vector<int> b, int sa, int sb, int l) { //sa/sb = starting index for a and b, l = length of the pattern
	int x = 0;
	//cout << "Comparing a[" << sa << "] to b[" << sb << "]" << endl;
	for (int i = 0; i < l; i++)
		if (a[sa + i] != b[sb + i])
			x++;
	if (x == 0)
		//cout << "   Comparison was successful!" << endl;
	return x;
}

int findMax(vector<int> x) { //Find the location of the max value in a vector
	int y = x[0], z = 0;
	//cout << "Player responses after pattern: ";
	//print(x);
	for (int i = 0; i < x.size(); i++)
		if (x[i] > y) {
			y = x[i];
			z = i;
		}
	return z;
}

int findCommon(vector<int> x) { //Finds the most common element of a given vector
	vector<int> count; //Count of all of the values in the vector
	count.resize(1);
	count[0] = 0;
	for (int i = 0; i < x.size(); i++) {
		if (x[i] > count.size() - 1) {
			int diff = count.size() - 1; //Difference between the needed value and the available indexes in the vector
			count.resize(x[i] + 1);
			for (int i = diff; i < count.size(); i++)
				count[i] = 0;
		}
		count[x[i]]++;
	}
	return findMax(count);
}

int findAll(vector<int> x, int y) { //Returns the # of times a given element appears in vector x
	int count = 0;
	for (int i = 0; i < x.size(); i++)
		if (x[i] == y)
			count++;
	return count;
}

int getInput() {
	string x;
	do {
		cout << "Input your move: ";
		cin >> x;
	} while (int(x[0]) < 49 || int(x[0]) > 52 || x.length() != 1);
	if (int(x[0]) == 52)
		exit(0);

	return stoi(x)-1;
}

string translate(int x) {
	if (x == 0)
		return "rock";
	else if (x == 1)
		return "paper";
	else if (x == 2)
		return "scissors";
	else {
		string y = "ERROR: UNKNOWN MOVE ";
		y.push_back(x);
		return y;
	}

}

int beatMove(int x) {
	if (x != 2)
		return x + 1;
	else
		return 0;
}

void analysis(string name) { //TODO: FIX THIS SHIT
	vector<int> data;
	vector<string> patterns;
	read(data, name);
	for (int l = 2; l < 4; l++) { //Checking for patterns between length 2 and 4
		for (int i = 0; i < data.size() - l; i++) {
			string x;
			x.resize(l);
			for (int k = 0; k < l; k++) {
				char y = (char)data[i + k];
				cout << "Y: " << y << endl;
				x[k] = y;
			}
			patterns.push_back(x);
			cout << x << endl;
			for (int j = l; j < data.size() - l; j++) {
				if (compare(data, data, i, j, l) == 0) {
					patterns.push_back(x);
				}
			}
		}
	}
	for (int i = 0; i < patterns.size(); i++) {
		cout << patterns[i] << " ";
	}

}

void resolve(int a, int b, vector<int> &record) {
	//0 = tie, 1 = a wins, 2 = b wins
	int x;
	if (a == b)
		x = 0;
	else if (a == 0 && b == 2)
		x = 1;
	else if (a == 2 && b == 0)
		x = 2;
	else if (a > b)
		x = 1;
	else if (b > a)
		x = 2;
	else
		cout << "ERROR: UNKNOWN RESULT!" << endl;
	if (x == 0)
		cout << "Your " << translate(a) << " tied with the AI's " << translate(b) << "!" << endl;
	else if (x == 1)
		cout << "Your " << translate(a) << " beat the AI's " << translate(b) << "!" << endl;
	else if (x == 2)
		cout << "Your " << translate(a) << " lost to the AI's " << translate(b) << "!" << endl;

	record.push_back(x);
	cout << "\n\n\n";

}

void nextMoveGuess(vector<int> db, int s, int l, vector<int> &pChoice) { //Determines the most common next move for the player based on the past.

	vector<int> x; //Choices, for example x[0] is the amount of rocks that the user is likely to choose next.
	x.resize(3);
	vector<int> p; //A vector holding the pattern itself
	p.resize(l);
	for (int i = 0; i < l; i++)
		p[i] = db[s + i];
	for (int i = s; i < db.size() - l; i++)
		if (compare(db, p, i, 0, l) == 0)
			pChoice[db[i + l]]++;
	//cout << "After ";
	//print(p);
	//cout << " you usually go " << findMax(pChoice) << "." << endl;

}

int AI(vector<int> db, vector<int> record) { //TODO: Add in patterns of n length, with longer patterns having a higher weight. Highest weight pattern wins.
	vector<int> pChoice; //Player choice
	pChoice.resize(3);
	if (db.size() < 5) {
		//cout << "The AI does not have enough data, and will guess this turn." << endl;
		return rand() % 3;
	}
	for (int i = 0; i < db.size() - 3; i++) {
		if (compare(db, db, i, db.size() - 3, 3) == 0) {
			nextMoveGuess(db, i, 3, pChoice);
			i = db.size() - 3;
		}
	}
	if (record[record.size() - 1] == 1) {//If the player just won with a certain move
		pChoice[db[db.size() - 1]] += 1;
		//cout << "The AI also detected that you just won with " << translate(db[db.size() - 1]) << " and will likely play it again." << endl;
	}
	else if (record[record.size() - 1] == 2 && record[record.size() - 2] == 2 && db[db.size() - 1] == db[db.size() - 2]) { //If the player lost the last two games
		pChoice[beatMove(beatMove(db[db.size() - 1]))] += 2;
		pChoice[beatMove(db[db.size() - 1])] += 1;
		//cout << "The AI also detected that you lost the last two rounds with " << translate(db[db.size() - 1]) << "and will likely switch tactics." << endl;
	} if (pChoice[findMax(pChoice)] != 0) {
		cout << "The AI has made a prediction for this round based on your play style!" << endl;
		return beatMove(findMax(pChoice));
	}
	else {
		//cout << "The AI could not detect any patterns." << endl;
		//cout << "Because of this, it will just counter your most common move." << endl;
		return beatMove(findCommon(db));
	}
}

void play(vector<int> &db, string name, string filetitle) {
	ofstream recordFile(filetitle, ios::app);
	ofstream playerFile(name, ios::app);
	vector<int> record;
	record.resize(0);
	read(record, filetitle);
	while (1) {
		int x = getInput();
		resolve(x, AI(db, record), record);
		cout << "Current Record: " << findAll(record, 1) << " - " << findAll(record, 2) << endl;
		db.resize(db.size() + 1);
		db[db.size() - 1] = x;
		recordFile << record[record.size() - 1];
		playerFile << x;
	}
}

void start() {
	cout << "########################" << endl;
	cout << "Welcome to Gob's RPS AI!" << endl;
	cout << "########################" << endl;
	cout << "\n\nThings to keep in mind..." << endl;
	cout << "	- 1 is rock, 2 is paper, 3 is scissors." << endl;	
	cout << "	- To save and exit, input 4 at any time." << endl;
	cout << "	- Have fun!" << endl;
	cout << "\n\n\n" << endl;
}

int _tmain(int argc, _TCHAR* argv[]) {

	srand(time(NULL));
	string name, filetitle;
	int x;
	vector<int> db; //Database
	//db.resize(0);

	start();
	cout << "What is your name? ";
	cin >> name;
	if (name == "analysis") {
		cout << "Initializing analysis mode..." << endl;
		cout << "Please input player to analyze: ";
		cin >> name;
		analysis(name);
		cin.ignore();
		cin.ignore();
		cin.ignore();
		return 0;
	}

	filetitle = name;
	filetitle.insert(filetitle.length(), "Record.txt");
	name.insert(name.length(), ".txt");
	cout << "\n";
	read(db, name);
	if (db.size() > 0) {
		cout << "User recognized, history loaded!" << endl;
	} else
		cout << "Could not recognize user, creating new file." << endl;

	cout << "\n\n\n" << endl;
	play(db, name, filetitle);

	return 0;

}