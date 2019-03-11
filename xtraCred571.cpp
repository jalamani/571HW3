#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h> 

using namespace std;

//<# of tasks><time to execute><active power @ 1188  Mhz><active  power  @  918  Mhz>  <active  power  @  648  Mhz>  <active  power  @  384 Mhz> <idle power @ lowest frequency>
//All other rows represent: <name of task> <deadline/period> <WCET @ 1188 Mhz> <WCET @ 918 Mhz> <WCET @ 648 Mhz> <WCET @ 384 Mhz>.
int main ( int argc, char *argv[] )
{
	ifstream infile;
	string argv2 = "EDF";
	int array[36];
int i=0;
int j=-1;
int k=0;
int orderSize = 0;
int numTask[5]; //#times each task is executed
int periods[5][10]; //assumes a task doesnt appear more than 10 times in a hyperperiod
int n=0;
int tmp;
int tmp2;
int tmp3;
int totalEnergy=0;
int totalTime = 0;
float totalIdle=0;
float convertedrunLength;
int flagCounter = 0;
int tobeInserted[50];
int idleFlag[50];
int timeStarted[50];
int order[50];
int orderNames[50];
int ordernameswIdle[50];
int execution1188[50];
int frequencies[50];
int runLength[50];
string temp[6];
string taskNames[6];
string values[37];
char cNum[10] ;

                infile.open ("input.txt", ifstream::in);
                if (infile.is_open())
                {
                        while (infile.good())
                        {
                                infile.getline(cNum, 256, ' ');

                                array[i]= atoi(cNum) ;
                                if(j==5)
                                {
                                	temp[k] = cNum;
                                	taskNames[k] = temp[k].substr(3);
                                	k++;
                                	j = 0;
                                }
                                i++;
                                j++;

                        }
                        infile.close();
                }
                else
                {
                        cout << "Error opening file";
                }
                //hyperPeriod = array[1];
                /*deadlines = 5*(i+1))+2
				7
				12
				17
				22
				27
                */
                for(i = 0; i < 5; i ++)
                {
                	numTask[i] = array[1]/array[(5*(i+1))+2];
                }
                for(i = 0; i<5; i++)
                {
                	k = numTask[i];
                	orderSize = orderSize + numTask[i];
                	for(j = 0; j < k; j++)
                	{
                		periods[i][j] = array[(5*(i+1))+2] * (j+1); //list of periods/deadlines
                		order[n] = array[(5*(i+1))+2] * (j+1);//array of periods/deadlines
                		orderNames[n] = i+1;//array of associated tasknames
                		execution1188[n] = array[(5*(i+1))+3];//array of execution times of this frequency
                		n++;
                	}
                }