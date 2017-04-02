#include <pthread.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

#define NUM_THREADS 5
#define NUM_FLIGHTS 10
#define FLIGHT_CAPACITY 5

// data structures for storing the flight information
int booking_status[NUM_FLIGHTS] = {0};
pthread_mutex_t mutex_booking_status = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;    

struct query_data
{
    string query;
    int flight_number;
};

void *process_query(void *arg)
{
    query_data* current_query = (query_data *) arg;
    for(;;)
    {
        pthread_mutex_lock(&mutex_booking_status);
        pthread_cond_wait(&cond, &mutex_booking_status);    
        cout << current_query -> query << current_query -> flight_number; 
        pthread_mutex_unlock(&mutex_booking_status);
        pthread_exit(NULL);
    }
}

int main(int argc,char *argv[])
{

    //thread data_structures
    pthread_t threads[NUM_THREADS];
    query_data queries[NUM_THREADS];

    for(int i = 0;i < NUM_THREADS;i++)
    {
        int rc = pthread_create(&threads[i],NULL,process_query,&queries[i]);
        if(rc)
        {
            cout << "Thread " << i << " creation failed." << endl;
            return -1;
        }
    }
    // begin processing the transactions file
    ifstream transactions_file("transactions");    
    for(string line; getline(transactions_file,line);)
    {
        if(line == "END")
        {
            cout << "[END] All queries have been processed. Have a nice day!" << endl;
            return 0;
        }

        pthread_mutex_lock(&mutex_booking_status);
        istringstream iss(line);
        query_data *current_query;
        iss >> current_query->query >> current_query->flight_number;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex_booking_status);
        
    }
    return 0;
}
