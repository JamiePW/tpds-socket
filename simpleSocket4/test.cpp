#include "common.h"

using namespace std;

int main() {
    string myStr;
    char myChar[30];

    for (int i=0;i<2000;i++) {
        myStr = strRand(10);
        
        myStr.copy(myChar, 10, 0);
        *(myChar + 10) = 0;  //this is very important!!!
        printf("%s\n", myChar);
    }
    

    return 0;
}