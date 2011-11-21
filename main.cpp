#include <iostream>
#include <fstream>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "End.h"
#include "main.h"
#include "parse.h"
using namespace std;



//lynn is here   jas try
//hello world~

//for parameter measuring the efficiency!!!
int* finish_time;
int access_time = 0;
int collision_time = 0;

//for connection parameters
int lte::preambleNum = default_preambleNum;
int lte::EndNum = default_EndNum;
int lte::Simulation_Time = default_Simulation_Time;
int lte::MAX = default_MAX;
int lte::RandomBackoffIndex = default_RandomBackoffIndex;
int lte::firstWaiting = default_firstWaiting;
int lte::secondWaiting = default_secondWaiting;
int lte::traffic_type = default_start;

//

//global changeable variables
int _time = 1;// the order of the subframe
int* preamble;
int _count = 0;//to record the tutle number of successful and abort End
int _goodEnd = 0;
//

void responseForUseSamePreamble();
void set_traffic(End* end);
// to set the probablity of successful receive, the input is the number of the restransmission
int setProbablity( int );
void cleanup() {
	delete preamble;
	delete finish_time;
}


int main(int argc, char** argv) {

	lineParsing(argc, argv); //./simulate -end endnum -rand RandomBackoffIndex -type start_scenario
	cout << "connection scenario is setting..." << endl;
	
	preamble = new int[lte::preambleNum];
	finish_time = new int[lte::EndNum];
	
	//end definitions!!
    End end[lte::EndNum];
	set_traffic(end);
	
	//
	
    srand(time(NULL));
    for(; _count < lte::EndNum ; _time +=5 ){
	for( int n = 0; n < lte::EndNum; n++ )
	    end[n].setpreamble(); 
	/*-----debug-----
	for (int i = 0; i < 54; i++)
	    cout << i << " : " << preamble[i] << endl;
	//-----debug-----*/
	responseForUseSamePreamble();
	/*-----debug-----
	for (int i = 0; i < 54; i++)
	    cout << i << " : " << preamble[i] << endl;
	//-----debug-----*/
	for( int n = 0; n < lte::EndNum; n++ ){
	    end[n].responseForOnlyOne();
	    end[n].settime();
	}
 
	cout << _time << "\t" << _count << "\t" << _goodEnd << endl;
	
	for (int i = 0; i < lte::preambleNum; i++) {
	    preamble[i] = 0;
	}
    }
	
	cleanup();
    return 0;
}
void responseForUseSamePreamble()
{
    for( int i = 0; i < lte::preambleNum;  i++ ){
	if( preamble[i] > 1 ){
	//preamble[i] = setProbablity(1); 
	    if (setProbablity(1) == 0) { 
		preamble[i] = 0;
		/*/------debug-----
		cout << "setpro = 0\n";
		//------debug-----*/
	    }
	}
  }
}
int setProbablity( int retransmit )
{
 double p = 1 - exp((double) retransmit*(-1));
 int p1 = p*1000;
 int p2 = (1.0 - p)*1000;
 
 int key = rand() % (p1 + p2);
 
 if (key <= p1) return 1;
 else return 0;
}

void set_traffic(End* end) {
	if (lte::traffic_type == uniform_start) {
		for (int i = 0; i < lte::EndNum; i++) 
			end[i].setStartTime((rand()%12000)*5 + 1);
	}
	/*
	Beta(a,b): a=3 b=4    Beta(3,4) = 1/60
	p(t) = 6/10^5*t^2*(10-t)^3
	access_intensity(i) = EndNum * integral of p(t) from ti to ti+1
	*/
	else if (lte::traffic_type == beta_start) {
		char* fileName;
		string line;
		
		int index = 0;
		double tmp;
		int settime;
		
		switch(lte::EndNum) {
		case 100:
			fileName = "betabase.txt";
			break;
		case 1000:
			fileName = "betabase1000.txt";
			break;
		case 5000:
			fileName = "betabase5000.txt";
			break;
		case 10000:
			fileName = "betabase10000.txt";
			break;
		case 30000:
			fileName = "betabase30000.txt";
			break;
		default:
			cout << lte::EndNum << " is not included, please choose 100, 1000, 5000, 10000 or 30000!!!" << endl;
			exit(1);
		}
		
		ifstream inFile(fileName);
		
		while(getline(inFile, line)) {
			tmp = atof(line.c_str());
			settime = (int)floor(tmp*2000.0);
			end[index].setStartTime(settime*5 + 1);
			index++;
		}
	}
}