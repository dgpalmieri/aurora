/*
 * File: timing.cpp
 *
 * Functions for gathering timing information and computing timing statistics.
 *
 */

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <vector>
#include <iostream>
#include <iomanip>
#include "timing.hpp"

#ifdef SHM
    #include <mpp/shmem.h>           /* inter-PE data transfer via shmem */
#elif MPI
    #include <mpi.h>                 /* inter-PE data transfer via MPI   */
#endif

#ifdef MPI
    #include <machine/runtime.h>
    #include <sys/tera_task.h>
 #endif

 int my_PE;
 int num_PES;

 /*---------------------------------------------------------------------------*/
/* Poll the processor clock for the current time.  The routine used for this */
/* is platform specific.  Return value might be in ticks or microseconds.    */
/*                                                                           */
unsigned long get_time(unsigned long prevtime) {
    unsigned long mytime=0;
    #if defined(SP2) || defined(SUN)
        struct timeval Tvalue;
        gettimeofday(&Tvalue, NULL);
        mytime = (Tvalue.tv_sec * 1000000 + Tvalue.tv_usec) - prevtime;
    #elif MTA
        mytime = terart_clock(prevtime);
    #else
        mytime = clock() - prevtime;
    #endif

    return mytime;
}

/*---------------------------------------------------------------------------*/
/* Determine the platform specific unit of time to use in converting elapsed */
/* and average processor time to seconds.  Might be ticks or microseconds.   */
/*                                                                           */
double get_freq(void) {
    double frequency=0.0;
    #if defined(SP2) || defined(SUN)
        frequency = 1000000.0;
    #elif MTA
        frequency = tera_clock_freq();
    #else
        frequency = (double) CLOCKS_PER_SEC;
    #endif

    return frequency;

}

double get_average(unsigned long sum, double frequency) {
    return sum / (num_PES * frequency);
}

double get_elapsed_time(unsigned long num, double denom) {
    return num / denom;
}

/*---------------------------------------------------------------------------*/
/* A routine for computing and printing timing statistics.  All PEs in the   */
/* processing group must call this routine, as it contains global barriers.  */
/*                                                                           */
void compute_time(unsigned long accum, int num_PES_used) {

    double frequency, average, elapsed;
    unsigned long max=0, sum=0;

    #if defined(SHM)

        int  i;
        std::vector<long> pwrk1(_SHMEM_REDUCE_MIN_WRKDATA_SIZE);
        std::vector<long> pwrk2(_SHMEM_REDUCE_MIN_WRKDATA_SIZE);
        std::vector<long> psync1(_SHMEM_REDUCE_SYNC_SIZE);
        std::vector<long> psync2(_SHMEM_REDUCE_SYNC_SIZE);

        for (i=0 ; i<_SHMEM_REDUCE_SYNC_SIZE ; i++)
            psync1[i] = psync2[i] = _SHMEM_SYNC_VALUE;

    #endif

    /* pick up the unit of time used on this machine */
    frequency = get_freq();

    /* not all timed sections may involve all PEs (image transfer for example) */
    if (num_PES_used > 1) {

        #ifdef SHM
            /* find the wall-clock start and finish and sum elapsed times on each PE */
            shmem_barrier_all();
            shmem_long_max_to_all(&max, &accum, 1, 0, 0, num_PES, pwrk1, psync1);
            shmem_long_sum_to_all(&sum, &accum, 1, 0, 0, num_PES, pwrk2, psync2);

        #elif MPI
            /* find the wall-clock start and finish and sum elapsed times on each PE */
            MPI_Barrier(MPI_COMM_WORLD);
            MPI_Reduce(&accum, &max, 1, MPI_LONG, MPI_MAX, 0, MPI_COMM_WORLD);
            MPI_Reduce(&accum, &sum, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
        #endif

        /* compute average of elapsed times on each PE */
        average = get_average(sum, frequency);

        /* compute elapsed time for the PE group (wall-clock) */
        elapsed = get_elapsed_time(max, frequency);

        if (my_PE == 0) {
            std::cerr << std::fixed << std::setprecision(4);
            std::cerr << " AVERAGE = " << average << "s";
            std::cerr << " (average time for each PE to finish the task)\n";
            std::cerr << " ELAPSED = " << elapsed << "s";
            std::cerr << " (wall clock time for the PE group to finish)\n\n";
            std::cerr.flush();
        }
    } 
    else {
        /* compute elapsed time for this job (wall-clock) */
        elapsed = get_elapsed_time(accum, frequency);
        std::cerr << std::fixed << std::setprecision(4);
        std::cerr << " ELAPSED = " << elapsed << "s";
        std::cerr << " (wall clock time for this task to finish)\n\n";
        std::cerr.flush();
    }
}

/*---------------------------------------------------------------------------*/

void print_timing(
char   *msg1,
double seconds,
char   *msg2
)
{
    std::cout << std::fixed << std::setprecision(2);    
    if (seconds < 60.0)
        std::cout << msg1 << " " << seconds << " seconds" << msg2;
    else if ( seconds < 3600.0)
        std::cout << msg1 << " " << seconds*0.0166666 << " minutes" << msg2 << "\n";
    else
        std::cout << msg1 << " " << seconds*0.0002777777 << " hours" << msg2 << "\n";
}

/*---------------------------------------------------------------------------*/

