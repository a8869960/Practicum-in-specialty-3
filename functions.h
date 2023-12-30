//
// Created by varsem on 11.11.23.
//
#pragma once
#include <iostream>
#include <cmath>

using namespace std;

enum class io_status
{
    success,
    error_open,
    error_read,
    none
};

class ARGS
{
public:
    char *filename = nullptr;
    int m = 0;
    int p = 0;
    io_status status = io_status::none;

    double first = 0;
    double second = 0;

    bool is_last = false;

    int count = 0;

    double local_max = -1.7976931348623158e+308;
    double global_max = 0;

    int local_answer = 0;
    int global_answer = 0;

    void print()
    {
        cout << "filename  | " << filename << endl;
        cout << "loc ans   | " << local_answer << endl;
        cout << "local max | " << local_max << endl;
        cout << "glob max  | " << global_max << endl;
        cout << "first     | " << first << endl;
        cout << "second    | " << second << endl;
        cout << "count     | " << count << endl;
        cout << "status    | ";
        switch (status)
        {
            case io_status::success:
                cout << "success" << endl;
                break;

            case io_status::error_open:
                cout << "error open" << endl;
                break;

            case io_status::error_read:
                cout << "error read" << endl;
                break;

            case io_status::none:
                cout << "none" << endl;
                break;
        }
        cout << "is last   |";
        if(is_last)
            cout << "true" << endl;
        else
            cout << "false" << endl;
        cout << endl;
    }
};

void reduce_sum(int p, double* a = nullptr, int n = 0);

void remember_elements(ARGS *arg);

void *process_main(void *args);

// 1 || 1 2 | 7 0 6 6 13 || 2 3 0 0
