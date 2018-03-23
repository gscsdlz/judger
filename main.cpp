#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/resource.h>

using namespace std;

char cmdStr[100];

int compiler()
{
    pid_t pid = fork();
    if(pid) {  //parent
        int status = 0;
        waitpid(pid, &status, 0);
        return status;
    } else {
        freopen("/dev/null", "w", stderr);
        execl("/usr/bin/g++", "g++", "-o", "./user", "./user.cpp", "-Wall", NULL);
        return 0;
    }
}

int run()
{
    pid_t pid = fork();
    if(pid) { //parent
        int status;
        waitpid(pid, &status, 0);
        if(WIFEXITED(status))
            return 0;
        else if(WIFSIGNALED(status)) {
            if(WTERMSIG(status) == SIGXCPU || WTERMSIG(status) == SIGALRM)
                cout << "Time limit error" << endl;
            else if(WTERMSIG(status) == SIGSEGV)
                cout << "Array out of border" << endl;
            else if(WTERMSIG(status) == SIGFPE)
                cout << "Calc error" << endl;
            return -1;
        } else {
            cout << "runtime error" << endl;
            return -1;
        }
    } else {
        struct rlimit LIM;
        LIM.rlim_cur = 1;
        LIM.rlim_max = LIM.rlim_cur;
        setrlimit(RLIMIT_CPU, &LIM);
        alarm(1);
        freopen("./in.txt", "r", stdin);
        freopen("./user.txt", "w", stdout);
        execl("./user", "./user", NULL);
    }
}

int compare_file()
{
    ifstream uin("./user.txt");
    ifstream sin("./out.txt");

    bool emptyFileSig = true;

    while(!uin.eof() && !sin.eof()) {
        emptyFileSig = false;
        string u, s;
        getline(uin, u);
        getline(sin, s);
        if(u != s)
            return -1;
    }

    if(emptyFileSig)
        return -1;
    else
        return 0;
}

int main()
{
    if(compiler() == 0) {
        cout << "compiler ok" << endl;
        cout << "begin to run" << endl;
        if(run() == 0) {
            cout << "complete run" << endl;
            cout << "begin to compare file" << endl;
            if(compare_file() == 0)
                cout << "answer is ok" << endl;
            else
                cout << "wrong answer" << endl;

        }
    } else {
        cout << "compiler error" << endl;
    }
    return 0;
}
