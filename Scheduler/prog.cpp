#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>

#define MAX_OBJS (10)  // always put brackets around expressions

int verbose = 0;
int dotrace = 1;

// really convenient way to implement a global conditional trace function
// note the do { } while .. this is important if you do if (cond) trace("yoo") else { something }

#define trace(fmt...)      do { if (dotrace > 3) { printf(fmt); fflush(stdout); } } while(0)
//#define trace(fmt...)      do { if (0) { printf(fmt); fflush(stdout); } } while(0)
//#define trace(fmt...)      do { } while(0)

// this is the base class of objects
class GObj {
   static int counter; // static class counter telling me how many objects were created
   static int deleted; // static class counter telling me how many objects were deleted
protected: 
   int id; // unique ID for this object
public:
   GObj() { id = counter++; }
   ~GObj() { deleted++; }

//   virtual
//   void display() { trace("obj<%d>\n", ((GObj*)NULL)->id); }
   virtual void display() { trace("[%-40s]: <%d>\n", __PRETTY_FUNCTION__, ((GObj*)this)->id); }
   virtual void fct()     { trace("[%-40s]:\n",__PRETTY_FUNCTION__) ; }
};

class Rectangular: public GObj {
protected:
   int w;
   int h;
public:
   Rectangular(int W, int H) { w = W; h = H; }
   ~Rectangular() { }

   void display() { trace("[%-40s]: <%d> w=%d h=%d\n", __PRETTY_FUNCTION__, id, w, h); }
};


class Square : public Rectangular {
public:
   Square(int L) : Rectangular(L,L) { }
   // ~Square() { }  // really not needed

   void display() { trace("[%-40s]: <%d> l=%d\n", __PRETTY_FUNCTION__, id, w); }
   virtual void fct()     { trace("%s:\n",__PRETTY_FUNCTION__) ; }
};


class Circle : public GObj {
   float radius;
public:
   Circle(float r) { radius = r; }
   ~Circle() { }

   void display() { trace("[%-40s]: <%d> r=%f\n", __PRETTY_FUNCTION__, id, radius); }
   // no fct() overwrite
};

int GObj::counter = 0;
int GObj::deleted = 0;

GObj* gobjs[MAX_OBJS];

main(int argc, char **argv)
{
     int c;
     int count = 1;
     int segv = 0;

     // proper way to parse arguments
     while ((c = getopt(argc,argv,"vsV:t:i:")) != -1 )
     {
         switch(c) {
         case 'v': 
             verbose = 1;
             break;
         case 's': 
             segv = 1;
             break;
         case 'V':
             verbose = atoi(optarg);
             break;
         case 't':
             dotrace = atoi(optarg);
             break;
         case 'i':
             count = 1000*atoi(optarg);
          break;
         }
     }

     printf("verbose = %d  trace = %d\n",verbose,dotrace); 

     if ((argc - optind) < 2) {
         printf("Now, I told you I need two fixed arguments, what the heck are you asking me to do?\n");
         exit(1);
     }
     printf("%d <%s> <%s>\n", argc, argv[optind], argv[optind+1]);
     // note these additional arguments do nothing

     for (int i=count; i> 0; --i) { 
         gobjs[0] = new Rectangular(10,20);
         gobjs[1] = new Rectangular(30,40);
         gobjs[2] = new Circle(1.0);
         gobjs[3] = new Circle(3.0);
         gobjs[4] = new Square(12);
         gobjs[5] = new Square(14);
         gobjs[6] = new GObj();
     }

     for (int i=0; i < 7; i++) 
         gobjs[i]->display();

     if (segv) 
        ((GObj*)NULL)->display();

}


