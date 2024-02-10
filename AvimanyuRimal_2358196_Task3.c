// C-Programming Assignment Task-03(Finding prime numbers from multiple text files using multithreading)
// Student Id : 2358196
// Name : Avimanyu Rimal

// Including necessary header files
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

// Global variables for time tracking and prime number storage
clock_t start, end;
double cpu_time;
int *prime_arr, primecount = 0;
FILE *fileptr;

// Mutex for thread synchronization
pthread_mutex_t mutex;

// Structure to define start and finish indices for each thread
struct calculate
{
    int start;
    int finish;
};

// Function to check if a number is prime using recursion
int check_prime(int n, int i)
{
    if (n <= 2)
        return (n == 2) ? 1 : 0;
    if (n % i == 0)
        return 0;
    if (i * i > n)
        return 1;
    return check_prime(n, i + 1);
}

// Function executed by each thread to find prime numbers in a specific range
void *is_prime(void *ptr)
{
    struct calculate *arg = ptr;
    int st = arg->start, fi = arg->finish;
    int i, j;

    // Iterating through the assigned range to find prime numbers
    for (i = st; i <= fi; i++)
    {
        int num = *(prime_arr + i);

        // Checking if the number is prime
        if (check_prime(num, 2) == 1)
        {
            // Locking the mutex before updating shared variables
            pthread_mutex_lock(&mutex);

            // Incrementing prime count and writing the prime number to the output file
            primecount++;
            fprintf(fileptr, "%d\n", num);

            // Unlocking the mutex after updating shared variables
            pthread_mutex_unlock(&mutex);
        }
    }
}

// Main function
void main()
{
    // Initializing variables
    int i, n = 0, j = 0, nof = 3;
    FILE *fptr;
    char filename[15], chr;
    int no_of_threads;

    // Counting the total number of prime numbers across all files
    for (i = 1; i <= nof; i++)
    {
        sprintf(filename, "PrimeData%d.txt", i);
        fptr = fopen(filename, "r");
        chr = getc(fptr);

        // Counting the number of lines in each file
        while (chr != EOF)
        {
            if (chr == '\n')
            {
                if (chr != ' ')
                    n++;
            }
            chr = getc(fptr);
        }
        fclose(fptr);
    }

    // User input for the number of threads to be used
    printf("Find prime numbers from total (%d) numbers to check. Use multithreading to make the process faster", n);
    printf("\n\nEnter the number of threads to use: ");
    scanf("%d", &no_of_threads);

    // Creating an array of structures to define thread ranges
    struct calculate divider[no_of_threads];

    // Dynamically allocating memory for the prime number array
    prime_arr = (int *)malloc(n * sizeof(int));

    // Reading prime numbers from files and storing them in the array
    for (i = 1; i <= nof; i++)
    {
        sprintf(filename, "PrimeData%d.txt", i);
        fptr = fopen(filename, "r");

        while (!feof(fptr))
        {
            fscanf(fptr, "%d\n", (prime_arr + j));
            j++;
        }
        fclose(fptr);
    }

    // Calculating equal slices of data for each thread
    int slice_list[no_of_threads];
    for (i = 0; i < no_of_threads; i++)
        slice_list[i] = n / no_of_threads;

    // Handling any remaining elements not evenly distributed
    int reminder = n % no_of_threads;
    for (i = 0; i < reminder; i++)
        slice_list[i]++;

    // Defining start and end indices for each thread
    int start_list[no_of_threads], end_list[no_of_threads];
    for (i = 0; i < no_of_threads; i++)
    {
        if (i == 0)
            start_list[i] = 0;
        else
            start_list[i] = end_list[i - 1] + 1;
        end_list[i] = start_list[i] + slice_list[i] - 1;
    }

    // Opening the output file for writing
    fileptr = fopen("OutputPrimeData.txt", "w");

    // Starting the clock for time tracking
    start = clock();

    // Initializing the mutex for thread synchronization
    pthread_mutex_init(&mutex, NULL);

    // Creating threads to find prime numbers
    pthread_t threads[no_of_threads];
    for (i = 0; i < no_of_threads; i++)
    {
        divider[i].start = start_list[i];
        divider[i].finish = end_list[i];
        pthread_create(&threads[i], NULL, is_prime, &divider[i]);
    }

    // Waiting for all threads to finish
    for (i = 0; i < no_of_threads; i++)
        pthread_join(threads[i], NULL);

    // Destroying the mutex after all threads are done
    pthread_mutex_destroy(&mutex);

    // Stopping the clock and calculating the CPU time
    end = clock();
    cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf(" Total Time taken = %lf seconds  ", cpu_time);

    // Displaying the total count of prime numbers
    printf("\n\n Total Prime Count: %d\n", primecount);

    // Writing a summary to the output file
    fprintf(fptr, "\n\nThere are a total of %d prime numbers.", primecount);

    // Freeing the allocated memory
    free(prime_arr);

    // Displaying the completion message
    printf("\nProcess complete!!\n\n\"OutputPrimeData.txt\" file has been created.\n");
    
    // Closing the output file
    fclose(fileptr);
}
