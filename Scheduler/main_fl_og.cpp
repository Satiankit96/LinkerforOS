#include <stdio.h>
#include <iostream>
#include <getopt.h>
#include <string.h>
#include<vector>
#include <deque>

using namespace std;  

typedef enum {CREATED, READY, RUNNING, BLOCKED, DONE} proc_states;
typedef enum {TO_READY, TO_RUNNING, TO_BLOCKED, TO_PREEMPT, TO_DONE} proc_transitions;

// defining classes for Process and Event
class Process {
    public:
    int pid;
    int arrival_time;
    int cpu_time;
    int cpu_burst;
    int io_burst;
    int stat_prio;
    int dyn_prio;

    int rem_time;
    int finish_time;
    int total_io_time;
    int cw_time;
    int time_at_ready;
    proc_states curr_state;
    int last_ts;
    bool from_io;
};

class Event {
    public:
    Process *proc;
    int timestamp;
    proc_states ps;
    proc_transitions pt;
};

deque <Process*> runQ;


class Sched {
    public:
    std::string name;
    virtual void addProcess(Process* proc){}
    virtual Process* getProcess(){}
    
};

class FCFSsched : public Sched {
    public:
    void addProcess(Process* proc){
        //cout << "Done!" << endl;
        runQ.push_back(proc);
    }
    Process* getProcess(){
        
        if (!runQ.empty()){
            Process* temp = runQ.front();   
            runQ.pop_front();
            return temp;
        } else {
            return nullptr;
        }
    }
};

class LCFSsched : public Sched {
    public:
    void addProcess(Process* proc){
        //cout << "Done!" << endl;
        runQ.push_back(proc);
    }
    Process* getProcess(){
        
        if (!runQ.empty()){
            Process* temp = runQ.back();   
            runQ.pop_back();
            return temp;
        } else {
            return nullptr;
        }
    }
};

class SRTFsched : public Sched {
    public:
    void addProcess(Process* proc){
        //cout << "Done!" << endl;
        if (!runQ.empty()){
            int idx;
            for (int i = 0; i< runQ.size(); i++) {
                    if (proc->rem_time <= runQ[i]->rem_time) {idx = i; break;}
                }
                runQ.insert(runQ.begin()+idx, proc);
        } else {
            runQ.push_back(proc);
        }
    }
    Process* getProcess(){
        
        if (!runQ.empty()){
            Process* temp = runQ.front();   
            runQ.pop_front();
            return temp;
        } else {
            return nullptr;
        }
    }
};



Sched* schedmode;

std::string getSched(char x){    
        if (x == 'F'){
            schedmode  = new FCFSsched();
            return "FCFS";
        } else if (x == 'L') {
            schedmode = new LCFSsched();
            return "LCFS";
        } else if (x == 'S') {
            schedmode  = new SRTFsched();
            return "SRTF";
        } else if (x == 'R') {
            schedmode  = new FCFSsched();
            return "RR";
        } else if (x == 'P') {
            schedmode  = new FCFSsched();
            return "PRIO";
        } else if (x == 'E') {
            schedmode  = new FCFSsched();
            return "PREPRIO";
        } 
}
// initializing vectors and deques required. Also some global variables.

vector <Process> p;
deque <Event*> events;
int verbose = 0;
vector <int> randvals;
Sched my_scheduler;
char sched;
int quantum=0, maxprio=4;
int ofs = 0;

int myrandom(int burst) { 
    // as given by prof; generates random numbers

    return 1 + (randvals[ofs++] % burst); 
    }

read_rfile(char *fileloc){
    // read the rfile numbers into a vector randvals.

    FILE *r_file = fopen(fileloc, "r");
    char line[50];
    int lineno=0;
    
    while (fgets(line, sizeof(line), r_file) != NULL) {
        if (lineno == 0) {lineno ++;}       // to ignore the first line which has the count of random numbers 
        else {randvals.push_back(atoi(strtok(line, " \t\n")));}
    }
    fclose(r_file);
}

read_input_file(char *fileloc){
    // read the given input file; generate a process object for each process and store into a vector p.


    FILE *input_file = fopen(fileloc, "r");
    char line[100];
    char *token;
    int lineno=-1;
    
    
    while (fgets(line, sizeof(line), input_file) != NULL) {
        Process p1;
        lineno++;

        p1.pid = lineno; 
        p1.arrival_time = atoi(strtok(line, " \t\n")); 
        p1.cpu_time = atoi(strtok(NULL, " \t\n"));
        p1.cpu_burst = atoi(strtok(NULL, " \t\n"));
        p1.io_burst = atoi(strtok(NULL, " \t\n"));
        p1.stat_prio = myrandom(maxprio); 
        p1.dyn_prio = p1.stat_prio -1;
        p1.rem_time = p1.cpu_time;
        p1.finish_time = p1.arrival_time;
        p1.total_io_time = 0;
        p1.cw_time = 0;
        p1.curr_state = CREATED;
        p1.last_ts =0;
        p1.from_io = false;
        p.push_back(p1);

    }
    //cout << p[0].stat_prio << "\t" << p[0].dyn_prio;
    fclose(input_file);
}

print_eventq(){
    for (int i = 0; i < events.size(); i++) {
 
      cout << events[i]->proc->pid << " " << events[i]->timestamp << " "<< events[i]->ps << "\t";
    }
    cout << endl;
}

Event* get_event(){
    // return nullptr if empty else the first event's ptr 


    if (!events.empty()){
    Event* first = events.front();
    events.pop_front(); 
    //cout << first->ps << "\n";
    return first;
    } else {
        return nullptr;
    }
    
}

void put_event(Event *evt) {
    int idx, i;
    for (i = 0; i<events.size(); i++) {
        if (evt->timestamp < events[i]->timestamp) {idx = i; break;}
    }
    events.insert(events.begin()+idx, evt);
}


// simulation variables
bool CALL_SCHED = false;
//bool from_io = false;
int io_proc = 0;
int io_start = 0;
double total_io = 0.0;
Process* current_running_process = nullptr;


void simulation() {
    Event* curr_event;
    int curr_time = p[0].arrival_time;

    
    while ((curr_event = get_event())){
        //cout << "arrival time " << curr_event->proc->arrival_time << "\n";
        Process *curr_process = curr_event->proc;
        

        switch(curr_event->pt){
            case TO_READY:
            {

                curr_time = curr_event->timestamp;
               
                if (verbose == 1 ) {cout << curr_time << " Process " << curr_process->pid << " from " << curr_event->ps  <<" to ready state" << " rem " << curr_process->rem_time << endl;}
                // must come from blocked or preempt
                
                Event *e = new Event;
                e->proc = curr_process;
                e->ps = READY;
                e->pt = TO_RUNNING;
                //e->timestamp = curr_process->arrival_time;
                e->timestamp = curr_time;

                print_eventq();
                put_event(e);
                print_eventq();
                
                curr_process->time_at_ready = curr_time;
                curr_process->curr_state = READY;
                // must add to runQ
                schedmode->addProcess(curr_process);
                
                //cout << "Hi" << curr_process->curr_state << endl;
                CALL_SCHED = true;

                // to check if we have any processes still in io. If not we update the total_io time
                if (curr_process->from_io == true){
                    curr_process->from_io = false;
                    io_proc--;
                    if (io_proc == 0) {
                        total_io += curr_time - io_start;
                    }
                }
                
                break;
            
            }
                
            case TO_RUNNING:
                { 
                if (current_running_process != nullptr) {
                        
                        //cout << "Hi" << current_running_process->last_ts << endl ;
                        curr_event->timestamp = current_running_process->last_ts;                    
                        
                        print_eventq();
                        put_event(curr_event);
                        print_eventq();
    
                        continue;
                } 
                else {
                    curr_process = schedmode->getProcess();
                    //cout << "YOO " << curr_process->pid << endl; 
                    current_running_process = curr_process;
                }
                
                if (curr_process->rem_time>0) {
                
                curr_process->curr_state = RUNNING;
                curr_process->cw_time += curr_time - curr_process->time_at_ready;
                // create event for either preempt or running
                int given_cpu_time = myrandom(curr_process->cpu_burst);

                

                if (curr_process->rem_time <= given_cpu_time){
                    // process finished 
                    
                    if (verbose == 1 ) {cout << curr_time  << " Process " << curr_process->pid << " from " << curr_event->ps  <<" to running state cb = " << curr_process->rem_time << endl;}

                    curr_time += curr_process->rem_time;
                    curr_process->finish_time = curr_time;
                    
                    curr_process->rem_time = 0;
                    curr_process->last_ts = curr_time;
                    
                    Event *e = new Event;
                    e->proc = curr_process;
                    e->ps = DONE;
                    e->pt = TO_DONE;
                    e->timestamp = curr_time;
                    
                    print_eventq();
                    put_event(e);
                    print_eventq();   

                    continue;
                } else {
                    
                    if (verbose == 1 ) {cout << curr_time  << " Process " << curr_process->pid << " from " << curr_event->ps  <<" to running state cb = " << given_cpu_time << endl;}

                    curr_process->rem_time -= given_cpu_time;
                    curr_time += given_cpu_time;
                    curr_process->finish_time += given_cpu_time;
                    

                }
                
                curr_process->last_ts = curr_time; 
                
                Event *e = new Event;
                e->proc = curr_process;
                e->ps = RUNNING;
                e->pt = TO_BLOCKED;
                e->timestamp = curr_time;
                         
                print_eventq();

                put_event(e);
                
                print_eventq();                 
                break;
                }else {
                    current_running_process = nullptr;
                    continue;}
                }

            case TO_BLOCKED:
                {
                curr_process->from_io = true;
                curr_time = curr_event->timestamp; 
                // create event for when process becomes ready again
                if (io_proc == 0) {   
                    io_start = curr_time;
                }
                io_proc++;
                    // process in io mode
            
                    // call myrandom and get io time
                int io_time = myrandom(curr_process->io_burst);

                
                if (verbose == 1) {cout  << curr_time << " Process " << curr_process->pid << " from " << curr_event->ps  <<" to block state io = " << io_time << endl;}
                
                //curr_time += io_time;
                curr_process->last_ts = curr_time+io_time;
                curr_process->finish_time += io_time;
                curr_process->total_io_time += io_time;
                curr_process->curr_state = BLOCKED;


                Event *e2 = new Event;
                e2->proc = curr_process;
                e2->ps = BLOCKED;
                e2->pt = TO_READY;
                e2->timestamp = curr_time+io_time;

                print_eventq();

                put_event(e2);
                print_eventq();

                current_running_process = nullptr;

                CALL_SCHED = true;
                
                break;}
            
            case TO_PREEMPT:
                {
                    
                if (verbose == 1) {cout  << curr_time << " Preempting process " << curr_process->pid << " from " << curr_event->ps  <<" \n";}

                current_running_process = nullptr;
                curr_process->curr_state = READY;

                // add to runq
                schedmode->addProcess(curr_process);

                CALL_SCHED = true;
                
                break;}
            case TO_DONE:
            {
                if (verbose == 1) {cout << "Process " << current_running_process->pid << " Done"<< endl ;}
                curr_time = curr_event->timestamp;  
                current_running_process = nullptr;
                
            }
        }
        delete curr_event; curr_event = nullptr;
        // call sched

    }


    // Print final output
    
    cout << my_scheduler.name << endl;
    double cpu_util =0.0;
    double tt = 0.0;
    double total_wt = 0.0;
    for(int i=0; i< p.size(); i++){
        
        cpu_util += p[i].cpu_time;
        tt += p[i].finish_time - p[i].arrival_time;
        total_wt += p[i].cw_time;

        printf("%04d: %4d %4d %4d %4d %1d | %5d %5d %5d %5d\n",p[i].pid, p[i].arrival_time, p[i].cpu_time, p[i].cpu_burst, p[i].io_burst, p[i].stat_prio, 
            p[i].finish_time, p[i].finish_time - p[i].arrival_time, p[i].total_io_time, p[i].cw_time); 
        
    }
    int sim_finish = curr_time;
    printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n",sim_finish, 100*cpu_util/sim_finish, 100 * total_io / sim_finish, tt / p.size(), total_wt / p.size(), 100 * (double)p.size() / sim_finish);
    
}

main(int argc, char* argv[]) {

    int c;
    
    while ((c = getopt(argc,argv,"vtes:")) != -1 )
     {
        
         switch(c) {
         case 'v': 
             verbose = 1;
             break;
         case 's': 
            //cout << "s = 1\n";
            sscanf(optarg, "%s", &sched);
            my_scheduler.name = getSched(sched);
            sscanf(optarg+1, "%d:%d", &quantum, &maxprio);

            //cout << sched << "\t" <<quantum <<"\t"<<maxprio << "\n";
            break;
         case 't':
             //cout << "t = 1\n";
             break;
         case 'e':
            // cout<< "e = 1\n";
            break;
        case ':':
            printf("Provide option value!\n");
            break;
        case '?':
            cout << "Unrecognized option\n";
            break;
         }
     }

    if ((argc - optind) < 2) {
         printf("\nGive me My precious! arguments\n");
         exit(1);
     }

    char *in_file = argv[optind++];
    char *r_file = argv[optind];
    read_rfile(r_file);
    read_input_file(in_file);


    // Initialize the events queue
     
    for (int i= 0 ; i < p.size(); i++) {
        Event *e1 = new Event;

        e1->proc = &p[i];
        e1->timestamp = p[i].arrival_time;
        e1->ps = CREATED;
        e1->pt = TO_READY;

        events.push_back(e1);
    
    //  delete e1;    
    }
    print_eventq();

    
    // des layer
    
    simulation();
}
