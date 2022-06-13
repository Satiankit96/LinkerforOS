#include <stdio.h>
#include <iostream>
#include <getopt.h>
#include <string.h>
#include<vector>
#include <limits>
#include <deque>
#include <algorithm>
#include <sstream>
#include <cstdlib>

using namespace std;  

class IO_req {
    public:
    int pid;
    int arrival_time;
    int req_time;
    int track;
    int start_time=0;
    int end_time=0;
};

// since I'm done early, I shall add a ton of comments
char algo;
deque<IO_req> io_requests;                              // stores all the info about the io_request
deque<pair<int,int>> io_s;                              // stores pairs of io_req and track which are later used to populate io_queue in simulation
deque<int> io_queue;                                    // queue of io_requests populated during simulation accordingly
int AQ = 0;                             
deque<int>* add_queue = new deque<int>;                 // add queue for flook 
deque<int>* active_queue = new deque<int>;              // active queue for flook
int tot_movement;                                       
bool v = false;
int prev_track = 0;                                     // stores track of the previous io_request

class Sched {
    public:
    virtual int get_io() = 0;
    virtual void remove_io(int p) {
        // removes the io_request p from the io_queue. Same for all the algos except flook

        io_queue.erase(remove(io_queue.begin(), io_queue.end(), p), io_queue.end());
    }  
};

class FIFOsched : public Sched {
    public:
    int get_io(){
        return io_queue.front();            // return io as they enter the io_queue
    }
};

class SSTFsched : public Sched {
    public:
    
    int get_io(){
        // calculate shortest distance of new track from prev_track and return io with the min distance.

        int min_dist = numeric_limits<int>::max();
        int min_io = 0;
        int trackO = 0;
        while (trackO<io_queue.size())
        {
            int t = abs(io_requests[io_queue[trackO]].track -prev_track);
            
            if (v) {cout << io_queue[trackO] << " : " << t << "\t"; }
            
            if (t < min_dist){
                min_dist = t;
                min_io = io_queue[trackO];
            }
            trackO++;
        }
        if (v) {cout << endl; }
        return min_io;
    }
};

class LOOKsched : public Sched {
    public:
    int dir = 1;
    int get_io(){
        // returns io with min distance in the current direction;
        // if none found then we change direction and call the function again. 

        int min_dist = numeric_limits<int>::max();
        int min_io = 0;
        int t = numeric_limits<int>::max();
        bool change_dir = true;

        for (int i=0;i<io_queue.size();i++) {
            t = dir * (io_requests[io_queue[i]].track - prev_track); 

            if (t<0) {continue;}                            // we only want positive distances towards current direction.

            if (v) {cout << io_queue[i] << " : " << t << "\t"; }
            
            if (t < min_dist){
                min_dist = t;
                min_io = io_queue[i];
                change_dir = false;                     // this indicates that we found atleast one min distance and dont need to change directions.
            }
        }
        if (change_dir) {
            dir *= -1;                                  // change directions. Multiplying by -1 does it conveniently.
            if (v) {cout << "change directon to " << dir << endl;}
            min_io = get_io();                          // call get_io again with the changed direction.
        }
        else {
            if (v) {cout << "dir : " << dir << endl; }
        }
        return min_io;
    }
};

class CLOOKsched : public Sched {
    public:
    int get_io(){
        // travels only in one direction. If no more tracks ahead fall back to the minimum track and start again

        int min_dist = numeric_limits<int>::max();
        int min_io = 0;
        int min_track = numeric_limits<int>::max();
        int t;
        bool start_over = true;

        for (int i=0;i<io_queue.size();i++) {
            t = io_requests[io_queue[i]].track - prev_track; 

            if (t<0) {continue;}

            if (v) {cout << io_queue[i] << " : " << t << "\t"; }
            
            if (t < min_dist){
                min_dist = t;
                min_io = io_queue[i];
                start_over = false;                 // if we found atleast one min distance then we dont have to start over.
            }
        }
        if (start_over) {
            if (v) {cout << "start over " <<  endl;}
            
            //find io with minimum track
            for (int i=0;i<io_queue.size();i++){
                t = io_requests[io_queue[i]].track;
                if (t < min_track) {
                    min_track = t;
                    min_io = io_queue[i];
                }
            }
        }
        if (v) {cout << endl;}
        return min_io;
    }
};

void print_queues(){                        // print active and add queues for debugging.
    cout << "Q[" << AQ << "]:" << "\t";

    for( int i=0;i<active_queue->size();i++){
        int t = io_requests[active_queue->at(i)].track - prev_track;
        cout << active_queue->at(i) << " : " << t << "\t";
    }

    cout << "Q[" << 1 - AQ << "]:" << "\t";
    
    for( int i=0;i<add_queue->size();i++){
        int t = io_requests[add_queue->at(i)].track - prev_track;
        cout << add_queue->at(i) << " : " << t << "\t";
    }
    cout << endl;
}

class FLOOKsched : public Sched {
    public:
    int dir = 1;
    
    int get_io(){
        // an added check for active and add queues. Rest of implementation is like LOOk

        int min_dist = numeric_limits<int>::max();
        int min_io = 0;
        int t = numeric_limits<int>::max();
        bool change_dir = true;

        //check both queues
        if (active_queue->size() == 0) {
            // exchange both queues
            deque<int>* temp = active_queue;
            active_queue = add_queue;
            add_queue = temp;

            // exchange AQ
            // if (AQ == 0){AQ = 1;} 
            // else {AQ = 0;}
            AQ = 1- AQ;
        }
        if (v) {cout << "AQ" << AQ << "dir : " << dir << "\t";
                print_queues(); }

        for (int i=0;i<active_queue->size();i++) {
            t = dir * (io_requests[active_queue->at(i)].track - prev_track); 

            if (t<0) {continue;}

            if (v) {cout << active_queue->at(i) << " : " << t << "\t"; }
            
            if (t < min_dist){
                min_dist = t;
                min_io = active_queue->at(i);
                change_dir = false;
            }
        }
        if (change_dir) {
            dir *= -1;
            if (v) {cout << "change directon to " << dir << endl;}
            min_io = get_io();
        }
        else {
            if (v) {cout << "dir : " << dir << endl; }
        }
        return min_io;
    }

    void remove_io(int p) {
        active_queue->erase(remove(active_queue->begin(), active_queue->end(), p), active_queue->end());
    }
};


Sched* schedmode;

void  getSched(char x){    
        if (x == 'i'){
            schedmode  = new FIFOsched();
        } else if (x == 'j') {
            schedmode = new SSTFsched();
        } else if (x == 's') {
            schedmode  = new LOOKsched();
        } else if (x == 'c') {
            schedmode  = new CLOOKsched();
        } else if (x == 'f') {
            schedmode  = new FLOOKsched();
        } 
}

int sim_time = 0;


void simulation(){

    
    int io_count = 0;
    int active = -1;
    

    while(io_s.size()!=0 || io_queue.size() != 0 || active_queue->size() != 0 || add_queue->size() != 0){
    
    // 1) If a new I/O arrived to the system at this current time → add request to IO-queue ?
        
        if (io_s.front().first == sim_time){
            
          if (v){cout << sim_time << ":\t" << io_count << " add " << io_s.front().second << endl;}  
          
          if (algo == 'f') {
              if (v) { cout << "Q=" << AQ <<  " (" << io_count << ": " << io_s.front().second << " )" << endl;}
                add_queue->push_back(io_count);
          } else {
              io_queue.push_back(io_count);
          } 
            
            io_requests[io_count].req_time = sim_time;
            io_s.pop_front();
            io_count++;
        }

    // 2) If an IO is active and completed at this time → Compute relevant info and store in IO request for final summary
        if ((active != -1) && io_requests[active].end_time == sim_time) {
            if (v) {cout << sim_time << ":\t" << active << " finish " << sim_time - io_requests[active].req_time << endl; }
            schedmode->remove_io(active);
            active = -1;
        }
    

    // 4) If no IO request active now (after (2)) but IO requests are pending → Fetch the next request and start the new IO.
        if ((active == -1) && (io_queue.size() != 0 || active_queue->size() != 0 || add_queue->size() != 0)) {
                
                active = schedmode->get_io(); 
                if (v){ cout << sim_time << ":\t" << active << " issue " << io_requests[active].track << " " << prev_track <<  endl;}
                io_requests[active].start_time = sim_time;
                io_requests[active].end_time = abs(prev_track - io_requests[active].track) + sim_time;
                tot_movement += abs(prev_track - io_requests[active].track);
                if (prev_track == io_requests[active].track) {
                    continue;    
                }
                prev_track = io_requests[active].track;
            }
    
    // 5) If no IO request is active now and no IO requests pending → exit simulation
        if ((active == -1) && io_queue.size() == 0 && io_s.size() == 0 && active_queue->size() == 0 && add_queue->size() == 0) {
            break;} 
        
    // 6) Increment time by 1
        sim_time++;
    }
}


void read_input_file(char *fileloc){
    // read the given input file; generate a process object for each process and store into a vector p.
    
    FILE *input_file = fopen(fileloc, "r");

    char buf[100];
    int lineno=0;
    int io_req;
    int io_track;

    string line;
    stringstream clean_file;
    
    // clean the input file
    while (fgets(buf, sizeof(buf), input_file) != NULL) {
        if (buf[0] == '#') continue; 
        clean_file << buf;
    }
    
    int i =0;
    // storing the instructions
    while(getline(clean_file, line)) {
        sscanf(line.c_str(), "%d %d", &io_req, &io_track);
        IO_req io;
        io.pid = i;
        io.arrival_time = io_req;
        io.track = io_track;
        
        io_requests.push_back(io);
        io_s.push_back(make_pair(io_req, io_track));
        i++;
    }
    fclose(input_file);   
}


void print_out(){
    
    double avg_turnaround=0;
    double avg_waittime;
    int max_waittime = 0;
    double t = 0;

    for (int i =0; i<io_requests.size();i++){
    
        printf("%5d: %5d %5d %5d\n",io_requests[i].pid, io_requests[i].arrival_time, io_requests[i].start_time, io_requests[i].end_time);
        
        avg_turnaround += (double)(io_requests[i].end_time - io_requests[i].req_time);
        t = (double)(io_requests[i].start_time - io_requests[i].req_time);
        if (t > max_waittime) {
            max_waittime = t;
        }
        avg_waittime += t;

    } 
    // use double not float
    printf("SUM: %d %d %.2lf %.2lf %d\n", sim_time,  tot_movement, 
                                                double(avg_turnaround/double(io_requests.size())) , 
                                                double(avg_waittime / double(io_requests.size())) ,
                                                max_waittime);   
}

main(int argc, char* argv[]) {
  
    int c;
  
    while ((c = getopt(argc,argv,"vs:")) != -1 )
     {
         switch(c) {
         case 's': 
            sscanf(optarg, "%s", &algo);
            getSched(algo);
            break;
         case 'v':
            v = true;
             break;
         }
     }
    
    if ((argc - optind) < 1) {
         printf("\nGive me My precious! arguments\n");
         exit(1);
     }
    
    char *in_file = argv[optind++];
    read_input_file(in_file);    
    simulation();
    print_out();
}