#include<iostream>
#include<string>
#include<fstream>
#include<vector>

using namespace std;

int main(){
	string file;
	cout << "Ingrese nombre del archivo:";
	cin >> file;

	string line;
	ifstream myfile;
	vector<string> palabras;

	myfile.open (file);

	while ( getline (myfile,line)){
			if((line.length() >= 0) && (line!="\0")){
			cout << line << endl;
		}
	}
}
