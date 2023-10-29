#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#define MAXSTRESS 100

void sigint_handler(int signum);
void settings();

char scramble[256];
char command[512];
int htrmax = 12;
char menuChar = '\0';
char nissHtr = '\0';
char noLimits = '\0';
char line[256];
char line2[256];
char line3[128];
char useNissHtr[4] = "";


int main() {
    printf("Welcome to HtrTrainer 1.0.\nIf ^C is not working use ^Z.\n");
    signal(SIGTSTP, sigint_handler);
    char scramble[256];
    int stressNissy = 0;
    char allCases = '\0';
    int settingsSet = 0;
    int htrFound = 0;
    int htrLength = 0;
    char scrambleFileName[] = "scramble.txt";
    char returnFileName[] = "return.txt";
    char htrFileName[] = "outputhtr.txt";

    while(1){
        menuChar = '\0';
        if(!settingsSet){
            settings();
            settingsSet = 1;
        } else {
            while(menuChar != 'Y' && menuChar != 'N' && menuChar != 'S'){
                printf("Do you want to continue? (Y or N, S for settings): ");
                scanf("%c", &menuChar);
                fflush(stdin); 
            }
            if(menuChar == 'N'){
                printf("Exiting...\n"); exit(0); 
            } else if (menuChar == 'S'){
                settings();
            } else if (menuChar == 'Y'){
            } else {
                printf("settings error\n"); exit(1);
            }
        }
        stressNissy = 0;
        htrFound = 0;
        htrLength = 0;
        strcpy(scramble, "");
        FILE *scrambleFile = fopen(scrambleFileName, "w");
        FILE *returnFile = fopen(returnFileName, "w");
        FILE *htrFile = fopen(htrFileName, "w");
        fclose(returnFile);
        fclose(scrambleFile);
        scrambleFile = fopen(scrambleFileName, "w+");
        returnFile = fopen(returnFileName, "w+");

        if (scrambleFile == NULL || returnFile == NULL) {
            perror("Error opening output files");
            return 1;
        }

        if(noLimits == 'Y'){
            snprintf(command, sizeof(command), "nissy solve htr %s \"$(nissy scramble dr | tee scramble.txt)\" > return.txt", useNissHtr);
            system(command);
            while (fgets(line3, sizeof(line3), returnFile)) {
            char *open_paren = strchr(line3, '(');
            char *close_paren = strchr(line3, ')');
            if (open_paren != NULL && close_paren != NULL) {
                    char number_str[4]; 
                    int num_length = close_paren - open_paren - 1;

                    if (num_length > 0 && num_length < sizeof(number_str)) {
                        strncpy(number_str, open_paren + 1, num_length);
                        number_str[num_length] = '\0';

                        htrLength = atoi(number_str);
                    }
                }
            }
            printf("Optimal HTR: %d", htrLength); 
            fflush(stdin);
            snprintf(command, sizeof(command), "nissy solve htr -c %s -M %d \"$(cat scramble.txt)\" > return.txt", useNissHtr, htrLength);
            system(command);
            fseek(returnFile, 0, SEEK_SET);
            fscanf(returnFile, "%d", &htrFound); fflush(returnFile);
        } else {
            printf("\nScrambles checked: "); fflush(stdout);
            while(stressNissy < MAXSTRESS && htrFound == 0){
                snprintf(command, sizeof(command), "nissy solve htr -c %s -M %d \"$(nissy scramble dr | tee scramble.txt)\" > return.txt", useNissHtr, htrmax);
                system(command);
                fflush(returnFile);
                printf("%d ", stressNissy+1); fflush(stdout);
                fseek(returnFile, 0, SEEK_SET);
                fscanf(returnFile, "%d", &htrFound); fflush(returnFile);
                stressNissy++;
            }
            if(stressNissy == MAXSTRESS) {
                printf("no scramble found, Exiting...\n"); fflush(stdout); sleep(1); exit(0);
            }
        }
    
        printf("\nSolutions found: %d\n", htrFound);

        fgets(scramble, sizeof(scramble), scrambleFile);
        scramble[strcspn(scramble, "\n")] = '\0';
        printf("\nScramble: %s\n", scramble);

        printf("Press Enter to see solution.\n");
        fflush(stdout);
        while (getchar() != '\n');
        if (noLimits == 'Y') {
            snprintf(command, sizeof(command), "nissy solve htr -p %s -M %d \"%s\" | tee -a %s\n", useNissHtr, htrLength, scramble, htrFileName);
        }
        else {
            snprintf(command, sizeof(command), "nissy solve htr -p %s -M %d \"%s\" | tee -a %s\n", useNissHtr, htrmax, scramble, htrFileName);
        }
        system(command);
        sleep(1);
        fclose(scrambleFile);
        fclose(returnFile);
    }
    return 0;
}

void sigint_handler(int signum) {
    int pid = getpid();
    int fid = fork();
    if (!fid){
        printf("Received SIGTSTP (^Z). Exiting...\n");
        sleep(1);
        kill(pid, SIGKILL);
        exit(1);
    }
}

void settings(){
        printf("--- Settings ---\n");
        htrmax = 12;
        nissHtr = '\0';
        noLimits = '\0';
        while(noLimits != 'Y' && noLimits != 'N'){
            printf("random HTR length (Y or N): ");
            scanf("%c", &noLimits);
            fflush(stdin); 
        }
        if(noLimits == 'N'){
            while(htrmax < 0 || htrmax >= 12){
                printf("max HTR: ");
                scanf("%d", &htrmax);
                fflush(stdin);  
            }
        }
        while(nissHtr != 'Y' && nissHtr != 'N'){
            printf("htr with niss (Y or N, heavy task): ");
            scanf("%c", &nissHtr);
            fflush(stdin); 
        }
        if (nissHtr == 'Y') strcpy(useNissHtr, "-N"); else strcpy(useNissHtr, "");
        printf("Settings done.\n");
}


/*
    printf("Press Enter to continue.");
    fflush(stdout); // Flush the output buffer to ensure the message is displayed immediately
    // Wait for the user to press Enter
    while (getchar() != '\n');
*/