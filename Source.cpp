#include "Parameters.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

string FILENAME;
int DELAY;

string GenNewFileName();
void sync_subs(string newfile);
string ChangeTimeStamps(string line);
string CreateNewTimestamps(string s_h, string s_m, string s_s, string s_ms, string e_h, string e_m, string e_s, string e_ms);
string AddDelayToTimestamp(string hour, string minute, string second, string millisecond);

int main(int argc, char** argv) {
	if (argc == 1) {
		FILENAME = default_FILENAME;
		DELAY = default_DELAY;
	}
	else {
		if (argv[1] == "-h") {
			cout << "sync subs.exe [FILENAME] [DELAY]" << endl;
		}
		FILENAME = argv[1];
		DELAY = stoi(argv[2]);
	}

	string newfile = GenNewFileName();
	
	sync_subs(newfile);
	
	return 0;
}

string GenNewFileName() {

	stringstream ss;
	ss << "syncd_" << FILENAME;

	return ss.str();

}

void sync_subs(string newfile) {
	fstream OldFile;
	OldFile.open(FILENAME, ios::in);

	if (!OldFile) {
		cout << "Couldn't open file" << endl;
		
	}else {
		fstream NewFile;
		NewFile.open(newfile, ios::out);

		string line;
		
		while (getline(OldFile, line)) {

			size_t found = line.rfind("-->");
			if (found != std::string::npos) {

				NewFile << ChangeTimeStamps(line) << endl;

			}
			else {

				NewFile << line << endl;
			}

		}
	}

}

string ChangeTimeStamps(string line) {

	string S_hours, S_minutes, S_seconds, S_milliseconds, E_hours, E_minutes, E_seconds, E_milliseconds;
	int using_unit = 0;
	stringstream S_ms, S_sec, S_min, S_hr, E_ms, E_sec, E_min, E_hr;

	for (int i = 0; i < line.length(); i++) {

		if (line[i] != ' ' && line[i] != '-') {
			if (line[i] == ':' || line[i] == '>' || line[i] == ',') {
				using_unit++;
			}
			else {
				switch (using_unit)
				{

				case 0:
				{
					S_hr << line[i];
					break;
				}
				case 1:
				{
					S_min << line[i];
					break;
				}
				case 2:
				{
					S_sec << line[i];
					break;
				}
				case 3:
				{
					S_ms << line[i];
					break;
				}
				case 4:
				{
					E_hr << line[i];
					break;
				}
				case 5:
				{
					E_min << line[i];
					break;
				}
				case 6:
				{
					E_sec << line[i];
					break;
				}
				case 7:
				{
					E_ms << line[i];
					break;
				}

				}
			}

		}
	}
	
	return CreateNewTimestamps(
		S_hr.str(), S_min.str(), S_sec.str(), S_ms.str(),
		E_hr.str(), E_min.str(), E_sec.str(), E_ms.str());
}



string CreateNewTimestamps(string s_h, string s_m, string s_s, string s_ms, string e_h, string e_m, string e_s, string e_ms) {
	
	stringstream ss;

	ss << AddDelayToTimestamp(s_h, s_m, s_s, s_ms) << " --> " << AddDelayToTimestamp(e_h, e_m, e_s, e_ms);

	return ss.str();
}

string AddDelayToTimestamp(string hour, string minute, string second, string millisecond) {

	int h, m, s, ms;
	h = stoi(hour);
	m = stoi(minute);
	s = stoi(second);
	ms = stoi(millisecond);

	string s_h, s_m, s_s, s_ms;

	int HR=0, MIN=0, SEC=0, MLS=0;
	if(DELAY > 0){
		MLS = ms + DELAY;
		while (MLS >= 1000) {
			SEC++;
			MLS -= 1000;
		}

		SEC += s;
		while (SEC >= 60) {
			MIN++;
			SEC -= 60;
		}

		MIN += m;
		while (MIN >= 60) {
			HR++;
			MIN -= 60;
		}

		HR += h;

		s_h = to_string(HR);
		s_m = to_string(MIN);
		s_s = to_string(SEC);
		s_ms = to_string(MLS);

		if (HR < 10) {
			s_h.insert(0, "0");
		}
		if (MIN < 10) {
			s_m.insert(0, "0");
		}
		if (SEC < 10) {
			s_s.insert(0, "0");
		}
		if (MLS < 100) {
			if (MLS < 10) {
				s_ms.insert(0, "00");
			}
			else {
				s_ms.insert(0, "0");
			}
		}
	}
	else {
		MLS = ms + DELAY;
		while (MLS < 0) {
			SEC--;
			MLS += 1000;
		}

		SEC += s;
		while (SEC < 0) {
			MIN--;
			SEC += 60;
		}

		MIN += m;
		while (MIN < 0) {
			HR--;
			MIN += 60;
		}

		HR += h;
		if (HR < 0) {
			HR = 0;
		}

		s_h = to_string(HR);
		s_m = to_string(MIN);
		s_s = to_string(SEC);
		s_ms = to_string(MLS);

		if (HR < 10) {
			s_h.insert(0, "0");
		}
		if (MIN < 10) {
			s_m.insert(0, "0");
		}
		if (SEC < 10) {
			s_s.insert(0, "0");
		}
		if (MLS < 100) {
			if (MLS < 10) {
				s_ms.insert(0, "00");
			}
			else {
				s_ms.insert(0, "0");
			}
		}
	}


	stringstream ss;
	ss << s_h << ":" << s_m << ":" << s_s << "," << s_ms;

	cout << ss.str() << endl;

	return ss.str();
}