//============================================================================
// Name        : Proj2.cpp
// Author      : Amauri Hopewell
// Version     : 1.0
// Copyright   : Copyright © 2023 SNHU COCE
// Description : CS300 project 2
// This project loads a CSV file filled with university courses, 
// consisting of a number, title, and possible prerequisite list.
// It sorts through the courses to ensure each one has a valid prerequisite, and then 
// sorts the course list by title, allowing the user to load the courses, display the courses,
// and search for a specific course with a text input menu.
// To store the courses, I created a special struct, and used a vector-based approach for my main data structure,
// because vectors are simple to work with and have many pre-built algorithms in C++.
//============================================================================

//Include files necessary for printing, file input, vectors, and sorting
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std; //use namespace STD for convenience

//struct to hold course information
struct Course {
    string number;
    string title;
    vector<string> prereqNumbers;

    // Less than operator (for ordering and thus sorting)
    bool operator<(const Course& other) const {
        int comparison = number.compare(other.number);
        if (comparison < 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    // Greater than operator (for ordering and thus sorting)
    bool operator==(const Course& other) const {
        int comparison = number.compare(other.number);
        if (comparison > 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    
    // Equal to operator (for ordering and thus sorting)
    bool operator>(const Course& other) const {
        int comparison = number.compare(other.number);
        if (comparison == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    void printSummary() //print just basic information about a course
    {
        cout << "Number : " << number << " Title : " << title;
        cout << endl;
    }

    void printAll() //print all information about a course
    {
        cout << "Number : " << number << " Title : " << title;
        cout << endl << "Prerequisites: ";

        if (prereqNumbers.size() > 0)
        {
            
            for (int i=0; i < prereqNumbers.size(); i++)
            {
                cout << prereqNumbers[i];
                if (i < prereqNumbers.size() - 1)//do not add a separating comma after the last prerequisite
                {
                   cout << ", ";
                }
            }
        }
        else {
            cout << "No prerequisites";
        }
        cout << "\n";
    }

};


//The following function is used for the 2-pass approach for loading the file
//to ensure that a prerequisite exists
bool courseExists(vector<string> courseNumbers, string testNumber) {
    for (string num : courseNumbers)
    {
        if (num.compare(testNumber) == 0)
        {
            return true;
        }
        
    }
    return false; //only activates if a match is not found   
}


// The following function is used for the 2-pass approach for loading the file
//to ensure the format for a given line (input as a vector of cells, with each cell a string) is valid
bool validLineFormat(vector<string> courseNumbers, vector<string> line) {
    int count = line.size();
    if (count == 2) //If there are no prerequisites
    {
        return true;
    }
    else if (count > 2) //if there are 3 or more parameters (and so possible perequisites)
    {
        for (int i = 2; i < line.size(); i++) //loop through prerequisite courses
        {
            if (courseExists(courseNumbers, line[i])) //check if the prerequisite is in the course list
            {
                ; //   do nothing if prerequisite is valid
            }
            else
            {
                return false; //if prerequisite not in list, quit looping and discard line
            }
        }
        return true; // if no invalid courses detected in for loop
    }
    else //if 1 or 0 parameters, cannot be a valid line
    {
        return false; //discard line
    }
}


void searchCourse(vector<Course> courses, string courseNumber) { //Find if a course is in the list of courses
    for (Course course : courses)
    {
        if (course.number.compare(courseNumber) == 0) //if the coursenumbers match
        {
            course.printAll(); //print detailed course information
            return; //stop once a match is found
        }
    }
    cout << "Course " << courseNumber << " not found.\n"; //This line is only reached if no match is found
}

//This loads all the courses from a file, by opening the file, 
// performing a first pass to load all the data from the file,
// then performing a second pass to ensure only those with valid prereqs are loaded, and 
// finally, sorting the courses by title. 
vector<Course> readFile(string filename) {
    vector<string> courseNumbers; //for formatting check
    vector<Course> courses; //for final storage
    vector<vector<string>> allRows; //for handling variable number of columns as described here https://stackoverflow.com/questions/63141038/c-read-a-file-with-arbitrary-number-of-rows-and-columns-into-a-vector-of-vect
    string currentRow; //for storing current row before splitting different cells apart  
    
    //open the file
    ifstream file;  
    file.open(filename);

    //1st pass over the data: read each line in from the file, and split up the cells 
    while (getline(file, currentRow))
    {
        vector<string> currentRowDelimited; // for storing current row after splitting different cells apart
        string currentCell="";
        for (int i = 0; i < currentRow.size(); i++) //go through all characters in row
        {
            if (currentRow.at(i) == ',') //split on commas
            {
                if (!(currentCell == ""))//if string is not empty, we've completed parsing a cell
                {
                    currentRowDelimited.push_back(currentCell);
                }
                currentCell = "";//reset since we hit the delimiter
            }
            else
            {
                currentCell = currentCell + currentRow.at(i);//add the next character in the cell
            }
        }
        if (currentRowDelimited.size() >= 2) //if there are at least 2 parameters in this row
        {
            allRows.push_back(currentRowDelimited); //hold all values for later validation
            courseNumbers.push_back(currentRowDelimited[0]);//record course number for later validation
        }
    }

    //2nd pass over the data: ensure each row is valid by making sure there are no courses with invalid prerequisites
    for (vector<string> delimitedRow : allRows)
    {
        bool lineValid = validLineFormat(courseNumbers, delimitedRow);//check if each line is in valid format
        if (lineValid) //If line is valid, create list of prerequisites
        {
            vector<string> preRequisites;
            for (int i = 2; i < delimitedRow.size(); i++) //start at the 3rd parameter, which will be the 1st prereq
                                                          //If there are no prereqs, the loop will not execute,
                                                           //so there won't be a problem
            {
                preRequisites.push_back(delimitedRow[i]); 
            }

            //use prerequisite list and build course
            Course newCourse;  
            newCourse.number = delimitedRow[0]; 
            newCourse.title = delimitedRow[1]; 
            newCourse.prereqNumbers= preRequisites;//it's ok if there are no prereqs; it will just be bull

            //add to new list of courses
            courses.push_back(newCourse);
        }
        else
        {
            ; //if line is not valid, do nothing, discard the line
        }
    }

    //step 4: sort the valid course list before returning
    stable_sort(courses.begin(), courses.end());//ensure the courses are sorted alphanumerically, using built-in funciton from <algorithm>
    for (Course c : courses) //With courses sorted, sort the prereqs for each course
    {
        stable_sort(c.prereqNumbers.begin(), c.prereqNumbers.end());//ensure the prerequisite list is sorted too
    }

    return courses; //return the sorted list
}









/**
 * The one and only main() method
 */
int main(int argc, char* argv[]) {
    //create variables for processing
    vector<Course> courseList; 
    string searchInput; //for user input
    string menuString = "\n1. Load Data Structure \n2. Print Course List. \n3. Print Course.\n9. Exit.\n"; 
    bool exit = false;

    cout << "Welcome to the course planner.\n"; //initial message

    while (exit == false)
    {
        // process command line arguments
        cout << menuString << endl;
        cout << "What would you like to do? ";
        cin >> argc; 
        switch (argc) {

        case 1: //load courses if 1 is pressed
            courseList = readFile("CS 300 ABCU_Advising_Program_Input.csv");
            cout << "Courses loaded succesfully.\n";
            break;

        case 2: //print courses or note no courses loaded
            if (courseList.size() == 0)
            {
                cout << "Cannot print course list yet; you must load courses first.\n";
            }
            else
            {
                cout << "Here is a sample schedule:\n\n";
                for (Course c : courseList)
                {
                    c.printSummary(); //print out basic course information, but not prereqs (as shown in sample output)
                }
            }
            break;
        
        case 3: //allow searching for a specific course
            if (courseList.size() == 0)
            {
                cout << "Cannot search course list yet; you must load courses first.\n";
            }
            else
            {
                cout << "What course do you want to know about? ";
                cin >> searchInput;
                searchCourse(courseList, searchInput); //search for course and print out detailed information if found
            }
            break;

        case 9: //exit key
            exit = true; 
            cout << "Thank you for using the course planner!" << endl;
            break;

        default: //Tell user the input is invalid if not among menu choices
            cout << argc << " is not a valid option.\n"; 
        }
    }
    
    return 0;
}
