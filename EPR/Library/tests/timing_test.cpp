#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "../timing.h"
#include <chrono>
#include <thread>
#include <fstream>
#include <cstdio>

/*******************************************************
 *                 Auxillary functions                 *
 *******************************************************/
void sleep(int sleepTime) { //time in milliseconds
    auto start_tick = clock();
    auto end_tick = clock();
    while ((end_tick - start_tick)/CLOCKS_PER_SEC * 1000 < sleepTime)
    {
        end_tick = clock();
    }
    
}

void setupComputeTimeTest(unsigned long accum, int numPESUsed, void (*computeTime) (unsigned long, int)) {
    FILE *o = stderr;
    stderr = std::fopen("computetimeoutput.txt", "w");
    computeTime(accum, numPESUsed);
    fclose(stderr);
    stderr = o;
}

void setupPrintTimingTest(char * msg1, double duration, char * msg2, void (*printTiming) (char *, double, char *)) {
    FILE *o = stdout;
    stdout = std::fopen("printtimingoutput.txt", "w");
    printTiming(msg1, duration, msg2);
    fclose(stdout);
    stderr = o;
}

std::string formatDuration(int seconds) {
    if(seconds < 60) {
        double secondsRoundedSecondDecimal = floor(100*seconds+0.5)/100;
        std::string secondsWithTwoDecimals = std::to_string(secondsRoundedSecondDecimal).substr(0, 4);
        return secondsWithTwoDecimals;
    } else if (seconds < 3600) {
        double minutesRoundedSecondDecimal = floor(100*seconds*0.0166666+0.5)/100;
        std::string minutesWithTwoDecimals = std::to_string(minutesRoundedSecondDecimal).substr(0, 4);
        return minutesWithTwoDecimals;
    } else {
        double hoursRoundedSecondDecimal = floor(100*seconds*0.0002777777+0.5)/100;
        std::string hoursWithTwoDecimals = std::to_string(hoursRoundedSecondDecimal).substr(0, 4);
        return hoursWithTwoDecimals;
    }
}

std::string readFile(std::string filename) {
    std::ifstream file(filename);
    std::string output;
    std::string temp;
    while(std::getline(file, temp)) {
        temp.erase(std::remove(temp.begin(), temp.end(), '\n'), temp.end());
        output += temp;
    }

    return output;
}


/*******************************************************
 *                    Test Cases                       *
 *******************************************************/
TEST_CASE("testing the get_time function") {
    for(int seconds=1; seconds < 6; seconds++) {
        for(int iterations=0; iterations < 6; iterations++) {
            auto val1 = get_time(0);
            sleep(seconds * 1000);
            auto val2 = get_time(0);
            CHECK((unsigned long) (val2 - val1)/CLOCKS_PER_SEC == seconds);
        }
    }     
}

TEST_CASE("Testing the compute_time function") {
    setupComputeTimeTest(10000000000, 4, &compute_time);
    std::string lines;
    std::string temp;
    std::string message = " AVERAGE = -nans (average time for each PE to finish the task) ELAPSED = 0.0000s (wall clock time for the PE group to finish)";
    std::ifstream file("computetimeoutput.txt");
    while(std::getline(file, temp)) {
        temp.erase(std::remove(temp.begin(), temp.end(), '\n'), temp.end());
        lines += temp;
    }

    CHECK(lines == message);
}


TEST_CASE("Testing the get_freq function") {
    CHECK(get_freq() ==  (double) CLOCKS_PER_SEC);
}

TEST_CASE("Testing the print_timing function") {
    SUBCASE("Seconds less than a minute") {
        int seconds = 6;
        setupPrintTimingTest((char *) "The process took", seconds, (char *) " to complete", &print_timing);
        std::string formattedSeconds = formatDuration(seconds);
        std::string message = "The process took " + formattedSeconds + " seconds to complete";
        std::string output = readFile("printtimingoutput.txt");
        CHECK(output == message);
    }

    SUBCASE("Seconds minute or more and less than an hour") {
        int seconds = 60;
        setupPrintTimingTest((char *) "The process took", seconds, (char *) " to complete", &print_timing);
        std::string formattedMinutes = formatDuration(seconds);
        std::string message = "The process took " + formattedMinutes + " minutes to complete";
        std::string output = readFile("printtimingoutput.txt");
        CHECK(output == message);
    }

    SUBCASE("Seconds more than an hour") {
        int seconds = 3660;
        setupPrintTimingTest((char *) "The process took", seconds, (char *) " to complete", &print_timing);
        std::string formattedMinutes = formatDuration(seconds);
        std::string message = "The process took " + formattedMinutes + " hours to complete";
        std::string output = readFile("printtimingoutput.txt");
        CHECK(output == message);
    }

    std::remove("printtimingoutput.txt");
    std::remove("computetimeoutput.txt");
}