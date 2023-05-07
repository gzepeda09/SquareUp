#include <iostream>
#include <cstring>

using namespace std;

int main()
{
    string main[100];
    string mainline[100];

    for (int i = 0; i < 24; i++) {
        if (i < 10) {
            main[i] = "0" + to_string(i);
        }
        else {
            main[i] = to_string(i);
        }
    }

    mainline[0] = "convert frame_" + main[0] + "_delay-0.12s.gif " +
    "frame_" + main[1] + "_delay-0.12s.gif -gravity center -append " + 
    "map1-" + main[0] + ".png";

    //cout << mainline[0] << endl;

    for (int i = 1; i < 23; i++) {
        mainline[i] = "convert map1-" + main[i - 1] + ".png " +
        "frame_" + main[i + 1] + "_delay-0.12s.gif" + 
        " -gravity center -append map1-" + main[i] + ".png";
    }

    for (int i = 0; i < 24; i++) {
        //cout << mainline[i] << endl;
        const char* command = mainline[i].c_str();
        system(command);
    }

    return 0;
}
