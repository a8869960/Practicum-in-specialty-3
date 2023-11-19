//
// Created by varsem on 11.11.23.
//
#pragma once
#include <iostream>

using namespace std;

enum class io_status
{
    success,
    error_open,
    error_read,
    none
};

enum class type_inf
{
    empty,
    one,
    many,
    unknown
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
    double prelast = 0;
    double last = 0;

    type_inf type = type_inf::unknown;

    double local_max = -1.7976931348623158e+308;
    double global_max = 0;

    int local_answer = 0;
    int global_answer = 0;

    void print()
    {
        cout << "filename  | " << filename << endl;
        cout << "loc ans   | " << local_answer << endl;
        cout << "local max | " << local_max << endl;
        cout << "first     | " << first << endl;
        cout << "second    | " << second << endl;
        cout << "type      | ";
        switch (type)
        {
            case type_inf::unknown:
                cout << "unknown" << endl;
                break;

            case type_inf::empty:
                cout << "empty" << endl;
                break;

            case type_inf::one:
                cout << "one" << endl;
                break;

            case type_inf::many:
                cout << "many" << endl;
                break;
        }
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
        cout << endl;
    }
};

void reduce_sum(int p, double* a = nullptr, int n = 0);

void remember_elements(ARGS *arg);

void *process_main(void *args);

// 1 || 1 2 | 7 0 6 6 13 || 2 3 0 0