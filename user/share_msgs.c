#include "kernel/types.h" //Has typedefs for common types in xv6
#include "user/user.h"

#define bool char
#define true 1
#define false 0

int main(void)
{
    printf("Parent has started!\n");
    msgenroll(); //Enroll the parent (so it definitely has a buffer before child created)
    int parentPid = getpid(); //Get parent pid for child to have as a recipient
    int childPid = fork(); //Fork into a parent and child
    
    if(childPid == 0) //If child
    {
        msgenroll(); //Enroll the child
        volatile int myNumber = 16; //Number for the child to send
        volatile bool flag = true;

        msgsend((void*)&myNumber, sizeof(int), sizeof(bool), parentPid); //Send the number
        msgsend((void*)&flag, sizeof(bool), 0, parentPid); //Send the flag

        volatile bool parentSentData = false;
        while(!parentSentData) //Wait until child sent data over
            msgread((void*)&parentSentData, sizeof(bool), 0);

        volatile int parentNumber = -2;
        msgread((void*)&parentNumber, sizeof(int), sizeof(bool)); //Receive parent's number
        printf("Child received from parent: %d\n", parentNumber);

        printf("Child is terminating\n"); 
    }
    else //If parent
    {
        printf("Parent is waiting for child to send data...\n");
        volatile bool childSentData = false; //Checks if the child sent the data
        while(!childSentData) //Wait until child sent data over
            msgread((void*)&childSentData, sizeof(bool), 0);

        printf("Parent has gotten a signal to read a number!\n");

        volatile int childNumber = -1;
        msgread((void*)&childNumber, sizeof(int), sizeof(bool)); //Receive child's number
        printf("Parent received from child: %d\n", childNumber);

        childNumber += 17; //Add just to do something to change the value
        msgsend((void*)&childNumber, sizeof(int), sizeof(bool), childPid); //Send the number
        printf("Parent sent: %d\n", childNumber);

        volatile bool flag = true;
        msgsend((void*)&flag, sizeof(bool), 0, childPid); //Send the flag
    }
}