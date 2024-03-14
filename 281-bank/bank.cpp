//Project Identifier: 292F24D17A4455C1B5133EDD8C7CEAA0C9570A98
//
//  bank.cpp
//  281-p3
//
//  Created by Rahul Suri on 10/26/23.
//

#include "bank.hpp"

using namespace std;

void bank::printHelp() {
    cout << "To create an EECS 281 bank, please do the following: " << "\n";
    cout << "-f / --file [filename] " << "\n";
    cout << "Enter a filename to register users first" << "\n";
    cout << "-v / --verbose " << "\n";
    cout << "Print useful messages about processes taking place";
}

void bank::get_opts(int c, char ** v) {
    // These are used with getopt_long()
    opterr = false; //"opting" out of letting the program handle our errors for us (false)
    int choice; //option that our user chooses
    int choice_index = 0;
    struct option long_options[] = {
        {"file", required_argument, nullptr, 'f'},
        {"verbose", no_argument, nullptr, 'v'},
        {"median", no_argument, nullptr, 'm'},
        {"help", no_argument, nullptr, 'h'},
        {nullptr, 0, nullptr, '\0'}
  };  // long_options[]

  while ((choice = getopt_long(c, v, "f:vh", long_options, &choice_index)) != -1) {
      switch (choice) {
          case 'f':
              registered = true;
              regFile = optarg;
              break;
          case 'v':
              verbose = true;
              break;
          case 'h':
              printHelp();
              exit(0);
    }  // switch ..choice
  }  // while
}  // if ..mode
// getMode()

unsigned long long bank::convert_time(string time) {
    string filtered = "000000000000";
    
    uint32_t j = 0;
    for (uint32_t i = 0; i < time.length(); i++) {
        if (time[i] != ':') {
            filtered[j] = time[i];
            j++;
        }
    }
    
    return stoull(filtered);
}

void bank::registration() {
    ifstream file(regFile);
    string line;
    
    if(!file.is_open()) {
        cerr << "Registration file failed to open\n";
        exit(1);
    }
    
    while(getline(file, line)) {
        istringstream iss(line);
        
        string ti;
        string id;
        string pin;
        string bal;
        
        getline(iss, ti, '|');
        getline(iss, id, '|');
        getline(iss, pin, '|');
        getline(iss, bal, '|');
    
        vector<place*> incoming;
        vector<place*> outgoing;
        unordered_set<string> ips;
        profile info{convert_time(ti), static_cast<uint32_t>(stoi(pin)), static_cast<uint32_t>(stoi(bal)), incoming, outgoing, false, ips};
        users[id] = info;
    }//while
    
    file.close();
}//registration()

void bank::login(string id, uint32_t pin, string ip) {
    if (users.find(id) != users.end() && users[id].pin == pin) {
        if (verbose == true) {
            cout << "User " << id << " logged in.\n";
        }
        if (users[id].ips.find(ip) == users[id].ips.end()) {
            curr_users.push_back(id);
        }
        users[id].ips.insert(ip);
    }
    else {
        if (verbose == true) {
            cout << "Failed to log in " << id << ".\n";
        }
    }
}

void bank::logout(string id, string ip) {
    auto it = users[id].ips.find(ip);
    auto currit = find(curr_users.begin(), curr_users.end(), id);
    if (currit != curr_users.end() && it != users[id].ips.end()) {
        curr_users.erase(currit);
        string ip = *it;
        users[id].ips.erase(it);
        
        if (verbose == true) {
            cout << "User " << id << " logged out.\n";
        }
    }
    else {
        if (verbose == true) {
            cout << "Failed to log out " << id << ".\n";
        }
    }
}

bool bank::fraud_check(place placement) {
    string sr = placement.sender;
    if (find(users[sr].ips.begin(), users[sr].ips.end(), placement.ip) != users[sr].ips.end()) {
        //no fraud
        return false;
    }
    else {
        //ip was not in the senders list, thus there is fraud (true)
        return true;
    }
}

bool bank::discounted(string user, place transaction) {
    if (transaction.exec - users[user].reg_time > 50000000000) {
        return true;
    }
    else {
        return false;
    }
}

void bank::execute_trans() {
    place* curr_trans = transactions.top();
    
//    cout << " Looking at transaction: " << curr_trans->id << ".\n";
    //calculating fee
    uint32_t fee = curr_trans->amt / 100;
    if (fee > 450) {
        fee = 450;
    }
    else if (fee < 10) {
        fee = 10;
    }
    
    if (discounted(curr_trans->sender, *curr_trans)) {
        fee = (fee * 3) / 4;
    }
    
    //if (o)nly sender is covering fee, do appropriate checks
    if (curr_trans->cover == 'o') {
        
        //checking that the sender can afford the transaction amount as well as the fee
        if (users[curr_trans->sender].balance >= (curr_trans->amt + fee)) {
            
            //sending money to sender and recip:
            users[curr_trans->sender].balance -= (curr_trans->amt + fee);
            users[curr_trans->recip].balance += curr_trans->amt;
            
            //adding to executed
            executed.push_back(curr_trans);
            
            //verbose
            if(verbose == true) {
                cout << "Transaction executed at " << curr_trans->exec << ": $" <<
                curr_trans->amt << " from " << curr_trans->sender << " to " << curr_trans->recip << ".\n";
            }
        }
        
        //the sender cannot afford it :(
        else {
            if (verbose == true) {
                cout << "Insufficient funds to process transaction " << curr_trans->id << ".\n";
            }
        }
    }
    //if there are (s)hared fees, do appropriate checks
    else {
        uint32_t sender_fee;
        uint32_t recip_fee;
        
        //calculating fees for sender and recip depending on if the number is even or odd.
        if (fee % 2 == 0) {
            sender_fee = fee / 2;
            recip_fee = fee / 2;
        }
        else {
            sender_fee = (fee / 2) + 1;
            recip_fee = fee / 2;
        }
        
        //checking if both people can afford it
        if (users[curr_trans->sender].balance >= (curr_trans->amt + sender_fee)
            && users[curr_trans->recip].balance >= recip_fee) {
            
            //sending money to sender and recip
            users[curr_trans->sender].balance -= (curr_trans->amt + sender_fee);
            users[curr_trans->recip].balance += (curr_trans->amt - recip_fee);
            
            //adding to executed
            executed.push_back(curr_trans);
            
            //verbose
            if(verbose == true) {
//                cout << " PQ SIZE: " << transactions.size() << "\n";
//                cout << " Transaction: " << curr_trans->id << " Success.\n";
                cout << "Transaction executed at " << curr_trans->exec << ": $" <<
                curr_trans->amt << " from " << curr_trans->sender << " to " << curr_trans->recip << ".\n";
            }
        }
        
        //they cant afford it (brokies!)
        else {
            if (verbose == true) {
//                cout << " PQ SIZE: " << transactions.size() << "\n";
//                cout << " Transaction: " << curr_trans->id << " Failure.\n";
                cout << "Insufficient funds to process transaction " << curr_trans->id << ".\n";
            }
        }
    }//if/else
    
    transactions.pop();
}

void bank::place_trans(place placement) {
    //first check that the execution time is after the placement time and within 3 days
    if (placement.exec - placement.place_time <= 3000000) {
        
        //next check that the sender is registered
        if (users.find(placement.sender) != users.end()) {
            
            //now make sure the recip is registered
            if (users.find(placement.recip) != users.end()) {
                
                //now make sure that the execution date is after both the sd/rp's registration dates
                if (placement.place_time > users[placement.sender].reg_time
                    && placement.place_time > users[placement.recip].reg_time) {
                    
                    //now check that the sender is logged in currently
                    if (find(curr_users.begin(), curr_users.end(), placement.sender) != curr_users.end()) {
                        
                        //last, make sure there is no fraud.
                        if (!fraud_check(placement)) {
                            while (!transactions.empty() && transactions.top()->exec <= placement.place_time) {
                                execute_trans();
                            }
                            placed.push_back(placement);
                            transactions.push(&(placed.back()));
//                            cout << " just placed ID: " << transactions.top()->id << "\n";
                            if (verbose == true) {
                                cout << "Transaction placed at " << placement.place_time << ": $" << placement.amt << " from "
                                << placement.sender << " to " << placement.recip << " at " << placement.exec << ".\n";
                            }
                            trans_id++;
                        }
                        else {
                            if (verbose == true) {
                                cout << "Fraudulent transaction detected, aborting request.\n";
                            }
                        }
                    }
                    else {
                        if (verbose == true) {
                            cout << "Sender " << placement.sender << " is not logged in.\n";
                        }
                    }
                }
                else {
                    if (verbose == true) {
                        cout << "At the time of execution, sender and/or recipient have not registered.\n";
                    }
                }
            }
            else {
                if (verbose == true) {
                    cout << "Recipient " << placement.recip << " does not exist.\n";
                }
            }
        }
        else {
            if (verbose == true) {
                cout << "Sender " << placement.sender << " does not exist.\n";
            }
        }
    }
    else {
        if (verbose == true) {
            cout << "Select a time less than three days in the future.\n";
        }
    }
}

void bank::get_commands() {
    string command;
    
    while(cin >> command) {
        
        if (command[0] == 'l') {
            string id;
            uint32_t pin;
            string ip;
            
            cin >> id >> pin >> ip;
            login(id, pin, ip);
        }
        else if (command[0] == 'o') {
            string id;
            string ip;
            
            cin >> id >> ip;
            logout(id, ip);
        }
        else if (command[0] == 'p') {
            string timestamp;
            string ip;
            string sender;
            string recipient;
            uint32_t amount;
            string exec;
            char cover;
            
            cin >> timestamp >> ip >> sender >> recipient >> amount >> exec >> cover;
            
            place trans_place{trans_id, convert_time(timestamp), ip, sender, recipient, amount, convert_time(exec), cover};
            
            //error checking
            if (trans_place.place_time >= curr_time) {
                curr_time = trans_place.place_time;
            }
            else {
                cerr << "Invalid decreasing timestamp in 'place' command.";
                exit(1);
            }
            
            if (trans_place.exec < trans_place.place_time) {
                cerr << "You cannot have an execution date before the current timestamp.";
                exit(1);
            }
            
            place_trans(trans_place);
        }
        else if (command[0] == '#') {
            string junk;
            getline(cin, junk);
        }
        else if (command == "$$$"){
            while (!transactions.empty())
                execute_trans();
            break;
        }
    }
}

void bank::list(unsigned long long x, unsigned long long y) {
    uint32_t counter = 0;
    
    placecomp compare;
    auto first = lower_bound(executed.begin(), executed.end(), x, compare);
    auto last = lower_bound(executed.begin(), executed.end(), y, compare);
    
    for (; first != last; first++) {
        if ((*first)->amt != 1) {
            cout << (*first)->id << ": " << (*first)->sender << " sent " << (*first)->amt
            << " dollars to " << (*first)->recip << " at " << (*first)->exec << ".\n";
        }
        else {
            cout << (*first)->id << ": " << (*first)->sender << " sent " << (*first)->amt
            << " dollar to " << (*first)->recip << " at " << (*first)->exec << ".\n";
        }
        counter++;
    }
    
    if (counter != 1) {
        cout << "There were " << counter << " transactions that were placed between time " << x << " to " << y << ".\n";
    }
    else {
        cout << "There was " << counter << " transaction that were placed between time " << x << " to " << y << ".\n";
    }
}

void bank::revenue(unsigned long long x, unsigned long long y) {
    uint32_t total_revenue = 0;
    
    placecomp compare;
    auto first = lower_bound(executed.begin(), executed.end(), x, compare);
    auto last = lower_bound(executed.begin(), executed.end(), y, compare);
    
    for (; first != last; first++) {
        uint32_t fee = (*first)->amt / 100;
        if (fee > 450) {
            fee = 450;
        }
        else if (fee < 10) {
            fee = 10;
        }

        if (discounted((*first)->sender, **first)) {
            fee = (fee * 3) / 4;
        }
            
        total_revenue += fee;
    }
    cout << "281Bank has collected " << total_revenue << " dollars in fees over";
    
    unsigned long long period = y - x;
    
    string timeStr = to_string(period);
    timeStr = string(12 - timeStr.length(), '0') + timeStr;

        string years = timeStr.substr(0, 2);
        string months = timeStr.substr(2, 2);
        string days = timeStr.substr(4, 2);
        string hours = timeStr.substr(6, 2);
        string minutes = timeStr.substr(8, 2);
        string seconds = timeStr.substr(10, 2);

    if (years != "00") {
        if (years[0] == '0') {
            if (years[1] == '1') {
                cout << " 1 year";
            }
            else {
                cout << " " << years[1] << " years";
            }
        }
        else {
            cout << " " << years << " years";
        }
    }
    if (months != "00") {
        if (months[0] == '0') {
            if (months[1] == '1') {
                cout << " 1 month";
            }
            else {
                cout << " " << months[1] << " months";
            }
        }
        else {
            cout << " " << months << " months";
        }
    }
    if (days != "00") {
        if (days[0] == '0') {
            if (days[1] == '1') {
                cout << " 1 day";
            }
            else {
                cout << " " << days[1] << " days";
            }
        }
        else {
            cout << " " << days << " days";
        }
    }
    if (hours != "00") {
        if (hours[0] == '0') {
            if (hours[1] == '1') {
                cout << " 1 hour";
            }
            else {
                cout << " " << hours[1] << " hours";
            }
        }
        else {
            cout << " " << hours << " hours";
        }
    }
    if (minutes != "00") {
        if (minutes[0] == '0') {
            if (minutes[1] == '1') {
                cout << " 1 minute";
            }
            else {
                cout << " " << minutes[1] << " minutes";
            }
        }
        else {
            cout << " " << minutes << " minutes";
        }
    }
    if (seconds != "00") {
        if (seconds[0] == '0') {
            if (seconds[1] == '1') {
                cout << " 1 second";
            }
            else {
                cout << " " << seconds[1] << " seconds";
            }
        }
        else {
            cout << " " << seconds << " seconds";
        }
    }
    cout << ".\n";
}

void bank::history(string id) {
    
    if (users[id].hist == false) {
        users[id].incoming.reserve(executed.size());
        users[id].outgoing.reserve(executed.size());
        
        for (auto ptr : executed) {
            if (ptr->sender == id) {
                users[id].outgoing.push_back(ptr);
            }
            if (ptr->recip == id) {
                users[id].incoming.push_back(ptr);
            }
        }
    }
    
    cout << "Customer " << id << " account summary:\n";
    cout << "Balance: $" << users[id].balance << "\n";
    size_t total_trans = users[id].incoming.size() + users[id].outgoing.size();
    cout << "Total # of transactions: " << total_trans << "\n";
    cout << "Incoming " << users[id].incoming.size() << ":\n";
    auto it = users[id].incoming.begin();
    if (users[id].incoming.size() > 10) {
        it = users[id].incoming.end() - 10;
    }
    
    for (; it != users[id].incoming.end(); it++) {
        if ((*it)->amt != 1) {
            cout << (*it)->id << ": " << (*it)->sender << " sent " << (*it)->amt
            << " dollars to " << (*it)->recip << " at " << (*it)->exec << ".\n";
        }
        else {
            cout << (*it)->id << ": " << (*it)->sender << " sent " << (*it)->amt
            << " dollar to " << (*it)->recip << " at " << (*it)->exec << ".\n";
        }
    }
    
    cout << "Outgoing " << users[id].outgoing.size() << ":\n";
    it = users[id].outgoing.begin();
    if (users[id].outgoing.size() > 10) {
        it = users[id].outgoing.end() - 10;
    }
    
    for (; it != users[id].outgoing.end(); it++) {
        if ((*it)->amt != 1) {
            cout << (*it)->id << ": " << (*it)->sender << " sent " << (*it)->amt
            << " dollars to " << (*it)->recip << " at " << (*it)->exec << ".\n";
        }
        else {
            cout << (*it)->id << ": " << (*it)->sender << " sent " << (*it)->amt
            << " dollar to " << (*it)->recip << " at " << (*it)->exec << ".\n";
        }
    }
    
    users[id].hist = true;
}

void bank::summarize(unsigned long long date) {
    unsigned long long lowerb;
    unsigned long long upperb;
    
    unsigned long long subtract = date % 1000000;
    lowerb = date - subtract;
    upperb = lowerb + 1000000;
    
    cout << "Summary of [" << lowerb << ", " << upperb << "):\n";
    
    uint32_t counter = 0;
    uint32_t total_revenue = 0;
    
    placecomp compare;
    auto first = lower_bound(executed.begin(), executed.end(), lowerb, compare);
    auto last = lower_bound(executed.begin(), executed.end(), upperb, compare);

    for (; first != last; first++) {
        if ((*first)->amt != 1) {
            cout << (*first)->id << ": " << (*first)->sender << " sent " << (*first)->amt
            << " dollars to " << (*first)->recip << " at " << (*first)->exec << ".\n";
        }
        else {
            cout << (*first)->id << ": " << (*first)->sender << " sent " << (*first)->amt
            << " dollar to " << (*first)->recip << " at " << (*first)->exec << ".\n";
        }
        counter++;
        
        uint32_t fee = (*first)->amt / 100;
        if (fee > 450) {
            fee = 450;
        }
        else if (fee < 10) {
            fee = 10;
        }

        if (discounted((*first)->sender, **first)) {
            fee = (fee * 3) / 4;
        }
            
        total_revenue += fee;
    }
    
    if (counter != 1) {
        cout << "There were a total of " << counter << " transactions, 281Bank has collected " << total_revenue << " dollars in fees.\n";
    }
    else {
        cout << "There was a total of 1 transaction, 281Bank has collected " << total_revenue << " dollars in fees.\n";
    }
}

void bank::get_queries() {
    placecomp compare;
    sort(executed.begin(), executed.end(), compare);
    char query;
    while (cin >> query) {
        if (query == 'l') {
            string x;
            string y;
            
            cin >> x >> y;
            unsigned long long x_real = convert_time(x);
            unsigned long long y_real = convert_time(y);
            
            list(x_real, y_real);
        }
        else if (query == 'r') {
            string x;
            string y;
            
            cin >> x >> y;
            unsigned long long x_real = convert_time(x);
            unsigned long long y_real = convert_time(y);
            
            revenue(x_real, y_real);
        }
        else if (query == 'h') {
            string id;
            
            cin >> id;
            
            if (users.find(id) != users.end()) {
                history(id);
            }
            else {
                cout << "User " << id << " does not exist.\n";
            }
        }
        else if (query == 's') {
            string date;
            
            cin >> date;
            
            unsigned long long real_date = convert_time(date);
            
            summarize(real_date);
        }
    }
}

void bank::run_tests() {
    
    //testing users registration
    for (const auto& pair : users) {
        cout << "User ID: " << pair.first << "\n";
        cout << "Register Time: " << pair.second.reg_time << "\n";
        cout << "PIN: " << pair.second.pin << "\n";
        cout << "Balance: " << pair.second.balance << "\n";
    }
    
    //testing priority queue of transactions
    
}

int main(int argc, char ** argv) {
    
    bank eecsbank;
    
    eecsbank.get_opts(argc, argv);
    eecsbank.registration();
    
    eecsbank.get_commands();
    eecsbank.get_queries();
}
