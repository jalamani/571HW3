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
                
                /*
                WCET@1188
                array[8]-w1
				array[14]-w2
				array[20]-w3
				array[26]-w4
				array[32]-w5

				deadlines
				7
				13
				19
				25
				31
                */
                if(argv2 == "EDF") //replace with argv[2] == 'EDF' @highest FREQ
                {
                	 for(i=0;i<orderSize-1;i++)
					 {
					 	for(j=0;j<orderSize-i-1;j++)
						 {
							 if(order[j]>order[j+1]) //orders array of deadlines from lowest to highest
							 {
								 tmp=order[j];
								 tmp2=orderNames[j];
								 tmp3=execution1188[j];
								 order[j]=order[j+1];
								 orderNames[j]=orderNames[j+1];
								 execution1188[j]=execution1188[j+1];
								 order[j+1]=tmp;
								 orderNames[j+1]=tmp2;
								 execution1188[j+1]=tmp3;
							 }
						 }
					 }
					 for(j = 0; j<orderSize;j++) //initialize arrays to zero to be used later, 
					 {
					 	idleFlag[j] = 0;
					 	tobeInserted[j] = 0;
					 }
					 for(j = 0; j<orderSize;j++) 		 	
					{
						frequencies[j]=1188; //initializes frequencies to 1188
						if(j==0) //j=0 is a special case where runlength =/= runlength+execution
							runLength[j]=execution1188[j];
						else
						{
							runLength[j] = runLength[j-1]+execution1188[j]; //sums runlength with each execution time
							for(i = 0; i<j; i++)
								if((orderNames[j]==orderNames[i])& (runLength[j-1]<order[i])) //checks if the taskname has already been executed and needs to wait to return
									{
										idleFlag[j] = 1;
										tobeInserted[j] = order[i]; //execution times for idle time finishes
										runLength[j] = order[i]+execution1188[j]; //sum of runlength begins with task return time					
									}
						}
					}
					
					for(j = 0; j<orderSize;j++)//size of array will lengthen by the size of flagCounter to insert task times of returning processes
						if(idleFlag[j]==1)
							flagCounter++;
					for(j = orderSize+flagCounter; j>=0;j--)
					{
						if(idleFlag[j]==1)
						{
							
							for(i = (orderSize+flagCounter); i>=j;i--)
							{
								runLength[i+1] = runLength[i]; //shift the array to make room for inserting the periods of idle times
								orderNames[i+1] = orderNames[i]; //task names are also shifted down to make room for idle name
							}
							runLength[j] = tobeInserted[j]; //idle time insertion
							orderNames[j] = 0; //0 will be used to check if a task is idle at time j
						}
					}
					for(i = 4; i>=0;i--)
							{
								taskNames[i+1]=taskNames[i];//task names were retrieved starting at 0 and needs to start at 1
							}
					taskNames[0] = "\nIDLE"; 
					cout<<taskNames[orderNames[0]]<<"		1	1188	"<<runLength[0] <<"	"<<runLength[0]*array[2]/1000<<"."<<runLength[0]*array[2]%1000<<"J "; //output format. energy needs to convert to Joules
					totalEnergy = runLength[0]*array[2]; //array[2] is the location of power
					for(j = 1; j<(orderSize+flagCounter);j++)
					{
						if(orderNames[j]==0) //at j, 0 is a mark of idle time
						{
							cout<<taskNames[orderNames[j]]<<"	"<<runLength[j-1]<<"	IDLE	"<<runLength[j]-runLength[j-1] <<"	"<<runLength[j]*array[6]/1000<<"."<<runLength[j]*array[6]%1000<<"J ";
                			totalEnergy = totalEnergy + (runLength[j]*array[6]);
                			totalIdle = totalIdle + runLength[j] - runLength[j-1];
                		}
                		else if(orderNames[j]==1)
                		{
                			cout<<"\n"<<taskNames[orderNames[j]]<<"		"<<runLength[j-1]<<"	1188	"<<runLength[j]-runLength[j-1] <<"	"<<runLength[j]*array[2]/1000<<"."<<runLength[j]*array[2]%1000<<"J ";
                			totalEnergy = totalEnergy + (runLength[j]*array[2]);
                		}
                		else
                		{
                			cout<<taskNames[orderNames[j]]<<"		"<<runLength[j-1]<<"	1188	"<<runLength[j]-runLength[j-1] <<"	"<<runLength[j]*array[2]/1000<<"."<<runLength[j]*array[2]%1000<<"J ";
                			totalEnergy = totalEnergy + (runLength[j]*array[2]);
                		}
                	}
                	convertedrunLength = runLength[orderSize+flagCounter-1];
                	cout<<"\nTotal energy = "<<totalEnergy/1000<<"."<<totalEnergy%1000<<"J"<<endl;
                	cout<<"Total idle time % = "<<totalIdle/convertedrunLength<<endl;
                	cout<<"Total execution time = "<<runLength[orderSize+flagCounter-1]<<endl;
                }
                if(argv2 == "RM")
                {
                	for(i=0;i<orderSize-1;i++)
					 {
					 	for(j=0;j<orderSize-i-1;j++)
						 {
							 if(execution1188[j]>execution1188[j+1])//orders by execution time
							 {
							 	 tmp=order[j];
								 tmp2=orderNames[j];
								 tmp3=execution1188[j];
								 order[j]=order[j+1];
								 orderNames[j]=orderNames[j+1];
								 execution1188[j]=execution1188[j+1];
								 order[j+1]=tmp;
								 orderNames[j+1]=tmp2;
								 execution1188[j+1]=tmp3;
							 }
						 }
					 }
					 for(j = 0; j<orderSize;j++)
					 {
					 	idleFlag[j] = 0;
					 	tobeInserted[j] = 0;
					 }
					 for(j = 0; j<orderSize;j++)					 	
					{
						frequencies[j]=1188;
						if(j==0)
							runLength[j]=execution1188[j];
						else
						{
							runLength[j] = runLength[j-1]+execution1188[j];
							for(i = 0; i<j; i++)
								if((orderNames[j]==orderNames[i])& (runLength[j-1]<order[i]))
									{
										idleFlag[j] = 1;
										tobeInserted[j] = order[i];
										runLength[j] = order[i]+execution1188[j];										
									}
						}
					}
					
					for(j = 0; j<orderSize;j++)
						if(idleFlag[j]==1)
							flagCounter++;
					for(j = orderSize+flagCounter; j>=0;j--)
					{
						if(idleFlag[j]==1)
						{
							
							for(i = (orderSize+flagCounter); i>=j;i--)
							{
								runLength[i+1] = runLength[i];
								orderNames[i+1] = orderNames[i];
							}
							runLength[j] = tobeInserted[j];
							orderNames[j] = 0;
						}
					}
					for(i = 4; i>=0;i--)
							{
								taskNames[i+1]=taskNames[i];
							}
					taskNames[0] = "\nIDLE";
					cout<<taskNames[orderNames[0]]<<"		1	1188	"<<runLength[0] <<"	"<<runLength[0]*array[2]/1000<<"."<<runLength[0]*array[2]%1000<<"J ";
					totalEnergy = runLength[0]*array[2];
					for(j = 1; j<(orderSize+flagCounter);j++)
					{
						if(orderNames[j]==0)
						{
							cout<<taskNames[orderNames[j]]<<"	"<<runLength[j-1]<<"	IDLE	"<<runLength[j]-runLength[j-1] <<"	"<<runLength[j]*array[6]/1000<<"."<<runLength[j]*array[6]%1000<<"J ";
                			totalEnergy = totalEnergy + (runLength[j]*array[6]);
                			totalIdle = totalIdle + runLength[j] - runLength[j-1];
                		}
                		else if(orderNames[j]==1)
                		{
                			cout<<"\n"<<taskNames[orderNames[j]]<<"		"<<runLength[j-1]<<"	1188	"<<runLength[j]-runLength[j-1] <<"	"<<runLength[j]*array[2]/1000<<"."<<runLength[j]*array[2]%1000<<"J ";
                			totalEnergy = totalEnergy + (runLength[j]*array[2]);
                		}
                		else
                		{
                			cout<<taskNames[orderNames[j]]<<"		"<<runLength[j-1]<<"	1188	"<<runLength[j]-runLength[j-1] <<"	"<<runLength[j]*array[2]/1000<<"."<<runLength[j]*array[2]%1000<<"J ";
                			totalEnergy = totalEnergy + (runLength[j]*array[2]);
                		}
                	}
                	convertedrunLength = runLength[orderSize+flagCounter-1];
                	cout<<"\nTotal energy = "<<totalEnergy/1000<<"."<<totalEnergy%1000<<"J"<<endl;
                	cout<<"Total idle time % = "<<totalIdle/convertedrunLength<<endl;
                	cout<<"Total execution time = "<<runLength[orderSize+flagCounter-1]<<endl;
                }
 std::cin >> i;
  return 0;
}


//When  a  task  is  scheduled,  it  should  output  the  following: <task  name> 
// <time  started>  <frequency  task  run  at>  <how  long  it  ran  for>  <energy  consumed  in  Joules>.  If  you  are 
//scheduling an idle period, use ‘IDLE’ as the task name and frequency value.  