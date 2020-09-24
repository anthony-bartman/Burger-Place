/*
*	Anthony Bartman
*	Dr. Rothe
*	Lab 4
* 	Operating Systems
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "burger.h"
#include "time.h"

/**
 * Introduction –
 *   This lab has students simulate a restaurant that has customers and burger and fryer cooks. Each
 * customer and cook will be created using their own unique thread and run the code simulatneously.
 * The program will read in a file that will "simulate" a day at the restaurant. The day will end 
 * once all of the fryer and burger cooks finish creating their maximum amount from the restaurant
 * file. 
 *  
 * Design – 
 *   To begin my design, I began to write down everything I knew about each attribute of the lab and
 * how will I store them in memory. After I started understanding how the lab is suppose to function,
 * I began to create a bunch of structs to store each object as described in the lab handout. I then 
 * began to realize I need to find a thread-safe linked list in order to store burgers and frys on a
 * "warming tray". After this, I began to work on the logic of my thread methods by using a global 
 * mutex in order to lock and unlock the linkedlist whenever adding or removing items to the "warming 
 * trays." Finally, I outputted the end of the day statistics about the customers and cooks.
 * 
 * Build Instructions –
 *   To run my program, you must have a text file that is formatted correctly as stated by the lab 
 * descriptio, in order to setup the restaurant correctly.  Once a valid file has been created, 
 * you can run my burger lab code by using:
 *                               " ./main <nameOfResturauntFile>"
 * Analysis –
 *   My implementation is a little unfair because if a customer only wants fries or only wants burgers,
 * that customer will always get to go to the front of the line because he only needs one of his desires
 * fulfilled rather than having a burger and fry order filled. The customer with only one order will get
 * a lot more food in a short amount of time compared to the other customers. My definition of fair is
 * that everyone gets the same amount of food for the same amount of time. If the behavior described in
 * the analysis section of the lab was implemented, this would not be fair because if a customer wants
 * more food than the other customer, he could potentially never get his food if another customer wants
 * less food than him. In order to give a customer an order number every time they need more food, I
 * would probably implement a FIFO queue system in order to achieve this implementation. This queue would
 * be able to choose which customer ordered first and which customer to give the correct order to as well.
 * The Burger Place should not have limitless warmers because that is not practical. To modify my code in
 * order to make it more reasonable, I would have a flag to tell the burger and fryer threads that if you
 * reach this amount of elements in the warmers, stop creating burgers until some are taken out.  
 * 
 * Conclusion –
 *   The hardest part of this lab was understanding conceptually how multi-threading works and how to 
 * debug my code while using multi-threading. I enjoyed the idea of creating a restuaraunt in order to
 * understanding multi-threading. I think this is an excellent introduction for how multi-threading works.
 */

//Helper Method
static void displayBurgerDay(burgerPlaceData *dayData);

//Main Method
int main(int argc, char *argv[])
{
    //Argv[0] = makefile, Argv[1] = burgerPlaceFile
    if (argc == 2)
    {
        burgerPlaceData *dayData = readFile(argv[1]);
        displayBurgerDay(dayData);
        startDay(dayData);
        deleteDayData(dayData);
        return 0;
    }
    else
    {
        printf("Error!");
        printf("Enter a valid file as the 2nd argument");
        return 0;
    }
}

//Method used to display the restauraunt file contents
static void displayBurgerDay(burgerPlaceData *dayData)
{
    printf("\n|[  Burger Place Data  ]|\n");
    if (dayData->burgerData->numCooks != 0)
    {
        printf("\n == Burger Data: ==\n");
        printf("Burger Cooks: %d\n", dayData->burgerData->numCooks);
        printf("Burger Cook Time: %d\n", dayData->burgerData->cookTime);
        printf("Total Burgers: %d\n", dayData->burgerData->totalBurgers);
    }
    else
    {
        printf("\n || No Burger Cooks Today! ||\n");
    }
    if (dayData->fryerData->numCooks != 0)
    {
        printf("\n == Fryer Data: ==\n");
        printf("Fryer Cooks: %d\n", dayData->fryerData->numCooks);
        printf("Fryer Cook Time: %d\n", dayData->fryerData->cookTime);
        printf("Total Frys: %d\n", dayData->fryerData->totalFrys);
    }
    else
    {
        printf("\n || No Fryer Cooks Today! ||\n");
    }
    if (dayData->totalCustomers != 0)
    {

        printf("\n ** Number of Customers: %d", dayData->totalCustomers);
        printf("\n == Customer Data: ==\n");
        for (int i = 0; i < dayData->totalCustomers; i++)
        {
            printf("Customer %d Burgers: %d Frys: %d Wait Time: %d\n", i,
                   dayData->customerData[i].buyBurger, dayData->customerData[i].buyFrys,
                   dayData->customerData[i].waitTime);
        }
    }
    else
    {
        printf("\n || No Customer's At Restauraunt! ||\n");
    }
}
