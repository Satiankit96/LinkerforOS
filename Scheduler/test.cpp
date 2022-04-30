#include<iostream>
#include<vector>
#include<stdio.h>
#include<algorithm>
using namespace std;


int main(){
    int cpu = min(10, min(20, 3));

    switch(cpu) {
        case 10:
        cout << "10";
        break;
        case 20:
        cout << "20";
        break;
        case 3:
        cout << "it works";
        break;
    }
}