#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUSES 3
#define MAX_SEATS 50
#define MAX_PASSENGERS 50
#define NAME_LEN 50

typedef struct {
    char route[50];
    int seats[MAX_SEATS];
} Bus;

typedef struct Passenger {
    char name[NAME_LEN];
    int busNo;
    int seatNo;
    struct Passenger *next;
} Passenger;

Passenger *head = NULL;

typedef struct {
    char names[MAX_PASSENGERS][NAME_LEN];
    int front, rear;
} Queue;

Queue bookingQueue = {.front = -1, .rear = -1};


typedef struct {
    char name[NAME_LEN];
    int busNo;
    int seatNo;
} Booking;

Booking undoStack[MAX_PASSENGERS];
int top = -1;


void initBuses(Bus buses[]);
void viewBuses(Bus buses[]);
void enqueue(char name[]);
void dequeue();
int isQueueEmpty();
void pushUndo(char name[], int busNo, int seatNo);
Booking popUndo();
void addPassenger(char name[], int busNo, int seatNo);
void removePassenger(char name[], int busNo, int seatNo);
void showPassengers();
void bookTicket(Bus buses[]);
void cancelTicket(Bus buses[]);
void undoBooking(Bus buses[]);


int main() {
    Bus buses[MAX_BUSES];
    int choice;

    initBuses(buses);

    while (1) {
        printf("\n=== Smart Bus Ticket Booking System ===\n");
        printf("1. View Buses & Available Seats\n");
        printf("2. Book Ticket\n");
        printf("3. Cancel Ticket\n");
        printf("4. Undo Last Booking\n");
        printf("5. Show Passenger List\n");
        printf("6. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: viewBuses(buses); break;
            case 2: bookTicket(buses); break;
            case 3: cancelTicket(buses); break;
            case 4: undoBooking(buses); break;
            case 5: showPassengers(); break;
            case 6: exit(0);
            default: printf("Invalid choice!\n");
        }
    }
    return 0;
}

void initBuses(Bus buses[]) {
    strcpy(buses[0].route, "Dhaka → Chittagong");
    strcpy(buses[1].route, "Dhaka → Sylhet");
    strcpy(buses[2].route, "Dhaka → Khulna");

    for (int i = 0; i < MAX_BUSES; i++) {
        for (int j = 0; j < MAX_SEATS; j++) {
            buses[i].seats[j] = 0;
        }
    }
}

void viewBuses(Bus buses[]) {
    printf("\nAvailable Buses:\n");
    for (int i = 0; i < MAX_BUSES; i++) {
        printf("Bus %d: %s\nSeats: ", i + 1, buses[i].route);
        for (int j = 0; j < MAX_SEATS; j++) {
            printf("%d[%s] ", j + 1, buses[i].seats[j] ? "Booked" : "Free");
            if ((j+1) % 10 == 0) printf("\n       "); // line break after every 10 seats
        }
        printf("\n");
    }
}


void enqueue(char name[]) {
    if (bookingQueue.rear == MAX_PASSENGERS - 1) {
        printf("Booking queue full!\n");
        return;
    }
    if (bookingQueue.front == -1) bookingQueue.front = 0;
    bookingQueue.rear++;
    strcpy(bookingQueue.names[bookingQueue.rear], name);
}

void dequeue() {
    if (isQueueEmpty()) return;
    bookingQueue.front++;
    if (bookingQueue.front > bookingQueue.rear) {
        bookingQueue.front = bookingQueue.rear = -1;
    }
}

int isQueueEmpty() {
    return bookingQueue.front == -1;
}

void pushUndo(char name[], int busNo, int seatNo) {
    if (top < MAX_PASSENGERS - 1) {
        top++;
        strcpy(undoStack[top].name, name);
        undoStack[top].busNo = busNo;
        undoStack[top].seatNo = seatNo;
    }
}

Booking popUndo() {
    Booking b = {"", -1, -1};
    if (top == -1) {
        printf("No booking to undo!\n");
        return b;
    }
    b = undoStack[top];
    top--;
    return b;
}


void addPassenger(char name[], int busNo, int seatNo) {
    Passenger *newP = (Passenger*)malloc(sizeof(Passenger));
    strcpy(newP->name, name);
    newP->busNo = busNo;
    newP->seatNo = seatNo;
    newP->next = head;
    head = newP;
}

void removePassenger(char name[], int busNo, int seatNo) {
    Passenger *temp = head, *prev = NULL;
    while (temp != NULL) {
        if (strcmp(temp->name, name) == 0 && temp->busNo == busNo && temp->seatNo == seatNo) {
            if (prev == NULL) head = temp->next;
            else prev->next = temp->next;
            free(temp);
            return;
        }
        prev = temp;
        temp = temp->next;
    }
}

void showPassengers() {
    Passenger *temp = head;
    printf("\nPassenger List:\n");
    if (!temp) {
        printf("No passengers booked yet.\n");
        return;
    }
    while (temp) {
        printf("%s - Bus %d Seat %d\n", temp->name, temp->busNo + 1, temp->seatNo + 1);
        temp = temp->next;
    }
}


void bookTicket(Bus buses[]) {
    char name[NAME_LEN];
    int busNo, seatNo;

    printf("Enter your name: ");
    scanf(" %[^\n]", name);
    enqueue(name);

    printf("Enter bus number (1-%d): ", MAX_BUSES);
    scanf("%d", &busNo);
    busNo--;

    printf("Enter seat number (1-%d): ", MAX_SEATS);
    scanf("%d", &seatNo);
    seatNo--;

    if (busNo < 0 || busNo >= MAX_BUSES || seatNo < 0 || seatNo >= MAX_SEATS) {
        printf("Invalid bus or seat number!\n");
        dequeue();
        return;
    }

    if (buses[busNo].seats[seatNo] == 1) {
        printf("Seat already booked!\n");
        dequeue();
        return;
    }

    buses[busNo].seats[seatNo] = 1;
    addPassenger(name, busNo, seatNo);
    pushUndo(name, busNo, seatNo);

    printf("Ticket booked for %s on Bus %d Seat %d\n", name, busNo + 1, seatNo + 1);
    dequeue();
}

void cancelTicket(Bus buses[]) {
    char name[NAME_LEN];
    int busNo, seatNo;

    printf("Enter your name: ");
    scanf(" %[^\n]", name);
    printf("Enter bus number: ");
    scanf("%d", &busNo);
    busNo--;
    printf("Enter seat number: ");
    scanf("%d", &seatNo);
    seatNo--;

    if (busNo < 0 || busNo >= MAX_BUSES || seatNo < 0 || seatNo >= MAX_SEATS) {
        printf("Invalid bus or seat number!\n");
        return;
    }

    if (buses[busNo].seats[seatNo] == 0) {
        printf("No booking found!\n");
        return;
    }

    buses[busNo].seats[seatNo] = 0;
    removePassenger(name, busNo, seatNo);
    printf("Booking cancelled for %s on Bus %d Seat %d\n", name, busNo + 1, seatNo + 1);
}

void undoBooking(Bus buses[]) {
    Booking last = popUndo();
    if (last.busNo == -1) return;

    buses[last.busNo].seats[last.seatNo] = 0;
    removePassenger(last.name, last.busNo, last.seatNo);
    printf("Last booking undone for %s on Bus %d Seat %d\n", last.name, last.busNo + 1, last.seatNo + 1);
}
