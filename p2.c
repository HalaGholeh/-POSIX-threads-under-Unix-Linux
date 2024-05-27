#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <semaphore.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <GL/glut.h>
#include <math.h>
#include <stdbool.h>



//define valriables
#define MAX_ITEMS 10
#define STRING_LENGTH 100
#define NUMBER_OF_TEAMS 5
#define ITEMS_RANGE_NUMBER 5
#define ITEMS_QUANTITY_RANGE 3
#define TIME_CONSTANT 1
#define LOW_AMI_RATE 0.5
#define CUSTOMER_ARRIVE_LOWER 0
#define CUSTOMER_ARRIVE_UPPER 4
#define TIME_TO_END 1
#define SHOPPING_LOWER_TIME 3
#define SHOPPING_UPPER_TIME 6
#define NUMBER_OF_EMPLOYEES 5
#define PICKING_TIME_CONSTANT 200000


struct Item *shm2;
int pip[2];
int text_pip[2];
size_t items_number;  //counting number of items in supermarket

//item struct
struct Item
{
    char name[256];
    double price;
    int quantity;
    int section;
    int id;
    int ami;
};

typedef struct Item Item;

struct Color {
    float r, g, b;
};

struct item {
    float x1, y1, x2, y2;
    struct Color color;
    char item_name[50];
    int item_id;
    int item_quantity;
};

struct CustomerArea {
    float x1, y1, x2, y2;
    struct Color color;
    char label[50];
};

struct TimeArea {
    float x1, y1, x2, y2;
    struct Color color;
    char label[50];
    float time;
};

struct Cart {
    float x1, y1, x2, y2;
    struct Color color;
    char label[50];
    int quantity;
};

struct MessageRectangle {
    float x1, y1, x2, y2;
    struct Color color;
    char msg_content[100];
};

struct Team {
    struct item items[MAX_ITEMS];
    struct Cart team_cart;
    int numItems;
    int team_id;
};

//global vars
int screenWidth, screenHeight; 
int team1_items, team2_items, team3_items, team4_items, team5_items;
struct Team teamBlue, teamGreen, teamPurple, teamYellow, teamRed;
struct CustomerArea customerSquare;
struct MessageRectangle messageRectangle;
struct TimeArea timeSquare;
int customer_count;
int start_time;
void drawSquare(float x1, float y1, float x2, float y2);
void drawTeam(struct Team team);
void updateTeam(struct Team *team, float startX, float y1, float y2);
void display(void);
void drawTimeSquare(struct TimeArea timeSquare);
void drawCustomerSquare(struct CustomerArea customer);
void drawMessageRecSquare(struct MessageRectangle messageRec);
void reshape(int width, int height);

int check_message(const char *message) {
    return (strstr(message, "Manager") != NULL || strstr(message, "Open") != NULL || strstr(message, "closing") != NULL);
}

void display(void) {
    
    // Initialize positions, number of items, and color for each team
    teamBlue.numItems = team1_items;
    teamBlue.team_id = 0;
    updateTeam(&teamBlue, -12.25, 6.5, 8.5);

    teamGreen.numItems = team2_items;
    teamGreen.team_id = 1;
    updateTeam(&teamGreen, -12.25, 5.5, 3.5);

    teamPurple.numItems = team3_items;
    teamPurple.team_id = 2;
    updateTeam(&teamPurple, -12.25, 2.5, 0.5);

    teamYellow.numItems = team4_items;
    teamYellow.team_id = 3;
    updateTeam(&teamYellow, -12.25, -0.5, -2.5);

    teamRed.numItems = team5_items;
    teamRed.team_id = 4;
    updateTeam(&teamRed, -12.25, -3.5, -5.5);

    // Set the customer square properties
    customerSquare.x1 = -9.75;
    customerSquare.y1 = -6.5;
    customerSquare.x2 = -7.75;
    customerSquare.y2 = -8.5;
    
    timeSquare.x1 = -12.25;
    timeSquare.y1 = -6.5;
    timeSquare.x2 = -10.25;
    timeSquare.y2 = -8.5;


    // Set the message rectangle properties
    messageRectangle.x1 = -7.25;
    messageRectangle.y1 = -6.5;
    messageRectangle.x2 = 12.00;
    messageRectangle.y2 = -8.5;
    messageRectangle.color.r = 0.2;
    messageRectangle.color.g = 0.1;
    messageRectangle.color.b = 1.0;

    // Drawing the teams
    drawTeam(teamBlue);
    drawTeam(teamGreen);
    drawTeam(teamPurple);
    drawTeam(teamYellow);
    drawTeam(teamRed); 
    
    strcpy(timeSquare.label, "Time in sec");
    drawTimeSquare(timeSquare);
    // Draw the customer square and the message rectangle
    strcpy(customerSquare.label, "#Customers");
    drawCustomerSquare(customerSquare);
    drawMessageRecSquare(messageRectangle);
    
    glFlush();
}

void drawTimeSquare(struct TimeArea timeSquare) {
    glColor3f(timeSquare.color.r, timeSquare.color.g, timeSquare.color.b);
    drawSquare(timeSquare.x1, timeSquare.y1, timeSquare.x2, timeSquare.y2);

    float centerX = (timeSquare.x1 + timeSquare.x2) / 2.0f;
    float centerY = (timeSquare.y1 + timeSquare.y2) / 2.0f;

    int labelLength = strlen(timeSquare.label);
    float labelWidth = labelLength * 0.15f; 
    float labelHeight = 0.15f; 

    float labelY = centerY + 0.5f; 
    
     // Display the time label
    glColor3f(0.0f, 0.0f, 0.0f);
    glRasterPos2f(centerX - (labelWidth / 2.0f) - 0.1f, labelY);
    for (int i = 0; timeSquare.label[i] != '\0'; ++i) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, timeSquare.label[i]);
    }

    // Display the time value
    char timeStr[20];
    snprintf(timeStr, sizeof(timeStr), "%.2f", timeSquare.time);
    int timeLength = strlen(timeStr);
    float timeWidth = timeLength * 0.15f;
    float timeY = centerY - 0.15f;

    glRasterPos2f(centerX - (timeWidth / 2.0f), timeY);
    for (int i = 0; timeStr[i] != '\0'; ++i) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, timeStr[i]);
    }
}

void drawCustomerSquare(struct CustomerArea customer) {
    glColor3f(customer.color.r, customer.color.g, customer.color.b);
    drawSquare(customer.x1, customer.y1, customer.x2, customer.y2);

    float centerX = (customer.x1 + customer.x2) / 2.0f;
    float centerY = (customer.y1 + customer.y2) / 2.0f;

    int labelLength = strlen(customer.label);
    float labelWidth = labelLength * 0.15f; 
    float labelHeight = 0.15f; 

    float labelY = centerY + 0.5f; 
    
    // Display the label
    glColor3f(0.0f, 0.0f, 0.0f);
    glRasterPos2f(centerX - (labelWidth / 2.0f) - 0.1f, labelY);
    for (int i = 0; customer.label[i] != '\0'; ++i) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, customer.label[i]);
    }
    // Display the number of customers
    char countStr[20];
    snprintf(countStr, sizeof(countStr), "%d", customer_count);
    int countLength = strlen(countStr);
    float countWidth = countLength * 0.15f;
    float countY = centerY - 0.15f;

    glRasterPos2f(centerX - (countWidth / 2.0f), countY);
    for (int i = 0; countStr[i] != '\0'; ++i) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, countStr[i]);
    }
}

void drawMessageRecSquare(struct MessageRectangle messageRec) {
    glColor3f(messageRec.color.r, messageRec.color.g, messageRec.color.b);
    drawSquare(messageRec.x1, messageRec.y1, messageRec.x2, messageRec.y2);

    // Calculate center of the cart square
    float centerX = (messageRec.x1 + messageRec.x2) / 2.0f;
    float centerY = (messageRec.y1 + messageRec.y2) / 2.0f;

    // Calculate label width and height
    int labelLength = strlen(messageRec.msg_content); 
    float labelWidth = labelLength * 0.15f; // Adjust this scaling factor as needed
    float labelHeight = 0.15f; // Adjust this based on the font size

    // Position the label slightly above the center of the square
    float labelX = centerX - (labelWidth / 2.0f) - 0.2f;
    float labelY = centerY + labelHeight; // Increment the Y-coordinate
    
    // Set font and text properties for clarity and beauty
    glColor3f(0.0f, 0.0f, 0.0f); // Set label color to black for better visibility

    // Render label character by character using the selected font
    glRasterPos2f(labelX, labelY);
    for (int i = 0; messageRec.msg_content[i] != '\0'; ++i) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, messageRec.msg_content[i]); 
    }
}


void drawSquare(float x1, float y1, float x2, float y2) {
    glBegin(GL_POLYGON);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
}

void drawTeam(struct Team team) {
    for (int i = 0; i < team.numItems; ++i) {
        // Draw the item square
        glColor3f(team.items[i].color.r, team.items[i].color.g, team.items[i].color.b);
        drawSquare(team.items[i].x1, team.items[i].y1, team.items[i].x2, team.items[i].y2);

        // Calculate the center of the item square
        float centerX = (team.items[i].x1 + team.items[i].x2) / 2.0f;
        float centerY = (team.items[i].y1 + team.items[i].y2) / 2.0f;

        // Display the item name
        glColor3f(0.0f, 0.0f, 0.0f);
        int itemNameLength = strlen(team.items[i].item_name);
        float itemNameWidth = itemNameLength * 0.1f;
        float itemNameHeight = 0.15f;

        float itemNameY = centerY + 0.5f;
        glRasterPos2f(centerX - (itemNameWidth / 2.0f), itemNameY);

        for (int j = 0; team.items[i].item_name[j] != '\0'; ++j) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, team.items[i].item_name[j]);
        }

        // Display the item quantity
        char quantityStr[20];
        snprintf(quantityStr, sizeof(quantityStr), "%d", team.items[i].item_quantity);
        int quantityLength = strlen(quantityStr);
        float quantityWidth = quantityLength * 0.15f;
        float quantityY = centerY - 0.15f;

        glRasterPos2f(centerX - (quantityWidth / 2.0f), quantityY);

        for (int j = 0; quantityStr[j] != '\0'; ++j) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, quantityStr[j]);
        }
    }

    // Draw the cart square
    glColor3f(team.team_cart.color.r, team.team_cart.color.g, team.team_cart.color.b);
    drawSquare(team.team_cart.x1, team.team_cart.y1, team.team_cart.x2, team.team_cart.y2);

    // Calculate the center of the cart square
    float centerX = (team.team_cart.x1 + team.team_cart.x2) / 2.0f;
    float centerY = (team.team_cart.y1 + team.team_cart.y2) / 2.0f;

    // Display the cart label
    int cartLabelLength = strlen(team.team_cart.label);
    float cartLabelWidth = cartLabelLength * 0.15f;
    float cartLabelHeight = 0.15f;

    float cartLabelX = centerX - (cartLabelWidth / 2.0f);
    float cartLabelY = centerY + cartLabelHeight + 0.25f;

    glColor3f(0.0f, 0.0f, 0.0f);

    glRasterPos2f(cartLabelX, cartLabelY);
    for (int i = 0; i < cartLabelLength; ++i) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, team.team_cart.label[i]);
    }

    // Display the cart quantity
    char cartQuantityStr[20];
    snprintf(cartQuantityStr, sizeof(cartQuantityStr), "%d", team.team_cart.quantity);
    int cartQuantityLength = strlen(cartQuantityStr);
    float cartQuantityWidth = cartQuantityLength * 0.15f;
    float cartQuantityY = centerY - 0.15f;

    glRasterPos2f(centerX - (cartQuantityWidth / 2.0f), cartQuantityY);
    for (int i = 0; cartQuantityStr[i] != '\0'; ++i) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, cartQuantityStr[i]);
    }
}
void updateTeam(struct Team *team, float startX, float y1, float y2) {
    float width = (22.25 - (team->numItems * 0.5)) / (team->numItems);
        int i = 0;
        for (int k = 0; k < items_number; k++) {
            if (shm2[k].section == team->team_id) {
                team->items[i].x1 = startX;
                team->items[i].y1 = y1;
                team->items[i].x2 = startX + width;
                team->items[i].y2 = y2;
                startX += width + 0.5;
                strcpy(team->items[i].item_name, shm2[k].name);
                team->items[i].item_id = shm2[k].id;
                team->items[i].item_quantity = shm2[k].quantity;
                i++;
            }
        }

    float cartX1 = startX;
    float cartX2 = cartX1 + 2.0;
    team->team_cart.x1 = cartX1;
    team->team_cart.y1 = y1;
    team->team_cart.x2 = cartX2;
    team->team_cart.y2 = y2;
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glutPostRedisplay();
}

bool isFullScreen = true;

void keyboard(unsigned char key, int x, int y) {
    if (key == 'q' || key == 'Q') {
        if (isFullScreen) {
            glutReshapeWindow(800, 600); // Set the window size to a non-fullscreen size
            glutPositionWindow(100, 100); // Set the window position if needed
            isFullScreen = false;
        } else {
            glutFullScreen(); // Re-enter fullscreen mode
            isFullScreen = true;
        }
    }
}

void update(){
    
   for (int k = 0; k < items_number; k++){
      switch (shm2[k].section){
        case 0: 
           for(int h = 0; h < team1_items; h++){
              if(teamBlue.items[h].item_id == shm2[k].id){
                 if(shm2[k].quantity != teamBlue.items[h].item_quantity){
                     teamBlue.items[h].item_quantity = shm2[k].quantity;
                 }
              }
           }
           break;
           
        case 1: 
           for(int h = 0; h < team2_items; h++){
              if(teamGreen.items[h].item_id == shm2[k].id){
                 if(shm2[k].quantity != teamGreen.items[h].item_quantity){
                     teamGreen.items[h].item_quantity = shm2[k].quantity;
                 }
              }
           }
           break;
           
        case 2: 
           for(int h = 0; h < team3_items; h++){
              if(teamPurple.items[h].item_id == shm2[k].id){
                 if(shm2[k].quantity != teamPurple.items[h].item_quantity){
                     teamPurple.items[h].item_quantity = shm2[k].quantity;
                 }
              }
           }
           break;
           
        case 3: 
           for(int h = 0; h < team4_items; h++){
              if(teamYellow.items[h].item_id == shm2[k].id){
                 if(shm2[k].quantity != teamYellow.items[h].item_quantity){
                     teamYellow.items[h].item_quantity = shm2[k].quantity;
                 }
              }
           }
           break;
           
        case 4: 
           for(int h = 0; h < team5_items; h++){
              if(teamRed.items[h].item_id == shm2[k].id){
                 if(shm2[k].quantity != teamRed.items[h].item_quantity){
                     teamRed.items[h].item_quantity = shm2[k].quantity;
                 }
              }
           }
           break;
           
        default:
           break;
      }
   }
   
    char buffer[40];  // Adjust the buffer size as needed
    ssize_t bytesRead = read(pip[0], buffer, sizeof(buffer));
    int section, id, quantity;
    if (bytesRead == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
        // No data available to read from the pipe
        // Handle this case as needed
    } else {
        perror("Read failed");
        exit(EXIT_FAILURE);
    }
    } else if (bytesRead > 0) {
     sscanf(buffer, "%d %d %d", &section, &id, &quantity);
     if (section == -1){
        customer_count++;
     }
     else if (section == -2){
        customer_count--;
     }
     else{
     char item_name [30];
     for(int k = 0; k < items_number; k++){
        if (shm2[k].id == id){
           strcpy(item_name, shm2[k].name);
        }
     }
     switch (section){
      case 0:
         teamBlue.team_cart.quantity = quantity;
         strcpy(teamBlue.team_cart.label, item_name);
         break;
      case 1:
         teamGreen.team_cart.quantity = quantity;
         strcpy(teamGreen.team_cart.label, item_name);
         break;
      case 2:
         teamPurple.team_cart.quantity = quantity;
         strcpy(teamPurple.team_cart.label, item_name);
         break;
      case 3:
         teamYellow.team_cart.quantity = quantity;
         strcpy(teamYellow.team_cart.label, item_name);
         break;
      case 4:
         teamRed.team_cart.quantity = quantity;
         strcpy(teamRed.team_cart.label, item_name);
         break;
     }
    }
   }
   char buffer2[100];  // Adjust the buffer size as needed
    ssize_t bytesRead2 = read(text_pip[0], buffer2, sizeof(buffer2));
    if (bytesRead == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
        // No data available to read from the pipe
        // Handle this case as needed
    } else {
        perror("Read failed");
        exit(EXIT_FAILURE);
    }
    } else if (bytesRead > 0 && check_message(buffer2) != 0) {
        strcpy(messageRectangle.msg_content, buffer2);
    }
   
    int current_time = glutGet(GLUT_ELAPSED_TIME);

    int end_time = TIME_TO_END * 60;

    if (start_time == 0) {
       start_time = current_time + (end_time * 1000); // Start from end time
    }

    
    // Calculate the remaining time in seconds
    timeSquare.time = ((start_time - current_time) / 1000.0);
    if (timeSquare.time < 0.0f){
      timeSquare.time = 0;
    }
   
    for (int k = 0; k < team1_items; k++){
       for (int j = 0; j < items_number; j++){
           if (shm2[j].id == teamBlue.items[k].item_id){
              if (shm2[j].ami == shm2[j].quantity){
                     teamBlue.items[k].color.r = 0.38;
                     teamBlue.items[k].color.g = 1.0;
                     teamBlue.items[k].color.b = 0.25;
              }
              else if (shm2[j].ami > shm2[j].quantity && shm2[j].quantity > (shm2[j].ami * LOW_AMI_RATE)){
                     teamBlue.items[k].color.r = 0.08;
                     teamBlue.items[k].color.g = 0.74;
                     teamBlue.items[k].color.b = 1.0;
              }
              else if (0 < shm2[j].quantity && shm2[j].quantity < (shm2[j].ami * LOW_AMI_RATE)){
                     teamBlue.items[k].color.r = 1.0;
                     teamBlue.items[k].color.g = 0.48;
                     teamBlue.items[k].color.b = 0.09;
              }
              else if (shm2[j].quantity == 0){
                     teamBlue.items[k].color.r = 1.0;
                     teamBlue.items[k].color.g = 0.0;
                     teamBlue.items[k].color.b = 0.0;               
              }
           }
       }
    }
    for (int k = 0; k < team2_items; k++){
       for (int j = 0; j < items_number; j++){
           if (shm2[j].id == teamGreen.items[k].item_id){
              if (shm2[j].ami == shm2[j].quantity){
                     teamGreen.items[k].color.r = 0.38;
                     teamGreen.items[k].color.g = 1.0;
                     teamGreen.items[k].color.b = 0.25;
              }
              else if (shm2[j].ami > shm2[j].quantity && shm2[j].quantity > (shm2[j].ami * LOW_AMI_RATE)){
                     teamGreen.items[k].color.r = 0.08;
                     teamGreen.items[k].color.g = 0.74;
                     teamGreen.items[k].color.b = 1.0;
              }
              else if (0 < shm2[j].quantity && shm2[j].quantity < (shm2[j].ami * LOW_AMI_RATE)){
                     teamGreen.items[k].color.r = 1.0;
                     teamGreen.items[k].color.g = 0.48;
                     teamGreen.items[k].color.b = 0.09;
              }
              else if (shm2[j].quantity == 0){
                     teamGreen.items[k].color.r = 1.0;
                     teamGreen.items[k].color.g = 0.0;
                     teamGreen.items[k].color.b = 0.0;               
              }
           }
       }
    }
    for (int k = 0; k < team3_items; k++){
       for (int j = 0; j < items_number; j++){
           if (shm2[j].id == teamPurple.items[k].item_id){
              if (shm2[j].ami == shm2[j].quantity){
                     teamPurple.items[k].color.r = 0.38;
                     teamPurple.items[k].color.g = 1.0;
                     teamPurple.items[k].color.b = 0.25;
              }
              else if (shm2[j].ami > shm2[j].quantity && shm2[j].quantity > (shm2[j].ami * LOW_AMI_RATE)){
                     teamPurple.items[k].color.r = 0.08;
                     teamPurple.items[k].color.g = 0.74;
                     teamPurple.items[k].color.b = 1.0;
              }
              else if (0 < shm2[j].quantity && shm2[j].quantity < (shm2[j].ami * LOW_AMI_RATE)){
                     teamPurple.items[k].color.r = 1.0;
                     teamPurple.items[k].color.g = 0.48;
                     teamPurple.items[k].color.b = 0.09;
              }
              else if (shm2[j].quantity == 0){
                     teamPurple.items[k].color.r = 1.0;
                     teamPurple.items[k].color.g = 0.0;
                     teamPurple.items[k].color.b = 0.0;               
              }
           }
       }
    }
    for (int k = 0; k < team4_items; k++){
       for (int j = 0; j < items_number; j++){
           if (shm2[j].id == teamYellow.items[k].item_id){
              if (shm2[j].ami == shm2[j].quantity){
                     teamYellow.items[k].color.r = 0.38;
                     teamYellow.items[k].color.g = 1.0;
                     teamYellow.items[k].color.b = 0.25;
              }
              else if (shm2[j].ami > shm2[j].quantity && shm2[j].quantity > (shm2[j].ami * LOW_AMI_RATE)){
                     teamYellow.items[k].color.r = 0.08;
                     teamYellow.items[k].color.g = 0.74;
                     teamYellow.items[k].color.b = 1.0;
              }
              else if (0 < shm2[j].quantity && shm2[j].quantity < (shm2[j].ami * LOW_AMI_RATE)){
                     teamYellow.items[k].color.r = 1.0;
                     teamYellow.items[k].color.g = 0.48;
                     teamYellow.items[k].color.b = 0.09;
              }
              else if (shm2[j].quantity == 0){
                     teamYellow.items[k].color.r = 1.0;
                     teamYellow.items[k].color.g = 0.0;
                     teamYellow.items[k].color.b = 0.0;               
              }
           }
       }
    }
    for (int k = 0; k < team5_items; k++){
       for (int j = 0; j < items_number; j++){
           if (shm2[j].id == teamRed.items[k].item_id){
              if (shm2[j].ami == shm2[j].quantity){
                     teamRed.items[k].color.r = 0.38;
                     teamRed.items[k].color.g = 1.0;
                     teamRed.items[k].color.b = 0.25;
              }
              else if (shm2[j].ami > shm2[j].quantity && shm2[j].quantity > (shm2[j].ami * LOW_AMI_RATE)){
                     teamRed.items[k].color.r = 0.08;
                     teamRed.items[k].color.g = 0.74;
                     teamRed.items[k].color.b = 1.0;
              }
              else if (0 < shm2[j].quantity && shm2[j].quantity < (shm2[j].ami * LOW_AMI_RATE)){
                     teamRed.items[k].color.r = 1.0;
                     teamRed.items[k].color.g = 0.48;
                     teamRed.items[k].color.b = 0.09;
              }
              else if (shm2[j].quantity == 0){
                     teamRed.items[k].color.r = 1.0;
                     teamRed.items[k].color.g = 0.0;
                     teamRed.items[k].color.b = 0.0;               
              }
           }
       }
    }
    if (teamBlue.team_cart.quantity == 0){
         teamBlue.team_cart.color.r = 1.0;
         teamBlue.team_cart.color.g = 0.0;
         teamBlue.team_cart.color.b = 0.0;
    }
    else{
         teamBlue.team_cart.color.r = 1.0;
         teamBlue.team_cart.color.g = 1.0;
         teamBlue.team_cart.color.b = 0.03;
    }
    if (teamPurple.team_cart.quantity == 0){
         teamPurple.team_cart.color.r = 1.0;
         teamPurple.team_cart.color.g = 0.0;
         teamPurple.team_cart.color.b = 0.0;
    }
    else{
         teamPurple.team_cart.color.r = 1.0;
         teamPurple.team_cart.color.g = 1.0;
         teamPurple.team_cart.color.b = 0.03;
    }
    if (teamYellow.team_cart.quantity == 0){
         teamYellow.team_cart.color.r = 1.0;
         teamYellow.team_cart.color.g = 0.0;
         teamYellow.team_cart.color.b = 0.0;
    }
    else{
         teamYellow.team_cart.color.r = 1.0;
         teamYellow.team_cart.color.g = 1.0;
         teamYellow.team_cart.color.b = 0.03;
    }
    if (teamGreen.team_cart.quantity == 0){
         teamGreen.team_cart.color.r = 1.0;
         teamGreen.team_cart.color.g = 0.0;
         teamGreen.team_cart.color.b = 0.0;
    }
    else{
         teamGreen.team_cart.color.r = 1.0;
         teamGreen.team_cart.color.g = 1.0;
         teamGreen.team_cart.color.b = 0.03;
    }
    if (teamRed.team_cart.quantity == 0){
         teamRed.team_cart.color.r = 1.0;
         teamRed.team_cart.color.g = 0.0;
         teamRed.team_cart.color.b = 0.0;
    }
    else{
         teamRed.team_cart.color.r = 1.0;
         teamRed.team_cart.color.g = 1.0;
         teamRed.team_cart.color.b = 0.03;
    }
   if (customer_count == 0){ 
    customerSquare.color.r = 1.0;
    customerSquare.color.g = 0.0;
    customerSquare.color.b = 0.0;
   }
   else{
    customerSquare.color.r = 0.2;
    customerSquare.color.g = 0.1;
    customerSquare.color.b = 1.0;
   }
   
   if(timeSquare.time <= 10){
    timeSquare.color.r = 1.0;
    timeSquare.color.g = 0.0;
    timeSquare.color.b = 0.0;
   }
   else{
    timeSquare.color.r = 0.2;
    timeSquare.color.g = 0.1;
    timeSquare.color.b = 1.0;
   }
    
    glutPostRedisplay();
}


//function for reading a file
char ** readFile( char *fileName, size_t *linesCounter)
{
    //open items file for reading
    FILE * myFile = fopen(fileName, "r");
    if (myFile == NULL)
    {
        perror("Error");
        exit(1);
    }

    char buffer[100]; //buffer to store each file temorarily
    *linesCounter = 0;
    char **lines = NULL; //pointer to an array of strings

    while (fgets(buffer, sizeof(buffer), myFile) != NULL)
    {
        buffer[strcspn(buffer, "\n")] = '\0'; //replace the new line with null
        char *line = malloc(strlen(buffer)+1);
        strcpy(line, buffer);
        lines = realloc(lines, (*linesCounter + 1) * sizeof(char *)); //resize the array of lines
        if (lines == NULL)
        {
            perror("memory allocation error...");
            exit(1);
        }
        lines[(*linesCounter) ++] = line; //add the line to the array
    }
    fclose(myFile); //close the file
    return lines;
}

struct ThreadArgs {
    int *cart;
    struct Item *shm2;
    sem_t *sem2;
    sem_t *sem3;
    sem_t *sem4;
    int *pip;
    int *text_pip;
    int items_number;
    int section;
};

void *employees_function(void *args) {
    struct ThreadArgs *employeeArgs = (struct ThreadArgs *)args;
    while(1){
    while(employeeArgs->cart[1]>0){
        sem_wait(employeeArgs->sem3);
        if(employeeArgs->cart[1] == 0){
            sem_post(employeeArgs->sem3);
            break;
        }
        else{
            sem_wait(employeeArgs->sem2);
            usleep(PICKING_TIME_CONSTANT);
            employeeArgs->cart[1] --;
            sem_wait(employeeArgs->sem4);
            char message[40];
            sprintf(message, "%d %d %d", employeeArgs->section, employeeArgs->cart[0], employeeArgs->cart[1]);
            write(employeeArgs->pip[1], message, sizeof(message));
            sem_post(employeeArgs->sem4);
            for(int counter=0;counter<employeeArgs->items_number;counter++){
                if(employeeArgs->shm2[counter].id == employeeArgs->cart[0]){
                    employeeArgs->shm2[counter].quantity++;
                }
            }
            sem_post(employeeArgs->sem3);
            sem_post(employeeArgs->sem2);
        }
        printf("cart has %d quantity &id %d\n",employeeArgs->cart[1], employeeArgs->cart[0]);
    }
    }
}







int main(int argc, char **argv)
{
    
    //program launch
    //here is parent code
    //save all children pids
    int children=0;
    int childrenPids[1000];

    char **items = readFile("items.txt", &items_number);

    //creat shared memory for storage
    key_t shmkey = ftok("shm.txt", 65); //create unique key
    int shmid = shmget(shmkey, items_number * sizeof(struct Item), IPC_CREAT | 0666); //return identifer
    if (shmid == -1)
    {
        perror("Allocation 1 faild...\n");
        exit(1);
    }
    struct Item *shm = (struct Item *) shmat(shmid, NULL, 0); //attach to the shm


    //creat shared memory for shelves
    key_t shmkey2 = ftok("shm2.txt", 65);
    int shmid2 = shmget(shmkey2, items_number * sizeof(struct Item), IPC_CREAT | 0666); //return identifer
    if (shmid2 == -1)
    {
        perror("Allocation 1 faild...\n");
        exit(1);
    }
    shm2 = (struct Item *) shmat(shmid2, NULL, 0); //attach to the shm

    //copy the content of items into the shm
    for (int i = 0; i<items_number; i++)
    {
        sscanf(items[i], "%[^,],%lf,%d,%d,%d,%d", shm[i].name, &shm[i].price, &shm[i].quantity, &shm[i].section, &shm[i].id, &shm[i].ami);
    }


    
    // fill shm2 with products with ami for each product as quantity
    for(int i=0;i < items_number; i++){
        strcpy(shm2[i].name, shm[i].name);
        shm2[i].price=shm[i].price;
        shm2[i].section=shm[i].section;
        shm2[i].id=shm[i].id;
        shm2[i].ami=shm[i].ami;
        if(shm[i].quantity>shm[i].ami){
            shm2[i].quantity=shm[i].ami;
            shm[i].quantity-=shm[i].ami;
        }
        else{
            shm2[i].quantity=shm[i].quantity;
            shm[i].quantity=0;
        }
    }


    //create semaphores for storage, shelves, cart, pip
    sem_t sem;
    sem_init(&sem, 0, 1);

    sem_t sem2;
    sem_init(&sem2, 0, 1);

    sem_t sem3;
    sem_init(&sem3, 0, 1);
    
    sem_t sem4;
    sem_init(&sem4, 0, 1);

    if (pipe(pip) == -1 || pipe(text_pip) == -1)
    {
        perror("PIPE");
        exit(1);
    }
    if (fcntl(pip[0], F_SETFL, O_NONBLOCK) == -1)
    {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }
    if (fcntl(text_pip[0], F_SETFL, O_NONBLOCK) == -1)
    {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }
    
    //free allocated memory
    for (int i = 0; i<items_number; i++)
    {
        free(items[i]);
    }
    free(items);

    
    printf("Openning :>\n\n");
    int pid;
    //opengl process
    if((pid=fork()) == -1)
    {
        printf("Fork failure... Exiting\n");
        exit(-1);
    }
    if(pid == 0)
    {
    customer_count = 0;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Supermarket");

    glClearColor(0.0, 0.0, 0.0, 0.0); // Black background
    glMatrixMode(GL_PROJECTION);      // Setup viewing projection
    glLoadIdentity();                 // Start with identity matrix

    float aspectRatio = (float)glutGet(GLUT_SCREEN_WIDTH) / (float)glutGet(GLUT_SCREEN_HEIGHT);
    glOrtho(-10.0 * aspectRatio, 10.0 * aspectRatio, -10.0, 10.0, -1.0, 1.0);
    team1_items = 0;
    team2_items = 0;
    team3_items = 0;
    team4_items = 0;
    team5_items = 0;
    
    for (int k=0; k < items_number; k++){
       if (shm2[k].section == 0)
          team1_items++;
       else if (shm2[k].section == 1)
          team2_items++;
       else if (shm2[k].section == 2)
          team3_items++;
       else if (shm2[k].section == 3)
          team4_items++;
       else if (shm2[k].section == 4)
          team5_items++;
    }

    glutDisplayFunc(display);
    glutReshapeFunc(reshape); // Set the reshape callback function
    glutKeyboardFunc(keyboard); // Set the keyboard function for key press event

    glutIdleFunc(update);
    glutFullScreen(); // Set the window to full screen

    glutMainLoop();
    return 0;
    }
    char msg [1000];
    strcpy(msg, "Open. Welcome to Our Store :)");
    write(text_pip[1], msg, sizeof(msg));
    childrenPids[children]=pid;
    children++;
    // Creating teams
    for (int i = 0; i < NUMBER_OF_TEAMS; i++) 
    {
        
        if ((pid = fork()) == -1)
        {
            printf("Fork failure... Exiting\n");
            exit(-1);
        }
        if (pid == 0)
        {
            //hala and luai code
            int cart[2];
            cart[0] = 0;
            cart[1] = 0;
            struct ThreadArgs threadArgs;
            threadArgs.cart=cart;
            threadArgs.shm2=shm2;
            threadArgs.sem2=&sem2;
            threadArgs.sem3=&sem3;
            threadArgs.sem4=&sem4;
            threadArgs.pip=pip;
            threadArgs.text_pip=text_pip;
            threadArgs.items_number=items_number;
            threadArgs.section = i;

            // Create a thread and pass the array as an argument
            for(int counter=0; counter<NUMBER_OF_EMPLOYEES; counter++){
                pthread_t employee;
                if (pthread_create(&employee, NULL, employees_function, (void *)&threadArgs) != 0) {
                    printf("Error creating thread\n");
                    exit(EXIT_FAILURE);
                }
            }
            

            while (1) { // Infinite loop for continuous checking
                for (int j = 0; j < items_number ; j++) {
                    if (shm2[j].section == i) {
                        if (shm2[j].quantity < (shm2[j].ami * LOW_AMI_RATE) && shm[j].quantity>0)  {
                            while(1){
                                if(cart[1] == 0)
                                break;
                            }
                            sem_wait(&sem3);
                            cart[0] = shm2[j].id;
                            printf("manager of team %d is bringing %s\n", i, shm2[j].name);
                            int maxQInCart = shm2[j].ami - shm2[j].quantity; //maximum quantity in cart
                            if (maxQInCart>0){
                                sprintf(msg, "Manager %d is filling cart with %d units of %s", i, maxQInCart, shm2[j].name);
                                write(text_pip[1], msg, sizeof(msg));
                            }
                            int flag = 0;
                            while (shm[j].quantity > 0 && flag < maxQInCart) {
                                sem_wait(&sem);
                                shm[j].quantity--;
                                sem_post(&sem);
                                cart[1] ++;
                                flag ++;
                                sem_wait(&sem4);
                                char message[40];
                                sprintf(message, "%d %d %d", i , cart[0] ,cart[1]);
                                write(pip[1], message, sizeof(message));
                                sem_post(&sem4);
                                usleep(PICKING_TIME_CONSTANT);
                            }
                            sem_post(&sem3);
                            printf("manager of team %d has filled cart with %d pieces of %s\n", i,cart[1], shm2[j].name);

                        }
                    }
                }

            }



        }
        //save pids for teams
        childrenPids[children]=pid;
        children++;
    }
    
    //seed rand function
    srand(time(NULL));
    time_t timeS,timeE;
    time_t time_of_openning, current_time;
    time(&timeS);
    time(&time_of_openning);
    int c=0;

    //setup finished let us open doors
    while(1)
    {
        //new user-defined number of customers every user-defined time
        int newCustomersNum=rand()%(CUSTOMER_ARRIVE_UPPER-CUSTOMER_ARRIVE_LOWER +1) +CUSTOMER_ARRIVE_LOWER; //here we need to define random number of customers after each amount of time
        time(&timeE);
        double timeD= difftime(timeE, timeS);
        if(timeD>=TIME_CONSTANT || c==0)
        {
            time(&timeS);
            c=1;

            for(int counter=0; counter< newCustomersNum; counter++)
            {
                if ((pid = fork()) == -1)
                {
                    printf("fork failure ... getting out\n");
                    fflush(stdout);
                    exit (2);
                    break;
                }

                if(pid == 0)
                {
                    //here is customer code
                    sem_wait(&sem3);
                    char msg2[40];
                    strcpy (msg2, "-1 0 0");
                    write(pip[1], msg2, sizeof(msg2));
                    sem_post(&sem3);
                    printf("customer %d enters supermarket\n", getpid());

                    //shopping time
                    srand(time(NULL) + counter);
                    int sleepingTime = rand() % (SHOPPING_UPPER_TIME - SHOPPING_LOWER_TIME + 1) +SHOPPING_LOWER_TIME;
                    sleep (sleepingTime);
                    //Done with shopping

                    //items to buy
                    int numItemsToBuy= (rand() % ITEMS_RANGE_NUMBER)+1;
                    Item inCartItems[numItemsToBuy];
                    int itemsIndecies[numItemsToBuy];


                    //semaphore to items shared memory to avoid conflicts
                    sem_wait(&sem2);

                    //getting items in cart
                    for(int counter=0; counter<numItemsToBuy; counter++)
                    {
                        int itemNumber= rand() % items_number;
                        int quantity= (rand() % ITEMS_QUANTITY_RANGE) +1;
                        if(quantity > shm2[itemNumber].quantity)
                        {
                            if(shm2[itemNumber].quantity == 0)     //if quantity for this item is zero move on
                                continue;
                            quantity= shm2[itemNumber].quantity;
                            shm2[itemNumber].quantity-= quantity;  //update quantity for this item in shared memory
                        }
                        else{
                            shm2[itemNumber].quantity-=quantity;
                            printf("customer %d picked %d of item: %s\n",getpid(), quantity, shm2[itemNumber].name);
                        }
                    }
                    //release semaphore
                    sem_post(&sem2);
                    sem_wait(&sem3);
                    char msg[40];
                    strcpy (msg, "-2 0 0");
                    write(pip[1], msg, sizeof(msg));
                    sem_post(&sem3);
                    exit(EXIT_SUCCESS);
                }

                //parent code
                //save pids for customers
                childrenPids[children]=pid;
                children++;

            }
        }
        int out_of_stock=0;
        for(int j=0; j<items_number;j++){
            if(shm[j].quantity > 0)
                out_of_stock=1;
        }
        time(&current_time);
        double passed_time= (difftime(current_time, time_of_openning))/60;
        if(passed_time >= TIME_TO_END || out_of_stock == 0){
            //close ipcs
            //destroy semaphores
            sem_destroy(&sem);
            sem_destroy(&sem2);
            sem_destroy(&sem3);
            sem_destroy(&sem4);

            //close pipes
            close(pip[0]);
            close(pip[1]);

            //close shared memories
            shmdt(shm); //detach from shm
            shmctl(shmid, IPC_RMID, NULL); //remove the shared memory

            shmdt(shm2);
            shmctl(shmid2, IPC_RMID, NULL);

            //killing processes
            for(int counter=1; counter<children; counter++)
            {
                kill(childrenPids[counter], 9);

            }
            if (out_of_stock == 0){
               strcpy(msg, "Storage out of stock, we are closing :(");
               write(text_pip[1], msg, sizeof(msg));
            }
            else {
               strcpy(msg, "Time has ended, we are closing :(");
               write(text_pip[1], msg, sizeof(msg));
            }
            close(text_pip[0]);
            close(text_pip[1]);

            sleep(5);
            kill(childrenPids[0], 9);
            //exit the program
            exit(EXIT_SUCCESS);
        }
    }
    
    return 0;
}
