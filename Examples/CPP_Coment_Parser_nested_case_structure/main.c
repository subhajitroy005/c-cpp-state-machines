/*
* Comment parser: Read c style comments form a code. Read ony multiple line comment not // 
* State chart: state_chart.ddd in ./
*/


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "drv_file_read.h" //  API for reading the file contents


// Enum containing the signals [File read characters are signals here ]
enum Signals {
    CHAR_SIG , STAR_SIG , SLASH_SIG , EOF_SIGNAL
};

// Enum containing the states
enum  States{
    CODE , SLASH , COMMENT , STAR , DONE , ERROR
};

/* Variables and structure */
struct application_status{
    enum States state;      // Hold the current state
    enum Signals signal;    // Hold the signal form polling events

    /* Counter tracking the comments */
    int counter;
};




/* Functions */
void transition(struct application_status* application , enum States next_state) // Transition call
{
    application->state = next_state; // Update for next state transition
}

void init(struct application_status* application)  // Initialization call
{
    application->counter = 0; //  reset the counter
    application->state = CODE; // Entry state is the code by default

    /* Initialize the file driver*/
    file_drv_init("file.c");
}

void dispatch(struct application_status* application)
{
    switch(application->state){ //  Dispatching the current state
        case CODE:
            switch(application->signal){
                case CHAR_SIG:
                    // No operation
                break;

                case STAR_SIG:
                    // No opration
                break;

                case SLASH_SIG:
                    /* If a slash signal then next state will be SLASH state */
                    transition(application, SLASH);
                break;

                case EOF_SIGNAL:
                    /*  EOF signal means file has ended and go to 
                        END state and print the counter. 
                    */
                    transition(application, DONE);
                break;
            }
        break;


        case SLASH:
            switch(application->signal){
                case CHAR_SIG:
                    /* After slash a star is needed for comment else code */
                    transition(application, CODE);
                break;

                case STAR_SIG:
                    /* Slash to star is entering in the comment */
                    transition(application, COMMENT);
                break;

                case SLASH_SIG:
                    /* After slash a star is needed for comment else code */
                    transition(application, CODE);
                break;

                case EOF_SIGNAL:
                    /*  EOF signal means file has ended and go to 
                        END state and print the counter. 
                    */
                    transition(application, DONE);
                break;
            }
        break;

        case COMMENT:
            switch(application->signal){
                case CHAR_SIG:
                    /*  Comment will end at star signal so no operation remain in comment 
                        and count the comment chars
                    */
                   application->counter++;
                break;

                case STAR_SIG:
                    /* After comment star means end of comment and go to start signal */
                    transition(application, STAR);
                break;

                case SLASH_SIG:
                    /*  Comment will end at star signal so no operation remain in comment 
                        and count the comment chars
                    */
                   application->counter++; 
                break;

                case EOF_SIGNAL:
                    /*  EOF signal means file has ended and go to 
                        END state and print the counter. 
                    */
                    transition(application, DONE);
                break;
            }
        break;

        case STAR:
            switch(application->signal){
                case CHAR_SIG:
                    /*  Expect slash but char means still in coment so go to 
                        comment and count this char 
                    */
                   application->counter++;
                   transition(application, COMMENT);
                break;

                case STAR_SIG:
                    /* Star means stil wait for slash to comeplete comment no operation
                    */
                break;

                case SLASH_SIG:
                    /* got the slash so comment done and go to code again */
                    transition(application, CODE);
                break;

                case EOF_SIGNAL:
                    /*  EOF signal means file has ended and go to 
                        END state and print the counter. 
                    */
                    transition(application, DONE);
                break;
            }
        break;

        case DONE:
            /* Final state so only print and exit sequence */
            printf("Read complete!\n");
            printf("%d   chars in comment!\n", application->counter);
            file_drv_deinit(); // close the file
            // Exit the application
            exit(EXIT_SUCCESS);
        break;
    }
}


void poll_event_signals(struct application_status* application) //  Read the file and generate event signals based on read char
{
    char temp_char = char_read_from_file();
    if(temp_char == '/'){
         application->signal = SLASH_SIG;
    } else if(temp_char == '*') {
         application->signal = STAR_SIG;
    } else if (temp_char == EOF){
        application->signal = EOF_SIGNAL;
    } else {
         application->signal = CHAR_SIG;
    }
}

/* Global variable */
struct application_status application;

/* Entry : main() */
int main()
{
    init(&application); // Initialization 

    while(1){
        poll_event_signals(&application); // Poll the event signals
        dispatch(&application);           // Dispatch the events
    }

    return EXIT_SUCCESS;
}