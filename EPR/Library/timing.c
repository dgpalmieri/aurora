/*
 * File: timing.c
 *
 * Functions for gathering timing information and computing timing statistics.
 *
 */

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#if defined(SHM)
 #include <mpp/shmem.h>           /* inter-PE data transfer via shmem */
#elif defined(MPI)
 #include <mpi.h>                 /* inter-PE data transfer via MPI   */
#endif

#if defined(MTA)
 #include <machine/runtime.h>
 #include <sys/tera_task.h>
#endif

int my_PE;
int num_PES;
#if 0
extern int my_PE;
extern int num_PES;
#endif

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
#elif defined(MTA)
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
#elif defined(MTA)
  frequency = tera_clock_freq();
#else
  frequency = (double) CLOCKS_PER_SEC;
#endif

  return frequency;

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
  long pwrk1[_SHMEM_REDUCE_MIN_WRKDATA_SIZE];
  long pwrk2[_SHMEM_REDUCE_MIN_WRKDATA_SIZE];
  long psync1[_SHMEM_REDUCE_SYNC_SIZE];
  long psync2[_SHMEM_REDUCE_SYNC_SIZE];

  for (i=0 ; i<_SHMEM_REDUCE_SYNC_SIZE ; i++)
    psync1[i] = psync2[i] = _SHMEM_SYNC_VALUE;

#endif

  /* pick up the unit of time used on this machine */
  frequency = get_freq();

  /* not all timed sections may involve all PEs (image transfer for example) */
  if (num_PES_used > 1) {

#if defined(SHM)

    /* find the wall-clock start and finish and sum elapsed times on each PE */
    shmem_barrier_all();
    shmem_long_max_to_all(&max, &accum, 1, 0, 0, num_PES, pwrk1, psync1);
    shmem_long_sum_to_all(&sum, &accum, 1, 0, 0, num_PES, pwrk2, psync2);

#elif defined(MPI)

    /* find the wall-clock start and finish and sum elapsed times on each PE */
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Reduce(&accum, &max, 1, MPI_LONG, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&accum, &sum, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

#endif

    /* compute average of elapsed times on each PE */
    average = sum / (num_PES * frequency);

    /* compute elapsed time for the PE group (wall-clock) */
    elapsed = max / frequency;

    if (my_PE == 0) {
      fprintf(stderr," AVERAGE = %.4lfs", average);
      fprintf(stderr," (average time for each PE to finish the task)\n");
      fprintf(stderr," ELAPSED = %.4lfs", elapsed);
      fprintf(stderr," (wall clock time for the PE group to finish)\n\n");
      fflush(stderr);
    }

  } else {

    /* compute elapsed time for this job (wall-clock) */
    elapsed = accum / frequency;

    fprintf(stderr," ELAPSED = %.4lfs", elapsed);
    fprintf(stderr," (wall clock time for this task to finish)\n\n");
    fflush(stderr);
  }
}

/*---------------------------------------------------------------------------*/

void print_timing(
char   *msg1,
double seconds,
char   *msg2
)
{
  if (seconds < 60.0)
    printf("%s %.2lf seconds%s", msg1, seconds, msg2);
  else if ( seconds < 3600.0)
    printf("%s %.2lf minutes%s\n", msg1, seconds*0.0166666, msg2);
  else
    printf("%s %.2lf hours%s\n", msg1, seconds*0.0002777777, msg2);
}

/*---------------------------------------------------------------------------*/

