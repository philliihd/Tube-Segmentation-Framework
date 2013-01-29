#include "parameters.hpp"
#include <fstream>
#include <cmath>
#include <iostream>
using namespace std;

vector<string> split(string str, string delimiter) {
	vector<string> list;
	int start = 0;
	int end = str.find(delimiter);
	while(end != str.npos) {
		list.push_back(str.substr(start, end-start));
		start = end+1;
		end = str.find(delimiter, start);
	}
	// add last
	list.push_back(str.substr(start));

	return list;
}

paramList initParameters() {
	paramList parameters;

	std::ifstream file;
	file.open("parameters/parameters");
	string line;
	getline(file, line);
	getline(file, line); // throw away the first comment line
	while(file.good()) {
		int pos = 0;
		pos = line.find(" ");
		string name = line.substr(0, pos);
		line = line.substr(pos+1);
		pos = line.find(" ");
		string type = line.substr(0, pos);
		line = line.substr(pos+1);
		pos = line.find(" ");
		string defaultValue = line.substr(0,pos);

		if(type == "bool") {
			BoolParameter v = BoolParameter(defaultValue == "true");
			parameters.bools[name] = v;
		} else if(type == "num") {
			line = line.substr(pos+1);
			pos = line.find(" ");
			float min = atof(line.substr(0,pos).c_str());
			line = line.substr(pos+1);
			pos = line.find(" ");
			float max = atof(line.substr(0,pos).c_str());
			line = line.substr(pos+1);
			float step = atof(line.c_str());
			NumericParameter v = NumericParameter(atof(defaultValue.c_str()), min, max, step);
			parameters.numerics[name] = v;
		} else if(type == "str") {
			vector<string> list ;
			if(line.size() > pos) {
				list = split(line.substr(pos+1), " ");
			}

			StringParameter v = StringParameter(defaultValue, list);
			parameters.strings[name] = v;
		} else {
			throw exception();
		}

		getline(file, line);
	}

	return parameters;
}

paramList setParameter(paramList parameters, string name, string value) {
	if(parameters.bools.count(name) > 0) {
		BoolParameter v = parameters.bools[name];
		v.set(true);
		parameters.bools[name] = v;
	} else if(parameters.numerics.count(name) > 0) {
		NumericParameter v = parameters.numerics[name];
		if(!v.validate(atof(value.c_str()))) {
			cout << "invalid value for " << name << endl;
			throw exception();
		}
		v.set(atof(value.c_str()));
		parameters.numerics[name] = v;
	} else if(parameters.strings.count(name) > 0) {
		StringParameter v = parameters.strings[name];
		if(!v.validate(value)) {
			cout << "invalid value for " << name << endl;
			throw exception();
		}
		v.set(value);
		parameters.strings[name] = v;
	} else {
		throw exception();
	}

	return parameters;

}

float getParam(paramList parameters, string parameterName) {
	if(parameters.numerics.count(parameterName) == 0) {
		cout << parameterName << " not found" << endl;
 		throw exception();
	}
	NumericParameter v = parameters.numerics[parameterName];
	return v.get();
}

bool getParamBool(paramList parameters, string parameterName) {
	if(parameters.bools.count(parameterName) == 0) {
		cout << parameterName << " not found" << endl;
		throw exception();
	}
	BoolParameter v = parameters.bools[parameterName];
	return v.get();
}

string getParamStr(paramList parameters, string parameterName) {
	if(parameters.strings.count(parameterName) == 0) {
		cout << parameterName << " not found" << endl;
		throw exception();
	}
	StringParameter v = parameters.strings[parameterName];
	return v.get();
}

paramList getParameters(int argc, char ** argv) {
	paramList parameters = initParameters();

    // Go through each parameter, first parameter is filename
    for(int i = 2; i < argc; i++) {
        string token = argv[i];
        if(token.substr(0,2) == "--") {
            // Check to see if the parameter has a value
            string nextToken;
            if(i+1 < argc) {
                nextToken = argv[i+1];
                if(nextToken.substr(0,2) == "--") {
                	nextToken = "";
                } else {
					i++;
                }
            } else {
                nextToken = "";
            }
			parameters = setParameter(parameters, token.substr(2), nextToken);
        }
    }

	return parameters;
}

BoolParameter::BoolParameter(bool defaultValue) {
	this->value = defaultValue;
}

bool BoolParameter::get() {
	return this->value;
}

void BoolParameter::set(bool value) {
	this->value = value;
}

NumericParameter::NumericParameter(float defaultValue, float min, float max, float step) {
	this->value = defaultValue;
	this->min = min;
	this->max = max;
	this->step = step;
}

float NumericParameter::get() {
	return this->value;
}

void NumericParameter::set(float value) {
	if(this->validate(value)) {
		this->value = value;
	}
}

bool NumericParameter::validate(float value) {
	return value >= min && value <= max && floor((value-min)/step) == (value-min)/step;
}

StringParameter::StringParameter(string defaultValue, vector<string> possibilities) {
	this->value = defaultValue;
	this->possibilities = possibilities;
}

string StringParameter::get() {
	return this->value;
}

void StringParameter::set(string value) {
	if(this->validate(value)) {
		this->value = value;
	}
}

bool StringParameter::validate(string value) {
	if(possibilities.size() > 0) {
		vector<string>::iterator it;
		bool found = false;
		for(it=possibilities.begin();it!=possibilities.end();it++){
			if(value == *it) {
				found = true;
				break;
			}
		}
		return found;
	} else {
		return true;
	}
}
