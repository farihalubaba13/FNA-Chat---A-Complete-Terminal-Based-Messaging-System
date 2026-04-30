// ================================================================
// TERMINAL BASED CHAT SYSTEM
// ================================================================
// Course: Data Structure (CSE 221)
// Assignment: Build a terminal-based application with 4-5 features
// ================================================================
// Features:
//   - User Authentication (Register/Login/Logout)
//   - Private and Group Chat
//   - Message Inbox with Queue
//   - Undo Delete with Stack
//   - Friend Management with Linked List
//   - User Search with Binary Search Tree
//   - Fast Login with Hash Table
//   - Admin Panel
//   - Password Recovery
//   - Message Forwarding and Search
// ===============================================================

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <vector>

#ifdef _WIN32
    #include <windows.h>    // For color on Windows
#else
    // For Linux/Mac - ANSI color codes
    #define SetConsoleTextAttribute(h, c)
#endif

using namespace std;

// ======================== COLOR CODES ========================
// For better UI experience

enum Color {
    BLACK = 0,
    BLUE = 1,
    GREEN = 2,
    CYAN = 3,
    RED = 4,
    MAGENTA = 5,
    YELLOW = 6,
    WHITE = 7,
    GRAY = 8,
    BRIGHT_BLUE = 9,
    BRIGHT_GREEN = 10,
    BRIGHT_CYAN = 11,
    BRIGHT_RED = 12,
    BRIGHT_MAGENTA = 13,
    BRIGHT_YELLOW = 14,
    BRIGHT_WHITE = 15
};

#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

void setColor(int color) {
    WORD attributes = 0;

    switch(color) {
        case BLACK: attributes = 0; break;
        case BLUE: attributes = 1; break;
        case GREEN: attributes = 2; break;
        case CYAN: attributes = 3; break;
        case RED: attributes = 4; break;
        case MAGENTA: attributes = 5; break;
        case YELLOW: attributes = 6; break;
        case WHITE: attributes = 7; break;
        case GRAY: attributes = 8; break;
        case BRIGHT_BLUE: attributes = 9; break;
        case BRIGHT_GREEN: attributes = 10; break;
        case BRIGHT_CYAN: attributes = 11; break;
        case BRIGHT_RED: attributes = 12; break;
        case BRIGHT_MAGENTA: attributes = 13; break;
        case BRIGHT_YELLOW: attributes = 14; break;
        case BRIGHT_WHITE: attributes = 15; break;
        default: attributes = 7; break;
    }

    SetConsoleTextAttribute(hConsole, attributes);
}
#else
void setColor(int color) {
    int ansiCode;
    switch(color) {
        case BLACK: ansiCode = 30; break;
        case BLUE: ansiCode = 34; break;
        case GREEN: ansiCode = 32; break;
        case CYAN: ansiCode = 36; break;
        case RED: ansiCode = 31; break;
        case MAGENTA: ansiCode = 35; break;
        case YELLOW: ansiCode = 33; break;
        case WHITE: ansiCode = 37; break;
        case GRAY: ansiCode = 90; break;
        case BRIGHT_BLUE: ansiCode = 94; break;
        case BRIGHT_GREEN: ansiCode = 92; break;
        case BRIGHT_CYAN: ansiCode = 96; break;
        case BRIGHT_RED: ansiCode = 91; break;
        case BRIGHT_MAGENTA: ansiCode = 95; break;
        case BRIGHT_YELLOW: ansiCode = 93; break;
        case BRIGHT_WHITE: ansiCode = 97; break;
        default: ansiCode = 37; break;
    }
    cout << "\033[1;" << ansiCode << "m";
}
#endif

void resetColor() {
#ifdef _WIN32
    SetConsoleTextAttribute(hConsole, 7);
#else
    cout << "\033[0m";
#endif
}

// ======================== DATE/TIME UTILITY ========================
string getCurrentTimestamp() {
    time_t now = time(0);
    char* dt = ctime(&now);
    string timestamp(dt);
    timestamp.pop_back(); // Remove newline
    return timestamp;
}

string getCurrentDate() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    stringstream ss;
    ss << ltm->tm_mday << "/" << (1 + ltm->tm_mon) << "/" << (1900 + ltm->tm_year);
    return ss.str();
}

// ======================== MESSAGE STRUCTURE ========================
struct Message {
    string sender;
    string text;
    string timestamp;
    bool isRead;
    Message* next;

    Message(string s, string t) : sender(s), text(t), next(nullptr) {
        timestamp = getCurrentTimestamp();
        isRead = false;
    }
};

// ======================== QUEUE FOR INBOX (FIFO) ========================
// Feature: Messages are stored in Queue - first sent, first seen
class MessageQueue {
private:
    Message* front;
    Message* rear;
    int size;

public:
    MessageQueue() : front(nullptr), rear(nullptr), size(0) {}

    // Enqueue - add message to inbox
    void enqueue(string sender, string text) {
        Message* newMsg = new Message(sender, text);
        if (rear == nullptr) {
            front = rear = newMsg;
        } else {
            rear->next = newMsg;
            rear = newMsg;
        }
        size++;
    }

    // Dequeue - remove and return oldest message
    Message* dequeue() {
        if (front == nullptr) return nullptr;
        Message* temp = front;
        front = front->next;
        if (front == nullptr) rear = nullptr;
        size--;
        return temp;
    }

    // Peek at oldest message without removing
    Message* peek() {
        return front;
    }

    bool isEmpty() {
        return front == nullptr;
    }

    int getSize() {
        return size;
    }

    // Display all messages
    void displayMessages() {
        if (isEmpty()) {
            setColor(RED);
            cout << "   [INFO] No messages in inbox.\n";
            resetColor();
            return;
        }

        Message* current = front;
        int msgNum = 1;
        while (current != nullptr) {
            setColor(CYAN);
            cout << "   " << msgNum << ". ";
            setColor(YELLOW);
            cout << "From: " << current->sender;
            setColor(WHITE);
            cout << " | " << current->timestamp << "\n";
            setColor(GREEN);
            cout << "      Message: " << current->text << "\n";
            resetColor();
            current->isRead = true;
            current = current->next;
            msgNum++;
        }
    }

    // Get message at index (for delete operation)
    Message* getMessageAtIndex(int index) {
        if (index < 0 || index >= size){
            return nullptr;
        }
        Message* current = front;
        for (int i = 0; i < index; i++) {
            current = current->next;
        }
        return current;
    }

    // Remove message at index (returns the removed message)
    Message* removeAtIndex(int index) {
        if (index < 0 || index >= size){
            return nullptr;
        }

        if (index == 0) {
            return dequeue();
        }

        Message* prev = front;
        for (int i = 0; i < index - 1; i++) {
            prev = prev->next;
        }

        Message* toRemove = prev->next;
        prev->next = toRemove->next;
        if (toRemove == rear) {
            rear = prev;
        }
        size--;
        return toRemove;
    }

    // Mark message as read
    void markAsRead(int index){
        if (index < 0 || index >= size){
            setColor(BRIGHT_RED);
            cout << "   [ERROR] Invalid message number!\n";
            resetColor();
            return;
        }

        Message* current = front;
        for (int i = 0; i < index; i++){
            current = current->next;
        }

        current->isRead = true;
        setColor(BRIGHT_GREEN);
        cout << "   [SUCCESS] Message marked as read!\n";
        resetColor();
    }

    // Mark message as unread
    void markAsUnread(int index){
        if (index < 0 || index >= size){
            setColor(BRIGHT_RED);
            cout << "   [ERROR] Invalid message number!\n";
            resetColor();
            return;
        }

        Message* current = front;
        for (int i = 0; i < index; i++){
            current = current->next;
        }

        current->isRead = false;
        setColor(BRIGHT_GREEN);
        cout << "   [SUCCESS] Message marked as unread!\n";
        resetColor();
    }

    // Save to file
    void saveToFile(ofstream& file){
        Message* current = front;
        while (current != nullptr){
            file << current->sender << "|" << current->text << "|" << current->timestamp << "|" << current->isRead << "\n";
            current = current->next;
        }
    }

    // Search messages by keyword and display results
    void searchAndDisplay(string keyword){
        if (isEmpty()){
            setColor(BRIGHT_RED);
            cout << "   [INFO] No messages to search.\n";
            resetColor();
            return;
        }

        Message* current = front;
        int found = 0;

        while (current != nullptr){
            // Search in sender name and message text
            size_t foundInSender = current->sender.find(keyword);
            size_t foundInText = current->text.find(keyword);

            if (foundInSender != string::npos || foundInText != string::npos){
                found++;
                setColor(BRIGHT_GREEN);
                cout << "\n   📧 Message #" << found << ":\n";
                resetColor();
                setColor(BRIGHT_CYAN);
                cout << "   From: ";
                setColor(BRIGHT_YELLOW);
                cout << current->sender << "\n";
                setColor(BRIGHT_CYAN);
                cout << "   Time: ";
                setColor(BRIGHT_WHITE);
                cout << current->timestamp << "\n";
                setColor(BRIGHT_CYAN);
                cout << "   Message: ";
                setColor(BRIGHT_GREEN);

                // Highlight the keyword
                string msgText = current->text;
                size_t pos = 0;
                while ((pos = msgText.find(keyword, pos)) != string::npos){
                    cout << msgText.substr(0, pos);
                    setColor(BRIGHT_RED);
                    cout << keyword;
                    setColor(BRIGHT_GREEN);
                    msgText = msgText.substr(pos + keyword.length());
                    pos = 0;
                }
                cout << msgText << "\n";
                resetColor();

                setColor(BRIGHT_CYAN);
                cout << "   Status: ";

                if (current->isRead){
                    setColor(BRIGHT_GREEN);
                    cout << "✓ Read\n";
                } else{
                    setColor(BRIGHT_YELLOW);
                    cout << "○ Unread\n";
                }
                resetColor();
            }
            current = current->next;
        }

        if (found == 0){
            setColor(BRIGHT_RED);
            cout << "   ❌ No messages found containing \"" << keyword << "\"\n";
            resetColor();
        } else{
            setColor(BRIGHT_GREEN);
            cout << "\n   ✅ Found " << found << " message(s) containing \"" << keyword << "\"\n";
            resetColor();
        }
    }

    // Get count of unread messages by traversing the queue
    int getUnreadCount(){
        int unreadCount = 0;
        Message* current = front;  // Start from the front

        // Traverse through all messages in the linked list
        while (current != nullptr){
            if (!current->isRead){  // If message is not read
                unreadCount++;
            }
            current = current->next;  // Move to next message
        }
        return unreadCount;
    }

    ~MessageQueue(){
        while (!isEmpty()){
            delete dequeue();
        }
    }
};

// ======================== STACK FOR UNDO DELETE (LIFO) ========================
// Feature: Stores deleted messages for recovery
class UndoStack{
private:
    struct StackNode{
        Message* msg;
        StackNode* next;
        StackNode(Message* m) : msg(m), next(nullptr){}
    };
    StackNode* top;

public:
    UndoStack() : top(nullptr){}

    void push(Message* msg){
        StackNode* newNode = new StackNode(msg);
        newNode->next = top;
        top = newNode;
    }

    Message* pop(){
        if (top == nullptr) return nullptr;
        StackNode* temp = top;
        Message* msg = temp->msg;
        top = top->next;
        delete temp;
        return msg;
    }

    bool isEmpty(){
        return top == nullptr;
    }

    ~UndoStack(){
        while (!isEmpty()){
            delete pop();
        }
    }
};

// ======================== FRIEND NODE (LINKED LIST) ========================
struct FriendNode{
    string username;
    FriendNode* next;
    FriendNode(string name) : username(name), next(nullptr){}
};

// ======================== USER STRUCTURE ========================
struct User {
    string username;
    string password;
    string email;           // For password recovery
    string securityQuestion; // Security question
    string securityAnswer;   // Security answer
    string lastSeen;
    bool isOnline;
    bool isAdmin;
    FriendNode* friends;
    MessageQueue* inbox;
    UndoStack* undoStack;
    User* next;

    User(string uname, string pwd, bool admin = false)
        : username(uname), password(pwd), email(""),
          securityQuestion(""), securityAnswer(""),
          isOnline(false), isAdmin(admin),
          friends(nullptr), inbox(new MessageQueue()),
          undoStack(new UndoStack()), next(nullptr),lastSeen("Never"){}

    ~User(){
        // Clean up friend list
        while (friends != nullptr){
            FriendNode* temp = friends;
            friends = friends->next;
            delete temp;
        }
        delete inbox;
        delete undoStack;
    }

    void addFriend(string friendName){
        FriendNode* newFriend = new FriendNode(friendName);
        newFriend->next = friends;
        friends = newFriend;
    }

    bool hasFriend(string friendName){
        FriendNode* current = friends;
        while (current != nullptr) {
            if (current->username == friendName){
                return true;
            }
            current = current->next;
        }
        return false;
    }

    void displayFriends(){
        if (friends == nullptr) {
            setColor(RED);
            cout << "   [INFO] No friends added yet.\n";
            resetColor();
            return;
        }

        setColor(CYAN);
        cout << "   Friends List: ";
        FriendNode* current = friends;
        while (current != nullptr) {
            setColor(GREEN);
            cout << current->username;
            if (current->next != nullptr) cout << ", ";
            current = current->next;
        }
        cout << "\n";
        resetColor();
    }

    int getUnreadCount(){
        // Call MessageQueue's efficient unread counter
        return inbox->getUnreadCount();
    }
};

// ======================== HASH TABLE FOR USER MANAGEMENT ========================
// Feature: O(1) average case login/search
class HashTable{
private:
    static const int TABLE_SIZE = 101;  // Prime number for better distribution
    User** table;

    int hashFunction(string key){
        int hash = 0;
        for (char c : key) {
            hash = (hash * 31 + c) % TABLE_SIZE;
        }
        return hash;
    }

public:
    HashTable(){
        table = new User*[TABLE_SIZE]();
    }

    ~HashTable(){
        for (int i = 0; i < TABLE_SIZE; i++){
            User* current = table[i];
            while (current != nullptr){
                User* temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] table;
    }

    void insert(User* user){
        int index = hashFunction(user->username);
        user->next = table[index];
        table[index] = user;
    }

    User* search(string username){
        int index = hashFunction(username);
        User* current = table[index];
        while (current != nullptr){
            if (current->username == username){
                return current;
            }
            current = current->next;
        }
        return nullptr;
    }

    bool remove(string username){
        int index = hashFunction(username);
        User* current = table[index];
        User* prev = nullptr;

        while (current != nullptr){
            if (current->username == username){
                if (prev == nullptr){
                    table[index] = current->next;
                } else{
                    prev->next = current->next;
                }
                delete current;
                return true;
            }
            prev = current;
            current = current->next;
        }
        return false;
    }

    // Get all users (for BST insertion)
    vector<User*> getAllUsers(){
        vector<User*> users;
        for (int i = 0; i < TABLE_SIZE; i++){
            User* current = table[i];
            while (current != nullptr){
                users.push_back(current);
                current = current->next;
            }
        }
        return users;
    }
};

// ======================== BST NODE FOR USER SEARCH ========================
// Feature: Fast user search with sorted output
struct BSTNode{
    User* user;
    BSTNode* left;
    BSTNode* right;

    BSTNode(User* u) : user(u), left(nullptr), right(nullptr) {}
};

class UserBST{
private:
    BSTNode* root;

    BSTNode* insertHelper(BSTNode* node, User* user){
        if (node == nullptr){
            return new BSTNode(user);
        }
        if (user->username < node->user->username){
            node->left = insertHelper(node->left, user);
        } else if (user->username > node->user->username){
            node->right = insertHelper(node->right, user);
        }
        return node;
    }

    BSTNode* searchHelper(BSTNode* node, string username){
        if (node == nullptr || node->user->username == username){
            return node;
        }
        if (username < node->user->username){
            return searchHelper(node->left, username);
        }
        return searchHelper(node->right, username);
    }

    void inorderHelper(BSTNode* node){
        if (node != nullptr) {
            inorderHelper(node->left);
            setColor(GREEN);
            cout << "   • " << node->user->username;
            if (node->user->isOnline){
                setColor(GREEN);
                cout << " 🟢 Online";
            } else{
                setColor(RED);
                cout << " ⚫ Offline";
            }
            cout << "\n";
            resetColor();
            inorderHelper(node->right);
        }
    }

    void destroyTree(BSTNode* node){
        if (node != nullptr) {
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
        }
    }

public:
    UserBST() : root(nullptr){}

    ~UserBST(){
        destroyTree(root);
    }

    void insert(User* user){
        root = insertHelper(root, user);
    }

    User* search(string username){
        BSTNode* result = searchHelper(root, username);
        return result ? result->user : nullptr;
    }

    void displayAllUsers(){
        if (root == nullptr){
            setColor(RED);
            cout << "   [INFO] No users found.\n";
            resetColor();
            return;
        }
        setColor(CYAN);
        cout << "\n   === ALL USERS (Sorted Alphabetically) ===\n";
        resetColor();
        inorderHelper(root);
    }

    void rebuildFromHashTable(HashTable& hashTable){
        destroyTree(root);
        root = nullptr;
        vector<User*> users = hashTable.getAllUsers();
        for (User* user : users) {
            insert(user);
        }
    }
};

// ======================== GROUP CHAT STRUCTURE ========================
struct Group{
    string groupName;
    string creator;
    vector<string> members;
    MessageQueue* messages;
    Group* next;

    Group(string name, string creatorName) : groupName(name), creator(creatorName),
                                              messages(new MessageQueue()), next(nullptr) {
        members.push_back(creatorName);
    }

    ~Group(){
        delete messages;
    }

    void addMember(string username){
        if (find(members.begin(), members.end(), username) == members.end()) {
            members.push_back(username);
        }
    }

    void sendMessage(string sender, string text){
        messages->enqueue(sender, text);
    }

    void displayMessages(){
        setColor(MAGENTA);
        cout << "\n   === Group: " << groupName << " ===\n";
        resetColor();
        messages->displayMessages();
    }

    void displayMembers(){
    setColor(BRIGHT_CYAN);
    cout << "\n   === Group Members: " << groupName << " ===\n";
    resetColor();

    if (members.empty()){
        setColor(BRIGHT_RED);
        cout << "   No members in this group.\n";
        resetColor();
        return;
    }

    setColor(BRIGHT_GREEN);
    cout << "   Creator: " << creator << " (Admin)\n";
    resetColor();

    setColor(BRIGHT_YELLOW);
    cout << "   Members:\n";
    resetColor();

    for (size_t i = 0; i < members.size(); i++){
        setColor(BRIGHT_WHITE);
        cout << "   " << (i + 1) << ". " << members[i];
        if (members[i] == creator){
            setColor(BRIGHT_GREEN);
            cout << " [Creator]";
        }
        resetColor();
        cout << "\n";
    }

    setColor(BRIGHT_CYAN);
    cout << "   Total Members: " << members.size() << "\n";
    resetColor();
}
};

// ======================== GROUP MANAGER (LINKED LIST) ========================
class GroupManager {
private:
    Group* head;

public:
    GroupManager() : head(nullptr){}

    ~GroupManager(){
        while (head != nullptr){
            Group* temp = head;
            head = head->next;
            delete temp;
        }
    }

    void createGroup(string name, string creator){
        Group* newGroup = new Group(name, creator);
        newGroup->next = head;
        head = newGroup;
        setColor(GREEN);
        cout << "   [SUCCESS] Group '" << name << "' created!\n";
        resetColor();
    }

    Group* findGroup(string name){
        Group* current = head;
        while (current != nullptr){
            if (current->groupName == name){
                return current;
            }
            current = current->next;
        }
        return nullptr;
    }

    void addToGroup(string groupName, string username, string requester){
        Group* group = findGroup(groupName);
        if (group == nullptr){
            setColor(RED);
            cout << "   [ERROR] Group not found!\n";
            resetColor();
            return;
        }

        if (group->creator != requester){
            setColor(RED);
            cout << "   [ERROR] Only group creator can add members!\n";
            resetColor();
            return;
        }

        group->addMember(username);
        setColor(GREEN);
        cout << "   [SUCCESS] " << username << " added to group '" << groupName << "'!\n";
        resetColor();
    }

    void listGroups(){
        if (head == nullptr){
            setColor(RED);
            cout << "   [INFO] No groups available.\n";
            resetColor();
            return;
        }

        setColor(CYAN);
        cout << "\n   === Available Groups ===\n";
        Group* current = head;
        while (current != nullptr){
            setColor(YELLOW);
            cout << "   • " << current->groupName;
            setColor(WHITE);
            cout << " (Created by: " << current->creator << ", Members: " << current->members.size() << ")\n";
            current = current->next;
        }
        resetColor();
    }
};

// ======================== CHAT SYSTEM MAIN CLASS ========================
class ChatSystem {
private:
    HashTable users;
    UserBST userBST;
    GroupManager groupManager;
    User* currentUser;
    bool running;

    void saveData() {
    ofstream userFile("users.txt");

    vector<User*> allUsers = users.getAllUsers();
    for (User* user : allUsers) {
        // Save user info with recovery data
        userFile << user->username << "|"
                 << user->password << "|"
                 << user->email << "|"
                 << user->securityQuestion << "|"
                 << user->securityAnswer << "|"
                 << user->isAdmin << "\n";

        // Save friends
        FriendNode* friendPtr = user->friends;
        while (friendPtr != nullptr) {
            userFile << "FRIEND|" << user->username << "|" << friendPtr->username << "\n";
            friendPtr = friendPtr->next;
        }
    }

    userFile.close();
    setColor(BRIGHT_GREEN);
    cout << "   [INFO] Data saved successfully!\n";
    resetColor();
}

    void backupData(){
        ofstream backup("backup_users.txt");
        if (!backup.is_open()){
            setColor(BRIGHT_RED);
            cout << "   [ERROR] Cannot create backup file!\n";
            resetColor();
            return;
        }

        vector<User*> allUsers = users.getAllUsers();

        for (User* user : allUsers) {
            backup << user->username << "|" << user->password << "|"
                   << user->email << "|" << user->securityQuestion << "|"
                   << user->securityAnswer << "|" << user->isAdmin << "|"
                   << user->lastSeen << "\n";

            // Backup friends
            FriendNode* friendPtr = user->friends;
            while (friendPtr != nullptr) {
                backup << "FRIEND|" << user->username << "|" << friendPtr->username << "\n";
                friendPtr = friendPtr->next;
            }
        }
        backup.close();

        setColor(BRIGHT_GREEN);
        cout << "   [SUCCESS] Data backed up successfully to 'backup_users.txt'!\n";
        resetColor();
    }

void loadData(){
    ifstream userFile("users.txt");
    if (!userFile.is_open()) return;

    string line;
    while (getline(userFile, line)) {
        // Skip friend lines for now
        if (line.find("FRIEND|") == 0) continue;

        stringstream ss(line);
        string username, password, email, securityQuestion, securityAnswer, isAdminStr;
        getline(ss, username, '|');
        getline(ss, password, '|');
        getline(ss, email, '|');
        getline(ss, securityQuestion, '|');
        getline(ss, securityAnswer, '|');
        getline(ss, isAdminStr, '|');

        bool isAdmin = (isAdminStr == "1");
        User* user = new User(username, password, isAdmin);
        user->email = email;
        user->securityQuestion = securityQuestion;
        user->securityAnswer = securityAnswer;

        users.insert(user);
        userBST.insert(user);
    }
    userFile.close();


        // Load friend relationships
        ifstream friendFile("friends.txt");
        if (friendFile.is_open()){
            while (getline(friendFile, line)){
                stringstream ss(line);
                string user1, user2;
                getline(ss, user1, '|');
                getline(ss, user2, '|');
                User* u = users.search(user1);
                if (u != nullptr){
                    u->addFriend(user2);
                }
            }
            friendFile.close();
        }
    }

    void showHeader(){
       #ifdef _WIN32
            system("cls");
       #else
            system("clear");
       #endif

       setColor(BRIGHT_YELLOW);
       cout << "╔═══════════════════════════════════════════════════════════╗\n";
       setColor(BRIGHT_MAGENTA);
       cout << "║                                                           ║\n";
       setColor(BRIGHT_YELLOW);
       cout << "║                    Welcome to FNA Chat                    ║\n";
       setColor(BRIGHT_GREEN);
       cout << "║              Terminal-Based Messaging System              ║\n";
       setColor(BRIGHT_MAGENTA);
       cout << "║              Data Structure(CSE 221) Project              ║\n";
       setColor(BRIGHT_GREEN);
       cout << "║                                                           ║\n";
       setColor(BRIGHT_YELLOW);
       cout << "╚═══════════════════════════════════════════════════════════╝\n";
       resetColor();

       setColor(BRIGHT_WHITE);
       cout << "                     Date: " << getCurrentDate() << "\n";
       setColor(GRAY);
       cout << "                     Time: " << getCurrentTimestamp() << "\n";
       resetColor();
       cout << "\n";
   }

    void showMainMenu() {
        showHeader();
        setColor(BRIGHT_YELLOW);
        cout << "   ┌─────────────────────────────────────────┐\n";
        cout << "   │                MAIN MENU                │\n";
        cout << "   ├─────────────────────────────────────────┤\n";
        setColor(BRIGHT_GREEN);
        cout << "   │  1. Register New Account                │\n";
        cout << "   │  2. Login                               │\n";
        setColor(BRIGHT_CYAN);
        cout << "   │  3. Forgot Password / Username          │\n";
        setColor(BRIGHT_RED);
        cout << "   │  4. Exit                                │\n";
        setColor(BRIGHT_YELLOW);
        cout << "   └─────────────────────────────────────────┘\n";
        resetColor();
        cout << "\n   Choose option: ";
    }

    void showUserMenu(){
        showHeader();
        setColor(GREEN);
        cout << "   Welcome, " << currentUser->username;
        if (currentUser->isOnline){
            setColor(GREEN);
            cout << " is now Online!";
        }
        resetColor();
        cout << "\n";
        setColor(CYAN);
        cout << "   ┌─────────────────────────────────────────┐\n";
        cout << "   │                CHAT MENU                │\n";
        cout << "   ├─────────────────────────────────────────┤\n";
        cout << "   │  1. Send Private Message                │\n";
        cout << "   │  2. View Inbox                          │\n";
        cout << "   │  3. Delete Message (with Undo)          │\n";
        cout << "   │  4. Undo Last Deletion                  │\n";
        cout << "   │  5. Manage Friends                      │\n";
        cout << "   │  6. Search Users                        │\n";
        cout << "   │  7. Group Chat                          │\n";
        cout << "   │  8. View All Users                      │\n";
        cout << "   │  9. Change Password                     │\n";
        cout << "   │ 10. Search Messages                     │\n";
        cout << "   │ 11. Forward Message                     │\n";
        cout << "   │ 12. Logout                              │\n";
        if (currentUser->isAdmin){
            setColor(RED);
        cout << "   │ 13. Admin Panel (Delete Users)          │\n";
            resetColor();
        }
        cout << "   └─────────────────────────────────────────┘\n";
        resetColor();
        cout << "\n   Choose option: ";
    }

void forgotPassword(){
    showHeader();
    setColor(BRIGHT_CYAN);
    cout << "   === PASSWORD RECOVERY ===\n";
    resetColor();

    int choice;
    setColor(BRIGHT_YELLOW);
    cout << "\n   1. Recover Username\n";
    cout << "   2. Reset Password (via Security Question)\n";
    cout << "   3. Reset Password (via Email)\n";
    cout << "   4. Contact Admin\n";
    cout << "   5. Back to Main Menu\n";
    resetColor();
    cout << "\n   Choose option: ";
    cin >> choice;

    if (choice == 1){
        // Recover Username
        string email;
        cout << "   Enter your registered email: ";
        cin >> email;

        bool found = false;
        vector<User*> allUsers = users.getAllUsers();

        for (User* user : allUsers){
            if (user->email == email){
                setColor(BRIGHT_GREEN);
                cout << "\n   Your username is: " << user->username << "\n";
                resetColor();
                found = true;
                break;
            }
        }

        if(!found){
            setColor(BRIGHT_RED);
            cout << "   [ERROR] No account found with this email!\n";
            resetColor();
        }

    } else if(choice == 2){
        // Reset via Security Question
        string username;
        cout << "   Enter your username: ";
        cin >> username;

        User* user = users.search(username);
        if(user == nullptr){
            setColor(BRIGHT_RED);
            cout << "   [ERROR] Username not found!\n";
            resetColor();
        } else{
            cout << "\n   Security Question: " << user->securityQuestion << "\n";
            cout << "   Your answer: ";

            string answer;
            cin.ignore();
            getline(cin, answer);

            if(answer == user->securityAnswer){
                string newPassword, confirmPassword;
                cout << "   Enter new password: ";
                cin >> newPassword;
                cout << "   Confirm new password: ";
                cin >> confirmPassword;

                if(newPassword == confirmPassword){
                    user->password = newPassword;
                    setColor(BRIGHT_GREEN);
                    cout << "   [SUCCESS] Password reset successfully!\n";
                    cout << "   Please login with your new password.\n";
                    resetColor();
                } else{
                    setColor(BRIGHT_RED);
                    cout << "   [ERROR] Passwords do not match!\n";
                    resetColor();
                }
            } else{
                setColor(BRIGHT_RED);
                cout << "   [ERROR] Incorrect security answer!\n";
                resetColor();
            }
        }

    } else if (choice == 3){
        // Reset via Email (simulated)
        string email;
        cout << "   Enter your registered email: ";
        cin >> email;

        bool found = false;
        vector<User*> allUsers = users.getAllUsers();
        User* targetUser = nullptr;

        for (User* user : allUsers){
            if(user->email == email){
                targetUser = user;
                found = true;
                break;
            }
        }

        if (found && targetUser != nullptr){
            setColor(BRIGHT_GREEN);
            cout << "\n   [INFO] Password reset link sent to " << email << "\n";
            cout << "   For demo purposes, you can reset now.\n";

            string newPassword, confirmPassword;
            cout << "   Enter new password: ";
            cin >> newPassword;
            cout << "   Confirm new password: ";
            cin >> confirmPassword;

            if(newPassword == confirmPassword){
                targetUser->password = newPassword;
                cout << "   [SUCCESS] Password reset successfully!\n";
                resetColor();
            } else{
                setColor(BRIGHT_RED);
                cout << "   [ERROR] Passwords do not match!\n";
                resetColor();
            }
        } else{
            setColor(BRIGHT_RED);
            cout << "   [ERROR] No account found with this email!\n";
            resetColor();
        }

    } else if(choice == 4){
        // Contact Admin
        setColor(BRIGHT_CYAN);
        cout << "\n   === CONTACT ADMIN ===\n";
        cout << "   Admin username: admin\n";
        cout << "   Please contact the system administrator for assistance.\n";
        cout << "   Provide your username and email for verification.\n";
        resetColor();
    }

    cout << "\n   Press Enter to continue...";
    cin.ignore();
    cin.get();
}

void registerUser(){
    showHeader();
    setColor(BRIGHT_CYAN);
    cout << "   === REGISTRATION ===\n";
    resetColor();

    string username, password, email, securityQuestion, securityAnswer;

    // Username validation
    cout << "   Enter username (5-20 characters): ";
    cin >> username;

    // Check username length
    if(username.length() < 5 || username.length() > 20){
        setColor(BRIGHT_RED);
        cout << "   [ERROR] Username must be between 5 and 20 characters!\n";
        resetColor();
        cout << "\n   Press Enter to continue...";
        cin.ignore();
        cin.get();
        return;
    }

    // Check if username already exists
    if(users.search(username) != nullptr){
        setColor(BRIGHT_RED);
        cout << "   [ERROR] Username '" << username << "' already exists!\n";
        resetColor();
        suggestUsernames(username);
        cout << "\n   Press Enter to continue...";
        cin.ignore();
        cin.get();
        return;
    }

    // Password validation
    cout << "   Enter password (min 4 characters): ";
    cin >> password;

    if (password.length() < 4){
        setColor(BRIGHT_RED);
        cout << "   [ERROR] Password must be at least 4 characters!\n";
        resetColor();
        cout << "\n   Press Enter to continue...";
        cin.ignore();
        cin.get();
        return;
    }

    // Email with skip option
    cout << "   Enter email (for password recovery) or 0 to skip: ";
    cin >> email;

    if (email == "0"){
        email = "";
        setColor(BRIGHT_YELLOW);
        cout << "   [WARNING] Email skipped. Password recovery will not be available!\n";
        resetColor();
    } else if(email.find('@') == string::npos || email.find('.') == string::npos){
        setColor(BRIGHT_YELLOW);
        cout << "   [WARNING] Invalid email format. Email saved as is.\n";
        resetColor();
    }

    cin.ignore(); // Clear buffer
    cout << "   Enter security question (e.g., What is your pet's name?) or 0 to skip: ";
    getline(cin, securityQuestion);

    if (securityQuestion == "0"){
        securityQuestion = "";
        securityAnswer = "";
        setColor(BRIGHT_YELLOW);
        cout << "   [WARNING] Security question skipped. Password recovery will not be available!\n";
        resetColor();
    } else{
        cout << "   Enter security answer: ";
        getline(cin, securityAnswer);

        if (securityAnswer.empty()) {
            setColor(BRIGHT_YELLOW);
            cout << "   [WARNING] Security answer cannot be empty!\n";
            securityAnswer = "default";
            resetColor();
        }
    }

    // Create new user
    User* newUser = new User(username, password, false);
    newUser->email = email;
    newUser->securityQuestion = securityQuestion;
    newUser->securityAnswer = securityAnswer;

    users.insert(newUser);
    userBST.insert(newUser);

    setColor(BRIGHT_GREEN);
    cout << "\n   ✓ [SUCCESS] Registration successful!\n";
    cout << "   ✓ Username: " << username << "\n";
    if (!email.empty()) cout << "   ✓ Email: " << email << "\n";
    else cout << "   ⚠  Email not provided (password recovery disabled)\n";
    if (!securityQuestion.empty()) cout << "   ✓ Security question saved for password recovery.\n";
    else cout << "   ⚠  Security question not set (password recovery disabled)\n";
    cout << "\n   Please login with your credentials.\n";
    resetColor();

    cout << "\n   Press Enter to continue...";
    cin.ignore();
    cin.get();
}

void suggestUsernames(string attemptedUsername){
    setColor(BRIGHT_CYAN);
    cout << "   Suggested usernames:\n";
    resetColor();

    vector<string> suggestions;
    suggestions.push_back(attemptedUsername + "123");
    suggestions.push_back(attemptedUsername + "_" + to_string(rand() % 1000));
    suggestions.push_back("the_" + attemptedUsername);
    suggestions.push_back(attemptedUsername + "_user");

    for (string sugg : suggestions){
        if (users.search(sugg) == nullptr){
            setColor(BRIGHT_GREEN);
            cout << "   • " << sugg << " (available)\n";
            resetColor();
        }
    }
}

void login(){
    showHeader();
    setColor(CYAN);
    cout << "   === LOGIN ===\n";
    resetColor();

    string username, password;
    cout << "   Username: ";
    cin >> username;
    cout << "   Password: ";
    cin >> password;

    User* user = users.search(username);
    if(user != nullptr && user->password == password){
        currentUser = user;
        currentUser->isOnline = true;
        currentUser->lastSeen = getCurrentTimestamp();
        setColor(GREEN);
        cout << "   [SUCCESS] Login successful! Welcome " << username << "!\n";
        resetColor();

        // Show notification for unread messages
        int unread = currentUser->getUnreadCount();
        if(unread > 0){
            setColor(YELLOW);
            cout << "   🔔 You have " << unread << " unread message(s)!\n";
            resetColor();
        }
    } else{
        setColor(RED);
        cout << "   [ERROR] Invalid username or password!\n";
        resetColor();
        currentUser = nullptr;
    }
    cout << "\n   Press Enter to continue...";
    cin.ignore();
    cin.get();
}

void sendPrivateMessage(){
    showHeader();
    setColor(CYAN);
    cout << "   === SEND PRIVATE MESSAGE ===\n";
    resetColor();

    string recipient, message;
    cout << "   Recipient username: ";
    cin >> recipient;

    User* receiver = users.search(recipient);
    if(receiver == nullptr){
        setColor(RED);
        cout << "   [ERROR] User not found!\n";
        resetColor();
        cout << "\n   Press Enter to continue...";
        cin.ignore();
        cin.get();
        return;
    }

    if(receiver->username == currentUser->username){
        setColor(RED);
        cout << "   [ERROR] Cannot send message to yourself!\n";
        resetColor();
        cout << "\n   Press Enter to continue...";
        cin.ignore();
        cin.get();
        return;
    }

    showTypingIndicator(currentUser->username);
    cin.ignore();
    cout << "   Message: ";
    getline(cin, message);
    receiver->inbox->enqueue(currentUser->username, message);
    setColor(GREEN);
    cout << "   [SUCCESS] Message sent to " << recipient << "!\n";

    if(receiver->isOnline){
        setColor(YELLOW);
        cout << "   🔔 " << recipient << " is online. Notification sent!\n";
        resetColor();
    }
    resetColor();

    cout << "\n   Press Enter to continue...";
    cin.get();
}

void showTypingIndicator(string username){
    setColor(BRIGHT_CYAN);
    cout << "   ✎ " << username << " is typing";

    for(int i = 0; i < 3; i++){
        cout << ".";
        cout.flush();

        #ifdef _WIN32
            Sleep(500);  // Windows: 500 milliseconds
        #else
            usleep(500000);  // Linux/Mac: 500,000 microseconds = 0.5 seconds
        #endif
    }

    cout << "\n";
    resetColor();
}

void forwardMessage(){
    showHeader();
    setColor(BRIGHT_CYAN);
    cout << "   === FORWARD MESSAGE ===\n";
    resetColor();
    if(currentUser->inbox->isEmpty()){
        setColor(BRIGHT_RED);
        cout << "   [INFO] No messages to forward.\n";
        resetColor();
        cout << "\n   Press Enter to continue...";
        cin.ignore();
        cin.get();
        return;
    }

    currentUser->inbox->displayMessages();
    cout << "\n   Enter message number to forward (0 to cancel): ";
    int msgNum;
    cin >> msgNum;

    if(msgNum == 0){
       return;
    }
    Message* msg = currentUser->inbox->getMessageAtIndex(msgNum - 1);
    if(msg == nullptr){
        setColor(BRIGHT_RED);
        cout << "   [ERROR] Invalid message number!\n";
        resetColor();
        cout << "\n   Press Enter to continue...";
        cin.ignore();
        cin.get();
        return;
    }

    string recipient;
    cout << "   Enter recipient username: ";
    cin >> recipient;

    User* receiver = users.search(recipient);
    if(receiver == nullptr){
        setColor(BRIGHT_RED);
        cout << "   [ERROR] User not found!\n";
        resetColor();
        cout << "\n   Press Enter to continue...";
        cin.ignore();
        cin.get();
        return;
    }

    string forwardedMsg = "[Forwarded from " + msg->sender + "]: " + msg->text;
    receiver->inbox->enqueue(currentUser->username, forwardedMsg);

    setColor(BRIGHT_GREEN);
    cout << "   [SUCCESS] Message forwarded to " << recipient << "!\n";
    resetColor();

    cout << "\n   Press Enter to continue...";
    cin.ignore();
    cin.get();
}

void viewInbox(){
    showHeader();
    setColor(BRIGHT_CYAN);
    cout << "   === INBOX ===\n";
    resetColor();

    if(currentUser->inbox->isEmpty()){
        setColor(BRIGHT_RED);
        cout << "   [INFO] Your inbox is empty.\n";
        resetColor();
    } else{
        currentUser->inbox->displayMessages();

        // Add options for message management
        setColor(BRIGHT_YELLOW);
        cout << "\n ┌─────────────────────────────────────┐\n";
        cout << "   │  Options:                           │\n";
        cout << "   │  1. Mark message as read            │\n";
        cout << "   │  2. Mark message as unread          │\n";
        cout << "   │  3. Back to Main Menu               │\n";
        cout << "   └─────────────────────────────────────┘\n";
        resetColor();
        cout << "\n   Choose option: ";
        int opt;
        cin >> opt;

        if(opt == 1 || opt == 2){
            int msgNum;
            cout << "   Enter message number: ";
            cin >> msgNum;

            if(opt == 1){
                currentUser->inbox->markAsRead(msgNum - 1);
            } else{
                currentUser->inbox->markAsUnread(msgNum - 1);
            }
        }
    }

    cout << "\n   Press Enter to continue...";
    cin.ignore();
    cin.get();
}

void deleteMessage(){
    showHeader();
    setColor(CYAN);
    cout << "   === DELETE MESSAGE ===\n";
    resetColor();

    if(currentUser->inbox->isEmpty()){
        setColor(RED);
        cout << "   [INFO] No messages to delete.\n";
        resetColor();
        cout << "\n   Press Enter to continue...";
        cin.ignore();
        cin.get();
        return;
    }

    currentUser->inbox->displayMessages();
    cout << "\n   Enter message number to delete (0 to cancel): ";
    int choice;
    cin >> choice;

    if (choice == 0) return;

    Message* deleted = currentUser->inbox->removeAtIndex(choice - 1);
    if (deleted != nullptr) {
        currentUser->undoStack->push(deleted);
        setColor(GREEN);
        cout << "   [SUCCESS] Message deleted! Use Undo to recover.\n";
        resetColor();
    } else{
        setColor(RED);
        cout << "   [ERROR] Invalid message number!\n";
        resetColor();
    }

    cout << "\n   Press Enter to continue...";
    cin.ignore();
    cin.get();
}

void undoDelete(){
    showHeader();
    setColor(CYAN);
    cout << "   === UNDO DELETE ===\n";
    resetColor();

    Message* recovered = currentUser->undoStack->pop();
    if (recovered != nullptr){
        currentUser->inbox->enqueue(recovered->sender, recovered->text);
        setColor(GREEN);
        cout << "   [SUCCESS] Message recovered!\n";
        resetColor();
        delete recovered;
    } else{
        setColor(RED);
        cout << "   [INFO] Nothing to undo.\n";
        resetColor();
    }

    cout << "\n   Press Enter to continue...";
    cin.ignore();
    cin.get();
}

void manageFriends(){
    int choice;
    do{
        showHeader();
        setColor(CYAN);
        cout << "   === FRIEND MANAGEMENT ===\n";
        resetColor();
        currentUser->displayFriends();
        cout << "\n";
        setColor(YELLOW);
        cout << "   1. Add Friend\n";
        cout << "   2. Remove Friend\n";
        cout << "   3. Back to Main Menu\n";
        resetColor();
        cout << "\n   Choose option: ";
        cin >> choice;

        if(choice == 1){
            string friendName;
            cout << "   Enter friend username: ";
            cin >> friendName;

            User* friendUser = users.search(friendName);
            if(friendUser == nullptr){
                setColor(RED);
                cout << "   [ERROR] User not found!\n";
                resetColor();
            } else if(friendName == currentUser->username){
                setColor(RED);
                cout << "   [ERROR] Cannot add yourself as friend!\n";
                resetColor();
            } else if(currentUser->hasFriend(friendName)){
                setColor(RED);
                cout << "   [ERROR] Already friends!\n";
                resetColor();
            } else{
                currentUser->addFriend(friendName);
                setColor(GREEN);
                cout << "   [SUCCESS] " << friendName << " added to friends!\n";
                resetColor();
            }
        }
        else if(choice == 2){
            // Remove friend
            if(currentUser->friends == nullptr){
                setColor(BRIGHT_RED);
                cout << "   [INFO] You have no friends to remove.\n";
                resetColor();
            } else{
                currentUser->displayFriends();
                string friendName;
                cout << "   Enter friend username to remove: ";
                cin >> friendName;

                if (!currentUser->hasFriend(friendName)){
                    setColor(BRIGHT_RED);
                    cout << "   [ERROR] Not friends with this user!\n";
                    resetColor();
                } else{
                    setColor(BRIGHT_YELLOW);
                    cout << "   Confirm removal? (y/n): ";
                    char confirm;
                    cin >> confirm;

                    if (confirm == 'y' || confirm == 'Y'){
                        FriendNode* current = currentUser->friends;
                        FriendNode* prev = nullptr;

                        while(current != nullptr){
                            if(current->username == friendName){
                                if(prev == nullptr){
                                    currentUser->friends = current->next;
                                } else{
                                    prev->next = current->next;
                                }
                                delete current;
                                setColor(BRIGHT_GREEN);
                                cout << "   [SUCCESS] Friend removed!\n";
                                resetColor();
                                break;
                            }
                            prev = current;
                            current = current->next;
                        }
                    } else{
                        setColor(BRIGHT_CYAN);
                        cout << "   [INFO] Cancelled.\n";
                        resetColor();
                    }
                }
            }
        }

        if (choice != 3) {
            cout << "\n   Press Enter to continue...";
            cin.ignore();
            cin.get();
        }

    } while (choice != 3);
}

void searchUsers(){
    showHeader();
    setColor(CYAN);
    cout << "   === SEARCH USER ===\n";
    resetColor();

    string username;
    cout << "   Enter username to search: ";
    cin >> username;

    User* found = userBST.search(username);
    if (found != nullptr){
        setColor(GREEN);
        cout << "   ✅ User Found!\n";
        cout << "   Username: " << found->username << "\n";
        cout << "   Status: " << (found->isOnline ? "🟢 Online" : "⚫ Offline") << "\n";
        if(!found->isOnline){
            cout << "   Last seen: " << found->lastSeen << "\n";
        }
        resetColor();
    } else{
        setColor(RED);
        cout << "   ❌ User not found!\n";
        resetColor();
    }

    cout << "\n   Press Enter to continue...";
    cin.ignore();
    cin.get();
}

void searchMessages() {
    showHeader();
    setColor(BRIGHT_CYAN);
    cout << "   === SEARCH MESSAGES ===\n";
    resetColor();

    string keyword;
    cout << "   Enter keyword to search: ";
    cin.ignore();
    getline(cin, keyword);

    if(keyword.empty()){
        setColor(BRIGHT_RED);
        cout << "   [ERROR] Keyword cannot be empty!\n";
        resetColor();
        cout << "\n   Press Enter to continue...";
        cin.get();
        return;
    }

    setColor(BRIGHT_YELLOW);
    cout << "\n   🔍 Searching for: \"" << keyword << "\"\n";
    cout << "   ═══════════════════════════════════════════\n";
    resetColor();

    // Use the MessageQueue's search method
    currentUser->inbox->searchAndDisplay(keyword);

    cout << "\n   Press Enter to continue...";
    cin.ignore();
    cin.get();
}

void groupChatMenu() {
    int choice;
    do{
        showHeader();
        setColor(BRIGHT_CYAN);
        cout << "   === GROUP CHAT ===\n";
        resetColor();
        groupManager.listGroups();
        cout << "\n";
        setColor(BRIGHT_YELLOW);
        cout << "   1. Create Group\n";
        cout << "   2. Send Message to Group\n";
        cout << "   3. View Group Messages\n";
        cout << "   4. Add Member to Group\n";
        cout << "   5. View Group Members\n";  // NEW OPTION
        cout << "   6. Back to Main Menu\n";
        resetColor();
        cout << "\n   Choose option: ";
        cin >> choice;

        if (choice == 1) {
            string groupName;
            cout << "   Enter group name: ";
            cin >> groupName;
            groupManager.createGroup(groupName, currentUser->username);
        }
        else if (choice == 2) {
            string groupName, message;
            cout << "   Enter group name: ";
            cin >> groupName;
            cin.ignore();
            cout << "   Message: ";
            getline(cin, message);

            Group* group = groupManager.findGroup(groupName);
            if (group != nullptr) {
                group->sendMessage(currentUser->username, message);
                setColor(BRIGHT_GREEN);
                cout << "   [SUCCESS] Message sent to group!\n";
                resetColor();
            } else {
                setColor(BRIGHT_RED);
                cout << "   [ERROR] Group not found!\n";
                resetColor();
            }
        }
        else if (choice == 3) {
            string groupName;
            cout << "   Enter group name: ";
            cin >> groupName;

            Group* group = groupManager.findGroup(groupName);
            if (group != nullptr) {
                group->displayMessages();
            } else {
                setColor(BRIGHT_RED);
                cout << "   [ERROR] Group not found!\n";
                resetColor();
            }
        }
        else if (choice == 4) {
            string groupName, username;
            cout << "   Enter group name: ";
            cin >> groupName;
            cout << "   Enter username to add: ";
            cin >> username;
            groupManager.addToGroup(groupName, username, currentUser->username);
        }
        else if (choice == 5) {  // NEW: View Group Members
            string groupName;
            cout << "   Enter group name: ";
            cin >> groupName;

            Group* group = groupManager.findGroup(groupName);
            if (group != nullptr) {
                group->displayMembers();
            } else {
                setColor(BRIGHT_RED);
                cout << "   [ERROR] Group not found!\n";
                resetColor();
            }
        }

        if (choice != 6) {
            cout << "\n   Press Enter to continue...";
            cin.ignore();
            cin.get();
        }
    } while (choice != 6);
}

void viewAllUsers(){
    showHeader();
    userBST.displayAllUsers();
    cout << "\n   Press Enter to continue...";
    cin.ignore();
    cin.get();
}

void changePassword() {
    showHeader();
    setColor(CYAN);
    cout << "   === CHANGE PASSWORD ===\n";
    resetColor();

    string oldPassword, newPassword;
    cout << "   Enter current password: ";
    cin >> oldPassword;

    if (currentUser->password != oldPassword) {
        setColor(RED);
        cout << "   [ERROR] Incorrect password!\n";
        resetColor();
    } else{
        cout << "   Enter new password: ";
        cin >> newPassword;
        currentUser->password = newPassword;
        setColor(GREEN);
        cout << "   [SUCCESS] Password changed successfully!\n";
        resetColor();
    }

    cout << "\n   Press Enter to continue...";
    cin.ignore();
    cin.get();
}

// ========== ADVANCED ADMIN: REMOVE FRIEND FUNCTION ==========
void adminRemoveFriend(){
    if(!currentUser->isAdmin){
        setColor(BRIGHT_RED);
        cout << "   [ERROR] Admin access required!\n";
        resetColor();
        return;
    }

    setColor(BRIGHT_CYAN);
    cout << "\n ╔════════════════════════════════════════════════╗\n";
    cout << "   ║     ADMIN: FRIEND MANAGEMENT (Super Mode)      ║\n";
    cout << "   ╚════════════════════════════════════════════════╝\n";
    resetColor();

    int removeChoice;
    setColor(BRIGHT_YELLOW);
    cout << "\n ┌─────────────────────────────────────┐\n";
    cout << "   │  1. Remove your OWN friend          │\n";
    cout << "   │  2. Remove friend from OTHER user   │\n";
    cout << "   │  3. Back to Admin Panel             │\n";
    cout << "   └─────────────────────────────────────┘\n";
    resetColor();
    cout << "\n   Choose option: ";
    cin >> removeChoice;

    if(removeChoice == 1){
        // ===== REMOVE ADMIN'S OWN FRIEND =====
        setColor(BRIGHT_GREEN);
        cout << "\n   === Removing from YOUR friends list ===\n";
        resetColor();

        // Display admin's current friends
        if(currentUser->friends == nullptr){
            setColor(BRIGHT_RED);
            cout << "   [INFO] You have no friends to remove.\n";
            resetColor();
            return;
        }

        setColor(BRIGHT_YELLOW);
        cout << "\n   Your current friends:\n";
        resetColor();
        currentUser->displayFriends();

        string friendName;
        cout << "\n   Enter friend username to remove: ";
        cin >> friendName;

        if (!currentUser->hasFriend(friendName)){
            setColor(BRIGHT_RED);
            cout << "   [ERROR] '" << friendName << "' is not in your friends list!\n";
            resetColor();
            return;
        }

        setColor(BRIGHT_YELLOW);
        cout << "   Confirm removal from your friends? (y/n): ";
        char confirm;
        cin >> confirm;

        if(confirm == 'y' || confirm == 'Y'){
            FriendNode* current = currentUser->friends;
            FriendNode* prev = nullptr;

            while(current != nullptr){
                if (current->username == friendName) {
                    if (prev == nullptr) {
                        currentUser->friends = current->next;
                    } else{
                        prev->next = current->next;
                    }
                    delete current;
                    setColor(BRIGHT_GREEN);
                    cout << "   [SUCCESS] Friend '" << friendName << "' removed from your list!\n";
                    resetColor();
                    return;
                }
                prev = current;
                current = current->next;
            }
        }else{
            setColor(BRIGHT_CYAN);
            cout << "   [INFO] Operation cancelled.\n";
            resetColor();
        }
    }
    else if(removeChoice == 2){
        // ===== REMOVE FRIEND FROM ANOTHER USER =====
        string username, friendName;
        cout << "\n   Enter username whose friend you want to remove: ";
        cin >> username;

        User* targetUser = users.search(username);
        if (targetUser == nullptr){
            setColor(BRIGHT_RED);
            cout << "   [ERROR] User not found!\n";
            resetColor();
            return;
        }

        // Prevent removing friends from another admin
        if(targetUser->isAdmin && targetUser->username != currentUser->username){
            setColor(BRIGHT_RED);
            cout << "   [ERROR] Cannot remove friends from another admin!\n";
            resetColor();
            return;
        }

        setColor(BRIGHT_YELLOW);
        cout << "\n   Current friends of '" << username << "':\n";
        resetColor();
        targetUser->displayFriends();

        if(targetUser->friends == nullptr){
            setColor(BRIGHT_RED);
            cout << "   [INFO] This user has no friends to remove.\n";
            resetColor();
            return;
        }

        cout << "\n   Enter friend username to remove from '" << username << "': ";
        cin >> friendName;

        if (!targetUser->hasFriend(friendName)) {
            setColor(BRIGHT_RED);
            cout << "   [ERROR] '" << friendName << "' is not a friend of '" << username << "'!\n";
            resetColor();
            return;
        }

        setColor(BRIGHT_YELLOW);
        cout << "   Confirm removal? (y/n): ";
        char confirm;
        cin >> confirm;

        if (confirm == 'y' || confirm == 'Y'){
            FriendNode* current = targetUser->friends;
            FriendNode* prev = nullptr;

            while(current != nullptr){
                if (current->username == friendName){
                    if (prev == nullptr){
                        targetUser->friends = current->next;
                    } else{
                        prev->next = current->next;
                    }
                    delete current;
                    setColor(BRIGHT_GREEN);
                    cout << "   [SUCCESS] Friend '" << friendName << "' removed from '" << username << "'s friends!\n";
                    resetColor();
                    return;
                }
                prev = current;
                current = current->next;
            }
        } else{
            setColor(BRIGHT_CYAN);
            cout << "   [INFO] Operation cancelled.\n";
            resetColor();
        }
    }
}

// ========== END OF ADMIN REMOVE FRIEND FUNCTION ==========
void adminPanel(){
    if(!currentUser->isAdmin){
        setColor(BRIGHT_RED);
        cout << "   [ERROR] Admin access required!\n";
        resetColor();
        return;
    }

    int choice;
    do{
        showHeader();
        setColor(BRIGHT_RED);
        cout << "   ╔══════════════════════════════════════╗\n";
        cout << "   ║           ADMIN PANEL                ║\n";
        cout << "   ╚══════════════════════════════════════╝\n";
        resetColor();
        cout << "\n";
        setColor(BRIGHT_YELLOW);
        cout << "   ┌─────────────────────────────────────┐\n";
        cout << "   │  1. List All Users                  │\n";
        cout << "   │  2. Delete User                     │\n";
        cout << "   │  3. Manage Friends (Admin Mode)     │\n";
        cout << "   │  4. View System Stats               │\n";
        cout << "   │  5. Backup Data                     │\n";
        cout << "   │  6. Back to Main Menu               │\n";
        cout << "   └─────────────────────────────────────┘\n";
        resetColor();
        cout << "\n   Choose option: ";
        cin >> choice;

        if(choice == 1){
            userBST.displayAllUsers();
        }
        else if(choice == 2){
            string username;
            cout << "   Enter username to delete: ";
            cin >> username;

            if(username == currentUser->username){
                setColor(BRIGHT_RED);
                cout << "   [ERROR] Cannot delete yourself!\n";
                resetColor();
            } else{
                User* userToDelete = users.search(username);
                if(userToDelete == nullptr){
                    setColor(BRIGHT_RED);
                    cout << "   [ERROR] User not found!\n";
                    resetColor();
                }
                else if(userToDelete->isAdmin){
                    setColor(BRIGHT_RED);
                    cout << "   [ERROR] Cannot delete another admin!\n";
                    resetColor();
                }
                else if (users.remove(username)){
                    userBST.rebuildFromHashTable(users);
                    setColor(BRIGHT_GREEN);
                    cout << "   [SUCCESS] User '" << username << "' deleted!\n";
                    resetColor();
                } else{
                    setColor(BRIGHT_RED);
                    cout << "   [ERROR] Failed to delete user!\n";
                    resetColor();
                }
            }
        } else if(choice == 3){
            adminRemoveFriend();  // Call the advanced friend management function
        } else if(choice == 4){
            setColor(BRIGHT_CYAN);
            cout << "\n ╔══════════════════════════════════════╗\n";
            cout << "   ║         SYSTEM STATISTICS            ║\n";
            cout << "   ╚══════════════════════════════════════╝\n";
            resetColor();

            vector<User*> allUsers = users.getAllUsers();
            int onlineCount = 0;
            int totalMessages = 0;
            int totalFriends = 0;
            int totalGroups = 0;

            for(User* u : allUsers){
                if (u->isOnline) onlineCount++;
                totalMessages += u->inbox->getSize();

                // Count friends
                FriendNode* f = u->friends;
                while (f != nullptr){
                    totalFriends++;
                    f = f->next;
                }
            }

            setColor(BRIGHT_GREEN);
            cout << "\n   • Total Users: " << allUsers.size() << "\n";
            cout << "   • Online Users: " << onlineCount << "\n";
            cout << "   • Offline Users: " << (allUsers.size() - onlineCount) << "\n";
            cout << "   • Total Messages: " << totalMessages << "\n";
            cout << "   • Total Friend Connections: " << totalFriends << "\n";
            resetColor();
        } else if(choice == 5){
            backupData();
        }

        if(choice != 6){
            cout << "\n   Press Enter to continue...";
            cin.ignore();
            cin.get();
        }
    } while (choice != 6);
}

void logout(){
    if (currentUser != nullptr) {
        currentUser->isOnline = false;
        currentUser->lastSeen = getCurrentTimestamp();
        currentUser = nullptr;
    }
    setColor(GREEN);
    cout << "   [INFO] Logged out successfully!\n";
    resetColor();
}

public:
    ChatSystem() : currentUser(nullptr), running(true){
    loadData();
    // Create admin user if none exists
    if (users.search("admin") == nullptr){
        User* admin = new User("admin", "adminF13", true);
        users.insert(admin);
        userBST.insert(admin);
    }
    }

    ~ChatSystem(){
        saveData();
    }

    void run(){
        while(running){
            if(currentUser == nullptr){
                // ========== MAIN MENU (Not Logged In) ==========
                showMainMenu();
                int choice;
                cin >> choice;

                switch(choice){
                    case 1: registerUser(); break;
                    case 2: login(); break;
                    case 3: forgotPassword(); break;
                    case 4: running = false; break;
                    default:
                    setColor(BRIGHT_RED);
                    cout << "   [ERROR] Invalid option!\n";
                    resetColor();
                    cout << "   Press Enter to continue...";
                    cin.ignore();
                    cin.get();
                }
        } else{
            // ========== USER MENU (Logged In) ==========
            showUserMenu();
            int choice;
            cin >> choice;

            switch(choice){
                case 1: sendPrivateMessage(); break;
                case 2: viewInbox(); break;
                case 3: deleteMessage(); break;
                case 4: undoDelete(); break;
                case 5: manageFriends(); break;
                case 6: searchUsers(); break;
                case 7: groupChatMenu(); break;
                case 8: viewAllUsers(); break;
                case 9: changePassword(); break;
                case 10: searchMessages(); break;
                case 11: forwardMessage(); break;
                case 12: logout(); break;
                case 13:
                    if(currentUser->isAdmin){
                        adminPanel();
                    } else{
                        setColor(BRIGHT_RED);
                        cout << "   [ERROR] Invalid option!\n";
                        resetColor();
                    }
                    break;
                default:
                setColor(BRIGHT_RED);
                cout << "   [ERROR] Invalid option!\n";
                resetColor();
                cout << "   Press Enter to continue...";
                cin.ignore();
                cin.get();
            }
        }
        }

        setColor(BRIGHT_CYAN);
        cout << "\n   Thank you for using FNA Chat System!\n";
        resetColor();
    }
};

// ======================== MAIN FUNCTION ========================
int main(){
    // Set console for UTF-8 support (for emojis)
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
    #endif

    ChatSystem chatSystem;
    chatSystem.run();

    return 0;
}
