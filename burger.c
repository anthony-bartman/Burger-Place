/*
*	Anthony Bartman
*	Dr. Rothe
*	Lab 4
* 	Operating Systems
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "burger.h"
#include "ll.h"
#include "time.h"

/* LinkedList code copied from github with a mutex, singly-linkedList was
 obtained from: https://github.com/r-medina/ll */

//Global Variables
static ll_t *burgerTray;
static ll_t *fryerTray;
static int maxBurgers, maxFrys = 0;
static int dayOverFlag = 1;
// ** Used to lock and unlock burger or fry trays
static pthread_mutex_t trayMutex;

//Thread Methods
void *burgerThread(void *burgers);
void *fryerThread(void *fryers);
void *customerThread(void *customers);

//Helper Methods
static void checkIfDayOver();

burgerPlaceData *readFile(char *fileName)
{
    burger *burgerCooks = malloc(sizeof(burger));
    fryer *fryerCooks = malloc(sizeof(fryer));
    int totalCustomers = 0;
    FILE *file = fopen(fileName, "r");
    //Burgers
    fscanf(file, "%d", &burgerCooks->numCooks);
    fscanf(file, "%d", &burgerCooks->cookTime);
    fscanf(file, "%d", &burgerCooks->totalBurgers);
    //Fryers
    fscanf(file, "%d", &fryerCooks->numCooks);
    fscanf(file, "%d", &fryerCooks->cookTime);
    fscanf(file, "%d", &fryerCooks->totalFrys);
    //Customers
    fscanf(file, "%d", &totalCustomers);
    customer *customers = malloc(sizeof(customer) * totalCustomers);
    for (int i = 0; i < totalCustomers; i++)
    {
        fscanf(file, "%d", &customers[i].buyBurger);
        fscanf(file, "%d", &customers[i].buyFrys);
        fscanf(file, "%d", &customers[i].waitTime);
        customers[i].numOrdersGrabbed = 0;
        customers[i].ordersFilled = 0;
    }
    //Return all data from file
    burgerPlaceData *dayData = malloc(sizeof(burgerPlaceData));
   // dayData->customerData = malloc(sizeof(customer) * totalCustomers);

    dayData->burgerData = burgerCooks;
    dayData->fryerData = fryerCooks;
    dayData->totalCustomers = totalCustomers;
    dayData->customerData = customers;

    return dayData;
}

void startDay(burgerPlaceData *dayData)
{
    //Init Burger and Fryer trays
    burgerTray = ll_new(ll_no_teardown);
    fryerTray = ll_new(ll_no_teardown);
    pthread_mutex_init(&trayMutex, NULL);
    int test = 0;
    //Thread Arrays
    pthread_t burg_thread[dayData->burgerData->numCooks];
    pthread_t fryer_thread[dayData->fryerData->numCooks];
    pthread_t customer_thread[dayData->totalCustomers];

    /*
     *  Initialize Pthreads
     */
    for (int i = 0; i < dayData->burgerData->numCooks; i++)
    {
        test = 0;
        if ((test = pthread_create(&burg_thread[i], NULL, &burgerThread, (void *)(dayData->burgerData))))
        {
            printf("Error!");
            printf("Burger Thread %d Creation Failed", i);
        }
    }
    for (int i = 0; i < dayData->fryerData->numCooks; i++)
    {
        test = 0;
        if ((test = pthread_create(&fryer_thread[i], NULL, &fryerThread, (void *)(dayData->fryerData))))
        {
            printf("Error!");
            printf("Fryer Thread %d Creation Failed", i);
        }
    }
    for (int i = 0; i < dayData->totalCustomers; i++)
    {
        test = 0;
        if ((test = pthread_create(&customer_thread[i], NULL, &customerThread, (void *)(dayData->customerData + i))))
        {
            printf("Error!");
            printf("Customer Thread %d Creation Failed", i);
        }
    }

    /*
     *  Wait for Pthreads to finish
     */
    dayOverFlag = 1;
    for (int i = 0; i < dayData->burgerData->numCooks; i++)
    {
        pthread_join(burg_thread[i], NULL);
    }
    for (int i = 0; i < dayData->fryerData->numCooks; i++)
    {
        pthread_join(fryer_thread[i], NULL);
    }
    dayOverFlag = 0;
    for (int i = 0; i < dayData->totalCustomers; i++)
    {
        pthread_join(customer_thread[i], NULL);
    }

    /*
     *  Day has Ended, Display Statistics
     */
    printf("\n-------------------------------\n");
    printf("     End of Day Statistics      \n");
    printf("-------------------------------\n");
    if (maxBurgers > 0)
    {
        printf("Max Burgers in warming tray: %d\n", maxBurgers);
    }
    if (maxFrys > 0)
    {
        printf("Max Frys in warming tray: %d\n", maxFrys);
    }
    for (int i = 0; i < dayData->totalCustomers; i++)
    {
        printf("Customer %d had their order filled %d times\n", i, dayData->customerData[i].ordersFilled);
    }
}

void deleteDayData(burgerPlaceData *burgerPlaceData)
{
    free(burgerPlaceData->burgerData);
    free(burgerPlaceData->fryerData);
    free(burgerPlaceData->customerData);
    free(burgerPlaceData);

    ll_delete(burgerTray);
    ll_delete(fryerTray);
}

//Thread Methods
void *burgerThread(void *burgers)
{
    burger *burgerData = ((burger *)burgers);
    for (int i = 0; i < burgerData->totalBurgers; i++)
    {
        usleep(burgerData->cookTime);
        pthread_mutex_lock(&trayMutex);
        ll_insert_first(burgerTray, "=");
        //Determine Max Burgers
        if (maxBurgers <= burgerTray->len)
        {
            maxBurgers = burgerTray->len;
        }
        pthread_mutex_unlock(&trayMutex);
    }
    pthread_exit(0);
}

void *fryerThread(void *fryers)
{
    fryer *fryerData = ((fryer *)fryers);
    for (int i = 0; i < fryerData->totalFrys; i++)
    {
        usleep(fryerData->cookTime);
        pthread_mutex_lock(&trayMutex);
        ll_insert_first(fryerTray, "+");
        //Determine Max Frys
        if (maxFrys <= fryerTray->len)
        {
            maxFrys = fryerTray->len;
        }
        pthread_mutex_unlock(&trayMutex);
    }
    pthread_exit(0);
}

void *customerThread(void *customers)
{
    customer *customerN = (customer *)customers;
    while (dayOverFlag)
    {
        //Burgers
        int burgersTaken = 0;
        while (burgersTaken != customerN->buyBurger)
        {
            if (ll_get_first(burgerTray))
            {
                pthread_mutex_lock(&trayMutex);
                ll_remove_first(burgerTray);
                burgersTaken++;

                //Check if customer's burger order is complete.
                if (burgersTaken == customerN->buyBurger)
                {
                    customerN->numOrdersGrabbed += 1;
                }
                pthread_mutex_unlock(&trayMutex);
            }
            checkIfDayOver();
        }

        //Frys
        int frysTaken = 0;
        while (frysTaken != customerN->buyFrys)
        {
            if (ll_get_first(fryerTray))
            {
                pthread_mutex_lock(&trayMutex);
                ll_remove_first(fryerTray);
                frysTaken++;

                //Check if customer fry order is complete.
                if (frysTaken == customerN->buyFrys)
                {
                    customerN->numOrdersGrabbed += 1;
                }
                pthread_mutex_unlock(&trayMutex);
            }
            checkIfDayOver();
        }
        //Wait time if the customer gets their fry and burger order completed
        if (customerN->numOrdersGrabbed == 2)
        {
            usleep(customerN->waitTime);
            customerN->ordersFilled += 1;
            customerN->numOrdersGrabbed = 0;
        }
        checkIfDayOver();
    }
    pthread_exit(0);
}

//Helper Methods
/**
 * This method checks if the burger and fryer cooks have created all of their 
 * elements for the day. If that is true, kill the customer threads and display
 * statistics for the day.
 */
static void checkIfDayOver()
{
    if (dayOverFlag == 0)
    {
        pthread_mutex_unlock(&trayMutex);
        pthread_exit(0);
    }
}
