//
// Created by varsem on 11.11.23.
//
#include "functions.h"

int main(int ac, char *av[])
{
    try
    {
        int p = ac - 1, i;

        //Заполним аргументы
        ARGS *args = new ARGS[p];
        for(i = 0; i < p; i++)
        {
            args[i].filename = av[i + 1];
            args[i].m = i;
            args[i].p = p;
        }

        // Создаем потоки
        pthread_t *threads = new pthread_t[p];
        for(i = 0; i < p; i++)
        {
            if(pthread_create(threads + i, 0, process_main, args + i))
            {
                cout << "Cannot create thread " << i << endl;
                delete[] threads;
                delete[] args;
                return -4;
            }
        }

        //Ждем потоки
        for(int i = 0; i < p; i++)
        {
            if(pthread_join(threads[i], 0))
                cout << "Cannot wait thread " << i << endl;
        }

        for(i = 0; i < p; i++)
        {
            if(args[i].status != io_status::none)
                return -1;
        }

//        for(i = 0; i < p; i++)
//            args[i].print();

        int global_answer = 0;

        for(i = 0; i < p; i++)
            global_answer += args[i].local_answer;

        cout << "ANSWER: " << global_answer << endl;

        delete[] threads;
        delete[] args;

    } catch (const bad_alloc& e)
    {
        cout << "Bad alloc" << endl;
        return -1;
    }

    return 0;
}