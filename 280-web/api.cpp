// Project UID c1f28c309e55405daf00c565d57ff9ad
//
//  main.cpp
//  p4-web
//
//  Created by Rahul Suri on 3/21/23.
//

#include <iostream>
#include <cstdio>
#include <list>
#include "json.hpp"

using nlohmann::json;
using namespace std;

class OHQueue {
public:
    
    OHQueue() {
        head = nullptr;
        tail = nullptr;
    }
    
    bool is_empty () const {
        if (head == nullptr) {
            return true;
        }
        return false;
    }
    
    
    void add_queue(string uniq, string loc) {
        
        //first initialize the student and their data and then add them to the list
        Student *newStudent = new Student();
        newStudent->uniqname = uniq;
        newStudent->location = loc;
        newStudent->position = students;
        newStudent->next = nullptr;
        newStudent->prev = tail;
        
        //if the list is empty, the added student is now the head and the tail
        if (tail == nullptr) {
            head = newStudent;
            tail = newStudent;
        }
        //if the list is not empty, the new student is now the tail
        else {
            tail->next = newStudent;
            tail = newStudent;
        }
        
        //changing private members accordingly
        queue.push_front(*newStudent);
        students++;
        
        //creating a json obj for response
        json output = {
            {"location", newStudent->location},
            {"position", newStudent->position},
            {"uniqname", newStudent->uniqname}
        };
        
        string output_str = output.dump(4) + "\n";
        
        //now, we must create the message thet the server will return to the user.
        cout << "HTTP/1.1 201 Created\n" <<
        "Content-Type: application/json; charset=utf-8\n" <<
        "Content-Length: " << output_str.length() << "\n\n" <<
        output_str;
        
    }
    
    
    //removing the student who is at the front of the queue
    void remove_queue() {
        if (head == nullptr) {
            return;
        }
        else {
            if (head->next == nullptr) {
                delete head;
            }
            tail = tail->prev;
            delete tail;
        }
        
        //iterating through the queue list to edit positions
        list<Student>::iterator it;
        
        for (it = queue.begin(); it != queue.end(); ++it) {
            it->position--;
        }
        
        //changing private members accordingly
        queue.pop_front();
        students--;
        
        //now, we must create the server response
        cout << "HTTP/1.1 204 No Content\n" <<
        "Content-Type: application/json; charset=utf-8\n" <<
        "Content-Length: 0\n\n";
    }
    
    //return the entire queue to ostream (/api/queue/)
    void get_queue() {
        json jsonobj;
        int count = 0;
        
        list<Student>::iterator it;
        
        //iterating through the queue and creating a json object using all info
        for (auto it = queue.rbegin(); it != queue.rend(); ++it) {
            json student = {
                {"location", it->location},
                {"position", it->position},
                {"uniqname", it->uniqname}
            };
            
            jsonobj.push_back(student);
            count++;
        }
        
        json results = {
            {"count", count},
            {"results", jsonobj}
        };
        
        
        //converting the json object to a string and finding length
        string jsonstr = results.dump(4) + "\n";
        size_t length = jsonstr.length();
        
        //creating the output message
        cout << "HTTP/1.1 200 OK\n"
        "Content-Type: application/json; charset=utf-8\n"
        "Content-Length: " << length << "\n\n" <<
        jsonstr;
    }
    
    //return the first person in the queue
    void get_head() const {
        json jsonobj;
        jsonobj = {
            {"location", head->location},
            {"position", head->position},
            {"uniqname", head->uniqname}
        };
        
        //converting the json object to a string and finding length
        string json_str = jsonobj.dump(4) + "\n";
        size_t length = json_str.length();
        
        //creating the output message
        cout << "HTTP/1.1 200 OK\n" <<
        "Content-Type: application/json; charset=utf-8\n" <<
        "Content-Length: " << length << "\n\n" <<
        json_str;
    }
    
    //if user submits an info request, (GET /api/), ouput info
    void get_info() const {
        json info;
        info = {
            {"queue_head_url", "http://localhost/queue/head/"},
            {"queue_list_url", "http://localhost/queue/"},
            {"queue_tail_url", "http://localhost/queue/tail/"}
        };
        string info_str = info.dump(4) + "\n";
        
        cout << "HTTP/1.1 200 OK\n" <<
        "Content-Type: application/json; charset=utf-8\n" <<
        "Content-Length: 160\n\n" <<
        info_str;
    }
    
    //creating error output if the HTTP path is incorrect
    void throw_error() const {
        cout << "HTTP/1.1 400 Bad Request\n" <<
        "Content-Type: application/json; charset=utf-8\n" <<
        "Content-Length: 0\n\n";
    }
    
    //the functions that will now be called by main
    
    //if its a get request..
    void get_request(string endpoint) {
        if (endpoint == "/api/") {
            get_info();
        } else if (endpoint == "/api/queue/") {
            get_queue();
        } else if (endpoint == "/api/queue/head/") {
            get_head();
        } else {
            throw_error();
        }
    }
    
    //if its a push request
    void post_request(string endpoint) {
        if (endpoint == "/api/queue/tail/") {
            string junk;
            json info;
            while (cin >> junk) {
                if (junk == "Content-Length:") {
                    cin >> junk;
                    cin >> info;
                    break;
                }
            }
            add_queue(info["uniqname"], info["location"]);
        } else {
            throw_error();
        }
    }
    
    void delete_request(string endpoint) {
        if (endpoint == "/api/queue/head/") {
            if (is_empty()) {
                throw_error();
            } else {
                remove_queue();
            }
        } else {
            throw_error();
        }
    }

private:
    struct Student {
        string uniqname;
        string location;
        int position;
        Student *prev;
        Student *next;
    };
    list<Student> queue;
    Student *head;
    Student *tail;
    int students = 1;
};

int main(int argc, const char * argv[]) {
    //input redirection
    #ifdef __APPLE__
    freopen("test02.in", "r", stdin);
    #endif
    
    //initializing our queue;
    OHQueue main_queue;
    
    //reading in info from cin using while loop
    string method;
    string endpoint;

    while (cin >> method >> endpoint) {
        //handling GET requests
        if (method == "GET") {
            main_queue.get_request(endpoint);
        }
        //handing POST requests
        else if (method == "POST") {
            main_queue.post_request(endpoint);
        }
        //handing DELETE requests
        else if (method == "DELETE") {
            main_queue.delete_request(endpoint);
        }
        else if (method == "EXIT") {
            return 0;
        }
    }
    return 0;
}
