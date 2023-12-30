//
// Created by varsem on 11.11.23.
//
#include "functions.h"
#define eps 1e-16

void reduce_sum(int p, double* a, int n)
{
    static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
    static pthread_cond_t c_in = PTHREAD_COND_INITIALIZER;
    static pthread_cond_t c_out = PTHREAD_COND_INITIALIZER;
    static int t_in = 0;
    static int t_out = 0;
    static double *r = nullptr;
    int i;

    if(p <= 1)
        return;
    pthread_mutex_lock(&m);

    if(r == nullptr)
        r = a;
    else
        for(i = 0; i < n; i++) r[i] += a[i];

    t_in++;
    if(t_in >= p)
    {
        t_out = 0;
        pthread_cond_broadcast(&c_in);
    }
    else
        while(t_in < p)
            pthread_cond_wait(&c_in, &m);

    if(r != a)
        for(i = 0; i < n; i++) a[i] = r[i];

    t_out++;
    if(t_out >= p)
    {
        t_in = 0;
        r = nullptr;
        pthread_cond_broadcast(&c_out);
    }
    else
        while(t_out < p)
            pthread_cond_wait(&c_out, &m);

    pthread_mutex_unlock(&m);
}

void file_check(ARGS *arg)
{
    FILE *f;

    f = fopen(arg->filename, "r");
    if(!f)
    {
        cout << "Can't open the file " << arg->filename << endl;
        arg->status = io_status::error_open;
        return;
    }

    double current = 0;
    int count = 0;
    while(fscanf(f, "%lf", &current) == 1)
        count++;

    arg->count = count;

    if(fscanf(f, "%lf", &current) != EOF)
    {
        cout << "Wrong input data in " << arg->filename << endl;
        arg->status = io_status::error_read;
        fclose(f);
        return;
    }

    fclose(f);

    if(arg->m == arg->p - 1)
        arg->is_last = true;

    return;
}

void remember_elements(ARGS *arg)
{
    if(arg->count == 0) return;

    FILE *f;

    f = fopen(arg->filename, "r");
    if(!f)
    {
        cout << "Can't open the file " << arg->filename << endl;
        arg->status = io_status::error_open;
        return;
    }

    if(fscanf(f, "%lf", &(arg->first)) == -1) {
        arg->status = io_status::error_read;
        fclose(f);
        return;
    }

    if(arg->count == 1) return;

    if(fscanf(f, "%lf", &(arg->second)) == -1)
    {
        arg->status = io_status::error_read;
        fclose(f);
        return;
    }
        
    fclose(f);
}

void arrange_elements(ARGS *arg)
{
    //Empty file
    if(arg->count == 0)
    {
        int i = 1;
        while(i < arg->p and (arg + i)->count == 0)
            i++;

        if(i == arg->p)
        {
            arg->is_last = true;
            return;
        }

        arg->first = (arg + i)->first;

        //Second element
        if((arg + i)->count >= 2)
        {
            arg->second = (arg + i)->second;
        } else
        {
            i++;

            if(i == arg->p)
            {
                arg->is_last = true;
                return;
            }

            while(i < arg->p and (arg + i)->count == 0)
                i++;

            if(i == arg->p)
            {
                arg->is_last = true;
                return;
            }

            arg->second = (arg + i)->first;
        }
    }

    //One element file
    if(arg->count == 1)
    {
        int i = 1;
        while(i < arg->p and (arg + i)->count == 0)
            i++;

        if(i == arg->p)
        {
            arg->is_last = true;
            return;
        }

        if(arg->m == arg->p - 1)
            arg->second = -1.7976931348623158e+308;
        else
            arg->second = (arg + i)->first;
    }
}

void fibonacci(ARGS *arg)
{
    if(arg->count == 0) return;

    FILE *f;

    f = fopen(arg->filename, "r");
    if(!f)
    {
        cout << "Can't open the file " << arg->filename << endl;
        arg->status = io_status::error_open;
        return;
    }

    double x1, x2, x3, max = -1.7976931348623158e+308;

    if(arg->count == 1)
    {
        if(arg->is_last) return;

//        cout << arg->first << " " << (arg + 1)->first << " " << (arg + 1)->second << " " << arg->filename << endl;
//        if(arg->m == arg->p -2) cout << (arg + 1)->second << " " << arg->filename << endl;
        if(fabs(arg->first + (arg + 1)->first - (arg + 1)->second) < eps)
        {
            if(arg->first >= (arg + 1)->first and arg->first >= (arg + 1)->second)
                arg->local_max = arg->first;
            else if((arg + 1)->first >= arg->first and (arg + 1)->first >= (arg + 1)->second)
                arg->local_max = (arg + 1)->first;
            else if((arg + 1)->second >= (arg + 1)->first and (arg + 1)->second >= arg->first)
                arg->local_max = (arg + 1)->second;
        }

        return;
    }

    if(fscanf(f, "%lf", &x1) == -1)
    {
        arg->status = io_status::error_read;
        fclose(f);
        return;
    }
    if(fscanf(f, "%lf", &x2) == -1)
    {
        arg->status = io_status::error_read;
        fclose(f);
        return;
    }

    while (fscanf(f, "%lf", &x3) == 1)
    {
        if(abs(x1 + x2 - x3) < eps)
        {
            if(x1 > max)
                max = x1;
            if(x2 > max)
                max = x2;
            if(x3 > max)
                max = x3;
        }

        x1 = x2;
        x2 = x3;
    }

    if(arg->is_last or arg->m == arg->p - 1)
    {
        arg->local_max = max;
        return;
    }

    if(abs(x2 - (arg + 1)->first + x1) < eps)
    {
        if(x1 > max)
            max = x1;
        if(x2 > max)
            max = x2;
    }

    if(abs(x2 + (arg + 1)->first - (arg + 1)->second) < eps)
    {
        if(x2 > max)
            max = x2;
    }

    arg->local_max = max;
}

void glob_max(ARGS *arg)
{
    if(arg->count == 0) return;

    double max = arg->local_max;
    for(int i = -arg->m; i < arg->p - arg->m; i++)
    {
        if((arg + i)->local_max > max)
            max = (arg + i)->local_max;
    }

    arg->global_max = max;
}

void loc_ans(ARGS *arg)
{
    if(arg->count == 0) return;

    FILE *f;

    f = fopen(arg->filename, "r");
    if(!f)
    {
        cout << "Can't open the file " << arg->filename << endl;
        arg->status = io_status::error_open;
        return;
    }

    double current;

    while(fscanf(f, "%lf", &current) == 1)
    {
        if(current > arg->global_max)
            arg->local_answer++;
    }

    fclose(f);
}

void *process_main(void *args)
{
    ARGS *arg = (ARGS*)args;
    int i, p = arg->p;

    file_check(arg);
    reduce_sum(p);

    int global_count = 0;
    for(i = -arg->m; i < p - arg->m; i++)
    {
        if((arg + i)->status == io_status::error_open or (arg + i)->status == io_status::error_read)
            return 0;

        global_count += (arg + i)->count;
    }

    if(global_count < 3) return 0;

    remember_elements(arg);
    reduce_sum(p);

    arrange_elements(arg);
    reduce_sum(p);

    fibonacci(arg);
    reduce_sum(p);

    for(i = -arg->m; i < p - arg->m; i++)
    {
        if((arg + i)->status == io_status::error_open or (arg + i)->status == io_status::error_read)
            return nullptr;
    }

    glob_max(arg);

    loc_ans(arg);
    reduce_sum(p);

    return 0;
}
