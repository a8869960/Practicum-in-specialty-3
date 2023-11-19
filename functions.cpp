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

void remember_elements(ARGS *arg)
{
    FILE *f;

    f = fopen(arg->filename, "r");
    if(!f)
    {
        cout << "Can't open the file " << arg->filename << endl;
        arg->status = io_status::error_open;
        return;
    }

    if(fscanf(f, "%lf", &(arg->first)) == -1)
    {
        arg->type = type_inf::empty;
        fclose(f);
        return;
    }

    if(fscanf(f, "%lf", &(arg->second)) == -1)
    {
        arg->type = type_inf::one;
        fclose(f);
        return;
    }

    arg->type = type_inf::many;
    
    double helper = 0;
    while(fscanf(f, "%lf", &helper) == 0)
        helper = 0;

    if(fscanf(f, "%lf", &helper) != EOF)
    {
        cout << "Wrong input data in " << arg->filename << endl;
        arg->status = io_status::error_read;
        fclose(f);
        return;
    }
        
    fclose(f);
}

void arrange_elements(ARGS *arg)
{
    //Empty file
    if(arg->type == type_inf::empty)
    {
        int i = 1;
        while(i < arg->p and (arg + i)->type == type_inf::empty)
            i++;

        arg->first = (arg + i)->first;

        //Second element
        if((arg + i)->type == type_inf::many)
        {
            arg->second = (arg + i)->second;
        } else
        {
            i++;

            while(i < arg->p and (arg + i)->type == type_inf::empty)
                i++;

            if(i > arg->p) return;

            arg->second = (arg + i)->first;
        }
    }

    //Second element
    if(arg->type == type_inf::one)
    {
        int i = 1;
        while(i < arg->p and (arg + i)->type != type_inf::many)
            i++;

        arg->second = (arg + i)->first;
    }
}

void fibonacci(ARGS *arg)
{
    if(arg->type == type_inf::empty) return;

    FILE *f;

    f = fopen(arg->filename, "r");
    if(!f)
    {
        cout << "Can't open the file " << arg->filename << endl;
        arg->status = io_status::error_open;
        return;
    }

    double x1, x2, x3, max = -1.7976931348623158e+308;

    if(arg->type == type_inf::one)
    {
        if(arg->m == arg->p - 1) return;

        if(abs(arg->first + (arg + 1)->first - (arg + 1)->second) < 1e-16)
            arg->local_max = arg->first;

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

    while (fscanf(f, "%lf", &x3) != -1)
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

    if(arg->m == arg->p - 1) return;

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

    double helper;
    if(fscanf(f, "%lf", &helper) != EOF)
    {
        cout << "Wrong input data in " << arg->filename << endl;
        arg->status = io_status::error_read;
        fclose(f);
        return;
    }

    arg->local_max = max;
}

void glob_max(ARGS *arg)
{
    if(arg->type == type_inf::empty) return;

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
    if(arg->type == type_inf::empty) return;

    FILE *f;

    f = fopen(arg->filename, "r");
    if(!f)
    {
        cout << "Can't open the file " << arg->filename << endl;
        arg->status = io_status::error_open;
        return;
    }

    double current;

    while(fscanf(f, "%lf", &current) != -1)
    {
        if(current > arg->global_max)
            arg->local_answer++;
    }

    double helper;
    if(fscanf(f, "%lf", &helper) != EOF)
    {
        cout << "Wrong input data in " << arg->filename << endl;
        arg->status = io_status::error_read;
        fclose(f);
        return;
    }

    fclose(f);
}

void *process_main(void *args)
{
    ARGS *arg = (ARGS*)args;
    int i, p = arg->p;

    remember_elements(arg);
    reduce_sum(p);

    for(i = -arg->m; i < p - arg->m; i++)
    {
        cout << "p";
        if((arg + i)->status == io_status::error_open or (arg + i)->status == io_status::error_read)
            return 0;
    }

    arrange_elements(arg);
    reduce_sum(p);

    fibonacci(arg);
    reduce_sum(p);

    for(i = -arg->m; i < p - arg->m; i++)
    {
        if((arg + i)->status == io_status::error_open or (arg + i)->status == io_status::error_read)
            return 0;
    }

    glob_max(arg);

    loc_ans(arg);
    reduce_sum(p);

    return 0;
}
