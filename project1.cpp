#include <bits/stdc++.h>
#include <limits>  // for clearing input buffer
#include <conio.h> // for _getch() on Windows systems
using namespace std;


void logout2();
void logout();
class customer {
public:
    string cust_name;
    string cust_mail;
    string cust_passwd;
    string cust_phone_no;

    customer(string name, string mail, string passwd, string phone_no)
        : cust_name(name), cust_mail(mail), cust_passwd(passwd), cust_phone_no(phone_no) {}
};

// Structure for a train
struct Train {
    int trainID;
    string trainName;
    string departure;
    string arrival;
    string source;
    string destination;
    int totalseat;
    int availSeats;
};

class TrainSchedule {
public:
    struct TreeNode {
        Train train;
        TreeNode* left;
        TreeNode* right;

        TreeNode(Train t) : train(t), left(nullptr), right(nullptr) {}
    };

    TreeNode* root;

    TrainSchedule() : root(nullptr) {
        loadTrainData();  // Load train data at initialization
    }

    void insertTrain(Train t) {
        root = insert(root, t);
    }



    Train* findTrain(int trainID) {
        TreeNode* result = search(root, trainID);
        return result ? &result->train : nullptr;
    }

    void printAllTrains(TreeNode* node) {
    if (node == nullptr) return; // Base case

    // Traverse the left subtree
    printAllTrains(node->left);

    // Print current train details
    cout << "Train ID: " << node->train.trainID << endl;
    cout << "Train Name: " << node->train.trainName << endl;
    cout << "Departure: " << node->train.departure << endl;
    cout << "Arrival: " << node->train.arrival << endl;
    cout << "Source: " << node->train.source << endl;
    cout << "Destination: " << node->train.destination << endl;
    cout << "No of requests: " << node->train.availSeats << endl;
    cout << "Total Seats: " << node->train.totalseat << endl;

    cout << "------------------------------------" << endl;

    // Traverse the right subtree
    printAllTrains(node->right);
}

// Wrapper function to initiate printing from the root
void displayAllTrains() {
    printAllTrains(root); // Call the in-order traversal from the root
}


private:
    // Helper function to insert train into the BST
    TreeNode* insert(TreeNode* node, Train t) {
        if (node == nullptr) return new TreeNode(t);

        if (t.trainID < node->train.trainID)
            node->left = insert(node->left, t);
        else
            node->right = insert(node->right, t);

        return node;
    }

    // Helper function to find a train by ID
    TreeNode* search(TreeNode* node, int trainID) {
        if (node == nullptr || node->train.trainID == trainID)
            return node;

        if (trainID < node->train.trainID)
            return search(node->left, trainID);
        return search(node->right, trainID);
    }

    // Load train data from file into BST
    void loadTrainData() {
        ifstream file("train_data.txt");
        if (!file.is_open()) {
            cerr << "Error: Could not open train data file.\n";
            return;
        }

        Train t;
        while (file >> t.trainID >> t.trainName >> t.departure >> t.arrival
                    >> t.source >> t.destination >> t.totalseat>>t.availSeats) {
            insertTrain(t);  // Insert each train into the BST
        }

        file.close();
    }
};

struct Passenger {
    string name;
    int age;
    string gender;
    int trainID;
    int seatNumber;  // Seat assigned later
    Passenger* next; // Pointer to next passenger (for linked list)

    Passenger(string n, int a, string g, int tID, int sNum)
        : name(n), age(a), gender(g), trainID(tID), seatNumber(sNum), next(nullptr) {}
};

// Define the PassengerList class to handle linked list of passengers
class PassengerList {
private:
    Passenger* head; // Head of the linked list

public:
    // Constructor to initialize the head pointer
    PassengerList() : head(nullptr) {}

    // Add a passenger to the list in sorted order by seat number
    void addPassenger(string name, int age, string gender, int trainID, int seatNumber) {
        Passenger* newPassenger = new Passenger(name, age, gender, trainID, seatNumber);

        // If the list is empty or the new passenger's seat number is smaller than the head
        if (head == nullptr || head->seatNumber > seatNumber) {
            newPassenger->next = head;
            head = newPassenger;
            return;
        }

        // Otherwise, find the correct position in the linked list (sorted by seat number)
        Passenger* current = head;
        while (current->next != nullptr && current->next->seatNumber < seatNumber) {
            current = current->next;
        }

        // Insert the new passenger into the correct position
        newPassenger->next = current->next;
        current->next = newPassenger;
    }

    // Function to display the passenger list
    void displayPassengers() {
        Passenger* current = head;
        while (current != nullptr) {
            cout << "Name: " << current->name << ", Age: " << current->age
                 << ", Gender: " << current->gender << ", Seat No: " << current->seatNumber << endl;
            current = current->next;
        }
    }
};

class Waitlist {
public:
    queue<Passenger*> waitlistQueue;

    // Add passenger to the queue (waitlist)
    void addToWaitlist(Passenger* p) {
        waitlistQueue.push(p);
    }

    // Get the next passenger from the waitlist
    Passenger* getNextFromWaitlist() {
        if (!waitlistQueue.empty()) {
            Passenger* p = waitlistQueue.front();
            waitlistQueue.pop();
            return p;
        }
        return nullptr;
    }
    // Method to clear all passengers in the waitlist
    void clear() {
        while (!waitlistQueue.empty()) {
            delete waitlistQueue.front(); // Free the memory for each passenger
            waitlistQueue.pop();          // Remove each passenger from the queue
        }
    }

    // Display the waitlist
    void displayWaitlist() {
        if (waitlistQueue.empty()) {
            cout << "No passengers in waitlist.\n";
            return;
        }
        queue<Passenger*> tempQueue = waitlistQueue;
        while (!tempQueue.empty()) {
            Passenger* p = tempQueue.front();
            cout << "Name: " << p->name << ", Age: " << p->age
                 << ", Gender: " << p->gender << ", Waitlist No: " << p->seatNumber << endl;
            tempQueue.pop();
        }
    }
};

unordered_map<int, Waitlist> trainWaitlists;
// Define a hash map to store the passenger lists for each train
unordered_map<int, Passenger*> trainPassengerList;

// Function to add a passenger to the corresponding train’s list
// Function to add a passenger to the linked list for a specific train
void addPassengerToTrain(int trainID, const string& name, int age, const string& gender, int seatNumber) {
    // Retrieve the train details by trainID using findTrain
    TrainSchedule schedule;
    Train* train = schedule.findTrain(trainID);  // Fetch the train data for validation

    // If train not found, print error and exit function
    if (!train) {
        cout << "Train ID " << trainID << " not found!" << endl;
        return;
    }

    // Ensure the seat number is within the valid range or move to waitlist
    Passenger* newPassenger = new Passenger(name, age, gender, trainID, seatNumber);

    if (seatNumber > train->totalseat) {
        // Add to the waitlist if seat number is above the total seat capacity
        int waitlistNumber = seatNumber - train->totalseat;
        newPassenger->seatNumber = waitlistNumber;  // Update seat number to represent waitlist position
        trainWaitlists[trainID].addToWaitlist(newPassenger);
        cout << "Passenger added to waitlist with waitlist number: " << waitlistNumber << endl;
    } else {
        // Add to the main passenger list if seat number is within the total seat capacity
        if (trainPassengerList.find(trainID) == trainPassengerList.end()) {
            trainPassengerList[trainID] = newPassenger;  // Start new list for this train
        } else {
            Passenger* temp = trainPassengerList[trainID];
            while (temp->next != nullptr) {
                temp = temp->next;
            }
            temp->next = newPassenger;  // Add to end of the list
        }
        cout << "Passenger added to main passenger list with seat number: " << seatNumber << endl;
    }
}

// Queue for waitlisted passengers
void clearAllWaitlists() {
    for (auto& entry : trainWaitlists) {
        entry.second.clear();  // Ensure each train's waitlist is empty before loading
    }
}

// Function to load passenger data from file and populate hash map of linked lists
void loadPassengersFromFile(const string& filename) {
    ifstream infile(filename);
    string first_name;
    string last_name;
    string gender;
    int age;
    int seatno;
    int trainID;

    if (!infile.is_open()) {
        cerr << "Error opening file!" << endl;
        return;
    }
// Clear current passenger data for each train before loading new data
    for (auto& entry : trainPassengerList) {
        Passenger* current = entry.second;
        while (current != nullptr) {
            Passenger* next = current->next;
            delete current;
            current = next;
        }
    }
    trainPassengerList.clear();
        clearAllWaitlists();

    // Read each line and process passenger details
    while (infile >> first_name >> last_name >> age >> gender >> trainID>>seatno) {

        string name = first_name + " " + last_name;
        // Add passenger to the respective linked list based on trainID
        addPassengerToTrain(trainID, name, age, gender, seatno);
    }

    infile.close();
}

// Function to display passengers of a particular train
void displayPassengers(int trainID) {
     if (trainPassengerList.find(trainID) == trainPassengerList.end() || trainPassengerList[trainID] == nullptr) {
        cout << "No passengers found for Train ID " << trainID << "." << endl;
        return;
    }

    Passenger* temp = trainPassengerList[trainID];
    cout << "Passengers in train " << trainID << ":" << endl;
    while (temp != nullptr) {
        cout << "Name: " << temp->name << ", Age: " << temp->age << ", Gender: " << temp->gender << ", Seat: " << temp->seatNumber << endl;
        temp = temp->next;
    }

    cout << "\nLogout or Go Back\n";
    cout << "1. Logout\n";
    cout << "2. Go Back\n";
    cout << "Please choose an option: ";
    int choice;
    cin >> choice;

    switch (choice) {
        case 1:
            logout();
            break;
        case 2:
        logout2();
            break;
        default:
            cout << "Invalid choice, please try again.\n";
            break;
    }
}


void displayTrainWaitlist(int trainID) {
    if (trainWaitlists.find(trainID) != trainWaitlists.end()) {
        cout << "Waitlist for Train ID " << trainID << ":\n";
        trainWaitlists[trainID].displayWaitlist();
    } else {
        cout << "No waitlist found for Train ID " << trainID << endl;
    }

         cout << "\nLogout or Go Back\n";
    cout << "1. Logout\n";
    cout << "2. Go Back\n";
    cout << "Please choose an option: ";
    int choice;
    cin >> choice;

    switch (choice) {
        case 1:
            logout();
            break;
        case 2:
        logout2();
            break;
        default:
            cout << "Invalid choice, please try again.\n";
            break;
    }
};

void signin_cust();
void displaySecondpage();
void logout();
void mainDashboard();
void bookTicket(TrainSchedule& schedule);
bool verifyTrainID(int,TrainSchedule &schedule);
void trainDetails(TrainSchedule &schedule);
void allocateseat(int train_id,int current_seat);
void yellow() { cout << "\033[0;33m"; }
void green() { cout << "\033[0;32m"; }
void red() { cout << "\033[0;31m"; }
void reset() { cout << "\033[0m"; }
string pnum, pswd;
void signin() {
    system("cls");
    cout << endl;

    cout << "Enter your ID (Phone number) to Sign In: ";
    yellow();
    cin >> pnum;
    reset();
    cout << "Enter your password: ";
    yellow();
    cin >> pswd;
    reset();

    ifstream myfile("cust_file.txt");
    string first_name, last_name, email, phone_no, password;
    bool found = false;

    while (myfile >> first_name >> last_name >> email >> phone_no >> password) {
        if ((phone_no == pnum) && (password == pswd)) {
            green();
            cout << "Welcome " << first_name << " " << last_name << " to the online ticket reservation system.\n";
            reset();
            found = true;
            mainDashboard();
            break;
        }
    }
    myfile.close();

    if (!found) {
        red();
        cout << "Invalid credentials. Please try again.\n";
        reset();
        signin_cust();
    }
}

void signup() {
    system("cls");
    cout << endl;
    string cust_name, cust_mail, cust_passwd, cust_phone_no;

    cout << "Hi! Welcome to the Online Ticket Reservation System :)\nEnter your full name: ";
    yellow();
    cin.ignore();
    getline(cin, cust_name);
    reset();
    cout << "Enter your Email: ";
    yellow();
    cin >> cust_mail;
    reset();
    cout << "Enter your Phone Number: ";
    yellow();
    cin >> cust_phone_no;
    reset();
    cout << "Enter Password: ";
    yellow();
    cin >> cust_passwd;
    reset();

    ofstream myfile("cust_file.txt", ios::app);
    myfile << cust_name << " " << cust_mail << " " << cust_phone_no << " " << cust_passwd << "\n";
    myfile.close();

    green();
    cout << "Signup successful! Please sign in to continue.\n";
    reset();
}

void signin_cust() {
    char choice;
    cout << "Incorrect phone number or password. Try again? (Y/N): ";
    cin >> choice;

    if (choice == 'Y' || choice == 'y') {
        signin();
    } else {
        displaySecondpage();
    }
}

void login_customer() {
    int signInChoice;
    while (true) {
        green();
        cout << "\n1. Customer Sign In\n2. Customer Sign Up (New Customer)\n3. Go back\nChoice (1/2/3): ";
        reset();
        cin >> signInChoice;

        if (signInChoice == 1) {
            signin();
            break;
        } else if (signInChoice == 2) {
            signup();
            signin();
            break;
        } else if (signInChoice == 3) {
            displaySecondpage();
            break;
        } else {
            cout << "Invalid choice. Please enter 1, 2, or 3.\n";
        }
    }
}

void displayFrontPage() {
    system("cls");
    yellow();
    cout << "\n\n\n\n\n\n\n\n\n";
    cout << "\t\tWELCOME TO OTRS\n";
    cout << "Press any key to proceed to login: ";
    reset();
}

void clear() { system("cls"); }

void displaySecondpage() {
    system("cls");

    char loginChoice;
    while (true) {
        green();
        cout << "Enter 'c' for Customer Login page "<<endl;
        cout << "Enter 'e' for exit program"<<endl;
        cout << "Choice: ";
        reset();
        cin >> loginChoice;

        if (loginChoice == 'c' || loginChoice == 'C') {
            clear();
            login_customer();
            break;

        }
        else if (loginChoice == 'e' || loginChoice == 'E') {

            exit(0);

        }

         else {
            cout << "Invalid input. Enter 'c' to continue.\n";
        }
    }
}

void cancelTicket(TrainSchedule& schedule) {
    string firstname;
    string lastname;
    int trainID, seatNumber = -1;

    // Prompt for passenger details
    cout << "Enter your name to cancel your ticket: ";

    cin>>firstname;
    cin>>lastname;

    cout << "Enter Train ID of your booking: ";
    cin >> trainID;

    // Open the passenger data file to search for the booking
    ifstream file1("passenger_data.txt");
    if (!file1.is_open()) {
        cout << "Error: Could not open passenger data file.\n";
        return;
    }

    // Read the passenger data and find the matching record
    string passengerfirstName;
    string passengerlastName;
    int age, trainId, seat;
    string gender;
    bool found = false;
    string tempFile = "temp_passenger_data.txt";
    ofstream tempFileStream(tempFile, ios::app);  // Temporary file to store data

    while (file1 >> passengerfirstName >> passengerlastName >> age >> gender >> trainId >> seat) {
        if (trainId == trainID && passengerfirstName == firstname && passengerlastName==lastname) {
            // If the booking matches, mark it as found and don't write it to the new temp file
            found = true;


            // Increment available seat for the train
            Train* train = schedule.findTrain(trainID);
            if (train) {
                train->availSeats++;  // Free up the seat
            }
            continue; // Skip adding this passenger to the new file (effectively removing it)
        }
        // Write non-matching passengers to the temporary file
        tempFileStream << passengerfirstName << " " << passengerlastName <<" "<< age << " " << gender << " " << trainId << " " << seat << "\n";
    }


    if (!found) {
        cout << "No booking found for the given details.\n";

    }
    else{ cout << "Booking found. Cancelling your ticket...\n";}

    // Replace the original passenger data file with the updated file
    file1.close();
    tempFileStream.close();
    remove("passenger_data.txt");
    rename(tempFile.c_str(), "passenger_data.txt");

      cout << "END!\n";
 cout << "\nLogout or Go Back\n";
        cout << "1. Logout\n";
        cout << "2. Go Back\n";
        cout << "Please choose an option: ";
         int choice;
        cin >> choice;

        switch (choice) {
            case 1:
                 logout();
                break;
            case 2:
              logout2();
                break;

            default:
                cout << "Invalid choice, please try again.\n";
                break;
        }

}
void mainDashboard() {
    int choice;
TrainSchedule schedule;
        loadPassengersFromFile("passenger_data.txt");
    while (true) {
        system("cls");
        green();
        cout << "\t\t\t\t\t -------------------------------------------------------- \n";
        cout << "\t\t\t\t\t|                                                        |\n";
        cout << "\t\t\t\t\t|             Welcome to Your Dashboard!                 |\n";
        cout << "\t\t\t\t\t|                                                        |\n";
        cout << "\t\t\t\t\t -------------------------------------------------------- \n\n";

        cout << "\t\t\t\t\t1. Book a Ticket\n";
        cout << "\t\t\t\t\t2. Train details\n";
        cout<< "\t\t\t\t\t3.  passenger list\n";
        cout<< "\t\t\t\t\t4.  waitlist\n";

        cout << "\t\t\t\t\t5. cancel ticket\n";
        cout << "\t\t\t\t\t6. Logout\n";

        reset();
        cout << "\n\t\t\t\t\tPlease choose an option: ";
        cin >> choice;
        int tid;

        switch (choice) {
            case 1:
                 bookTicket(schedule);
                break;
            case 2:
              trainDetails(schedule) ;
                break;
            case 3:

            cout<<"enter train id:";
            cin>>tid;
             displayPassengers(tid);
                break;
            case 4:
            cout<<"enter train id:";
            cin>>tid;

                  displayTrainWaitlist(tid);
                break;
                case 5:
              cancelTicket(schedule) ;
                break;
            case 6:
            logout();
           // Function to log out
                return;  // Exit the dashboard
            default:
                cout << "\n\t\t\t\t\tInvalid choice, please try again.\n";
                break;
        }
    }
}

void logout()
{
    displaySecondpage();
}
void logout2()
{
reset();

    ifstream myfile("cust_file.txt");
    string first_name, last_name, email, phone_no, password;
    bool found = false;

    while (myfile >> first_name >> last_name >> email >> phone_no >> password) {
        if ((phone_no == pnum) && (password == pswd)) {
            green();
            cout << "Welcome " << first_name << " " << last_name << " to the online ticket reservation system.\n";
            reset();
            found = true;
            mainDashboard();
            break;
        }
    }
    myfile.close();
}

void bookTicket(TrainSchedule& schedule) {
    // Declare variables for passenger information
    string name, gender;
    int age, trainID;
    int seatNumber = -1;  // Initialize as -1 (no seat assigned)

    // Take user input for passenger details
    cout << "Enter Passenger Name: ";
    cin.ignore();  // Clear newline from the buffer
    getline(cin, name);

    cout << "Enter Age: ";
    cin >> age;

    cout << "Enter Gender (Male/Female): ";
    cin >> gender;

    bool validTrain = false;
    Train* train = nullptr;

    while (!validTrain) {
        cout << "Enter Train ID: ";
        cin >> trainID;

        // Verify if the train ID exists
       if (!verifyTrainID(trainID, schedule)) {
    cout << "Error: Train ID not found. Please check the Train ID and try again.\n";

            // Offer the user a chance to re-enter the Train ID or exit
            char choice;
            cout << "Would you like to try again? (Y/N): ";
            cin >> choice;

            if (choice == 'Y' || choice == 'y') {
                continue;  // User chooses to re-enter the Train ID
            } else {
                cout << "Exiting the ticket booking process.\n";
                return;  // Exit the function
            }
        } else {
            validTrain = true;  // Train ID is valid, exit the loop
        }
    }
    Train* foundTrain = schedule.findTrain(trainID);
     ofstream file("passenger_data.txt", ios::app);
    if (file.is_open()) {
        // Write the passenger details to the file
        file << name << " " << age << " " << gender << " " << trainID << " " << foundTrain->availSeats+1 << "\n";
        file.close();  // Close the file after writing

        int current_seat=foundTrain->availSeats;
   allocateseat(trainID,current_seat);
    // Prompt user with options after booking
    cout<<"Your request has been submitted";
    cout << "\nLogout or Go Back\n";
    cout << "1. Logout\n";
    cout << "2. Go Back\n";
    cout << "Please choose an option: ";
    TrainSchedule schedule;
    int choice;
    cin >> choice;

    switch (choice) {
        case 1:
            logout();
            break;
        case 2:
        logout2();
            break;
        default:
            cout << "Invalid choice, please try again.\n";
            break;
    }
}}

bool verifyTrainID(int trainID, TrainSchedule& schedule) {
    // Search the train ID in the BST
    Train* foundTrain = schedule.findTrain(trainID);

    if (foundTrain != nullptr) {
        return true; // Train ID exists in the BST
    }
    return false; // Train ID not found
}



void trainDetails(TrainSchedule &schedule) {
    cout << "All Available Future Scheduled Trains:" << endl;
    cout << "------------------------------------" << endl;
    schedule.displayAllTrains();


    cout << "END!\n";
 cout << "\nLogout or Go Back\n";
        cout << "1. Logout\n";
        cout << "2. Go Back\n";
        cout << "Please choose an option: ";
         int choice;
        cin >> choice;

        switch (choice) {
            case 1:
                 logout();
                break;
            case 2:
              logout2();
                break;

            default:
                cout << "Invalid choice, please try again.\n";
                break;
        }
}

void allocateseat(int train_id,int current_seat){
 ifstream file1("train_data.txt");
    if (!file1.is_open()) {
        cout << "Error: Could not open passenger data file.\n";
        return;
    }
// Read the passenger data and find the matching record
    string trainname;
    string source;
    string arrival;
    string arrival_time;
    string reaching_time;
    int totalseat;
    int seatsallocated;
    int trainID;
    bool found = false;
    string tempFile = "temp_train_data.txt";
    ofstream tempFileStream(tempFile, ios::app);  // Temporary file to store data
     while (file1 >> trainID >> trainname >>arrival_time >> reaching_time >> source >> arrival >> totalseat >> seatsallocated) {
        if (trainID==train_id) {
            // If the booking matches, mark it as found and don't write it to the new temp file
            found = true;
            seatsallocated=current_seat+1;


        }
        // Write non-matching passengers to the temporary file
        tempFileStream << trainID << " " << trainname << " "<<arrival_time <<" "<< reaching_time << " " << source << " " << arrival << " " << totalseat << " "<< seatsallocated << " "<<"\n";
    }




    // Replace the original passenger data file with the updated file

    file1.close();
    tempFileStream.close();
      remove("train_data.txt");
    rename(tempFile.c_str(), "train_data.txt");

    }
int main() {

    displayFrontPage();
    _getch(); // Wait for a key press (Windows specific)
    displaySecondpage();
    return 0;
}
