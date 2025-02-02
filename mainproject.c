#include <ncurses.h>
#include <unistd.h> // for sleep function
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>


// terminal: 

#define x 184
#define y 45
#define z 4

void printFirstPage() {
    int height, width;
    getmaxyx(stdscr, height, width);

    int start_y = height / 2;
    int start_x = (width - strlen("WELCOME")) / 2;
    int start_x_RUGOE = (width - strlen("RUGOE")) / 2;

    mvprintw(start_y, start_x, "WELCOME");
    mvprintw(start_y + 1, start_x_RUGOE, "ROGUE");
    refresh();


    int loading_win_height = 3;
    int loading_win_width = 60;
    int loading_win_start_i = (width - loading_win_width) / 2;
    int loading_win_start_j = start_y + 3;

    WINDOW *loading_win = newwin(loading_win_height, loading_win_width, loading_win_start_j, loading_win_start_i);

        wattron(loading_win, A_REVERSE);

    for(int i = 0; i < 58; i++){
        
        mvwprintw(loading_win, 1, i + 1, " ");

        usleep(80000);

        wrefresh(loading_win);
    }

            wattroff(loading_win, A_REVERSE);

    clear();
}

int usersMenu() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    int height, width;
    getmaxyx(stdscr, height, width);

    int start_y = (height - 20) / 2;
    int start_x = (width - 30) / 2;  // Adjust width to better fit the content
    int win_height = 20;
    int win_width = 30;  // Adjusted width

    char c[] = "Welcome! please select an option";

    for (int i = 0; i < strlen(c); i++) {
        mvwprintw(stdscr, 1, (width - strlen(c)) / 2 + i, "%c", c[i]);
        refresh();
        usleep(20000);
    }

    WINDOW *win = newwin(win_height, win_width, start_y, start_x);
    box(win, 0, 0);
    refresh();
    wrefresh(win);

    wattron(win, A_REVERSE);
    mvwprintw(win, 1, 10, "User Menu");  // Add title
    wattroff(win, A_REVERSE);
    char *choices[] = {"Add User", "Log in", "Guest"};

    int highlight = 0;
    int choice;

    while (1) {
        for (int i = 0; i < 3; i++) {
            if (highlight == i) {
                wattron(win, A_REVERSE);
            }
            mvwprintw(win, (i + 1) * 5, 10, "%s", choices[i]);
            wattroff(win, A_REVERSE);
        }

        choice = wgetch(win);
        //mvprintw(height - 3, 0, "Key pressed: %d", choice);  // Debug line to show key value
        refresh();

        switch (choice) {
            case 65:
                highlight = (highlight - 1 + 3) % 3;
                break;
            case 66:
                highlight = (highlight + 1) % 3;
                break;
            case 10:  // Enter key
               // mvprintw(height - 2, 0, "You selected: %s", choices[highlight]);
               // refresh();
                goto end_loop;
                break;
            default:
                break;
        }
    }

end_loop:
    clear();
    return highlight;
    
}

int is_valid_email_format(const char *email) {
    int at_position = -1;
    int dot_position = -1;
    int length = strlen(email);

    if (length < 5) {
        return 0; // Email is too short to be valid
    }

    for (int i = 0; i < length; i++) {
        if (email[i] == '@') {
            if (at_position == -1) {
                at_position = i;
            } else {
                return 0; // Multiple '@' characters
            }
        } else if (email[i] == '.') {
            if (at_position != -1) {
                dot_position = i;
            }
        } else if (!isalnum(email[i]) && email[i] != '_' && email[i] != '-' && email[i] != '+') {
            return 0; // Invalid character
        }
    }

    if (at_position == -1 || dot_position == -1 || dot_position < at_position + 2 || dot_position == length - 1) {
        return 0; // Invalid positions of '@' or '.'
    }

    return 1; // Valid email format
}

void generate_random_password(char *password, int length) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#^&*()";
    int charset_size = sizeof(charset) - 1;

    // Seed the random number generator
    srand(time(NULL));

    // Ensure the password includes at least one uppercase letter and one number
    password[0] = 'A' + (rand() % 26);  // Uppercase letter
    password[1] = '0' + (rand() % 10);  // Number

    // Fill the rest of the password with random characters from the charset
    for (int i = 2; i < length; i++) {
        password[i] = charset[rand() % charset_size];
    }

    password[length] = '\0'; // Null-terminate the password

    // Shuffle the password to ensure randomness
    for (int i = 0; i < length; i++) {
        int j = rand() % length;
        char temp = password[i];
        password[i] = password[j];
        password[j] = temp;
    }
}


int checkPassword(char *password){
    int number = 0;
    int capital = 0;
    int length = strlen(password);
    for(int i = 0; i < length; i++){
        if(password[i] >= '0' && password[i] <= '9'){
            number = 1;
        }
        if(password[i] >= 'A' && password[i] <= 'Z'){
            capital = 1;
        }
    }

    if(capital == 1 && number == 1 && length >= 7){
        return 1;
    }

    return 0;
}

void addUser(){
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);


    int height, width;
    getmaxyx(stdscr, height, width);

    int win_height = 36;
    int win_width = 40;
    int start_y = (height - win_height) / 2;
    int start_x = (width - win_width) / 2;

    char c[] = {"Please choose Username, set an e-mail and create password"};

       for (int i = 0; i < strlen(c); i++) {
        mvwprintw(stdscr, 1, i + (width - strlen(c)) / 2, "%c", c[i]);
        refresh();
        usleep(20000);
    }

    refresh();

    WINDOW * win = newwin(win_height, win_width, start_y, start_x);
    box(win, 0, 0);
    refresh();
    wrefresh(win);
    wattron(win, A_REVERSE);
    mvwprintw(win, 1, (win_width - strlen("Enter your name:")) / 2, "Enter your name:");
    wrefresh(win);
    wattroff(win, A_REVERSE);
     char name[30], fileName[256];

     for(int i = 0; i < 30; i++){
        name[i] = '\0';
     }
     
    int ch, i = 0, valid = 0;
       // Collect the name without showing the characters
    FILE *namesFile = fopen("name.txt", "r");  // Open the namesFile in read mode
    // if (namesFile == NULL) {
    //     printf("Error opening namesFile\n");
    //     return 1;
    // }

    char line[256];

while (i < 29) {
    ch = wgetch(win);

    if (ch == KEY_BACKSPACE || ch == 127) {  // Handle backspace
        if (i > 0) {
            i--;
            mvwprintw(win, 3, (win_width - 30) / 2 + i, " ");  // Clear the character
            wrefresh(win);
            name[i] = '\0';
        }
    } else if (ch != '\n') {
        echo();
        name[i++] = ch;
        name[i] = '\0';
        mvwprintw(win, 3, (win_width - 30) / 2 + i - 1, "%c", ch);  // Display character
        wrefresh(win);
        noecho();
    } else if(ch == '\n') {
        if (valid == 1) {
            break;
        }
    }
    valid = 1;

    rewind(namesFile);
int j = 0;
while (fgets(line, sizeof(line), namesFile)) {
        line[strcspn(line, "\n")] = 0;
        if(strcmp(line, name) == 0){
        valid = 0;
        mvwprintw(win, 4,(win_width - 30) / 2 , "Username not valid!");
        refresh();
        }
        j++;
    }

    if(valid){
        mvwprintw(win, 4,(win_width - 30) / 2 , "                   ");
    }
}
    fclose(namesFile);

    name[i] = '\0';
    wattron(win, A_REVERSE);
    mvwprintw(win, 7, (win_width - strlen("Enter your E-mail:")) / 2, "Enter your E-mail:");
    wattroff(win, A_REVERSE);
    wrefresh(win);
    valid = 1;
    char email[30];
    FILE *emailsFile = fopen("email.txt", "r");

    i = 0;
    while(i < 29){
        ch = wgetch(win);
            if (ch == KEY_BACKSPACE || ch == 127) {  // Handle backspace
        if (i > 0) {
            i--;
            mvwprintw(win, 9, (win_width - 30) / 2 + i, " ");  // Clear the character
            wrefresh(win);
            email[i] = '\0';
        }
    }else if (ch != '\n') {

        echo();
        email[i++] = ch;
        email[i] = '\0';
        mvwprintw(win, 9, (win_width - 30) / 2 + i - 1, "%c", ch);  // Display character
        refresh();
        wrefresh(win);
        noecho();
        
    }else if(ch == '\n'){
        if(is_valid_email_format(email)){
            break;
        }
    }
    valid = 1;
    rewind(emailsFile);

while (fgets(line, sizeof(line), emailsFile)) {
        line[strcspn(line, "\n")] = 0;
        if(strcmp(line, email) == 0){
        valid = 0;
        //  mvwprintw(win, 4,(win_width - 30) / 2 , "Email not valid");
        // refresh();
        }
    }

        if(is_valid_email_format(email) == 1 && valid == 1){
            mvwprintw(win, 10,(win_width - 30) / 2 , "                   ");
            wrefresh(win);
        } else{
            mvwprintw(win, 10,(win_width - 30) / 2 , "Email not valid!");
            wrefresh(win);
        }
    

    }

    email[i] = '\0';

   // mvprintw(1,1,"|%s| |%s|", name, email);
    fclose(emailsFile);

    wattron(win, A_REVERSE);
    mvwprintw(win, 13, (win_width - strlen("Enter your E-mail password:")) / 2, "Enter your E-mail password:");
    wattroff(win, A_REVERSE);
    refresh();
    wrefresh(win);

    char emailPassword[30];

    i = 0;

    while(i < 29){
              ch = wgetch(win);
            if (ch == KEY_BACKSPACE || ch == 127) {  // Handle backspace
        if (i > 0) {
            i--;
            mvwprintw(win, 15, (win_width - 30) / 2 + i, " ");  // Clear the character
            wrefresh(win);
            emailPassword[i] = '\0';
        }
        }else if (ch != '\n') {
         echo();
         emailPassword[i++] = ch;
         emailPassword[i] = '\0';
         mvwprintw(win, 15, (win_width - 30) / 2 + i - 1, "%c", ch);  // Display character
         refresh();
         wrefresh(win);
         noecho();
        
    }else if(ch == '\n'){
            break;
    }

    }

    emailPassword[i] = '\0';

    wattron(win, A_REVERSE);
    mvwprintw(win, 19, (win_width - strlen("Enter your password")) / 2, "Enter your password:");
    mvwprintw(win, 21, (win_width - strlen("Set password")) / 2, "Set password");
    wattroff(win, A_REVERSE);
    mvwprintw(win, 22, (win_width - strlen("Random password")) / 2, "Random password");
    refresh();
    wrefresh(win);

    int highlight = 0;

    char *choice[] = {"Set password", "Random password"};

    while(1){
        ch = wgetch(win);

        if(ch == 65){
            highlight = (highlight - 1 + 2) % 2;
        } else if(ch == 66){
            highlight = (highlight + 1 + 2) % 2;
        } else if(ch == '\n'){
            break;
        }

        if(highlight == 0){
            wattron(win, A_REVERSE);
            mvwprintw(win, 21, (win_width - strlen("Set password")) / 2, "Set password");
            wattroff(win,A_REVERSE);
            mvwprintw(win, 22, (win_width - strlen("Random password")) / 2, "Random password");
            refresh();
            wrefresh(win);
        } else{
               wattron(win, A_REVERSE);
            mvwprintw(win, 22, (win_width - strlen("Random password")) / 2, "Random password");
            wattroff(win,A_REVERSE);
            mvwprintw(win, 21, (win_width - strlen("Set password")) / 2, "Set password");
            refresh();
            wrefresh(win);
        }
    }
    char password[30];
    if(highlight == 0){
        mvwprintw(win, 21, (win_width - strlen("Set password")) / 2, "                    ");
        mvwprintw(win, 22, (win_width - strlen("Random password")) / 2, "                      ");
        refresh();
        wrefresh(win);
        i = 0;
        while(i < 29){
                ch = wgetch(win);
            if (ch == KEY_BACKSPACE || ch == 127) {  // Handle backspace
        if (i > 0) {
            i--;
            mvwprintw(win, 21, (win_width - 30) / 2 + i, " ");  // Clear the character
            wrefresh(win);
            password[i] = '\0';
        }
        }else if (ch != '\n') {

         echo();
         password[i++] = ch;
         password[i] = '\0';
         mvwprintw(win, 21, (win_width - 30) / 2 + i - 1, "%c", ch);  // Display character
         refresh();
         wrefresh(win);
         noecho();
        
        } else if(ch == '\n'){
            if(checkPassword(password))
            break;
        }

        if(checkPassword(password) == 0){
            mvwprintw(win, 23, (win_width - 30) / 2, "Password is not valid!");
            refresh;
            wrefresh(win);
        } else{
            mvwprintw(win, 23, (win_width - 30) / 2, "                      ");
            refresh;
            wrefresh(win);
        }

    }

} else if(highlight == 1){
    mvwprintw(win, 21, (win_width - strlen("Set password")) / 2, "                    ");
    mvwprintw(win, 22, (win_width - strlen("Random password")) / 2, "                      ");
    refresh();
    wrefresh(win);
    generate_random_password(password, 20);
    mvwprintw(win, 21, (win_width - 30) / 2, "%s", password);  // Display character
    refresh();
    wrefresh(win);

}

FILE *Name, *Email, *Password, *EmailPassword;

  Name = fopen("name.txt", "a");
  Email = fopen("email.txt", "a");
  Password = fopen("password.txt", "a");
  EmailPassword = fopen("emailPassword.txt", "a");

  fprintf(Name, "%s\n", name);
  fprintf(Email, "%s\n", email);
  fprintf(Password, "%s\n", password);
  fprintf(EmailPassword, "%s\n", emailPassword);

  fclose(Name);
  fclose(Email);
  fclose(Password);
  fclose(EmailPassword);


    mvwprintw(win, 25, (win_width - 30) / 2, "-----");
    refresh();
    wrefresh(win);
    usleep(20000);
    mvwprintw(win, 26, (win_width - 30) / 2, "Username: %s", name);
    refresh();
    wrefresh(win);
    usleep(20000);
    mvwprintw(win, 27, (win_width - 30) / 2, "Email: %s", email);
    refresh();
    wrefresh(win);
    usleep(200000);
    mvwprintw(win, 28, (win_width - 30) / 2, "Emailpassword: %s", emailPassword);
    refresh();
    wrefresh(win);
    usleep(20000);
    mvwprintw(win, 29, (win_width - 30) / 2, "Password: %s", password);
    refresh();
    wrefresh(win);
    usleep(20000);
    mvwprintw(win, 30, (win_width - 30) / 2, "-----");
    refresh();
    wrefresh(win);
    mvwprintw(win, 32, (win_width - 30) / 2, "Press any key to continue!");
    refresh();
    wrefresh(win);

    getch();
    clear();
}

void logIn(char *user){
    initscr();
    keypad(stdscr, TRUE);

    char message[] = {"Please enter Your Username and password"};

    int height, width;
    getmaxyx(stdscr, height, width);

    wattron(stdscr, A_REVERSE);

    for(int i = 0; i < strlen(message); i++){
        mvprintw(1, (width - strlen(message)) / 2 + i, "%c", message[i]);
        refresh();
        usleep(20000);
    }

    wattroff(stdscr, A_REVERSE);

    int win_height = 20;
    int win_width = 40;
    int start_y = (height - win_height) / 2;
    int start_x = (width - win_width) / 2;

    WINDOW *win = newwin(win_height, win_width, start_y, start_x);
    box(win, 0, 0);
    refresh();
    wrefresh(win);

    
    char message_name[] = {"Please enter your Username!"};
    wattron(win, A_REVERSE);
    for(int i = 0; i < strlen(message_name); i++){
        mvwprintw(win, 1, (win_width - strlen(message_name)) / 2 + i,"%c", message_name[i]);
        wrefresh(win);
        usleep(20000);
    }
    wattroff(win, A_REVERSE);
    //FILE *Name = fopen("name.txt", "r");
    int ch, i = 0, valid = 0;
       // Collect the name without showing the characters
    FILE *namesFile = fopen("name.txt", "r");  // Open the namesFile in read mode
    // if (namesFile == NULL) {
    //     printf("Error opening namesFile\n");
    //     return 1;
    // }

    char line[256], name[30];
    int j;

while (i < 29) {
    ch = wgetch(win);

    if (ch == KEY_BACKSPACE || ch == 127) {  // Handle backspace
        if (i > 0) {
            i--;
            mvwprintw(win, 3, (win_width - 30) / 2 + i, " ");  // Clear the character
            wrefresh(win);
            name[i] = '\0';
        }
    } else if (ch != '\n') {
        echo();
        name[i++] = ch;
        name[i] = '\0';
        mvwprintw(win, 3, (win_width - 30) / 2 + i - 1, "%c", ch);  // Display character
        wrefresh(win);
        noecho();
    } else if(ch == '\n') {
        if (valid == 1) {
          //  mvprintw(1,1,"%d", j);
            break;
        }
    }
    valid = 0;

    rewind(namesFile);
    j = 0;

while (fgets(line, sizeof(line), namesFile)) {
        line[strcspn(line, "\n")] = 0;
        if(strcmp(line, name) == 0){
        valid = 1;
        mvwprintw(win, 4,(win_width - 30) / 2 , "                   ");
        refresh();
        wrefresh(win);
        break;
        }
        j++;
    }

    if(valid == 0){
        mvwprintw(win, 4,(win_width - 30) / 2 , "Username not valid!");
        refresh();
        wrefresh(win);
    }
}

    FILE *passwordsFile = fopen("password.txt", "r");

    char userPassword[40];

    while(j >= 0){

        fgets(userPassword, sizeof(userPassword), passwordsFile);
        j--;
    }

    userPassword[strlen(userPassword) - 1] = '\0';

    //mvprintw(1, 1, "%s", userPassword);

    char password_message[] = {"Please enter your password!"};
    wattron(win, A_REVERSE);
    for(int i = 0; i < strlen(password_message); i++){
        mvwprintw(win, 6, (win_width - strlen(password_message)) / 2 + i, "%C", password_message[i]);
        refresh();
        wrefresh(win);
        usleep(20000);
    }

    wattroff(win, A_REVERSE);

    char password[30];

    i = 0;

    while (i < 29) {
    ch = wgetch(win);

    if (ch == KEY_BACKSPACE || ch == 127) {  // Handle backspace
        if (i > 0) {
            i--;
            mvwprintw(win, 8, (win_width - 30) / 2 + i, " ");  // Clear the character
            wrefresh(win);
            password[i] = '\0';
        }
    } else if (ch != '\n') {
        echo();
        password[i++] = ch;
        password[i] = '\0';
        mvwprintw(win, 8, (win_width - 30) / 2 + i - 1, "%c", ch);  // Display character
        wrefresh(win);
        noecho();
    } else if(ch == '\n') {
        if (strcmp(userPassword, password) == 0) {
          //  mvprintw(1,1,"%d", j);
            break;
        }
    }


    if(strcmp(password, userPassword) == 0){
        mvwprintw(win, 10,(win_width - 30) / 2 , "                        ");
        refresh();
        wrefresh(win);
    }

      if(strcmp(password, userPassword) != 0){
        mvwprintw(win, 10,(win_width - 30) / 2 , "password is not correct!");
        refresh();
        wrefresh(win);
    }

}

mvwprintw(win, 12, (win_width - 8) / 2, "--------");
wrefresh(win);

for(int i = 0; i < strlen(name); i++){
    mvwprintw(win, 13, (win_width - strlen(name)) / 2 + i, "%c", name[i]);
    wrefresh(win);
    
    usleep(20000);
}

for(int i = 0; i < strlen(password); i++){
    mvwprintw(win, 15, (win_width - strlen(password)) / 2 + i, "%c", password[i]);
    wrefresh(win);
    
    usleep(20000);
}

    strcpy(user, name);

    getch();
    clear();
    
    
}

void guest(char *user){

    char message1[] = {"Please enter your name :)"};
    
    int height, width;
    getmaxyx(stdscr, height, width);

    wattron(stdscr, A_REVERSE);

    for(int i = 0; i < strlen(message1); i++){
        mvprintw(1, (width - strlen(message1)) / 2 + i, "%c", message1[i]);
        refresh();
        usleep(20000);
    }

    wattroff(stdscr, A_REVERSE);

    int win_height = 15;
    int win_width = 30;
    int start_y = (height - win_height) / 2;
    int start_x = (width - win_width) / 2;

    WINDOW *win = newwin(win_height, win_width, start_y, start_x);
    box(win, 0, 0);
    refresh();
    wrefresh(win);\

    char name[30];

    name[0] = 'G';
    name[1] = '_';

    int i = 2;

    int ch;

    while (i < 27) {
    ch = wgetch(win);

    if (ch == KEY_BACKSPACE || ch == 127) {  // Handle backspace
        if (i > 2) {
            i--;
            mvwprintw(win, 2, (win_width - 30) / 2 + i, " ");  // Clear the character
            wrefresh(win);
            name[i] = '\0';
        }
    } else if (ch != '\n') {
        echo();
        name[i++] = ch;
        name[i] = '\0';
        mvwprintw(win, 2, (win_width - 30) / 2 + i - 1, "%c", ch);  // Display character
        wrefresh(win);
        noecho();
    } else if(ch == '\n') {
            break;
        
    }

    }

    char c[6] = {"------"};

    for(int i = 0; i < 6; i++){
        mvwprintw(win, 4, (win_width - 6) / 2 + i, "%c", c[i]);
        refresh();
        wrefresh(win);
        usleep(20000);
    }

    for(int i = 0; i < strlen(name); i++){
        mvwprintw(win, 6, (win_width - strlen(name)) / 2 + i, "%c", name[i]);
        refresh();
        wrefresh(win);
        usleep(20000);
    }


    strcpy(user, name);
    
    getch();
    clear();

}

void settingMenu(FILE **savedGame, char name[], int isGuest, int *newGame, int *newDifficulty, int *newCharacter, int *newColor, int *music){

    cbreak();
    keypad(stdscr, TRUE);

    int length = strlen(name);
    char txt[10000] = {"/home/ahmadreza/ROGUE/saving/"};
    strcat(txt, name);

    if(isGuest == 1){

        *savedGame = NULL;

    } else{

    strcat(txt, "/map.txt");

    *savedGame = fopen(txt, "r");

    }

    WINDOW *win1;

    int height, width;
    getmaxyx(stdscr, height, width);

    int win1_height = 7;
    int win1_width = 20;
    int win1_start_y = 2;
    int win1_start_x = 20;

    win1 = newwin(win1_height, win1_width, win1_start_y, win1_start_x);
    box(win1, 0, 0);
    refresh();
    wrefresh(win1);

    WINDOW *win2;

    int win2_height = 20;
    int win2_width = 40;
    int win2_start_y = 2;
    int win2_start_x = 60;

    win2 = newwin(win2_height, win2_width, win2_start_y, win2_start_x);
    box(win2, 0, 0);
    refresh();
    wrefresh(win2);

    WINDOW *win4;
    int win4_height = 30;
    int win4_width = 35;
    int win4_start_y = 10;
    int win4_start_x = 20;

    win4 = newwin(win4_height, win4_width, win4_start_y, win4_start_x);
    box(win4, 0, 0);
    refresh();
    wrefresh(win4);

    WINDOW *win3;

        int win3_height = 40;
        int win3_width = 50;
        int win3_start_y = 2;
        int win3_start_x = 120;

        win3 = newwin(win3_height, win3_width, win3_start_y, win3_start_x);
        box(win3, 0, 0);
        refresh();
        wrefresh(win3);

    if(*savedGame == NULL){

    wattron(win1, COLOR_PAIR(0));

    mvwprintw(win1, 2, 5, "load game");

    wattroff(win1, COLOR_PAIR(0));

    } else{

    wattron(win1, A_REVERSE);

    mvwprintw(win1, 2, 5, "load game");

    wattroff(win1, A_REVERSE);

    }
    mvwprintw(win1, 4, 5, "new game");

    refresh();
    wrefresh(win1);



    // int ch;
    // int highlight = 0;
if(*savedGame == NULL){


    wattron(win1, COLOR_PAIR(1));

    mvwprintw(win1, 2, 5, "load game");

    wattroff(win1, COLOR_PAIR(1));

    } else{

    wattron(win1, A_REVERSE);

    mvwprintw(win1, 2, 5, "load game");

    wattroff(win1, A_REVERSE);

    }
    mvwprintw(win1, 4, 5, "new game");

    refresh();
    wrefresh(win1);



    int ch;
    int highlight = 0;

    while(1){
        ch = wgetch(win1);

        if(ch == 65){

            highlight = (highlight - 1 + 2) % 2;

        } else if(ch == 66){

            highlight = (highlight + 1) % 2;

        } else if(ch == '\n'){

            if(highlight == 1){
                *newGame = 1;
                break;
            } else{
                if(*savedGame != NULL){
                    *newGame = 0;
                    break;
                }
            }
        }


        if(highlight == 0){

            if(*savedGame == NULL){

    wattron(win1, COLOR_PAIR(1));

    mvwprintw(win1, 2, 5, "load game");

    wattroff(win1, COLOR_PAIR(1));

    } else{

    wattron(win1, A_REVERSE);

    mvwprintw(win1, 2, 5, "load game");

    wattroff(win1, A_REVERSE);

    }
    mvwprintw(win1, 4, 5, "new game");

    refresh();
    wrefresh(win1);

        } else{

            mvwprintw(win1, 2, 5, "load game");

            wattron(win1, A_REVERSE);

            mvwprintw(win1, 4, 5, "new game");

            wattroff(win1, A_REVERSE);

            refresh();
            wrefresh(win1);
        }

       

    }

    if(*newGame == 1){


        char message1[] = {"Please choose difficulty:"};

        wattron(win2, A_REVERSE);

        for(int i = 0; i < strlen(message1); i++){

            mvwprintw(win2, 1, (win2_width - strlen(message1)) / 2 + i, "%C", message1[i]);
            refresh();
            wrefresh(win2);

            usleep(20000);
        }

        wattroff(win2, A_REVERSE);

        char message2[] = {"level1  level2  level3"};

        int difficulty = 1;

        for(int i = 0; i < strlen(message2); i++){

            if((difficulty == 0 && i >= 0 && i <= 5) ||
                (difficulty == 1 && i >= 8 && i <= 13) ||
                 (difficulty == 2 && i >= 16 && i <= 21)){
                    wattron(win2, A_REVERSE);
                 }

            mvwprintw(win2, 3, (win2_width - strlen(message1)) / 2 + i, "%C", message2[i]);

              if((difficulty == 0 && i >= 0 && i <= 5) ||
                (difficulty == 1 && i >= 8 && i <= 13) ||
                 (difficulty == 2 && i >= 16 && i <= 21)){
                    wattroff(win2, A_REVERSE);
                 }

            refresh();
            wrefresh(win2);

        }

        while(1){
            ch = wgetch(win2);

            if(ch == 68){

                //mvprintw(1, 1, "lll");

                difficulty = (difficulty - 1 + 3) % 3;

            } else if(ch == 67){

                //mvprintw(1, 1, "rrr");
                difficulty = (difficulty + 1) % 3;

            } else if(ch == '\n'){

                break;

            }

            for(int i = 0; i < strlen(message2); i++){

            if((difficulty == 0 && i >= 0 && i <= 5) ||
                (difficulty == 1 && i >= 8 && i <= 13) ||
                 (difficulty == 2 && i >= 16 && i <= 21)){
                    wattron(win2, A_REVERSE);
                 }

            mvwprintw(win2, 3, (win2_width - strlen(message1)) / 2 + i, "%C", message2[i]);

              if((difficulty == 0 && i >= 0 && i <= 5) ||
                (difficulty == 1 && i >= 8 && i <= 13) ||
                 (difficulty == 2 && i >= 16 && i <= 21)){
                    wattroff(win2, A_REVERSE);
                 }

            refresh();
            wrefresh(win2);

        }

        }

        char message3[] = {"Please choose your character:"};
        
        wattron(win2, A_REVERSE);

         for(int i = 0; i < strlen(message3); i++){

            mvwprintw(win2, 5, (win2_width - strlen(message3)) / 2 + i, "%C", message3[i]);
            refresh();
            wrefresh(win2);

            usleep(20000);
        }

        wattroff(win2, A_REVERSE);

        // @  %  &  *

        char message4[] = {"  @  $  &  *  "};

        int character = 0;

        for(int i = 0; i < strlen(message4); i++){

            if((character == 0 && i == 2) || 
                 (character == 1 && i == 5) ||
                  (character == 2 && i == 8) ||
                   (character == 3 && i == 11)){
                    wattron(win2, A_REVERSE);
                   }

                   mvwprintw(win2, 7, (win2_width - strlen(message4)) / 2 + i, "%c", message4[i]);

            if((character == 0 && i == 2) || 
                 (character == 1 && i == 5) ||
                  (character == 2 && i == 8) ||
                   (character == 3 && i == 11)){
                    wattroff(win2, A_REVERSE);
                   }

                    refresh();
                    wrefresh(win2);


        }

        while(1){

            ch = wgetch(win2);

          if(ch == 68){

                //mvprintw(1, 1, "lll");

                character = (character - 1 + 4) % 4;

            } else if(ch == 67){

                //mvprintw(1, 1, "rrr");
                character = (character + 1) % 4;

            } else if(ch == '\n'){

                break;

            }

              for(int i = 0; i < strlen(message4); i++){

            if((character == 0 && i == 2) || 
                 (character == 1 && i == 5) ||
                  (character == 2 && i == 8) ||
                   (character == 3 && i == 11)){
                    wattron(win2, A_REVERSE);
                   }

                   mvwprintw(win2, 7, (win2_width - strlen(message4)) / 2 + i, "%c", message4[i]);

            if((character == 0 && i == 2) || 
                 (character == 1 && i == 5) ||
                  (character == 2 && i == 8) ||
                   (character == 3 && i == 11)){
                    wattroff(win2, A_REVERSE);
                   }

                    refresh();
                    wrefresh(win2);


        }



                
        }

        char message5[] = {"Please select your character's color:"};

        wattron(win2, A_REVERSE);

         for(int i = 0; i < strlen(message5); i++){

            mvwprintw(win2, 9, (win2_width - strlen(message5)) / 2 + i, "%C", message5[i]);
            refresh();
            wrefresh(win2);

            usleep(20000);
        }

        wattroff(win2, A_REVERSE);

        char message6[] = {" RED YELLOW GREEN WHITE CYAN"};

        int color = 1;

        for(int i = 0; i < strlen(message6); i++){

            if(i >= 1 && i <= 3 && color == 1){
                wattron(win2, COLOR_PAIR(1));
            }

            if(i >= 5 && i <= 10 && color == 2){
                wattron(win2, COLOR_PAIR(2));
            }

            if(i >= 12 && i <= 16 && color == 3){
                wattron(win2, COLOR_PAIR(3));
            }

            if(i >= 18 && i <= 22 && color == 4){
                wattron(win2, COLOR_PAIR(4));
            }

            if(i >= 24 && i <= 27 && color == 5){
                wattron(win2, COLOR_PAIR(5));
            }

            mvwprintw(win2, 11, (win2_width - strlen(message6)) / 2 + i, "%c", message6[i]);

            if(i >= 1 && i <= 3 && color == 1){
                wattroff(win2, COLOR_PAIR(1));
            }

            if(i >= 5 && i <= 10 && color == 2){
                wattroff(win2, COLOR_PAIR(2));
            }

            if(i >= 12 && i <= 16 && color == 3){
                wattroff(win2, COLOR_PAIR(3));
            }

            if(i >= 18 && i <= 22 && color == 4){
                wattroff(win2, COLOR_PAIR(4));
            }

            if(i >= 24 && i <= 27 && color == 5){
                wattroff(win2, COLOR_PAIR(5));
            }

            refresh();
            wrefresh(win2);


        }

        while(1){

            ch = wgetch(win2);

            if(ch == 68){

                //mvprintw(1, 1, "lll");

                color = (color - 1 + 5) % 5;

            } else if(ch == 67){

                //mvprintw(1, 1, "rrr");
                color = (color + 1) % 5;

            } else if(ch == '\n'){

                break;

            }

            if(color == 0)
                color = 5;

                    for(int i = 0; i < strlen(message6); i++){

            if(i >= 1 && i <= 3 && color == 1){
                wattron(win2, COLOR_PAIR(1));
            }

            if(i >= 5 && i <= 10 && color == 2){
                wattron(win2, COLOR_PAIR(2));
            }

            if(i >= 12 && i <= 16 && color == 3){
                wattron(win2, COLOR_PAIR(3));
            }

            if(i >= 18 && i <= 22 && color == 4){
                wattron(win2, COLOR_PAIR(4));
            }

            if(i >= 24 && i <= 27 && color == 5){
                wattron(win2, COLOR_PAIR(5));
            }

            mvwprintw(win2, 11, (win2_width - strlen(message6)) / 2 + i, "%c", message6[i]);

            if(i >= 1 && i <= 3 && color == 1){
                wattroff(win2, COLOR_PAIR(1));
            }

            if(i >= 5 && i <= 10 && color == 2){
                wattroff(win2, COLOR_PAIR(2));
            }

            if(i >= 12 && i <= 16 && color == 3){
                wattroff(win2, COLOR_PAIR(3));
            }

            if(i >= 18 && i <= 22 && color == 4){
                wattroff(win2, COLOR_PAIR(4));
            }

            if(i >= 24 && i <= 27 && color == 5){
                wattroff(win2, COLOR_PAIR(5));
            }

           // mvprintw(1,1,"%d", color);
            refresh();
            wrefresh(win2);


                    }
            

        }

        char message7[] = {"------"};

        for(int i = 0; i < strlen(message7); i++){

            mvwprintw(win2, 13, (win2_width - strlen(message7)) / 2 + i, "%c", message7[i]);

        }

        refresh();
        wrefresh(win2);

          char message8[] = {"difficulty: "};

        for(int i = 0; i < strlen(message8); i++){

            mvwprintw(win2, 15, (win2_width - strlen(message8)) / 2 + i, "%c", message8[i]);

            refresh();
            wrefresh(win2);

            usleep(20000);

        }

        mvwprintw(win2, 15, (win2_width + strlen(message8)) / 2, "%d", difficulty + 1);
        usleep(20000);

        refresh();
        wrefresh(win2);

          char message9[] = {"character: "};

        for(int i = 0; i < strlen(message9); i++){

            mvwprintw(win2, 16, (win2_width - strlen(message9)) / 2 + i, "%c", message9[i]);

            refresh();
            wrefresh(win2);

            usleep(20000);

        }

        mvwprintw(win2, 16, (win2_width + strlen(message9)) / 2, "%d", character + 1);
        usleep(20000);

        refresh();
        wrefresh(win2);

        char message10[] = {"color: "};

        for(int i = 0; i < strlen(message10); i++){

            mvwprintw(win2, 17, (win2_width - strlen(message10)) / 2 + i, "%c", message10[i]);

            refresh();
            wrefresh(win2);

            usleep(20000);

        }

        mvwprintw(win2, 17, (win2_width + strlen(message10)) / 2, "%d", color);
        usleep(20000);

        refresh();
        wrefresh(win2);

        *newCharacter = character + 1;

        *newColor = color;

        *newDifficulty = difficulty + 1;

    }

    // WINDOW *win4;
    // int win4_height = 30;
    // int win4_width = 35;
    // int win4_start_y = 10;
    // int win4_start_x = 20;

    // win4 = newwin(win4_height, win4_width, win4_start_y, win4_start_x);
    // box(win4, 0, 0);
    // refresh();
    // wrefresh(win4);

    char message_0[100] = {"Assassin_s_Creed_Unity"};
    char message_1[100] = {"Elise_s_Theme_Assassin_s_Creed"};
    char message_2[100] = {"Mrs. Sadie Adler, Widow"};
    char message_3[100] = {"Outlaws From The West"};
    char message_4[100] = {"Revelations_Requiescat_in_Pace"};
    char message_5[100] = {" Revenge Is A Dish Best Eaten"};
    char message_6[100] = {"Rod_Stewart_Young_Turks"};
    char message_7[100] = {"The_Evil_Within_Soundtrack"};
    char message_8[100] = {"Don't want music:("};
    char musicMessage[100] = {"please choose music :)"};

    wattron(win4, A_REVERSE);

    for(int i = 0; i < strlen(musicMessage); i++){

        mvwprintw(win4, 1, (win4_width - strlen(message_0) ) / 2 + i, "%c", musicMessage[i]);
        wrefresh(win4);
        usleep(25000);
    }

    mvwprintw(win4, 4, (win4_width - strlen(message_0) ) / 2, "%s", message_0);

    wattroff(win4, A_REVERSE);

    mvwprintw(win4, 7, (win4_width - strlen(message_1) ) / 2, "%s", message_1);
    mvwprintw(win4, 10, (win4_width - strlen(message_2) ) / 2, "%s", message_2);
    mvwprintw(win4, 13, (win4_width - strlen(message_3) ) / 2, "%s", message_3);
    mvwprintw(win4, 16, (win4_width - strlen(message_4) ) / 2, "%s", message_4);
    mvwprintw(win4, 19, (win4_width - strlen(message_5) ) / 2, "%s", message_5);
    mvwprintw(win4, 22, (win4_width - strlen(message_6) ) / 2, "%s", message_6);
    mvwprintw(win4, 25, (win4_width - strlen(message_7) ) / 2, "%s", message_7);
    mvwprintw(win4, 28, (win4_width - strlen(message_8) ) / 2, "%s", message_8);

    refresh();
    wrefresh(win4);

    while(1){

        ch = wgetch(win4);

        if(ch == 65){
                            //mvprintw(1, 1, "xxx");
                *music = (*music - 1 + 9) % 9;
                //refresh();

            } else if(ch == 66){

                //mvprintw(1, 1, "rrr");
                *music = (*music + 1) % 9;
                //refresh();

            } else if(ch == '\n'){

                break;

            }
    mvwprintw(win4, 4, (win4_width - strlen(message_0) ) / 2, "%s", message_0);
    mvwprintw(win4, 7, (win4_width - strlen(message_1) ) / 2, "%s", message_1);
    mvwprintw(win4, 10, (win4_width - strlen(message_2) ) / 2, "%s", message_2);
    mvwprintw(win4, 13, (win4_width - strlen(message_3) ) / 2, "%s", message_3);
    mvwprintw(win4, 16, (win4_width - strlen(message_4) ) / 2, "%s", message_4);
    mvwprintw(win4, 19, (win4_width - strlen(message_5) ) / 2, "%s", message_5);
    mvwprintw(win4, 22, (win4_width - strlen(message_6) ) / 2, "%s", message_6);
    mvwprintw(win4, 25, (win4_width - strlen(message_7) ) / 2, "%s", message_7);
    mvwprintw(win4, 28, (win4_width - strlen(message_8) ) / 2, "%s", message_8);

    wrefresh(win4);

        if(*music == 0){

            wattron(win4, A_REVERSE);
            mvwprintw(win4, 4, (win4_width - strlen(message_0) ) / 2, "%s", message_0);
            wattroff(win4, A_REVERSE);

        }

        if(*music == 1){

            wattron(win4, A_REVERSE);
            mvwprintw(win4, 7, (win4_width - strlen(message_1) ) / 2, "%s", message_1);
            wattroff(win4, A_REVERSE);

        }

        if(*music == 2){

            wattron(win4, A_REVERSE);
            mvwprintw(win4, 10, (win4_width - strlen(message_2) ) / 2, "%s", message_2);
            wattroff(win4, A_REVERSE);

        }

        if(*music == 3){

            wattron(win4, A_REVERSE);
            mvwprintw(win4, 13, (win4_width - strlen(message_3) ) / 2, "%s", message_3);
            wattroff(win4, A_REVERSE);

        }

        if(*music == 4){

            wattron(win4, A_REVERSE);
            mvwprintw(win4, 16, (win4_width - strlen(message_4) ) / 2, "%s", message_4);
            wattroff(win4, A_REVERSE);

        }

        if(*music == 5){

            wattron(win4, A_REVERSE);
            mvwprintw(win4, 19, (win4_width - strlen(message_5) ) / 2, "%s", message_5);
            wattroff(win4, A_REVERSE);

        }

        if(*music == 6){

            wattron(win4, A_REVERSE);
            mvwprintw(win4, 22, (win4_width - strlen(message_6) ) / 2, "%s", message_6);
            wattroff(win4, A_REVERSE);

        }

        if(*music == 7){

            wattron(win4, A_REVERSE);
            mvwprintw(win4, 25, (win4_width - strlen(message_7) ) / 2, "%s", message_7);
            wattroff(win4, A_REVERSE);

        }

        if(*music == 8){

            wattron(win4, A_REVERSE);
            mvwprintw(win4, 28, (win4_width - strlen(message_8) ) / 2, "%s", message_8);
            wattroff(win4, A_REVERSE);

        }

        wrefresh(win4);
        refresh();
    }


    getch();


}

void initScreen(char map[z][y][x], int difficulty, int *person_i, int *person_j, int *person_i_1,int *person_j_1,int *person_i_2,int *person_j_2,int *person_i_3,int *person_j_3){


    for(int k = 0; k < z; k++){

        for(int j = 0; j < y; j++){
//, person_i_1, person_j_1, person_i_2, person_j_2, person_i_3, person_j_3
            for(int i = 0; i < x; i++){
                map[k][j][i] = ' ';
            }
        }
    }

    srand(time(0)); 

    for(int k = 0; k < z; k++) {

    //int ranges[4] = {5, 15, 5, 15}; 

    int start1_i;
    int start1_j;
    int end1_i;
    int end1_j;

    start1_i=(rand() % 15);
    start1_j=(rand() % 5);
    end1_i=(rand() % 15) + 40;
    end1_j=(rand() % 5) + 13;

    
    for(int i = start1_i; i <= end1_i; i++){

        for(int j = start1_j; j <= end1_j; j++){

            if( i == start1_i || i == end1_i){

                map[k][j][i] = '|';
                continue;

            }

            if(j == start1_j || j == end1_j){

                map[k][j][i] = '-';
                continue;

            }


            map[k][j][i] = '.';

        }

            
    }

    int dar_1_1_i;
    int dar_1_1_j;

    dar_1_1_i = end1_i;

    dar_1_1_j = (rand() % (end1_j - start1_j - 1)) + start1_j + 1;

    map[k][dar_1_1_j][dar_1_1_i] = '+';

    int dar_1_2_i;
    int dar_1_2_j;

    dar_1_2_j = end1_j;

    dar_1_2_i = (rand() % (end1_i - start1_i - 1)) + start1_i + 1;

    map[k][dar_1_2_j][dar_1_2_i] = '+';

    if(k == 0){

        map[k][dar_1_1_j][dar_1_2_i] = '=';

        *person_i = dar_1_2_i;
        *person_j = dar_1_1_j;
    }

    for(int i = 0; i < 12 + (difficulty - 1) * 3; i++){

        while(1){

            int iii = (rand() % (end1_i - start1_i)) + start1_i;
            int jjj = (rand() % (end1_j - start1_j)) + start1_j;

            if(map[k][jjj][iii] == '.'){

                if(i <= 2){

                    map[k][jjj][iii] = 'g';//gold

                } else if (i < 5){

                    map[k][jjj][iii] = 'k'; //column
                }  else if (i < 6){

                    map[k][jjj][iii] = 'b';//black golg!
                } else{

                    map[k][jjj][iii] = 'T';
                }

                break;
            }



        }

        

    }



    //########################################################################################## otagh 1
    //##########################################################################################

    //usleep(50000);

    //srand(time(0)); 

   // int ranges[4] = {5, 15, 5, 15}; 

    int start2_i;
    int start2_j;
    int end2_i;
    int end2_j;

    start2_i=(rand() % 15) + 65;
    start2_j=(rand() % 5);
    end2_i=(rand() % 15) + 105;
    end2_j=(rand() % 5) + 13;
    
    for(int i = start2_i; i <= end2_i; i++){

        for(int j = start2_j; j <= end2_j; j++){

            if( i == start2_i || i == end2_i){

                map[k][j][i] = '|';
                continue;

            }

            if(j == start2_j || j == end2_j){

                map[k][j][i] = '-';
                continue;

            }


            map[k][j][i] = '.';

        }

            
    }

    int dar_2_3_i;
    int dar_2_3_j;

    dar_2_3_i = start2_i;

    dar_2_3_j = (rand() % (end2_j - start2_j - 1)) + start2_j + 1;

    map[k][dar_2_3_j][dar_2_3_i] = '+';




    int dar_2_2_i;
    int dar_2_2_j;

    dar_2_2_i = (rand() % (end2_i - start2_i - 1)) + start2_i + 1;

    dar_2_2_j = end2_j;


    map[k][dar_2_2_j][dar_2_2_i] = '+';




    int dar_2_1_i;
    int dar_2_1_j;

    dar_2_1_i = end2_i;

    dar_2_1_j = (rand() % (end2_j - start2_j - 1)) + start2_j + 1;

    map[k][dar_2_1_j][dar_2_1_i] = '+';

    if(k == 1){

        map[k][dar_2_1_j][dar_2_2_i] = 'S';
    }

    if(k == 2){

    *person_i_2 = dar_2_2_i;
    *person_j_2 = dar_2_1_j;

    }

    for(int i = 0; i < 12 +(difficulty - 1) * 3; i++){

        while(1){

            int iii = (rand() % (end2_i - start2_i)) + start2_i;
            int jjj = (rand() % (end2_j - start2_j)) + start2_j;

            if(map[k][jjj][iii] == '.'){

                if(i <= 2){

                    map[k][jjj][iii] = 'g';//gold

                } else if (i < 5){

                    map[k][jjj][iii] = 'k'; //column
                } else if(i < 6){

                    map[k][jjj][iii] = 'b';//black gold
                } else{

                    map[k][jjj][iii] = 'T';
                }

                break;
            }



        }

    }

    //############################################################################################# otagh 2
    //#############################################################################################


    int start3_i;
    int start3_j;
    int end3_i;
    int end3_j;

    start3_i=(rand() % 15) + 129;
    start3_j=(rand() % 5);
    end3_i=(rand() % 15) + 169;
    end3_j=(rand() % 5) + 13;
    
    for(int i = start3_i; i <= end3_i; i++){

        for(int j = start3_j; j <= end3_j; j++){


            if( i == start3_i || i == end3_i){

                map[k][j][i] = '|';
                continue;

            }

            if(j == start3_j || j == end3_j){

                map[k][j][i] = '-';
                continue;

            }

            map[k][j][i] = '.';

        }

            
    }

    int dar_3_3_i;
    int dar_3_3_j;

    dar_3_3_i = start3_i;

    dar_3_3_j = (rand() % (end3_j - start3_j - 1)) + start3_j + 1;

    map[k][dar_3_3_j][dar_3_3_i] = '+';




    int dar_3_2_i;
    int dar_3_2_j;

    dar_3_2_i = (rand() % (end3_i - start3_i - 1)) + start3_i + 1;

    dar_3_2_j = end3_j;


    map[k][dar_3_2_j][dar_3_2_i] = '+';



    for(int i = 0; i < 12 +(difficulty - 1) * 3; i++){

        while(1){

            int iii = (rand() % (end2_i - start3_i)) + start3_i;
            int jjj = (rand() % (end3_j - start3_j)) + start3_j;

            if(map[k][jjj][iii] == '.'){

                if(i <= 2){

                    map[k][jjj][iii] = 'g';//gold

                } else if(i < 5){

                    map[k][jjj][iii] = 'k'; //column
                } else if(i < 6){

                    map[k][jjj][iii] = 'b';//black gold
                } else{

                    map[k][jjj][iii] = 'T';
                }

                break;
            }



        }

    }

    //#####################################################################################otagh 3
    //#####################################################################################

    int start4_i;
    int start4_j;
    int end4_i;
    int end4_j;

    start4_i=(rand() % 15);
    start4_j=(rand() % 5) + 27;
    end4_i=(rand() % 15) + 40;
    end4_j=(rand() % 5) + 38;
    
    for(int i = start4_i; i <= end4_i; i++){

        for(int j = start4_j; j <= end4_j; j++){

            if( i == start4_i || i == end4_i){

                map[k][j][i] = '|';
                continue;

            }

            if(j == start4_j || j == end4_j){

                map[k][j][i] = '-';
                continue;

            }


            map[k][j][i] = '.';

        }

            
    }

    int dar_4_4_i;
    int dar_4_4_j;

    dar_4_4_i = (rand() % (end4_i - start4_i - 1)) + start4_i + 1;

    dar_4_4_j = start4_j;


    map[k][dar_4_4_j][dar_4_4_i] = '+';




    int dar_4_1_i;
    int dar_4_1_j;

    dar_4_1_i = end4_i;

    dar_4_1_j = (rand() % (end4_j - start4_j - 1)) + start4_j + 1;


    map[k][dar_4_1_j][dar_4_1_i] = '+';

    if(k == 2){

        map[k][dar_4_1_j][dar_4_4_i] = 'S';
    }

    if(k == 3){

    *person_i_3 = dar_4_4_i;
    *person_j_3 = dar_4_1_j;

    }


    for(int i = 0; i < 12 +(difficulty - 1) * 3; i++){

        while(1){

            int iii = (rand() % (end4_i - start4_i)) + start4_i;
            int jjj = (rand() % (end4_j - start4_j)) + start4_j;

            if(map[k][jjj][iii] == '.'){

                if(i <= 2){

                    map[k][jjj][iii] = 'g'; // gold

                } else if(i < 5){

                    map[k][jjj][iii] = 'k'; // column
                } else if (i < 6){

                    map[k][jjj][iii] = 'b';//black gold
                } else{

                    map[k][jjj][iii] = 'T';//trap
                }

                break;
            }



        }

    }

    //#################################################################################### otagh4
    //####################################################################################

    int start5_i;
    int start5_j;
    int end5_i;
    int end5_j;

    start5_i=(rand() % 15) + 65;
    start5_j=(rand() % 5) + 27;
    end5_i=(rand() % 15) + 105;
    end5_j=(rand() % 5) + 38;
    
    for(int i = start5_i; i <= end5_i; i++){

        for(int j = start5_j; j <= end5_j; j++){


            if( i == start5_i || i == end5_i){

                map[k][j][i] = '|';
                continue;

            }

            if(j == start5_j || j == end5_j){

                map[k][j][i] = '-';
                continue;

            }

            map[k][j][i] = '.';

        }

            
    }

    int dar_5_1_i;
    int dar_5_1_j;

    dar_5_1_i = end5_i;

    dar_5_1_j = (rand() % (end5_j - start5_j - 1)) + start5_j + 1;


    map[k][dar_5_1_j][dar_5_1_i] = '+';





    int dar_5_3_i;
    int dar_5_3_j;

    dar_5_3_i = start5_i;

    dar_5_3_j = (rand() % (end5_j - start5_j - 1)) + start5_j + 1;


    map[k][dar_5_3_j][dar_5_3_i] = '+';





    int dar_5_4_i;
    int dar_5_4_j;

    dar_5_4_j = start5_j;

    dar_5_4_i = (rand() % (end5_i - start5_i - 1)) + start5_i + 1;


    map[k][dar_5_4_j][dar_5_4_i] = '+';


    for(int i = 0; i < 12 +(difficulty - 1) * 3; i++){

        while(1){

            int iii = (rand() % (end5_i - start5_i)) + start5_i;
            int jjj = (rand() % (end5_j - start5_j)) + start5_j;

            if(map[k][jjj][iii] == '.'){

                if(i <= 2){

                    map[k][jjj][iii] = 'g';//gold

                } else if (i < 5){

                    map[k][jjj][iii] = 'k'; //column
                } else if(i < 6){

                    map[k][jjj][iii] = 'b';//black gold
                } else{

                    map[k][jjj][iii] = 'T';//trap
                }

                break;
            }



        }

    }

    //###############################################################################otagh5
    //###############################################################################

    int start6_i;
    int start6_j;
    int end6_i;
    int end6_j;

    start6_i=(rand() % 15) + 129;
    start6_j=(rand() % 5) + 27;
    end6_i=(rand() % 15) + 169;
    end6_j=(rand() % 5) + 38;
    
    for(int i = start6_i; i <= end6_i; i++){

        for(int j = start6_j; j <= end6_j; j++){

            if( i == start6_i || i == end6_i){

                map[k][j][i] = '|';
                continue;

            }
            
            if(j == start6_j || j == end6_j){

                map[k][j][i] = '-';
                continue;

            }


            map[k][j][i] = '.';

        }

            
    }

    int dar_6_3_i;
    int dar_6_3_j;

    dar_6_3_i = start6_i;

    dar_6_3_j = (rand() % (end6_j - start6_j - 1)) + start6_j + 1;


    map[k][dar_6_3_j][dar_6_3_i] = '+';





    int dar_6_4_i;
    int dar_6_4_j;

    dar_6_4_j = start6_j;

    dar_6_4_i = (rand() % (end6_i - start6_i - 1)) + start6_i + 1;


    map[k][dar_6_4_j][dar_6_4_i] = '+';

    if(k == 0){

        map[k][dar_6_3_j][dar_6_4_i] = 'S';
    }

    if(k == 1){

    *person_i_1 = dar_6_4_i;
    *person_j_1 = dar_6_3_j;

    }


    for(int i = 0; i < 12 +(difficulty - 1) * 3; i++){

        while(1){

            int iii = (rand() % (end6_i - start6_i)) + start6_i;
            int jjj = (rand() % (end6_j - start6_j)) + start6_j;

            if(map[k][jjj][iii] == '.'){

                if(i <= 2){

                    map[k][jjj][iii] = 'g';// gold

                } else if (i < 5){

                    map[k][jjj][iii] = 'k'; // column
                } else if(i < 6){

                    map[k][jjj][iii] = 'b';//black gold
                } else{

                    map[k][jjj][iii] = 'T';//trap
                }

                break;
            }



        }

    }

    //############################################################################ otagh6
    //############################################################################

    int ii;

    ii = dar_1_2_i;
    for(int j = dar_1_2_j + 1; j < dar_4_4_j; j++){

        if(j - dar_1_2_j < (dar_4_4_j - dar_1_2_j) / 2){
            map[k][j][ii] = '#';
            continue;
        } 

        if(j - dar_1_2_j > (dar_4_4_j - dar_1_2_j) / 2){
            map[k][j][ii] = '#';
            continue;
        } 

        if(dar_1_2_i <= dar_4_4_i){

        while(ii <= dar_4_4_i){

            map[k][j][ii] = '#';
            ii++;
        }

        ii--;

        } else{

            while(ii >= dar_4_4_i){

            map[k][j][ii] = '#';
            ii--;
        }

        ii++;
        }

    }


    ii = dar_2_2_i;
    for(int j = dar_2_2_j + 1; j < dar_5_4_j; j++){

        if(j - dar_2_2_j < (dar_5_4_j - dar_2_2_j) / 2){
            map[k][j][ii] = '#';
            continue;
        } 

        if(j - dar_2_2_j > (dar_5_4_j - dar_2_2_j) / 2){
            map[k][j][ii] = '#';
            continue;
        } 

        if(dar_2_2_i <= dar_5_4_i){

        while(ii <= dar_5_4_i){

            map[k][j][ii] = '#';
            ii++;
        }

        ii--;

        } else{

            while(ii >= dar_5_4_i){

            map[k][j][ii] = '#';
            ii--;
        }

        ii++;
        }

    }



    ii = dar_3_2_i;
    for(int j = dar_3_2_j + 1; j < dar_6_4_j; j++){

        if(j - dar_3_2_j < (dar_6_4_j - dar_3_2_j) / 2){
            map[k][j][ii] = '#';
            continue;
        } 

        if(j - dar_3_2_j > (dar_6_4_j - dar_3_2_j) / 2){
            map[k][j][ii] = '#';
            continue;
        } 

        if(dar_3_2_i <= dar_6_4_i){

        while(ii <= dar_6_4_i){

            map[k][j][ii] = '#';
            ii++;
        }

        ii--;

        } else{

            while(ii >= dar_6_4_i){

            map[k][j][ii] = '#';
            ii--;
        }

        ii++;
        }

    }

     int jj = dar_1_1_j;
    for(int i = dar_1_1_i + 1; i < dar_2_3_i; i++){

        if(i - dar_1_1_i < (dar_2_3_i - dar_1_1_i) / 2){
            map[k][jj][i] = '#';
            continue;
        } 

        if(i - dar_1_1_i  > (dar_2_3_i - dar_1_1_i) / 2){
            map[k][jj][i] = '#';
            continue;
        }
         

        if(dar_1_1_j <= dar_2_3_j){

        while(jj <= dar_2_3_j){

            map[k][jj][i] = '#';
            jj++;
        }

        jj--;

        } else{

            while(jj >= dar_2_3_j){

            map[k][jj][i] = '#';
            jj--;
        }

        jj++;
        }

    }


    jj = dar_2_1_j;
    for(int i = dar_2_1_i + 1; i < dar_3_3_i; i++){

        if(i - dar_2_1_i < (dar_3_3_i - dar_2_1_i) / 2){
            map[k][jj][i] = '#';
            continue;
        } 

        if(i - dar_2_1_i  > (dar_3_3_i - dar_2_1_i) / 2){
            map[k][jj][i] = '#';
            continue;
        }
         

        if(dar_2_1_j <= dar_3_3_j){

        while(jj <= dar_3_3_j){

            map[k][jj][i] = '#';
            jj++;
        }

        jj--;

        } else{

            while(jj >= dar_3_3_j){

            map[k][jj][i] = '#';
            jj--;
        }

        jj++;
        }

    }



    jj = dar_4_1_j;
    for(int i = dar_4_1_i + 1; i < dar_5_3_i; i++){

        if(i - dar_4_1_i < (dar_5_3_i - dar_4_1_i) / 2){
            map[k][jj][i] = '#';
            continue;
        } 

        if(i - dar_4_1_i  > (dar_5_3_i - dar_4_1_i) / 2){
            map[k][jj][i] = '#';
            continue;
        }
         

        if(dar_4_1_j <= dar_5_3_j){

        while(jj <= dar_5_3_j){

            map[k][jj][i] = '#';
            jj++;
        }

        jj--;

        } else{

            while(jj >= dar_5_3_j){

            map[k][jj][i] = '#';
            jj--;
        }

        jj++;
        }

    }


    jj = dar_5_1_j;
    for(int i = dar_5_1_i + 1; i < dar_6_3_i; i++){

        if(i - dar_5_1_i < (dar_6_3_i - dar_5_1_i) / 2){
            map[k][jj][i] = '#';
            continue;
        } 

        if(i - dar_5_1_i  > (dar_6_3_i - dar_5_1_i) / 2){
            map[k][jj][i] = '#';
            continue;
        }
         

        if(dar_5_1_j <= dar_6_3_j){

        while(jj <= dar_6_3_j){

            map[k][jj][i] = '#';
            jj++;
        }

        jj--;

        } else{

            while(jj >= dar_6_3_j){

            map[k][jj][i] = '#';
            jj--;
        }

        jj++;
        }

    }

    // for(int j = 0; j < y; j++){

    //     for(int i = 0; i < x; i++){

    //         mvprintw(j, i, "%c", map[k][j][i]);
    //     }

        
    // }


    // getch();


    }

    


}
//save_matrix_to_file(map, name,difficulty, color, character, *gold, person_i, person_j, person_i_1, person_j_1, person_i_2, person_j_2, person_i_3, person_j_3);

void save_matrix_to_file(char matrix[z][y][x], char *name,int difficulty, int color, int charact, int gold, char c, int blackGold, int health, int music, int person_i,int person_j,int person_i_1,int person_j_1,int person_i_2,int person_j_2,int person_i_3,int person_j_3) {
    clear();
    // mvprintw(1,1,"saving...");
    refresh();
    
    char folderName[10000] = {"/home/ahmadreza/ROGUE/saving/"};  // Replace with your actual path
    strcat(folderName, name);


    
    struct stat st;

    if (stat(folderName, &st) == 0 && S_ISDIR(st.st_mode)) {
    char fileName[10000];
    char dataFileName[10000];
    strcpy(dataFileName, folderName);
    strcat(dataFileName, "/data.txt");
    strcpy(fileName, folderName);
    strcat(fileName, "/map.txt");
   // snprintf(fileName, sizeof(fileName), "%s/map.txt", folderName);
    FILE *file = fopen(fileName, "w");
    FILE *dataFile = fopen(dataFileName, "w");
    if (file == NULL || dataFile == NULL) {
        perror("fopen() failed");
        //return 1;
    }

    for (int k = 0; k < z; k++) {
        for (int j = 0; j < y; j++) {
            for (int i = 0; i < x; i++) {
               if (matrix[k][j][i] == ' ') {
                   fprintf(file, ",");
               } else {
                    fprintf(file, "%c", matrix[k][j][i]);
               }
            if(i == x - 3 || i == x - 1)
            fprintf(file, "\n");
            }
        }
    }
    fprintf(dataFile, "%d\n", difficulty);
    fprintf(dataFile, "%d\n", color);
    fprintf(dataFile, "%d\n", charact);
    fprintf(dataFile, "%d\n", gold);
    fprintf(dataFile, "%c\n", c);
    fprintf(dataFile, "%d\n", blackGold);
    fprintf(dataFile, "%d\n", health);
    fprintf(dataFile, "%d\n", music);
    fprintf(dataFile, "%d\n", person_i);
    fprintf(dataFile, "%d\n", person_j);
    fprintf(dataFile, "%d\n", person_i_1);
    fprintf(dataFile, "%d\n", person_j_1);
    fprintf(dataFile, "%d\n", person_i_2);
    fprintf(dataFile, "%d\n", person_j_2);
    fprintf(dataFile, "%d\n", person_i_3);
    fprintf(dataFile, "%d\n", person_j_3);


    fclose(dataFile);
    fclose(file);

    } else{

    if (mkdir(folderName, 0777) == -1) {
        perror("Error creating directory");
        //return EXIT_FAILURE;
    }

    char fileName[10000];
    char dataFileName[10000];

    strcpy(dataFileName, folderName);
    strcat(dataFileName, "/data.txt");
    strcpy(fileName, folderName);
    strcat(fileName, "/map.txt");
    //snprintf(fileName, sizeof(fileName), "%s/map.txt", folderName);
    FILE *file = fopen(fileName, "w");
    FILE *dataFile = fopen(dataFileName, "w");
    if (file == NULL || dataFile == NULL) {
        perror("Error creating file");
        //return EXIT_FAILURE;
    }


    
    for (int k = 0; k < z; k++) {
        for (int j = 0; j < y; j++) {
            for (int i = 0; i < x; i++) {
               if (matrix[k][j][i] == ' ') {
                   fprintf(file, ",");
               } else {
                    fprintf(file, "%c", matrix[k][j][i]);
               }
            if(i == x - 3 || i == x - 1)
            fprintf(file, "\n");
            }
        }
        //fprintf(file, "\n\n\n\n\n");
    }

    fprintf(dataFile, "%d\n", difficulty);
    fprintf(dataFile, "%d\n", color);
    fprintf(dataFile, "%d\n", charact);
    fprintf(dataFile, "%d\n", gold);
    fprintf(dataFile, "%c\n", c);
    fprintf(dataFile, "%d\n", blackGold);
    fprintf(dataFile, "%d\n", health);
    fprintf(dataFile, "%d\n", music);
    fprintf(dataFile, "%d\n", person_i);
    fprintf(dataFile, "%d\n", person_j);
    fprintf(dataFile, "%d\n", person_i_1);
    fprintf(dataFile, "%d\n", person_j_1);
    fprintf(dataFile, "%d\n", person_i_2);
    fprintf(dataFile, "%d\n", person_j_2);
    fprintf(dataFile, "%d\n", person_i_3);
    fprintf(dataFile, "%d\n", person_j_3);

    fclose(dataFile);
    fclose(file);
    }

    int height, width;
    getmaxyx(stdscr, height, width);

    int start_y = height / 2;

    char cc[] = {"saving..."};


    int loading_win_height = 3;
    int loading_win_width = 60;
    int loading_win_start_i = (width - loading_win_width) / 2;
    int loading_win_start_j = start_y + 3;
    mvprintw(loading_win_start_j - 2, (width - strlen(cc)) / 2, "%s", cc);
    refresh();

    WINDOW *loading_win = newwin(loading_win_height, loading_win_width, loading_win_start_j, loading_win_start_i);

        wattron(loading_win, A_REVERSE);

    for(int i = 0; i < 58; i++){
        
        mvwprintw(loading_win, 1, i + 1, " ");

        usleep(80000);

        wrefresh(loading_win);
    }

            wattroff(loading_win, A_REVERSE);

    clear();

    

}

int loading_game(char *name, int *difficulty, int *charact, int *color, int *gold, int *blackGold, int *health, int*music, char map[z][y][x],char *c, int *floor,int *person_i,int *person_j,int *person_i_1,int *person_j_1,int *person_i_2,int *person_j_2,int *person_i_3,int *person_j_3){

    clear();
    refresh();
    char loadingPath[10000] = {"/home/ahmadreza/ROGUE/saving/"};
    strcat(loadingPath, name);

    char mapLoading[10000];
    strcpy(mapLoading, loadingPath);
    strcat(mapLoading, "/map.txt");

    char dataLoading[10000];
    strcpy(dataLoading, loadingPath);
    strcat(dataLoading, "/data.txt");

    FILE *mapFile = fopen(mapLoading, "r");
    FILE *dataFile = fopen(dataLoading, "r");

    fscanf(dataFile, "%d", difficulty);
    fscanf(dataFile, "%d", color);
    fscanf(dataFile, "%d", charact);
    fscanf(dataFile, "%d", gold);
    fscanf(dataFile, "%c", c);
    fscanf(dataFile, "%c", c);
    fscanf(dataFile, "%d", blackGold);
    fscanf(dataFile, "%d", health);
    fscanf(dataFile, "%d", music);
    fscanf(dataFile, "%d", person_i);
    fscanf(dataFile, "%d", person_j);
    fscanf(dataFile, "%d", person_i_1);
    fscanf(dataFile, "%d", person_j_1);
    fscanf(dataFile, "%d", person_i_2);
    fscanf(dataFile, "%d", person_j_2);
    fscanf(dataFile, "%d", person_i_3);
    fscanf(dataFile, "%d", person_j_3);

    fclose(dataFile);

    char h;
    for(int k = 0; k < z; k++){

        for(int j = 0; j < y; j++){

            for(int i = 0; i < x - 2; i++){

                fscanf(mapFile, "%c", &map[k][j][i]);
                if(map[k][j][i] == ',')
                    map[k][j][i] = ' ';
                if(map[k][j][i] == '@'|| map[k][j][i] == '$' || map[k][j][i] == '&' || map[k][j][i] == '*'){

                    *floor = k;
                }
                //printf("%d, %d, %c\n",j, i, map[k][j][i]);
            }
            fscanf(mapFile, "%c", &map[k][j][182]);
            fscanf(mapFile, "%c", &map[k][j][182]);
                if(map[k][j][182] == ',')
                    map[k][j][182] = ' ';
                if(map[k][j][182] == '@'|| map[k][j][182] == '$' || map[k][j][182] == '&' || map[k][j][182] == '*'){

                    *floor = k;
                }
            fscanf(mapFile, "%c", &map[k][j][183]);
                if(map[k][j][183] == ',')
                    map[k][j][183] = ' ';

                if(map[k][j][183] == '@'|| map[k][j][183] == '$' || map[k][j][183] == '&' || map[k][j][183] == '*'){

                    *floor = k;
                }
            fscanf(mapFile, "%c", &h);

        }
    }

    int height, width;
    getmaxyx(stdscr, height, width);

    int start_y = height / 2;

    char cc[] = {"loading..."};


    int loading_win_height = 3;
    int loading_win_width = 60;
    int loading_win_start_i = (width - loading_win_width) / 2;
    int loading_win_start_j = start_y + 3;
    mvprintw(loading_win_start_j - 2, (width - strlen(cc)) / 2, "%s", cc);
    refresh();

    WINDOW *loading_win = newwin(loading_win_height, loading_win_width, loading_win_start_j, loading_win_start_i);

        wattron(loading_win, A_REVERSE);

    for(int i = 0; i < 58; i++){
        
        mvwprintw(loading_win, 1, i + 1, " ");

        usleep(80000);

        wrefresh(loading_win);
    }

            wattroff(loading_win, A_REVERSE);

    clear();


}



int game(int is_guest, int newGame, int difficulty, int music, int charact, int color, char *name, int *gold, int *blackGold, int *health){

    srand(time(0)); 

    initscr();
    cbreak();
    keypad(stdscr, TRUE);

    char map[z][y][x];

    (*gold) = 0;
    (*health) = 100;
    int floor;

    int person_i, person_j, person_i_1, person_j_1, person_i_2, person_j_2, person_i_3, person_j_3;
    char c;
    if(is_guest == 1 || newGame == 1){

    initScreen(map, difficulty, &person_i, &person_j, &person_i_1, &person_j_1, &person_i_2, &person_j_2, &person_i_3, &person_j_3);

    c = '.';
    } else {

        newGame = 0;
        loading_game(name, &difficulty, &charact, &color, gold, blackGold, health, &music, map, &c, &floor, &person_i, &person_j, &person_i_1, &person_j_1, &person_i_2, &person_j_2, &person_i_3, &person_j_3);

    }

    // Initialize SDL
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        //printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        //printf("SDL_mixer could not initialize! Mix_Error: %s\n", Mix_GetError());
        return 1;
    }
    char message_0[100] = {"Assassin_s_Creed_Unity.mp3"};
    char message_1[100] = {"Elise_s_Theme_Assassin_s_Creed.mp3"};
    char message_2[100] = {"Mrs. Sadie Adler, Widow.mp3"};
    char message_3[100] = {"Outlaws From The West.mp3"};
    char message_4[100] = {"Revelations_Requiescat_in_Pace.mp3"};
    char message_5[100] = {" Revenge Is A Dish Best Eaten.mp3"};
    char message_6[100] = {"Rod_Stewart_Young_Turks.mp3"};
    char message_7[100] = {"The_Evil_Within_Soundtrack.mp3"};

    char musicc[1000] = {"/home/ahmadreza/ROGUE/musics/"};

    Mix_Music *musiccc = NULL;

    switch (music) {
        case 0:
            strcat(musicc, message_0);
            musiccc = Mix_LoadMUS(musicc);
            break;
        case 1:
            strcat(musicc, message_1);
            musiccc = Mix_LoadMUS(musicc);
            break;
        case 2:
            strcat(musicc, message_2);
            musiccc = Mix_LoadMUS(musicc);
            break;
        case 3:
            strcat(musicc, message_3);
            musiccc = Mix_LoadMUS(musicc);
            break;
        case 4:
            strcat(musicc, message_4);
            musiccc = Mix_LoadMUS(musicc);
            break;
        case 5:
            strcat(musicc, message_5);
            musiccc = Mix_LoadMUS(musicc);
            break;
        case 6:
            strcat(musicc, message_6);
            musiccc = Mix_LoadMUS(musicc);
            break;
        case 7:
            strcat(musicc, message_7);
            musiccc = Mix_LoadMUS(musicc);
            break;
        default:
            //printw("Invalid music selection\n");
            break;
    }

    Mix_PlayMusic(musiccc, -1);

    char character;

    switch(charact){

        case 1:
            character = '@';
            break;
        case 2:
            character = '$';
            break;
        case 3:
            character = '&';
            break;
        case 4:
            character = '*';
            break;
        default:
            character = '?';
    }
        if(floor == 0 && newGame == 0){
            goto floor0;
        } 

        if(floor == 1 && newGame == 0){
            goto floor1;
        }

        if(floor == 2 && newGame == 0){
            goto floor2;
        }

        if(floor == 3 && newGame == 0){
            goto floor3;
        }

    floor0:

    map[0][person_j][person_i] = character;

        for(int j = 0; j < y; j++){
            for(int i = 0; i < x; i++){

                if(j == person_j && i == person_i){

                    attron(COLOR_PAIR(color));
                }

                if(map[0][j][i] == 'g'){

                    attron(COLOR_PAIR(2));

                }
                if(map[0][j][i] == '-' || map[0][j][i] == '|'){

                    attron(COLOR_PAIR(6));

                }

                if(map[0][j][i] == '+' || map[0][j][i] == '#'){

                    attron(COLOR_PAIR(7));

                }

                if(map[0][j][i] != 'T'){
                mvprintw(j, i,"%c",  map[0][j][i]);
                } else{

                    mvprintw(j, i,".");
                }

                if(map[0][j][i] == '+' || map[0][j][i] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                if(map[0][j][i] == '-' || map[0][j][i] == '|'){

                    attroff(COLOR_PAIR(6));

                }

                if(j == person_j && i == person_i){

                    attroff(COLOR_PAIR(color));
                }

                if(map[0][j][i] == 'g'){

                    attroff(COLOR_PAIR(2));
                    
                }



            }
        }

    mvprintw(y - 1, 0, "Name: %s", name);
    mvprintw(y - 1, strlen(name) + 7, "Difficulty: level%d", difficulty);
    mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  "), "Character :");
                
    attron(COLOR_PAIR(color));

    mvprintw(y - 1,  strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 1, "%c", character);

    attroff(COLOR_PAIR(color));

    attron(COLOR_PAIR(2));

    mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 3, "gold");

    mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 8, ": %d", *gold);

    attroff(COLOR_PAIR(2));

    mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13, "black gold: %d", *blackGold);

    attron(COLOR_PAIR(5));

    mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health: %d", *health);

    attroff(COLOR_PAIR(5));


    refresh();

    char move;

    while(1){

        move = getch();

        if((move == 'q' && map[0][person_j - 1][person_i - 1] == 'S') ||
                (move == 'w' && map[0][person_j - 1][person_i] == 'S') ||
                    (move == 'e' && map[0][person_j - 1][person_i + 1] == 'S') ||
                        (move == 'd' && map[0][person_j][person_i + 1] == 'S') ||
                            (move == 'c' && map[0][person_j + 1][person_i + 1] == 'S') ||
                                (move == 'x' && map[0][person_j + 1][person_i] == 'S') ||
                                    (move == 'z' && map[0][person_j + 1][person_i - 1] == 'S') ||
                                        (move == 'a' && map[0][person_j][person_i - 1] == 'S')

                ){

                    map[0][person_j][person_i] = c;

                    break;

                }

        if(move == 'w'){

            if(map[0][person_j - 1][person_i] == '.' || map[0][person_j - 1][person_i] == '#' || map[0][person_j - 1][person_i] == '+' || map[0][person_j - 1][person_i] == 't'){


                map[0][person_j][person_i] = c;

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j, person_i, "%c", c);

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c =  map[0][person_j - 1][person_i];

                map[0][person_j - 1][person_i] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j - 1, person_i, "%c", character);
                attroff(COLOR_PAIR(color));

                person_j--;

                refresh();

                continue;
            } 

            if(map[0][person_j - 1][person_i] == 'g'){

                map[0][person_j][person_i] = c;

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j, person_i, "%c", c);

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c = '.';

                map[0][person_j - 1][person_i] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j - 1, person_i, "%c", character);
                attroff(COLOR_PAIR(color));

                person_j--;

                (*gold)++;
                attron(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 3, "gold");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 8, ": %d", *gold);

                attroff(COLOR_PAIR(2));


                continue;



            }

            if(map[0][person_j - 1][person_i] == 'b'){

                map[0][person_j][person_i] = c;

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j, person_i, "%c", c);

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c = '.';

                map[0][person_j - 1][person_i] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j - 1, person_i, "%c", character);
                attroff(COLOR_PAIR(color));

                person_j--;

                (*blackGold)++;

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13, "black gold: %d", *blackGold);

                continue;



            }

            if(map[0][person_j - 1][person_i] == 'T'){

                map[0][person_j][person_i] = c;

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j, person_i, "%c", c);

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c = 't';

                map[0][person_j - 1][person_i] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j - 1, person_i, "%c", character);
                attroff(COLOR_PAIR(color));

                person_j--;

                (*health)--;

                attron(COLOR_PAIR(5));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health:    ");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health: %d", *health);

                attroff(COLOR_PAIR(5));

                continue;



            }
        }


        if(move == 'x'){

            if(map[0][person_j + 1][person_i] == '.' || map[0][person_j + 1][person_i] == '#' || map[0][person_j + 1][person_i] == '+' || map[0][person_j + 1][person_i] == 't'){

                map[0][person_j][person_i] = c;

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j, person_i, "%c", c);

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c =  map[0][person_j + 1][person_i];

                map[0][person_j + 1][person_i] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j + 1, person_i, "%c", character);
                attroff(COLOR_PAIR(color));

                person_j++;

                refresh();

                continue;
            }

            if(map[0][person_j + 1][person_i] == 'g'){

                map[0][person_j][person_i] = c;

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j, person_i, "%c", c);

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c = '.';

                map[0][person_j + 1][person_i] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j + 1, person_i, "%c", character);
                attroff(COLOR_PAIR(color));

                person_j++;

                (*gold)++;

             attron(COLOR_PAIR(2));

             mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 3, "gold");

             mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 8, ": %d", *gold);

             attroff(COLOR_PAIR(2));

                continue;



            }

            if(map[0][person_j + 1][person_i] == 'b'){

                map[0][person_j][person_i] = c;

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j, person_i, "%c", c);

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c = '.';

                map[0][person_j + 1][person_i] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j + 1, person_i, "%c", character);
                attroff(COLOR_PAIR(color));

                person_j++;

                (*blackGold)++;

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13, "black gold: %d", *blackGold);

                continue;



            }

            if(map[0][person_j + 1][person_i] == 'T'){

                map[0][person_j][person_i] = c;

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j, person_i, "%c", c);

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c = 't';

                map[0][person_j + 1][person_i] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j + 1, person_i, "%c", character);
                attroff(COLOR_PAIR(color));

                person_j++;

                (*health)--;

                attron(COLOR_PAIR(5));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health:    ");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health: %d", *health);

                attroff(COLOR_PAIR(5));

                continue;



            }
        }

        if(move == 'a'){

            if(map[0][person_j][person_i - 1] == '.' || map[0][person_j][person_i - 1] == '#' || map[0][person_j][person_i - 1] == '+'  || map[0][person_j][person_i - 1] == 't'){

                map[0][person_j][person_i] = c;

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j, person_i, "%c", c);

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c =  map[0][person_j][person_i - 1];

                map[0][person_j][person_i - 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j, person_i - 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i--;

                refresh();

                continue;
            }

            if(map[0][person_j][person_i - 1] == 'g'){

                map[0][person_j][person_i] = c;

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j, person_i, "%c", c);

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c = '.';

                map[0][person_j][person_i - 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j, person_i - 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i--;

                (*gold)++;

                attron(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 3, "gold");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 8, ": %d", *gold);

                attroff(COLOR_PAIR(2));

                continue;



            }

            if(map[0][person_j][person_i - 1] == 'b'){

                map[0][person_j][person_i] = c;

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j, person_i, "%c", c);

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c = '.';

                map[0][person_j][person_i - 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j, person_i - 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i--;

                (*blackGold)++;

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13, "black gold: %d", *blackGold);

                continue;



            }

            if(map[0][person_j][person_i - 1] == 'T'){

                map[0][person_j][person_i] = c;

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j, person_i, "%c", c);

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c = 't';

                map[0][person_j][person_i - 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j, person_i - 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i--;

                (*health)--;

                attron(COLOR_PAIR(5));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health:    ");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health: %d", *health);

                attroff(COLOR_PAIR(5));

                continue;



            }
        }

        if(move == 'd'){

            if(map[0][person_j][person_i + 1] == '.' || map[0][person_j][person_i + 1] == '#' || map[0][person_j][person_i + 1] == '+' || map[0][person_j][person_i + 1] == 't'){

                map[0][person_j][person_i] = c;

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j, person_i, "%c", c);

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c =  map[0][person_j][person_i + 1];

                map[0][person_j][person_i + 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j, person_i + 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i++;

                refresh();

                continue;
            }

            if(map[0][person_j][person_i + 1] == 'g'){

                map[0][person_j][person_i] = c;

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j, person_i, "%c", c);

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c = '.';

                map[0][person_j][person_i + 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j, person_i + 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i++;

                (*gold)++;

                attron(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 3, "gold");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 8, ": %d", *gold);

                attroff(COLOR_PAIR(2));

                continue;



            }

            if(map[0][person_j][person_i + 1] == 'b'){

                map[0][person_j][person_i] = c;

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j, person_i, "%c", c);

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c = '.';

                map[0][person_j][person_i + 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j, person_i + 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i++;

                (*blackGold)++;

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13, "black gold: %d", *blackGold);

                continue;



            }

            if(map[0][person_j][person_i + 1] == 'T'){

                map[0][person_j][person_i] = c;

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j, person_i, "%c", c);

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c = 't';

                map[0][person_j][person_i + 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j, person_i + 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i++;

                (*health)--;

                attron(COLOR_PAIR(5));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health:    ");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health: %d", *health);

                attroff(COLOR_PAIR(5));

                continue;



            }
        }

        if(move == 'e'){

            if(map[0][person_j - 1][person_i + 1] == '.' || map[0][person_j - 1][person_i + 1] == '#' || map[0][person_j - 1][person_i + 1] == '+' || map[0][person_j - 1][person_i + 1] == 't'){

                map[0][person_j][person_i] = c;

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j, person_i, "%c", c);

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c =  map[0][person_j - 1][person_i + 1];

                map[0][person_j - 1][person_i + 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j - 1, person_i + 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i++;
                person_j--;

                refresh();

                continue;
            }

            if(map[0][person_j - 1][person_i + 1] == 'g'){

                map[0][person_j][person_i] = c;

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j, person_i, "%c", c);

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c = '.';

                map[0][person_j - 1][person_i + 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j - 1, person_i + 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i++;
                person_j--;

                (*gold)++;

                attron(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 3, "gold");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 8, ": %d", *gold);

                attroff(COLOR_PAIR(2));

                continue;



            }

            if(map[0][person_j - 1][person_i + 1] == 'b'){

                map[0][person_j][person_i] = c;

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j, person_i, "%c", c);

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c = '.';

                map[0][person_j - 1][person_i + 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j - 1, person_i + 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i++;
                person_j--;

                (*blackGold)++;

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13, "black gold: %d", *blackGold);


                continue;
            }
            
            if(map[0][person_j - 1][person_i + 1] == 'T'){

                map[0][person_j][person_i] = c;

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j, person_i, "%c", c);

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c = 't';

                map[0][person_j - 1][person_i + 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j - 1, person_i + 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i++;
                person_j--;

                (*health)--;

                attron(COLOR_PAIR(5));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health:    ");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health: %d", *health);

                attroff(COLOR_PAIR(5));


                continue;
            }
        }

        if(move == 'c'){

            if(map[0][person_j + 1][person_i + 1] == '.' || map[0][person_j + 1][person_i + 1] == '#' || map[0][person_j + 1][person_i + 1] == '+' || map[0][person_j + 1][person_i + 1] == 't'){

                map[0][person_j][person_i] = c;

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j, person_i, "%c", c);

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c =  map[0][person_j + 1][person_i + 1];

                map[0][person_j + 1][person_i + 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j + 1, person_i + 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i++;
                person_j++;

                refresh();

                continue;
            }

            if(map[0][person_j + 1][person_i + 1] == 'g'){

                map[0][person_j][person_i] = c;

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j, person_i, "%c", c);

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c = '.';

                map[0][person_j + 1][person_i + 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j + 1, person_i + 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i++;
                person_j++;

                (*gold)++;

                attron(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 3, "gold");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 8, ": %d", *gold);

                attroff(COLOR_PAIR(2));

                continue;



            }

            if(map[0][person_j + 1][person_i + 1] == 'b'){

                map[0][person_j][person_i] = c;

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j, person_i, "%c", c);

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c = '.';

                map[0][person_j + 1][person_i + 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j + 1, person_i + 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i++;
                person_j++;

                (*blackGold)++;

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13, "black gold: %d", *blackGold);


                continue;
            }

            if(map[0][person_j + 1][person_i + 1] == 'T'){

                map[0][person_j][person_i] = c;

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j, person_i, "%c", c);

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c = 't';

                map[0][person_j + 1][person_i + 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j + 1, person_i + 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i++;
                person_j++;

                (*health)--;

                attron(COLOR_PAIR(5));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health:    ");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health: %d", *health);

                attroff(COLOR_PAIR(5));


                continue;
            }
            

        }


        if(move == 'z'){

            if(map[0][person_j + 1][person_i - 1] == '.' || map[0][person_j + 1][person_i - 1] == '#' || map[0][person_j + 1][person_i - 1] == '+' || map[0][person_j + 1][person_i - 1] == 't'){

                map[0][person_j][person_i] = c;

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j, person_i, "%c", c);

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c =  map[0][person_j + 1][person_i - 1];

                map[0][person_j + 1][person_i - 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j + 1, person_i - 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i--;
                person_j++;

                refresh();

                continue;
            }

            if(map[0][person_j + 1][person_i - 1] == 'g'){

                map[0][person_j][person_i] = c;

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j, person_i, "%c", c);

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c = '.';

                map[0][person_j + 1][person_i - 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j + 1, person_i - 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i--;
                person_j++;

                (*gold)++;

                attron(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 3, "gold");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 8, ": %d", *gold);

                attroff(COLOR_PAIR(2));

                continue;



            }

            if(map[0][person_j + 1][person_i - 1] == 'b'){

                map[0][person_j][person_i] = c;

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j, person_i, "%c", c);

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c = '.';

                map[0][person_j + 1][person_i - 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j + 1, person_i - 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i--;
                person_j++;

                (*blackGold)++;

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13, "black gold: %d", *blackGold);


                continue;



            }

            if(map[0][person_j + 1][person_i - 1] == 'T'){

                map[0][person_j][person_i] = c;

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j, person_i, "%c", c);

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c = 't';

                map[0][person_j + 1][person_i - 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j + 1, person_i - 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i--;
                person_j++;

                (*health)--;

                attron(COLOR_PAIR(5));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health:    ");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health: %d", *health);

                attroff(COLOR_PAIR(5));



                continue;



            }
        }

        if(move == 'q'){

            if(map[0][person_j - 1][person_i - 1] == '.' || map[0][person_j - 1][person_i - 1] == '#' || map[0][person_j - 1][person_i - 1] == '+' || map[0][person_j - 1][person_i - 1] == 't'){

                map[0][person_j][person_i] = c;

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j, person_i, "%c", c);

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c =  map[0][person_j - 1][person_i - 1];

                map[0][person_j - 1][person_i - 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j - 1, person_i - 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i--;
                person_j--;

                refresh();

                continue;
            }

            if(map[0][person_j - 1][person_i - 1] == 'g'){

                map[0][person_j][person_i] = c;

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j, person_i, "%c", c);

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c = '.';

                map[0][person_j - 1][person_i - 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j - 1, person_i - 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i--;
                person_j--;

                (*gold)++;

                attron(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 3, "gold");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 8, ": %d", *gold);

                attroff(COLOR_PAIR(2));

                continue;



            }

            if(map[0][person_j - 1][person_i - 1] == 'b'){

                map[0][person_j][person_i] = c;

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j, person_i, "%c", c);

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c = '.';

                map[0][person_j - 1][person_i - 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j - 1, person_i - 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i--;
                person_j--;

                (*blackGold)++;

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13, "black gold: %d", *blackGold);

                continue;



            }

            if(map[0][person_j - 1][person_i - 1] == 'T'){

                map[0][person_j][person_i] = c;

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j, person_i, "%c", c);

                if(map[0][person_j][person_i] == '+' || map[0][person_j][person_i] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c = 't';

                map[0][person_j - 1][person_i - 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j - 1, person_i - 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i--;
                person_j--;

                (*health)--;

                attron(COLOR_PAIR(5));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health:    ");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health: %d", *health);

                attroff(COLOR_PAIR(5));

                continue;



            }
        }

        if(move == 's' && is_guest == 0){
        
        //save_matrix_to_file(map, name,difficulty, color, character, *gold);
        save_matrix_to_file(map, name,difficulty, color, charact, *gold, c, *blackGold, *health, music, person_i, person_j, person_i_1, person_j_1, person_i_2, person_j_2, person_i_3, person_j_3);
        

        return 0;
     
        }



    }


    clear();

    floor1:


    map[1][person_j_1][person_i_1] = character;

    for(int j = 0; j < y; j++){
        for(int i = 0; i < x; i++){

            if(map[1][j][i] == '@'){

                attron(COLOR_PAIR(color));
                mvprintw(j, i,"%c",  map[1][j][i]);
                attroff(COLOR_PAIR(color));
                continue;

            }

                if(map[1][j][i] == 'g'){

                    attron(COLOR_PAIR(2));
                    
                }

                if(map[1][j][i] == '-' || map[1][j][i] == '|'){

                    attron(COLOR_PAIR(6));

                }

                if(map[1][j][i] == '+' || map[1][j][i] == '#'){

                    attron(COLOR_PAIR(7));

                }

            if(map[1][j][i] == 'T'){
                mvprintw(j, i,".");
            }else{
            mvprintw(j, i,"%c",  map[1][j][i]);
            }

                if(map[1][j][i] == '-' || map[1][j][i] == '|'){

                    attroff(COLOR_PAIR(6));

                }

                if(map[1][j][i] == '+' || map[1][j][i] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                if(map[1][j][i] == 'g'){

                    attroff(COLOR_PAIR(2));
                    
                }



        }
    }

    mvprintw(y - 1, 0, "Name: %s", name);
    mvprintw(y - 1, strlen(name) + 7, "Difficulty: level%d", difficulty);
    mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  "), "Character :");
                
    attron(COLOR_PAIR(color));

    mvprintw(y - 1,  strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 1, "%c", character);

    attroff(COLOR_PAIR(color));

    attron(COLOR_PAIR(2));

    mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 3, "gold");

    mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 8, ": %d", *gold);

    attroff(COLOR_PAIR(2));

    mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13, "black gold: %d", *blackGold);

    attron(COLOR_PAIR(5));

    mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health: %d", *health);

    attroff(COLOR_PAIR(5));

    refresh();
    

    if(is_guest == 1 || newGame == 1)
    c = '.';

    while(1){

        move = getch();

        if((move == 'q' && map[1][person_j_1 - 1][person_i_1 - 1] == 'S') ||
                (move == 'w' && map[1][person_j_1 - 1][person_i_1] == 'S') ||
                    (move == 'e' && map[1][person_j_1 - 1][person_i_1 + 1] == 'S') ||
                        (move == 'd' && map[1][person_j_1][person_i_1 + 1] == 'S') ||
                            (move == 'c' && map[1][person_j_1 + 1][person_i_1 + 1] == 'S') ||
                                (move == 'x' && map[1][person_j_1 + 1][person_i_1] == 'S') ||
                                    (move == 'z' && map[1][person_j_1 + 1][person_i_1 - 1] == 'S') ||
                                        (move == 'a' && map[1][person_j_1][person_i_1 - 1] == 'S')

                ){

                    map[1][person_j_1][person_i_1] = c;

                    break;

                }

        if(move == 'w'){

            if(map[1][person_j_1 - 1][person_i_1] == '.' || map[1][person_j_1 - 1][person_i_1] == '#' || map[1][person_j_1 - 1][person_i_1] == '+' || map[1][person_j_1 - 1][person_i_1] == 't'){


                map[1][person_j_1][person_i_1] = c;

                if(map[1][person_j_1][person_i_1] == '+' || map[1][person_j_1][person_i_1] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_1, person_i_1, "%c", c);

                if(map[1][person_j_1][person_i_1] == '+' || map[1][person_j_1][person_i_1] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c =  map[1][person_j_1 - 1][person_i_1];

                map[1][person_j_1 - 1][person_i_1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_1 - 1, person_i_1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_j_1--;

                refresh();

                continue;
            }

            if(map[1][person_j_1 - 1][person_i_1] == 'g'){

                map[1][person_j_1][person_i_1] = c;

                if(map[1][person_j_1][person_i_1] == '+' || map[1][person_j_1][person_i_1] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_1, person_i_1, "%c", c);

                if(map[1][person_j_1][person_i_1] == '+' || map[1][person_j_1][person_i_1] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c = '.';

                map[1][person_j_1 - 1][person_i_1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_1 - 1, person_i_1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_j_1--;

                (*gold)++;

                attron(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 3, "gold");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 8, ": %d", *gold);

                attroff(COLOR_PAIR(2));

                continue;



            }

            if(map[1][person_j_1 - 1][person_i_1] == 'b'){

                map[1][person_j_1][person_i_1] = c;

                if(map[1][person_j_1][person_i_1] == '+' || map[1][person_j_1][person_i_1] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_1, person_i_1, "%c", c);

                if(map[1][person_j_1][person_i_1] == '+' || map[1][person_j_1][person_i_1] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c = '.';

                map[1][person_j_1 - 1][person_i_1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_1 - 1, person_i_1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_j_1--;

                (*blackGold)++;

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13, "black gold: %d", *blackGold);


                continue;



            }

            if(map[1][person_j_1 - 1][person_i_1] == 'T'){

                map[1][person_j_1][person_i_1] = c;

                if(map[1][person_j_1][person_i_1] == '+' || map[1][person_j_1][person_i_1] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_1, person_i_1, "%c", c);

                if(map[1][person_j_1][person_i_1] == '+' || map[1][person_j_1][person_i_1] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c = 't';

                map[1][person_j_1 - 1][person_i_1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_1 - 1, person_i_1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_j_1--;

                (*health)--;

                attron(COLOR_PAIR(5));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health:    ");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health: %d", *health);

                attroff(COLOR_PAIR(5));


                continue;



            }
        }


        if(move == 'x'){

            if(map[1][person_j_1 + 1][person_i_1] == '.' || map[1][person_j_1 + 1][person_i_1] == '#' || map[1][person_j_1 + 1][person_i_1] == '+' || map[1][person_j_1 + 1][person_i_1] == 't'){

                map[1][person_j_1][person_i_1] = c;

                if(map[1][person_j_1][person_i_1] == '+' || map[1][person_j_1][person_i_1] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_1, person_i_1, "%c", c);

                if(map[1][person_j_1][person_i_1] == '+' || map[1][person_j_1][person_i_1] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c =  map[1][person_j_1 + 1][person_i_1];

                map[1][person_j_1 + 1][person_i_1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_1 + 1, person_i_1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_j_1++;

                refresh();

                continue;
            }

            if(map[1][person_j_1 + 1][person_i_1] == 'g' || map[1][person_j_1 + 1][person_i_1] == 'b' || map[1][person_j_1 + 1][person_i_1] == 'T'){
                if(map[1][person_j_1 + 1][person_i_1] == 'g')
                (*gold)++;
                 if(map[1][person_j_1 + 1][person_i_1] == 'b')
                (*blackGold)++;
                if(map[1][person_j_1 + 1][person_i_1] == 'T')
                (*health)--;

                map[1][person_j_1][person_i_1] = c;

                if(map[1][person_j_1][person_i_1] == '+' || map[1][person_j_1][person_i_1] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_1, person_i_1, "%c", c);

                if(map[1][person_j_1][person_i_1] == '+' || map[1][person_j_1][person_i_1] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                if(map[1][person_j_1 + 1][person_i_1] == 'g' || map[1][person_j_1 + 1][person_i_1] == 'b'){
                c = '.';
                } else if(map[1][person_j_1 + 1][person_i_1] == 'T'){
                    c = 't';
                }

                map[1][person_j_1 + 1][person_i_1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_1 + 1, person_i_1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_j_1++;
                

                attron(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 3, "gold");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 8, ": %d", *gold);

                attroff(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13, "black gold: %d", *blackGold);

                attron(COLOR_PAIR(5));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health:    ");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health: %d", *health);

                attroff(COLOR_PAIR(5));

                refresh();

                continue;
            }
            
        }

        if(move == 'a'){

            if(map[1][person_j_1][person_i_1 - 1] == '.' || map[1][person_j_1][person_i_1 - 1] == '#' || map[1][person_j_1][person_i_1 - 1] == '+' || map[1][person_j_1][person_i_1 - 1] == 't'){

                map[1][person_j_1][person_i_1] = c;

                if(map[1][person_j_1][person_i_1] == '+' || map[1][person_j_1][person_i_1] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_1, person_i_1, "%c", c);

                if(map[1][person_j_1][person_i_1] == '+' || map[1][person_j_1][person_i_1] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c =  map[1][person_j_1][person_i_1 - 1];

                map[1][person_j_1][person_i_1 - 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_1, person_i_1 - 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i_1--;

                refresh();

                continue;
            }

            if(map[1][person_j_1][person_i_1 - 1] == 'g' || map[1][person_j_1][person_i_1 - 1] == 'b' || map[1][person_j_1][person_i_1 - 1] == 'T'){

                if(map[1][person_j_1][person_i_1 - 1] == 'g')
                (*gold)++;
                if(map[1][person_j_1][person_i_1 - 1] == 'b')
                (*blackGold)++;
                if(map[1][person_j_1][person_i_1 - 1] == 'T')
                (*health)--;
                map[1][person_j_1][person_i_1] = c;

                if(map[1][person_j_1][person_i_1] == '+' || map[1][person_j_1][person_i_1] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_1, person_i_1, "%c", c);

                if(map[1][person_j_1][person_i_1] == '+' || map[1][person_j_1][person_i_1] == '#'){

                    attroff(COLOR_PAIR(7));

                }
                if(map[1][person_j_1][person_i_1 - 1] == 'g' || map[1][person_j_1][person_i_1 - 1] == 'b'){
                c = '.';
                }
                if(map[1][person_j_1][person_i_1 - 1] == 'T'){
                c = 't';
                }

                map[1][person_j_1][person_i_1 - 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_1, person_i_1 - 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i_1--;

                attron(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 3, "gold");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 8, ": %d", *gold);

                attroff(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13, "black gold: %d", *blackGold);

                attron(COLOR_PAIR(5));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health:    ");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health: %d", *health);

                attroff(COLOR_PAIR(5));

                refresh();

                continue;
            }
        }

        if(move == 'd'){

            if(map[1][person_j_1][person_i_1 + 1] == '.' || map[1][person_j_1][person_i_1 + 1] == '#' || map[1][person_j_1][person_i_1 + 1] == '+' || map[1][person_j_1][person_i_1 + 1] == 't'){

                map[1][person_j_1][person_i_1] = c;

                if(map[1][person_j_1][person_i_1] == '+' || map[1][person_j_1][person_i_1] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_1, person_i_1, "%c", c);

                if(map[1][person_j_1][person_i_1] == '+' || map[1][person_j_1][person_i_1] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c =  map[1][person_j_1][person_i_1 + 1];

                map[1][person_j_1][person_i_1 + 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_1, person_i_1 + 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i_1++;

                refresh();

                continue;
            }

            if(map[1][person_j_1][person_i_1 + 1] == 'g' || map[1][person_j_1][person_i_1 + 1] == 'b' || map[1][person_j_1][person_i_1 + 1] == 'T'){
                if(map[1][person_j_1][person_i_1 + 1] == 'g')
                (*gold)++;
                if(map[1][person_j_1][person_i_1 + 1] == 'b')
                (*blackGold)++;
                if(map[1][person_j_1][person_i_1 + 1] == 'T')
                (*health)--;
                map[1][person_j_1][person_i_1] = c;

                if(map[1][person_j_1][person_i_1] == '+' || map[1][person_j_1][person_i_1] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_1, person_i_1, "%c", c);

                if(map[1][person_j_1][person_i_1] == '+' || map[1][person_j_1][person_i_1] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                if(map[1][person_j_1][person_i_1 + 1] == 'g' || map[1][person_j_1][person_i_1 + 1] == 'b'){
                c = '.';
                }
                if(map[1][person_j_1][person_i_1 + 1] == 'T'){
                    c = 't';
                }

                map[1][person_j_1][person_i_1 + 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_1, person_i_1 + 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i_1++;

                attron(COLOR_PAIR(2));
                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 3, "gold");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 8, ": %d", *gold);

                attroff(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13, "black gold: %d", *blackGold);

                attron(COLOR_PAIR(5));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health:    ");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health: %d", *health);

                attroff(COLOR_PAIR(5));

                refresh();

                continue;
            }
        }

        if(move == 'e'){

            if(map[1][person_j_1 - 1][person_i_1 + 1] == '.' || map[1][person_j_1 - 1][person_i_1 + 1] == '#' || map[1][person_j_1 - 1][person_i_1 + 1] == '+' || map[1][person_j_1 - 1][person_i_1 + 1] == 't'){

                map[1][person_j_1][person_i_1] = c;

                if(map[1][person_j_1][person_i_1] == '+' || map[1][person_j_1][person_i_1] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_1, person_i_1, "%c", c);

                if(map[1][person_j_1][person_i_1] == '+' || map[1][person_j_1][person_i_1] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c =  map[1][person_j_1 - 1][person_i_1 + 1];

                map[1][person_j_1 - 1][person_i_1 + 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_1 - 1, person_i_1 + 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i_1++;
                person_j_1--;

                refresh();

                continue;
            }

            if(map[1][person_j_1 - 1][person_i_1 + 1] == 'g' || map[1][person_j_1 - 1][person_i_1 + 1] == 'b' || map[1][person_j_1 - 1][person_i_1 + 1] == 'T'){
                if(map[1][person_j_1 - 1][person_i_1 + 1] == 'g')
                (*gold)++;
                if(map[1][person_j_1 - 1][person_i_1 + 1] == 'b')
                (*blackGold)++;
                if(map[1][person_j_1 - 1][person_i_1 + 1] == 'T')
                (*health)--;

                map[1][person_j_1][person_i_1] = c;

                if(map[1][person_j_1][person_i_1] == '+' || map[1][person_j_1][person_i_1] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_1, person_i_1, "%c", c);

                if(map[1][person_j_1][person_i_1] == '+' || map[1][person_j_1][person_i_1] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                if(map[1][person_j_1 - 1][person_i_1 + 1] == 'g' || map[1][person_j_1 - 1][person_i_1 + 1] == 'b'){
                c = '.';
                }
                if(map[1][person_j_1 - 1][person_i_1 + 1] == 'T'){
                    c = 't';
                }

                map[1][person_j_1 - 1][person_i_1 + 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_1 - 1, person_i_1 + 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i_1++;
                person_j_1--;


                attron(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 3, "gold");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 8, ": %d", *gold);

                attroff(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13, "black gold: %d", *blackGold);

                attron(COLOR_PAIR(5));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health:    ");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health: %d", *health);

                attroff(COLOR_PAIR(5));

                refresh();

                continue;
            }
        }

        if(move == 'c'){

            if(map[1][person_j_1 + 1][person_i_1 + 1] == '.' || map[1][person_j_1 + 1][person_i_1 + 1] == '#' || map[1][person_j_1 + 1][person_i_1 + 1] == '+' || map[1][person_j_1 + 1][person_i_1 + 1] == 't'){

                map[1][person_j_1][person_i_1] = c;

                if(map[1][person_j_1][person_i_1] == '+' || map[1][person_j_1][person_i_1] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_1, person_i_1, "%c", c);

                if(map[1][person_j_1][person_i_1] == '+' || map[1][person_j_1][person_i_1] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c =  map[1][person_j_1 + 1][person_i_1 + 1];

                map[1][person_j_1 + 1][person_i_1 + 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_1 + 1, person_i_1 + 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i_1++;
                person_j_1++;

                refresh();

                continue;
            }

            if(map[1][person_j_1 + 1][person_i_1 + 1] == 'g' || map[1][person_j_1 + 1][person_i_1 + 1] == 'b' || map[1][person_j_1 + 1][person_i_1 + 1] == 'T'){
                if(map[1][person_j_1 + 1][person_i_1 + 1] == 'g')
                (*gold)++;
                if(map[1][person_j_1 + 1][person_i_1 + 1] == 'b')
                (*blackGold)++;
                if(map[1][person_j_1 + 1][person_i_1 + 1] == 'T')
                (*health)--;

                map[1][person_j_1][person_i_1] = c;

                if(map[1][person_j_1][person_i_1] == '+' || map[1][person_j_1][person_i_1] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_1, person_i_1, "%c", c);

                if(map[1][person_j_1][person_i_1] == '+' || map[1][person_j_1][person_i_1] == '#'){

                    attroff(COLOR_PAIR(7));

                }
                if(map[1][person_j_1 + 1][person_i_1 + 1] == 'g' || map[1][person_j_1 + 1][person_i_1 + 1] == 'b'){
                c = '.';
                }
                if(map[1][person_j_1 + 1][person_i_1 + 1] == 'T'){
                    c = 't';
                }

                map[1][person_j_1 + 1][person_i_1 + 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_1 + 1, person_i_1 + 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i_1++;
                person_j_1++;


                attron(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 3, "gold");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 8, ": %d", *gold);

                attroff(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13, "black gold: %d", *blackGold);

                attron(COLOR_PAIR(5));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health:    ");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health: %d", *health);

                attroff(COLOR_PAIR(5));

                refresh();

                continue;
            }
        }


        if(move == 'z'){

            if(map[1][person_j_1 + 1][person_i_1 - 1] == '.' || map[1][person_j_1 + 1][person_i_1 - 1] == '#' || map[1][person_j_1 + 1][person_i_1 - 1] == '+' || map[1][person_j_1 + 1][person_i_1 - 1] == 't'){

                map[1][person_j_1][person_i_1] = c;

                if(map[1][person_j_1][person_i_1] == '+' || map[1][person_j_1][person_i_1] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_1, person_i_1, "%c", c);

                if(map[1][person_j_1][person_i_1] == '+' || map[1][person_j_1][person_i_1] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c =  map[1][person_j_1 + 1][person_i_1 - 1];

                map[1][person_j_1 + 1][person_i_1 - 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_1 + 1, person_i_1 - 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i_1--;
                person_j_1++;

                refresh();

                continue;
            }

            if(map[1][person_j_1 + 1][person_i_1 - 1] == 'g' || map[1][person_j_1 + 1][person_i_1 - 1] == 'b' || map[1][person_j_1 + 1][person_i_1 - 1] == 'T'){
                if(map[1][person_j_1 + 1][person_i_1 - 1] == 'g')
                (*gold)++;
                if(map[1][person_j_1 + 1][person_i_1 - 1] == 'b')
                (*blackGold)++;
                if(map[1][person_j_1 + 1][person_i_1 - 1] == 'T')
                (*health)--;

                map[1][person_j_1][person_i_1] = c;

                if(map[1][person_j_1][person_i_1] == '+' || map[1][person_j_1][person_i_1] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_1, person_i_1, "%c", c);

                if(map[1][person_j_1][person_i_1] == '+' || map[1][person_j_1][person_i_1] == '#'){

                    attroff(COLOR_PAIR(7));

                }
                if(map[1][person_j_1 + 1][person_i_1 - 1] == 'g' || map[1][person_j_1 + 1][person_i_1 - 1] == 'b'){
                c = '.';
                }
                if(map[1][person_j_1 + 1][person_i_1 - 1] == 'T'){
                    c = 't';
                }

                map[1][person_j_1 + 1][person_i_1 - 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_1 + 1, person_i_1 - 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i_1--;
                person_j_1++;


                attron(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 3, "gold");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 8, ": %d", *gold);

                attroff(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13, "black gold: %d", *blackGold);

                attron(COLOR_PAIR(5));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health:    ");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health: %d", *health);

                attroff(COLOR_PAIR(5));

                refresh();
                continue;
            }
        }

        if(move == 'q'){

            if(map[1][person_j_1 - 1][person_i_1 - 1] == '.' || map[1][person_j_1 - 1][person_i_1 - 1] == '#' || map[1][person_j_1 - 1][person_i_1 - 1] == '+' || map[1][person_j_1 - 1][person_i_1 - 1] == 't'){

                map[1][person_j_1][person_i_1] = c;

                if(map[1][person_j_1][person_i_1] == '+' || map[1][person_j_1][person_i_1] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_1, person_i_1, "%c", c);


                if(map[1][person_j_1][person_i_1] == '+' || map[1][person_j_1][person_i_1] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c =  map[1][person_j_1 - 1][person_i_1 - 1];

                map[1][person_j_1 - 1][person_i_1 - 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_1 - 1, person_i_1 - 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i_1--;
                person_j_1--;

                refresh();

                continue;
            }

            if(map[1][person_j_1 - 1][person_i_1 - 1] == 'g' || map[1][person_j_1 - 1][person_i_1 - 1] == 'b' || map[1][person_j_1 - 1][person_i_1 - 1] == 'T'){

                if(map[1][person_j_1 - 1][person_i_1 - 1] == 'g')
                (*gold)++;
                if(map[1][person_j_1 - 1][person_i_1 - 1] == 'b')
                (*blackGold)++;
                if(map[1][person_j_1 - 1][person_i_1 - 1] == 'T')
                (*health)--;

                map[1][person_j_1][person_i_1] = c;

                if(map[1][person_j_1][person_i_1] == '+' || map[1][person_j_1][person_i_1] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_1, person_i_1, "%c", c);

                if(map[1][person_j_1][person_i_1] == '+' || map[1][person_j_1][person_i_1] == '#'){

                    attroff(COLOR_PAIR(7));

                }
                if(map[1][person_j_1 - 1][person_i_1 - 1] == 'g' || map[1][person_j_1 - 1][person_i_1 - 1] == 'b')
                c = '.';
                if(map[1][person_j_1 - 1][person_i_1 - 1] == 'T')
                c = 't';

                map[1][person_j_1 - 1][person_i_1 - 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_1 - 1, person_i_1 - 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i_1--;
                person_j_1--;


                attron(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 3, "gold");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 8, ": %d", *gold);

                attroff(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13, "black gold: %d", *blackGold);

                attron(COLOR_PAIR(5));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health:    ");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health: %d", *health);

                attroff(COLOR_PAIR(5));

                refresh();
                continue;
            }

            
        }

        if(move == 's'&& is_guest == 0){
        
        //save_matrix_to_file(map, name,difficulty, color, character, *gold);
        //save_matrix_to_file(map, name,difficulty, color, charact, c, *gold, person_i, person_j, person_i_1, person_j_1, person_i_2, person_j_2, person_i_3, person_j_3);
        save_matrix_to_file(map, name,difficulty, color, charact, *gold, c, *blackGold, *health, music, person_i, person_j, person_i_1, person_j_1, person_i_2, person_j_2, person_i_3, person_j_3);


        return 0;
     
        }



    }


    clear();
    
    floor2:

    map[2][person_j_2][person_i_2] = character;

    for(int j = 0; j < y; j++){
        for(int i = 0; i < x; i++){

            if(map[2][j][i] == '@'){

                attron(COLOR_PAIR(color));
                mvprintw(j, i,"%c",  map[2][j][i]);
                attroff(COLOR_PAIR(color));
                continue;

            }
                if(map[2][j][i] == '-' || map[2][j][i] == '|'){

                    attron(COLOR_PAIR(6));

                }

                if(map[2][j][i] == '+' || map[2][j][i] == '#'){

                    attron(COLOR_PAIR(7));

                }

                if(map[2][j][i] == 'g'){

                    attron(COLOR_PAIR(2));
                    
                }

            if(map[2][j][i] == 'T'){
                mvprintw(j, i,".");
            }else{
            mvprintw(j, i,"%c",  map[2][j][i]);
            }

                if(map[2][j][i] == '-' || map[2][j][i] == '|'){

                    attroff(COLOR_PAIR(6));

                }

                if(map[2][j][i] == '+' || map[2][j][i] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                if(map[2][j][i] == 'g'){

                    attroff(COLOR_PAIR(2));
                    
                }



        }
    }

    mvprintw(y - 1, 0, "Name: %s", name);
    mvprintw(y - 1, strlen(name) + 7, "Difficulty: level%d", difficulty);
    mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  "), "Character :");
                
    attron(COLOR_PAIR(color));

    mvprintw(y - 1,  strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 1, "%c", character);

    attroff(COLOR_PAIR(color));

    attron(COLOR_PAIR(2));

    mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 3, "gold");

    mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 8, ": %d", *gold);

    attroff(COLOR_PAIR(2));

    mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13, "black gold: %d", *blackGold);

    attron(COLOR_PAIR(5));

    mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health: %d", *health);

    attroff(COLOR_PAIR(5));

    refresh();

        if(is_guest == 1 || newGame == 1)
    c = '.';

    while(1){

        move = getch();

        if((move == 'q' && map[2][person_j_2 - 1][person_i_2 - 1] == 'S') ||
                (move == 'w' && map[2][person_j_2 - 1][person_i_2] == 'S') ||
                    (move == 'e' && map[2][person_j_2 - 1][person_i_2 + 1] == 'S') ||
                        (move == 'd' && map[2][person_j_2][person_i_2 + 1] == 'S') ||
                            (move == 'c' && map[2][person_j_2 + 1][person_i_2 + 1] == 'S') ||
                                (move == 'x' && map[2][person_j_2 + 1][person_i_2] == 'S') ||
                                    (move == 'z' && map[2][person_j_2 + 1][person_i_2 - 1] == 'S') ||
                                        (move == 'a' && map[2][person_j_2][person_i_2 - 1] == 'S')

                ){

                    map[2][person_j_2][person_i_2] = c;

                    break;

                }

        if(move == 'w'){

            if(map[2][person_j_2 - 1][person_i_2] == '.' || map[2][person_j_2 - 1][person_i_2] == '#' || map[2][person_j_2 - 1][person_i_2] == '+' || map[2][person_j_2 - 1][person_i_2] == 't'){


                map[2][person_j_2][person_i_2] = c;

                if(map[2][person_j_2][person_i_2] == '+' || map[2][person_j_2][person_i_2] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_2, person_i_2, "%c", c);

                if(map[2][person_j_2][person_i_2] == '+' || map[2][person_j_2][person_i_2] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c =  map[2][person_j_2 - 1][person_i_2];

                map[2][person_j_2 - 1][person_i_2] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_2 - 1, person_i_2, "%c", character);
                attroff(COLOR_PAIR(color));

                person_j_2--;

                refresh();

                continue;
            }

            if(map[2][person_j_2 - 1][person_i_2] == 'g' || map[2][person_j_2 - 1][person_i_2] == 'b' || map[2][person_j_2 - 1][person_i_2] == 'T'){

                if(map[2][person_j_2 - 1][person_i_2] == 'g')
                (*gold)++;
                if(map[2][person_j_2 - 1][person_i_2] == 'b')
                (*blackGold)++;
                if(map[2][person_j_2 - 1][person_i_2] == 'T')
                (*health)--;

                map[2][person_j_2][person_i_2] = c;

                if(map[2][person_j_2][person_i_2] == '+' || map[2][person_j_2][person_i_2] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_2, person_i_2, "%c", c);

                if(map[2][person_j_2][person_i_2] == '+' || map[2][person_j_2][person_i_2] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                if(map[2][person_j_2 - 1][person_i_2] == 'g' || map[2][person_j_2 - 1][person_i_2] == 'b')
                c = '.';
                if(map[2][person_j_2 - 1][person_i_2] == 'T')
                c = 't';

                map[2][person_j_2 - 1][person_i_2] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_2 - 1, person_i_2, "%c", character);
                attroff(COLOR_PAIR(color));

                person_j_2--;


                attron(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 3, "gold");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 8, ": %d", *gold);

                attroff(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13, "black gold: %d", *blackGold);


                attron(COLOR_PAIR(5));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health:    ");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health: %d", *health);

                attroff(COLOR_PAIR(5));


                continue;
            }
        }


        if(move == 'x'){

            if(map[2][person_j_2 + 1][person_i_2] == '.' || map[2][person_j_2 + 1][person_i_2] == '#' || map[2][person_j_2 + 1][person_i_2] == '+' || map[2][person_j_2 + 1][person_i_2] == 't'){

                map[2][person_j_2][person_i_2] = c;

                if(map[2][person_j_2][person_i_2] == '+' || map[2][person_j_2][person_i_2] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_2, person_i_2, "%c", c);

                if(map[2][person_j_2][person_i_2] == '+' || map[2][person_j_2][person_i_2] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c =  map[2][person_j_2 + 1][person_i_2];

                map[2][person_j_2 + 1][person_i_2] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_2 + 1, person_i_2, "%c", character);
                attroff(COLOR_PAIR(color));

                person_j_2++;

                refresh();

                continue;
            }

            if(map[2][person_j_2 + 1][person_i_2] == 'g' || map[2][person_j_2 + 1][person_i_2] == 'b' || map[2][person_j_2 + 1][person_i_2] == 'T'){

                if(map[2][person_j_2 + 1][person_i_2] == 'g')
                (*gold)++;
                if(map[2][person_j_2 + 1][person_i_2] == 'b')
                (*blackGold)++;
                if(map[2][person_j_2 + 1][person_i_2] == 'T')
                (*health)--;

                map[2][person_j_2][person_i_2] = c;

                if(map[2][person_j_2][person_i_2] == '+' || map[2][person_j_2][person_i_2] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_2, person_i_2, "%c", c);

                if(map[2][person_j_2][person_i_2] == '+' || map[2][person_j_2][person_i_2] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                if(map[2][person_j_2 + 1][person_i_2] == 'g' || map[2][person_j_2 + 1][person_i_2] == 'b')
                c = '.';
                if(map[2][person_j_2 + 1][person_i_2] == 'T')
                c = 't';

                map[2][person_j_2 + 1][person_i_2] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_2 + 1, person_i_2, "%c", character);
                attroff(COLOR_PAIR(color));

                person_j_2++;


                attron(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 3, "gold");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 8, ": %d", *gold);

                attroff(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13, "black gold: %d", *blackGold);

                attron(COLOR_PAIR(5));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health:    ");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health: %d", *health);

                attroff(COLOR_PAIR(5));


                continue;
            }
        }

        if(move == 'a'){

            if(map[2][person_j_2][person_i_2 - 1] == '.' || map[2][person_j_2][person_i_2 - 1] == '#' || map[2][person_j_2][person_i_2 - 1] == '+' || map[2][person_j_2][person_i_2 - 1] == 't'){

                map[2][person_j_2][person_i_2] = c;

                if(map[2][person_j_2][person_i_2] == '+' || map[2][person_j_2][person_i_2] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_2, person_i_2, "%c", c);

                if(map[2][person_j_2][person_i_2] == '+' || map[2][person_j_2][person_i_2] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c =  map[2][person_j_2][person_i_2 - 1];

                map[2][person_j_2][person_i_2 - 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_2, person_i_2 - 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i_2--;

                refresh();

                continue;
            }

            if(map[2][person_j_2][person_i_2 - 1] == 'g' || map[2][person_j_2][person_i_2 - 1] == 'b' || map[2][person_j_2][person_i_2 - 1] == 'T'){

                if(map[2][person_j_2][person_i_2 - 1] == 'g')
                (*gold)++;
                if(map[2][person_j_2][person_i_2 - 1] == 'b')
                (*blackGold)++;
                if(map[2][person_j_2][person_i_2 - 1] == 'T')
                (*health)--;
                map[2][person_j_2][person_i_2] = c;

                if(map[2][person_j_2][person_i_2] == '+' || map[2][person_j_2][person_i_2] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_2, person_i_2, "%c", c);

                if(map[2][person_j_2][person_i_2] == '+' || map[2][person_j_2][person_i_2] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                if(map[2][person_j_2][person_i_2 - 1] == 'g' || map[2][person_j_2][person_i_2 - 1] == 'b')
                c = '.';
                if(map[2][person_j_2][person_i_2 - 1] == 'T')
                c = 't';

                map[2][person_j_2][person_i_2 - 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_2, person_i_2 - 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i_2--;


                attron(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 3, "gold");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 8, ": %d", *gold);

                attroff(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13, "black gold: %d", *blackGold);

                attron(COLOR_PAIR(5));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health:    ");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health: %d", *health);

                attroff(COLOR_PAIR(5));

                continue;
            }
        }

        if(move == 'd'){

            if(map[2][person_j_2][person_i_2 + 1] == '.' || map[2][person_j_2][person_i_2 + 1] == '#' || map[2][person_j_2][person_i_2 + 1] == '+' || map[2][person_j_2][person_i_2 + 1] == 't'){

                map[2][person_j_2][person_i_2] = c;

                if(map[2][person_j_2][person_i_2] == '+' || map[2][person_j_2][person_i_2] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_2, person_i_2, "%c", c);

                if(map[2][person_j_2][person_i_2] == '+' || map[2][person_j_2][person_i_2] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c =  map[2][person_j_2][person_i_2 + 1];

                map[2][person_j_2][person_i_2 + 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_2, person_i_2 + 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i_2++;

                refresh();

                continue;

            }

            if(map[2][person_j_2][person_i_2 + 1] == 'g' || map[2][person_j_2][person_i_2 + 1] == 'b' || map[2][person_j_2][person_i_2 + 1] == 'T'){

                if(map[2][person_j_2][person_i_2 + 1] == 'g')
                (*gold)++;
                if(map[2][person_j_2][person_i_2 + 1] == 'b')
                (*blackGold)++;
                if(map[2][person_j_2][person_i_2 + 1] == 'T')
                (*health)--;

                map[2][person_j_2][person_i_2] = c;

                if(map[2][person_j_2][person_i_2] == '+' || map[2][person_j_2][person_i_2] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_2, person_i_2, "%c", c);

                if(map[2][person_j_2][person_i_2] == '+' || map[2][person_j_2][person_i_2] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                if(map[2][person_j_2][person_i_2 + 1] == 'g' || map[2][person_j_2][person_i_2 + 1] == 'b')
                c = '.';
                if(map[2][person_j_2][person_i_2 + 1] == 'T')
                c = 't';

                map[2][person_j_2][person_i_2 + 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_2, person_i_2 + 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i_2++;


                attron(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 3, "gold");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 8, ": %d", *gold);

                attroff(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13, "black gold: %d", *blackGold);

                attron(COLOR_PAIR(5));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health:    ");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health: %d", *health);

                attroff(COLOR_PAIR(5));

                continue;
            }
        }

        if(move == 'e'){

            if(map[2][person_j_2 - 1][person_i_2 + 1] == '.' || map[2][person_j_2 - 1][person_i_2 + 1] == '#' || map[2][person_j_2 - 1][person_i_2 + 1] == '+' || map[2][person_j_2 - 1][person_i_2 + 1] == 't'){

                map[2][person_j_2][person_i_2] = c;

                if(map[2][person_j_2][person_i_2] == '+' || map[2][person_j_2][person_i_2] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_2, person_i_2, "%c", c);

                if(map[2][person_j_2][person_i_2] == '+' || map[2][person_j_2][person_i_2] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c =  map[2][person_j_2 - 1][person_i_2 + 1];

                map[2][person_j_2 - 1][person_i_2 + 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_2 - 1, person_i_2 + 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i_2++;
                person_j_2--;

                refresh();

                continue;

            }

            if(map[2][person_j_2 - 1][person_i_2 + 1] == 'g' || map[2][person_j_2 - 1][person_i_2 + 1] == 'b' || map[2][person_j_2 - 1][person_i_2 + 1] == 'T'){

                if(map[2][person_j_2 - 1][person_i_2 + 1] == 'g')
                (*gold)++;
                if(map[2][person_j_2 - 1][person_i_2 + 1] == 'b')
                (*blackGold)++;
                if(map[2][person_j_2 - 1][person_i_2 + 1] == 'T')
                (*health)--;

                map[2][person_j_2][person_i_2] = c;

                if(map[2][person_j_2][person_i_2] == '+' || map[2][person_j_2][person_i_2] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_2, person_i_2, "%c", c);

                if(map[2][person_j_2][person_i_2] == '+' || map[2][person_j_2][person_i_2] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                if(map[2][person_j_2 - 1][person_i_2 + 1] == 'g' || map[2][person_j_2 - 1][person_i_2 + 1] == 'b')
                c = '.';
                if(map[2][person_j_2 - 1][person_i_2 + 1] == 'T')
                c = 't';

                map[2][person_j_2 - 1][person_i_2 + 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_2 - 1, person_i_2 + 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i_2++;
                person_j_2--;


                attron(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 3, "gold");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 8, ": %d", *gold);

                attroff(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13, "black gold: %d", *blackGold);

                attron(COLOR_PAIR(5));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health:    ");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health: %d", *health);

                attroff(COLOR_PAIR(5));

                continue;
            }
        }

        if(move == 'c'){

            if(map[2][person_j_2 + 1][person_i_2 + 1] == '.' || map[2][person_j_2 + 1][person_i_2 + 1] == '#' || map[2][person_j_2 + 1][person_i_2 + 1] == '+' || map[2][person_j_2 + 1][person_i_2 + 1] == 't'){

                map[2][person_j_2][person_i_2] = c;

                if(map[2][person_j_2][person_i_2] == '+' || map[2][person_j_2][person_i_2] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_2, person_i_2, "%c", c);

                if(map[2][person_j_2][person_i_2] == '+' || map[2][person_j_2][person_i_2] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c =  map[2][person_j_2 + 1][person_i_2 + 1];

                map[2][person_j_2 + 1][person_i_2 + 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_2 + 1, person_i_2 + 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i_2++;
                person_j_2++;

                refresh();

                continue;
            }

            if(map[2][person_j_2 + 1][person_i_2 + 1] == 'g' || map[2][person_j_2 + 1][person_i_2 + 1] == 'b' || map[2][person_j_2 + 1][person_i_2 + 1] == 'T'){

                if(map[2][person_j_2 + 1][person_i_2 + 1] == 'g')
                (*gold)++;
                if(map[2][person_j_2 + 1][person_i_2 + 1] == 'b')
                (*blackGold)++;
                if(map[2][person_j_2 + 1][person_i_2 + 1] == 'T')
                (*health)--;

                map[2][person_j_2][person_i_2] = c;

                if(map[2][person_j_2][person_i_2] == '+' || map[2][person_j_2][person_i_2] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_2, person_i_2, "%c", c);

                if(map[2][person_j_2][person_i_2] == '+' || map[2][person_j_2][person_i_2] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                if(map[2][person_j_2 + 1][person_i_2 + 1] == 'g' || map[2][person_j_2 + 1][person_i_2 + 1] == 'b')
                c = '.';
                if(map[2][person_j_2 + 1][person_i_2 + 1] == 'T')
                c = 't';

                map[2][person_j_2 + 1][person_i_2 + 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_2 + 1, person_i_2 + 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i_2++;
                person_j_2++;


                attron(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 3, "gold");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 8, ": %d", *gold);

                attroff(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13, "black gold: %d", *blackGold);

                attron(COLOR_PAIR(5));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health:    ");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health: %d", *health);

                attroff(COLOR_PAIR(5));

                continue;
            }
        }


        if(move == 'z'){

            if(map[2][person_j_2 + 1][person_i_2 - 1] == '.' || map[2][person_j_2 + 1][person_i_2 - 1] == '#' || map[2][person_j_2 + 1][person_i_2 - 1] == '+' || map[2][person_j_2 + 1][person_i_2 - 1] == 't'){

                map[2][person_j_2][person_i_2] = c;

                if(map[2][person_j_2][person_i_2] == '+' || map[2][person_j_2][person_i_2] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_2, person_i_2, "%c", c);

                if(map[2][person_j_2][person_i_2] == '+' || map[2][person_j_2][person_i_2] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c =  map[2][person_j_2 + 1][person_i_2 - 1];

                map[2][person_j_2 + 1][person_i_2 - 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_2 + 1, person_i_2 - 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i_2--;
                person_j_2++;

                refresh();

                continue;
            }

            if(map[2][person_j_2 + 1][person_i_2 - 1] == 'g' || map[2][person_j_2 + 1][person_i_2 - 1] == 'b' || map[2][person_j_2 + 1][person_i_2 - 1] == 'T'){

                if(map[2][person_j_2 + 1][person_i_2 - 1] == 'g')
                (*gold)++;
                if(map[2][person_j_2 + 1][person_i_2 - 1] == 'b')
                (*blackGold)++;
                if(map[2][person_j_2 + 1][person_i_2 - 1] == 'T')
                (*health)--;

                map[2][person_j_2][person_i_2] = c;

                if(map[2][person_j_2][person_i_2] == '+' || map[2][person_j_2][person_i_2] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_2, person_i_2, "%c", c);

                if(map[2][person_j_2][person_i_2] == '+' || map[2][person_j_2][person_i_2] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                if(map[2][person_j_2 + 1][person_i_2 - 1] == 'g' || map[2][person_j_2 + 1][person_i_2 - 1] == 'b')
                c = '.';
                if(map[2][person_j_2 + 1][person_i_2 - 1] == 'T')
                c = 't';

                map[2][person_j_2 + 1][person_i_2 - 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_2 + 1, person_i_2 - 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i_2--;
                person_j_2++;


                attron(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 3, "gold");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 8, ": %d", *gold);

                attroff(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13, "black gold: %d", *blackGold);

                attron(COLOR_PAIR(5));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health:    ");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health: %d", *health);

                attroff(COLOR_PAIR(5));

                continue;
            }
        }

        if(move == 'q'){

            if(map[2][person_j_2 - 1][person_i_2 - 1] == '.' || map[2][person_j_2 - 1][person_i_2 - 1] == '#' || map[2][person_j_2 - 1][person_i_2 - 1] == '+' || map[2][person_j_2 - 1][person_i_2 - 1] == 't'){

                map[2][person_j_2][person_i_2] = c;

                if(map[2][person_j_2][person_i_2] == '+' || map[2][person_j_2][person_i_2] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_2, person_i_2, "%c", c);

                if(map[2][person_j_2][person_i_2] == '+' || map[2][person_j_2][person_i_2] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c =  map[2][person_j_2 - 1][person_i_2 - 1];

                map[2][person_j_2 - 1][person_i_2 - 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_2 - 1, person_i_2 - 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i_2--;
                person_j_2--;

                refresh();

                continue;

            }

            if(map[2][person_j_2 - 1][person_i_2 - 1] == 'g' || map[2][person_j_2 - 1][person_i_2 - 1] == 'b' || map[2][person_j_2 - 1][person_i_2 - 1] == 'T'){

                if(map[2][person_j_2 - 1][person_i_2 - 1] == 'g')
                (*gold)++;
                if(map[2][person_j_2 - 1][person_i_2 - 1] == 'b')
                (*blackGold)++;
                if(map[2][person_j_2 - 1][person_i_2 - 1] == 'T')
                (*health)--;

                map[2][person_j_2][person_i_2] = c;

                if(map[2][person_j_2][person_i_2] == '+' || map[2][person_j_2][person_i_2] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_2, person_i_2, "%c", c);

                if(map[2][person_j_2][person_i_2] == '+' || map[2][person_j_2][person_i_2] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                if(map[2][person_j_2 - 1][person_i_2 - 1] == 'g' || map[2][person_j_2 - 1][person_i_2 - 1] == 'b')
                c = '.';
                if(map[2][person_j_2 - 1][person_i_2 - 1] == 'T')
                c = 't';

                map[2][person_j_2 - 1][person_i_2 - 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_2 - 1, person_i_2 - 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i_2--;
                person_j_2--;


                attron(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 3, "gold");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 8, ": %d", *gold);

                attroff(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13, "black gold: %d", *blackGold);

                attron(COLOR_PAIR(5));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health:    ");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health: %d", *health);

                attroff(COLOR_PAIR(5));

                continue;
            }
        }

        if(move == 's' && is_guest == 0){
        
        //save_matrix_to_file(map, name,difficulty, color, character, *gold);
        save_matrix_to_file(map, name,difficulty, color, charact, *gold, c, *blackGold, *health, music, person_i, person_j, person_i_1, person_j_1, person_i_2, person_j_2, person_i_3, person_j_3);


        return 0;
     
        }


    }

    refresh();

    clear();

    floor3:

    map[3][person_j_3][person_i_3] = character;

    for(int j = 0; j < y; j++){
        for(int i = 0; i < x; i++){

            if(map[3][j][i] == '@'){

                attron(COLOR_PAIR(color));
                mvprintw(j, i,"%c",  map[3][j][i]);
                attroff(COLOR_PAIR(color));
                continue;

            }

                if(map[3][j][i] == 'g'){

                    attron(COLOR_PAIR(2));
                    
                }

                if(map[3][j][i] == '-' || map[3][j][i] == '|'){

                    attron(COLOR_PAIR(6));

                }

                if(map[3][j][i] == '+' || map[3][j][i] == '#'){

                    attron(COLOR_PAIR(7));

                }

            if(map[3][j][i] == 'T'){
                mvprintw(j, i,".");
            }else{
            mvprintw(j, i,"%c",  map[3][j][i]);
            }

                if(map[3][j][i] == '-' || map[3][j][i] == '|'){

                    attroff(COLOR_PAIR(6));

                }

                if(map[3][j][i] == '+' || map[3][j][i] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                if(map[3][j][i] == 'g'){

                    attroff(COLOR_PAIR(2));
                    
                }



        }
    }

    mvprintw(y - 1, 0, "Name: %s", name);
    mvprintw(y - 1, strlen(name) + 7, "Difficulty: level%d", difficulty);
    mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  "), "Character :");
                
    attron(COLOR_PAIR(color));

    mvprintw(y - 1,  strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 1, "%c", character);

    attroff(COLOR_PAIR(color));

    attron(COLOR_PAIR(2));

    mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 3, "gold");

    mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 8, ": %d", *gold);

    attroff(COLOR_PAIR(2));

    mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13, "black gold: %d", *blackGold);

    attron(COLOR_PAIR(5));

    mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health: %d", *health);

    attroff(COLOR_PAIR(5));

    refresh();

        if(is_guest == 1 || newGame == 1)
    c = '.';

    while(1){

        move = getch();

        if(move == 'w'){

            if(map[3][person_j_3 - 1][person_i_3] == '.' || map[3][person_j_3 - 1][person_i_3] == '#' || map[3][person_j_3 - 1][person_i_3] == '+' || map[3][person_j_3 - 1][person_i_3] == 't'){


                map[3][person_j_3][person_i_3] = c;

                if(map[3][person_j_3][person_i_3] == '+' || map[3][person_j_3][person_i_3] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_3, person_i_3, "%c", c);

                if(map[3][person_j_3][person_i_3] == '+' || map[3][person_j_3][person_i_3] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c =  map[3][person_j_3 - 1][person_i_3];

                map[3][person_j_3 - 1][person_i_3] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_3 - 1, person_i_3, "%c", character);
                attroff(COLOR_PAIR(color));

                person_j_3--;

                refresh();

                continue;
            }

            if(map[3][person_j_3 - 1][person_i_3] == 'g' || map[3][person_j_3 - 1][person_i_3] == 'b' || map[3][person_j_3 - 1][person_i_3] == 'T'){

                if(map[3][person_j_3 - 1][person_i_3] == 'g')
                (*gold)++;
                if(map[3][person_j_3 - 1][person_i_3] == 'b')
                (*blackGold)++;
                if(map[3][person_j_3 - 1][person_i_3] == 'T')
                (*health)--;

                map[3][person_j_3][person_i_3] = c;

                if(map[3][person_j_3][person_i_3] == '+' || map[3][person_j_3][person_i_3] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_3, person_i_3, "%c", c);

                if(map[3][person_j_3][person_i_3] == '+' || map[2][person_j_3][person_i_3] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                if(map[3][person_j_3 - 1][person_i_3] == 'g' || map[3][person_j_3 - 1][person_i_3] == 'b')
                c = '.';
                if(map[3][person_j_3 - 1][person_i_3] == 'T')
                c = 't';

                map[3][person_j_3 - 1][person_i_3] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_3 - 1, person_i_3, "%c", character);
                attroff(COLOR_PAIR(color));

                person_j_3--;

                attron(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 3, "gold");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 8, ": %d", *gold);

                attroff(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13, "black gold: %d", *blackGold);

                attron(COLOR_PAIR(5));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health:    ");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health: %d", *health);

                attroff(COLOR_PAIR(5));

                continue;
            }
        }

        if(move == 'e'){

            if(map[3][person_j_3 - 1][person_i_3 + 1] == '.' || map[3][person_j_3 - 1][person_i_3 + 1] == '#' || map[3][person_j_3 - 1][person_i_3 + 1] == '+' || map[3][person_j_3 - 1][person_i_3 + 1] == 't'){


                map[3][person_j_3][person_i_3] = c;

                if(map[3][person_j_3][person_i_3] == '+' || map[3][person_j_3][person_i_3] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_3, person_i_3, "%c", c);

                if(map[3][person_j_3][person_i_3] == '+' || map[3][person_j_3][person_i_3] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c =  map[3][person_j_3 - 1][person_i_3 + 1];

                map[3][person_j_3 - 1][person_i_3 + 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_3 - 1, person_i_3 +1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_j_3--;
                person_i_3++;

                refresh();

                continue;

            }

            if(map[3][person_j_3 - 1][person_i_3 + 1] == 'g' || map[3][person_j_3 - 1][person_i_3 + 1] == 'b' || map[3][person_j_3 - 1][person_i_3 + 1] == 'T' ){

                if(map[3][person_j_3 - 1][person_i_3 + 1] == 'g')
                (*gold)++;
                if(map[3][person_j_3 - 1][person_i_3 + 1] == 'b')
                (*blackGold)++;
                if(map[3][person_j_3 - 1][person_i_3 + 1] == 'T')
                (*health)--;

                map[3][person_j_3][person_i_3] = c;

                if(map[3][person_j_3][person_i_3] == '+' || map[3][person_j_3][person_i_3] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_3, person_i_3, "%c", c);

                if(map[3][person_j_3][person_i_3] == '+' || map[2][person_j_3][person_i_3] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                if(map[3][person_j_3 - 1][person_i_3 + 1] == 'g' || map[3][person_j_3 - 1][person_i_3 + 1] == 'b')
                c = '.';
                if(map[3][person_j_3 - 1][person_i_3 + 1] == 'T')
                c = 't';

                map[3][person_j_3 - 1][person_i_3 + 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_3 - 1, person_i_3 + 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_j_3--;
                person_i_3++;
                

                attron(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 3, "gold");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 8, ": %d", *gold);

                attroff(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13, "black gold: %d", *blackGold);

                attron(COLOR_PAIR(5));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health:    ");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health: %d", *health);

                attroff(COLOR_PAIR(5));

                continue;
            }
        }

        if(move == 'd'){

            if(map[3][person_j_3][person_i_3 + 1] == '.' || map[3][person_j_3][person_i_3 + 1] == '#' || map[3][person_j_3][person_i_3 + 1] == '+' || map[3][person_j_3][person_i_3 + 1] == 't'){


                map[3][person_j_3][person_i_3] = c;

                if(map[3][person_j_3][person_i_3] == '+' || map[3][person_j_3][person_i_3] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_3, person_i_3, "%c", c);

                if(map[3][person_j_3][person_i_3] == '+' || map[3][person_j_3][person_i_3] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c =  map[3][person_j_3][person_i_3 + 1];

                map[3][person_j_3][person_i_3 + 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_3, person_i_3 + 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i_3++;

                refresh();

                continue;

            }

            if(map[3][person_j_3][person_i_3 + 1] == 'g' || map[3][person_j_3][person_i_3 + 1] == 'b' || map[3][person_j_3][person_i_3 + 1] == 'T'){

                if(map[3][person_j_3][person_i_3 + 1] == 'g')
                (*gold)++;
                if(map[3][person_j_3][person_i_3 + 1] == 'b')
                (*blackGold)++;
                if(map[3][person_j_3][person_i_3 + 1] == 'T')
                (*health)--;

                map[3][person_j_3][person_i_3] = c;

                if(map[3][person_j_3][person_i_3] == '+' || map[3][person_j_3][person_i_3] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_3, person_i_3, "%c", c);

                if(map[3][person_j_3][person_i_3] == '+' || map[2][person_j_3][person_i_3] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                if(map[3][person_j_3][person_i_3 + 1] == 'g' || map[3][person_j_3][person_i_3 + 1] == 'b')
                c = '.';
                if(map[3][person_j_3][person_i_3 + 1] == 'T')
                c = 't';

                map[3][person_j_3][person_i_3 + 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_3, person_i_3 + 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i_3++;


                attron(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 3, "gold");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 8, ": %d", *gold);

                attroff(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13, "black gold: %d", *blackGold);

                attron(COLOR_PAIR(5));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health:    ");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health: %d", *health);

                attroff(COLOR_PAIR(5));

                continue;
            }
        }

        if(move == 'c'){

            if(map[3][person_j_3 + 1][person_i_3 + 1] == '.' || map[3][person_j_3 + 1][person_i_3 + 1] == '#' || map[3][person_j_3 + 1][person_i_3 + 1] == '+' || map[3][person_j_3 + 1][person_i_3 + 1] == 't'){


                map[3][person_j_3][person_i_3] = c;

                if(map[3][person_j_3][person_i_3] == '+' || map[3][person_j_3][person_i_3] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_3, person_i_3, "%c", c);

                if(map[3][person_j_3][person_i_3] == '+' || map[3][person_j_3][person_i_3] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c =  map[3][person_j_3 + 1][person_i_3 + 1];

                map[3][person_j_3 + 1][person_i_3 + 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_3 + 1, person_i_3 + 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i_3++;
                person_j_3++;

                refresh();

                continue;
            }

            if(map[3][person_j_3 + 1][person_i_3 + 1] == 'g' || map[3][person_j_3 + 1][person_i_3 + 1] == 'b' || map[3][person_j_3 + 1][person_i_3 + 1] == 'T'){

                if(map[3][person_j_3 + 1][person_i_3 + 1] == 'g')
                (*gold)++;
                if(map[3][person_j_3 + 1][person_i_3 + 1] == 'b')
                (*blackGold)++;
                if(map[3][person_j_3 + 1][person_i_3 + 1] == 'T')
                (*health)--;

                map[3][person_j_3][person_i_3] = c;

                if(map[3][person_j_3][person_i_3] == '+' || map[3][person_j_3][person_i_3] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_3, person_i_3, "%c", c);

                if(map[3][person_j_3][person_i_3] == '+' || map[2][person_j_3][person_i_3] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                if(map[3][person_j_3 + 1][person_i_3 + 1] == 'g' || map[3][person_j_3 + 1][person_i_3 + 1] == 'b')
                c = '.';
                if(map[3][person_j_3 + 1][person_i_3 + 1] == 'T')
                c = 't';

                map[3][person_j_3 + 1][person_i_3 + 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_3 + 1, person_i_3 + 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i_3++;
                person_j_3++;


                attron(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 3, "gold");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 8, ": %d", *gold);

                attroff(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13, "black gold: %d", *blackGold);
                
                attron(COLOR_PAIR(5));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health:    ");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health: %d", *health);

                attroff(COLOR_PAIR(5));

                continue;
            }
        }

        if(move == 'x'){

            if(map[3][person_j_3 + 1][person_i_3] == '.' || map[3][person_j_3 + 1][person_i_3] == '#' || map[3][person_j_3 + 1][person_i_3] == '+' || map[3][person_j_3 + 1][person_i_3] == 't'){


                map[3][person_j_3][person_i_3] = c;

                if(map[3][person_j_3][person_i_3] == '+' || map[3][person_j_3][person_i_3] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_3, person_i_3, "%c", c);

                if(map[3][person_j_3][person_i_3] == '+' || map[3][person_j_3][person_i_3] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c =  map[3][person_j_3 + 1][person_i_3];

                map[3][person_j_3 + 1][person_i_3] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_3 + 1, person_i_3, "%c", character);
                attroff(COLOR_PAIR(color));

                person_j_3++;

                refresh();

                continue;
            }

            if(map[3][person_j_3 + 1][person_i_3] == 'g' || map[3][person_j_3 + 1][person_i_3] == 'b' || map[3][person_j_3 + 1][person_i_3] == 'T'){

                if(map[3][person_j_3 + 1][person_i_3] == 'g')
                (*gold)++;
                if(map[3][person_j_3 + 1][person_i_3] == 'b')
                (*blackGold)++;
                if(map[3][person_j_3 + 1][person_i_3] == 'T')
                (*health)--;

                map[3][person_j_3][person_i_3] = c;

                if(map[3][person_j_3][person_i_3] == '+' || map[3][person_j_3][person_i_3] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_3, person_i_3, "%c", c);

                if(map[3][person_j_3][person_i_3] == '+' || map[2][person_j_3][person_i_3] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                if(map[3][person_j_3 + 1][person_i_3] == 'g' || map[3][person_j_3 + 1][person_i_3] == 'b')
                c = '.';
                if(map[3][person_j_3 + 1][person_i_3] == 'T')
                c = 't';          

                map[3][person_j_3 + 1][person_i_3] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_3 + 1, person_i_3, "%c", character);
                attroff(COLOR_PAIR(color));

                person_j_3++;


                attron(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 3, "gold");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 8, ": %d", *gold);

                attroff(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13, "black gold: %d", *blackGold);

                attron(COLOR_PAIR(5));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health:    ");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health: %d", *health);

                attroff(COLOR_PAIR(5));

                continue;
            }
        }

        if(move == 'z'){

            if(map[3][person_j_3 + 1][person_i_3 - 1] == '.' || map[3][person_j_3 + 1][person_i_3 - 1] == '#' || map[3][person_j_3 + 1][person_i_3 - 1] == '+' || map[3][person_j_3 + 1][person_i_3 - 1] == 't'){


                map[3][person_j_3][person_i_3] = c;

                if(map[3][person_j_3][person_i_3] == '+' || map[3][person_j_3][person_i_3] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_3, person_i_3, "%c", c);

                if(map[3][person_j_3][person_i_3] == '+' || map[3][person_j_3][person_i_3] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c =  map[3][person_j_3 + 1][person_i_3 - 1];

                map[3][person_j_3 + 1][person_i_3 - 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_3 + 1, person_i_3 - 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i_3--;
                person_j_3++;

                refresh();

                continue;
            }

            if(map[3][person_j_3 + 1][person_i_3 - 1] == 'g' || map[3][person_j_3 + 1][person_i_3 - 1] == 'b' || map[3][person_j_3 + 1][person_i_3 - 1] == 'T'){

                if(map[3][person_j_3 + 1][person_i_3 - 1] == 'g')
                (*gold)++;
                if(map[3][person_j_3 + 1][person_i_3 - 1] == 'b')
                (*blackGold)++;
                if(map[3][person_j_3 + 1][person_i_3 - 1] == 'T')
                (*health)--;

                map[3][person_j_3][person_i_3] = c;

                if(map[3][person_j_3][person_i_3] == '+' || map[3][person_j_3][person_i_3] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_3, person_i_3, "%c", c);

                if(map[3][person_j_3][person_i_3] == '+' || map[2][person_j_3][person_i_3] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                if(map[3][person_j_3 + 1][person_i_3 - 1] == 'g' || map[3][person_j_3 + 1][person_i_3 - 1] == 'b')
                c = '.';
                if(map[3][person_j_3 + 1][person_i_3 - 1] == 'T')
                c = 't';

                map[3][person_j_3 + 1][person_i_3 - 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_3 + 1, person_i_3 - 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i_3--;
                person_j_3++;
                

                attron(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 3, "gold");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 8, ": %d", *gold);

                attroff(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13, "black gold: %d", *blackGold);

                attron(COLOR_PAIR(5));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health:    ");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health: %d", *health);

                attroff(COLOR_PAIR(5));

                continue;
            }
        }

        if(move == 'a'){

            if(map[3][person_j_3][person_i_3 - 1] == '.' || map[3][person_j_3][person_i_3 - 1] == '#' || map[3][person_j_3][person_i_3 - 1] == '+' || map[3][person_j_3][person_i_3 - 1] == 't'){


                map[3][person_j_3][person_i_3] = c;

                if(map[3][person_j_3][person_i_3] == '+' || map[3][person_j_3][person_i_3] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_3, person_i_3, "%c", c);

                if(map[3][person_j_3][person_i_3] == '+' || map[3][person_j_3][person_i_3] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c =  map[3][person_j_3][person_i_3 - 1];

                map[3][person_j_3][person_i_3 - 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_3, person_i_3 - 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i_3--;

                refresh();

                continue;
            }

            if(map[3][person_j_3][person_i_3 - 1] == 'g' || map[3][person_j_3][person_i_3 - 1] == 'b' || map[3][person_j_3][person_i_3 - 1] == 'T'){

                if(map[3][person_j_3][person_i_3 - 1] == 'g')
                (*gold)++;
                if(map[3][person_j_3][person_i_3 - 1] == 'b')
                (*blackGold)++;
                if(map[3][person_j_3][person_i_3 - 1] == 'T')
                (health)--;

                map[3][person_j_3][person_i_3] = c;

                if(map[3][person_j_3][person_i_3] == '+' || map[3][person_j_3][person_i_3] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_3, person_i_3, "%c", c);

                if(map[3][person_j_3][person_i_3] == '+' || map[2][person_j_3][person_i_3] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                if(map[3][person_j_3][person_i_3 - 1] == 'g' || map[3][person_j_3][person_i_3 - 1] == 'b')
                c = '.';
                if(map[3][person_j_3][person_i_3 - 1] == 'T')
                c = 't';

                map[3][person_j_3][person_i_3 - 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_3, person_i_3 - 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i_3--;


                attron(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 3, "gold");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 8, ": %d", *gold);

                attroff(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13, "black gold: %d", *blackGold);

                attron(COLOR_PAIR(5));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health:    ");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health: %d", *health);

                attroff(COLOR_PAIR(5));

                continue;
            }
        }

        if(move == 'q'){

            if(map[3][person_j_3 - 1][person_i_3 - 1] == '.' || map[3][person_j_3 - 1][person_i_3 - 1] == '#' || map[3][person_j_3 - 1][person_i_3 - 1] == '+' || map[3][person_j_3 - 1][person_i_3 - 1] == 't'){


                map[3][person_j_3][person_i_3] = c;

                if(map[3][person_j_3][person_i_3] == '+' || map[3][person_j_3][person_i_3] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_3, person_i_3, "%c", c);

                if(map[3][person_j_3][person_i_3] == '+' || map[3][person_j_3][person_i_3] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                c =  map[3][person_j_3 - 1][person_i_3 - 1];

                map[3][person_j_3 - 1][person_i_3 - 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_3 - 1 , person_i_3 - 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_i_3--;
                person_j_3--;

                refresh();

                continue;
            }

            if(map[3][person_j_3 - 1][person_i_3 - 1] == 'g' || map[3][person_j_3 - 1][person_i_3 - 1] == 'b' || map[3][person_j_3 - 1][person_i_3 - 1] == 'T'){

                if(map[3][person_j_3 - 1][person_i_3 - 1] == 'g')
                (*gold)++;
                if(map[3][person_j_3 - 1][person_i_3 - 1] == 'b')
                (*blackGold)++;
                 if(map[3][person_j_3 - 1][person_i_3 - 1] == 'T')
                (*health)--;
                               
                map[3][person_j_3][person_i_3] = c;

                if(map[3][person_j_3][person_i_3] == '+' || map[3][person_j_3][person_i_3] == '#'){

                    attron(COLOR_PAIR(7));

                }

                mvprintw(person_j_3, person_i_3, "%c", c);

                if(map[3][person_j_3][person_i_3] == '+' || map[2][person_j_3][person_i_3] == '#'){

                    attroff(COLOR_PAIR(7));

                }

                if(map[3][person_j_3 - 1][person_i_3 - 1] == 'g' || map[3][person_j_3 - 1][person_i_3 - 1] == 'b')
                c = '.';
                if(map[3][person_j_3 - 1][person_i_3 - 1] == 'T')
                c = 't';

                map[3][person_j_3 - 1][person_i_3 - 1] = character;

                attron(COLOR_PAIR(color));
                mvprintw(person_j_3 - 1, person_i_3 - 1, "%c", character);
                attroff(COLOR_PAIR(color));

                person_j_3--;
                person_i_3--;


                attron(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 3, "gold");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 8, ": %d", *gold);

                attroff(COLOR_PAIR(2));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13, "black gold: %d", *blackGold);

                attron(COLOR_PAIR(5));

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health:    ");

                mvprintw(y - 1, strlen(name) + 7 + strlen("Difficulty: level  ") + strlen("character: ") + 13 + strlen("black gold:    "), "health: %d", *health);

                attroff(COLOR_PAIR(5));

                continue;
            }
        }

        if(move == 's' && is_guest == 0){
        
        //save_matrix_to_file(map, name,difficulty, color, character, *gold);
        save_matrix_to_file(map, name,difficulty, color, charact, *gold, c, *blackGold, *health, music, person_i, person_j, person_i_1, person_j_1, person_i_2, person_j_2, person_i_3, person_j_3);

        return 0;
     
        }


    }
    
}



int main() {
    initscr();
    start_color();
    noecho();
    curs_set(FALSE);

    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_WHITE, COLOR_BLACK);
    init_pair(5, COLOR_CYAN, COLOR_BLACK);
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(7, COLOR_BLUE, COLOR_BLACK);


    char name[50];
    int is_guest = 0;
    int newGame;
    int color = -1;
    int character = -1;
    int difficulty = -1;
    int music = 0;

    printFirstPage();
    int n = usersMenu();

    if(n == 0){
        addUser();
        logIn(name);
    } else if(n == 1){
        logIn(name);
    } else if(n == 2){
        guest(name);
        is_guest = 1;
    }

    FILE *savedGame;

    settingMenu(&savedGame, name, is_guest, &newGame, &difficulty, &character, &color, &music);

    char map[z][y][x];

    int s, w, gold, blackGold, health;

    if(is_guest == 1){

        int s = game(is_guest, newGame, difficulty, music, character, color, name, &gold, &blackGold, &health);
    }

    if(is_guest == 0){

        int s = game(is_guest, newGame, difficulty, music, character, color, name, &gold, &blackGold, &health);
    }

    getch();
    //getch();
    endwin();  // End ncurses mode
    return 0;
}