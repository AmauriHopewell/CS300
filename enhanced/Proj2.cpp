//============================================================================
// Name        : Proj2.cpp
// Author      : Amauri Hopewell
// Version     : 2.0 (CS-499 Milestone Three)
// Copyright   : Copyright 2023 SNHU COCE
// Description : ABCU course catalog. Loads a CSV of course number, title, and
//               prerequisites, drops rows whose prerequisites are not in the
//               file, and stores every valid course in three structures:
//               a vector (CS-300 baseline), a binary search tree, and a hash
//               map. Print-all is an in-order BST walk. Search uses the hash
//               map. Compare mode times all three. Menu 6 prints a topological
//               schedule so a course never appears before its prerequisites.
//               Sort key is course number.
//============================================================================

#include <algorithm>
#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

struct Course {
    string number;
    string title;
    vector<string> prereqNumbers;

    bool operator<(const Course& other) const {
        return number < other.number;
    }

    bool operator==(const Course& other) const {
        return number == other.number;
    }

    bool operator>(const Course& other) const {
        return number > other.number;
    }

    void printSummary() const {
        cout << "Number : " << number << " Title : " << title << endl;
    }

    void printAll() const {
        cout << "Number : " << number << " Title : " << title << endl;
        cout << "Prerequisites: ";
        if (prereqNumbers.empty()) {
            cout << "No prerequisites";
        } else {
            for (size_t i = 0; i < prereqNumbers.size(); i++) {
                cout << prereqNumbers[i];
                if (i + 1 < prereqNumbers.size()) {
                    cout << ", ";
                }
            }
        }
        cout << "\n";
    }
};

class CourseBST {
public:
    CourseBST() = default;
    ~CourseBST() { clear(); }

    CourseBST(const CourseBST&) = delete;
    CourseBST& operator=(const CourseBST&) = delete;

    CourseBST(CourseBST&& other) noexcept : root(other.root) {
        other.root = nullptr;
    }

    CourseBST& operator=(CourseBST&& other) noexcept {
        if (this != &other) {
            clear();
            root = other.root;
            other.root = nullptr;
        }
        return *this;
    }

    void clear() {
        destroy(root);
        root = nullptr;
    }

    void insert(const Course& course) {
        root = insertNode(root, course);
    }

    const Course* find(const string& number) const {
        Node* n = root;
        while (n != nullptr) {
            if (number == n->course.number) {
                return &n->course;
            }
            n = (number < n->course.number) ? n->left : n->right;
        }
        return nullptr;
    }

    void printInOrder() const {
        inOrder(root);
    }

private:
    struct Node {
        Course course;
        Node* left = nullptr;
        Node* right = nullptr;
        explicit Node(const Course& c) : course(c) {}
    };

    Node* root = nullptr;

    static Node* insertNode(Node* n, const Course& course) {
        if (n == nullptr) {
            return new Node(course);
        }
        if (course.number < n->course.number) {
            n->left = insertNode(n->left, course);
        } else if (course.number > n->course.number) {
            n->right = insertNode(n->right, course);
        }
        return n;
    }

    static void inOrder(const Node* n) {
        if (n == nullptr) {
            return;
        }
        inOrder(n->left);
        n->course.printSummary();
        inOrder(n->right);
    }

    static void destroy(Node* n) {
        if (n == nullptr) {
            return;
        }
        destroy(n->left);
        destroy(n->right);
        delete n;
    }
};

struct Catalog {
    vector<Course> vectorStore;
    CourseBST bst;
    unordered_map<string, Course> hash;

    void clear() {
        vectorStore.clear();
        bst.clear();
        hash.clear();
    }

    bool empty() const {
        return vectorStore.empty();
    }

    size_t size() const {
        return vectorStore.size();
    }
};

vector<string> splitCsv(const string& row) {
    vector<string> cells;
    string cell;
    for (char ch : row) {
        if (ch == ',') {
            cells.push_back(cell);
            cell.clear();
        } else if (ch != '\r') {
            cell += ch;
        }
    }
    cells.push_back(cell);
    return cells;
}

bool courseExists(const vector<string>& courseNumbers, const string& testNumber) {
    for (const string& num : courseNumbers) {
        if (num == testNumber) {
            return true;
        }
    }
    return false;
}

bool validLineFormat(const vector<string>& courseNumbers, const vector<string>& line) {
    if (line.size() < 2) {
        return false;
    }
    for (size_t i = 2; i < line.size(); i++) {
        if (line[i].empty()) {
            continue;
        }
        if (!courseExists(courseNumbers, line[i])) {
            return false;
        }
    }
    return true;
}

const Course* linearFind(const vector<Course>& courses, const string& courseNumber) {
    for (const Course& course : courses) {
        if (course.number == courseNumber) {
            return &course;
        }
    }
    return nullptr;
}

void fillBalanced(CourseBST& bst, const vector<Course>& courses, int lo, int hi);

bool loadCatalog(const string& filename, Catalog& catalog) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Could not open file: " << filename << "\n";
        return false;
    }

    vector<string> courseNumbers;
    vector<vector<string>> allRows;
    string currentRow;

    while (getline(file, currentRow)) {
        if (currentRow.empty()) {
            continue;
        }
        vector<string> cells = splitCsv(currentRow);
        if (cells.size() >= 2 && !cells[0].empty()) {
            allRows.push_back(cells);
            courseNumbers.push_back(cells[0]);
        }
    }

    catalog.clear();
    for (const vector<string>& delimitedRow : allRows) {
        if (!validLineFormat(courseNumbers, delimitedRow)) {
            continue;
        }
        Course newCourse;
        newCourse.number = delimitedRow[0];
        newCourse.title = delimitedRow[1];
        for (size_t i = 2; i < delimitedRow.size(); i++) {
            if (!delimitedRow[i].empty()) {
                newCourse.prereqNumbers.push_back(delimitedRow[i]);
            }
        }
        catalog.vectorStore.push_back(newCourse);
    }

    for (Course& c : catalog.vectorStore) {
        stable_sort(c.prereqNumbers.begin(), c.prereqNumbers.end());
    }
    stable_sort(catalog.vectorStore.begin(), catalog.vectorStore.end());

    for (const Course& c : catalog.vectorStore) {
        catalog.hash[c.number] = c;
    }
    // Inserting a sorted list into a BST would make a linked list.
    // Median-first fill keeps the tree balanced so search is actually O(log n).
    fillBalanced(catalog.bst, catalog.vectorStore, 0,
                 static_cast<int>(catalog.vectorStore.size()) - 1);
    return true;
}

void fillBalanced(CourseBST& bst, const vector<Course>& courses, int lo, int hi) {
    if (lo > hi) {
        return;
    }
    int mid = lo + (hi - lo) / 2;
    bst.insert(courses[mid]);
    fillBalanced(bst, courses, lo, mid - 1);
    fillBalanced(bst, courses, mid + 1, hi);
}

void printComplexityTable() {
    cout << "\nStructure     Search           Print all (sorted)\n";
    cout << "vector        O(n)              O(n log n) to sort, then O(n)\n";
    cout << "BST           O(log n) average  O(n) in-order walk\n";
    cout << "hash map      O(1) expected     not ordered\n";
    cout << "This program searches with the hash map and prints with the BST.\n";
    cout << "The vector is kept so compare mode can time the CS-300 baseline.\n";
    cout << "Topological schedule is Kahn's algorithm, O(V + E).\n";
}

void compareSearch(const Catalog& catalog, const string& courseNumber) {
    const int trials = 50000;
    const Course* fromVector = nullptr;
    const Course* fromBst = nullptr;
    const Course* fromHash = nullptr;

    auto t0 = chrono::high_resolution_clock::now();
    for (int i = 0; i < trials; i++) {
        fromVector = linearFind(catalog.vectorStore, courseNumber);
    }
    auto t1 = chrono::high_resolution_clock::now();
    for (int i = 0; i < trials; i++) {
        fromBst = catalog.bst.find(courseNumber);
    }
    auto t2 = chrono::high_resolution_clock::now();
    for (int i = 0; i < trials; i++) {
        auto it = catalog.hash.find(courseNumber);
        fromHash = (it == catalog.hash.end()) ? nullptr : &it->second;
    }
    auto t3 = chrono::high_resolution_clock::now();

    auto us = [](chrono::high_resolution_clock::time_point a,
                 chrono::high_resolution_clock::time_point b) {
        return chrono::duration_cast<chrono::microseconds>(b - a).count();
    };

    cout << "Search for " << courseNumber << " (" << trials << " trials)\n";
    cout << "vector O(n)     : " << us(t0, t1) << " us\n";
    cout << "bst    O(log n) : " << us(t1, t2) << " us\n";
    cout << "hash   O(1)     : " << us(t2, t3) << " us\n";

    const Course* found = fromHash ? fromHash : (fromBst ? fromBst : fromVector);
    if (found) {
        found->printAll();
    } else {
        cout << "Course " << courseNumber << " not found.\n";
    }
}

void printTopoSchedule(const Catalog& catalog) {
    unordered_map<string, int> indegree;
    unordered_map<string, vector<string>> dependents;

    for (const Course& c : catalog.vectorStore) {
        indegree[c.number] = 0;
    }
    for (const Course& c : catalog.vectorStore) {
        for (const string& pre : c.prereqNumbers) {
            if (indegree.count(pre) == 0) {
                continue;
            }
            indegree[c.number]++;
            dependents[pre].push_back(c.number);
        }
    }

    priority_queue<string, vector<string>, greater<string>> ready;
    for (const auto& kv : indegree) {
        if (kv.second == 0) {
            ready.push(kv.first);
        }
    }

    vector<string> order;
    while (!ready.empty()) {
        string n = ready.top();
        ready.pop();
        order.push_back(n);
        for (const string& d : dependents[n]) {
            indegree[d]--;
            if (indegree[d] == 0) {
                ready.push(d);
            }
        }
    }

    if (order.size() != catalog.size()) {
        cout << "Cycle in prerequisites; cannot build a full schedule.\n";
        return;
    }

    cout << "Schedule in prerequisite order (a course never appears before its prereqs):\n\n";
    for (const string& n : order) {
        catalog.hash.at(n).printSummary();
    }
}

void searchHash(const Catalog& catalog, const string& courseNumber) {
    auto it = catalog.hash.find(courseNumber);
    if (it == catalog.hash.end()) {
        cout << "Course " << courseNumber << " not found.\n";
        return;
    }
    it->second.printAll();
}

int main(int argc, char* argv[]) {
    Catalog catalog;
    string filename = (argc > 1) ? argv[1] : "ABCU_Advising_Program_Input.csv";
    string searchInput;
    int choice = 0;
    bool done = false;

    cout << "Welcome to the course planner.\n";

    while (!done) {
        cout << "\n1. Load Data Structure\n";
        cout << "2. Print Course List\n";
        cout << "3. Print Course\n";
        cout << "4. Compare search (vector vs BST vs hash)\n";
        cout << "5. Print complexity table\n";
        cout << "6. Print schedule in prerequisite order\n";
        cout << "9. Exit\n\n";
        cout << "What would you like to do? ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "That is not a valid option.\n";
            continue;
        }

        switch (choice) {
        case 1:
            if (loadCatalog(filename, catalog)) {
                cout << catalog.size() << " courses loaded from " << filename << ".\n";
            }
            break;

        case 2:
            if (catalog.empty()) {
                cout << "Cannot print course list yet; you must load courses first.\n";
            } else {
                cout << "Here is a sample schedule:\n\n";
                catalog.bst.printInOrder();
            }
            break;

        case 3:
            if (catalog.empty()) {
                cout << "Cannot search course list yet; you must load courses first.\n";
            } else {
                cout << "What course do you want to know about? ";
                cin >> searchInput;
                searchHash(catalog, searchInput);
            }
            break;

        case 4:
            if (catalog.empty()) {
                cout << "Cannot compare yet; you must load courses first.\n";
            } else {
                cout << "What course do you want to time? ";
                cin >> searchInput;
                compareSearch(catalog, searchInput);
            }
            break;

        case 5:
            printComplexityTable();
            break;

        case 6:
            if (catalog.empty()) {
                cout << "Cannot print a schedule yet; you must load courses first.\n";
            } else {
                printTopoSchedule(catalog);
            }
            break;

        case 9:
            done = true;
            cout << "Thank you for using the course planner!" << endl;
            break;

        default:
            cout << choice << " is not a valid option.\n";
            break;
        }
    }

    return 0;
}
