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
    sin.close();
    uin.close();
    if(emptyFileSig)
        return -1;
    else
        return 0;
}

void read_and_handle(char *path, string &cpp_str)
{
    ifstream in;
    in.open(path);
    bool multiComment = false;

    while(!in.eof()) {
        string str, tmp;
        getline(in, tmp);
        for(int i = 0; i < tmp.size(); i++) {
            if(tmp[i] == '*' && i < tmp.size() - 1 && tmp[i + 1] == '/') {
                multiComment = false;
                i++;
            }
            if(!multiComment && tmp[i] == '/' && i < tmp.size() - 1 && tmp[i + 1] == '/') {  //单行注释
                break;
            }
            if(tmp[i] == '/' && i < tmp.size() - 1 && tmp[i + 1] == '*') {
                multiComment = true;
                i++;
            }
            if(!multiComment) {
                if(tmp[i] >= 33 && tmp[i] <= 126) { //33 => ! 126 => ~
                    str.push_back(tmp[i]);
                }
            }
        }
        cpp_str += str;
    }
    in.close();
}


int compare_code()
{
    string ucpp, scpp;
    read_and_handle("./user.cpp", ucpp);
    read_and_handle("./std.cpp", scpp);

    int **dp = new int *[ucpp.size() + 1];

    for(int i = 0; i <= ucpp.size(); i++)
        dp[i] = new int [scpp.size() + 1];

    for(int i = 0; i <= ucpp.size(); i++)
        for(int j = 0; j <= scpp.size(); j++)
            dp[i][j] = 0;


    for(int i = 1; i <= scpp.size(); i++)
        dp[0][i] = dp[0][i - 1] + 1;
    for(int i = 1; i <= ucpp.size(); i++)
        dp[i][0] = dp[i - 1][0] + 1;
    for(int i = 1; i <= ucpp.size(); i++)
        for(int j = 1; j <= scpp.size(); j++) {
            int tmp = ucpp[ i - 1] == scpp[j - 1] ? 0 : 1;
            dp[i][j] = min( min(dp[i-1][j] + 1, dp[i][j-1] + 1), dp[i-1][j-1] + tmp );
        }
    int edit = dp[ucpp.size()][scpp.size()];
    return 100 - edit * 100 / max(ucpp.size(), scpp.size());
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
            else {
                cout << "wrong answer" << endl;
                cout << "begin to compare code" << endl;
                cout << compare_code() << endl;
            }

        }
    } else {
        cout << "compiler error" << endl;
    }
    return 0;
}
