/*
*	Anthony Bartman
*	Dr. Rothe
*	Lab 4
* 	Operating Systems
*/

#ifndef BURGER_H
#define BURGER_H

/* burger.h
 * 
 * This .h file holds all the public methods used in burger.c.
 * 
 */

/* Structures */
typedef struct burger{
    int numCooks;
    int cookTime;
    int totalBurgers;
} burger;

typedef struct fryer{
    int numCooks;
    int cookTime;
    int totalFrys;
}fryer;

typedef struct customer{
    int buyBurger;
    int buyFrys;
    int waitTime;
    int numOrdersGrabbed;//Used to see if customer had fry or burger order filled
    int ordersFilled;
}customer;

typedef struct burgerPlaceData{
    burger* burgerData;
    fryer* fryerData;
    int totalCustomers;
    customer* customerData;
}burgerPlaceData;

/* Method Declarations */

/* readFile()
 *     This method will read in a proper burger place file, then create
 * 	   and store those contents in a couple different structs stated above
 *     in order to simulate a day at a multi-threaded restauraunt. 
 *         Parameters:  fileName - the bugerplace file being stored 
 *         Returns: burgerPlaceData*, new created burger place data struct with 
 *     all of the data from file organized in memory
 */
burgerPlaceData* readFile(char *fileName);

/* startDay()
 *     This method will read in a proper matrix file type, then create
 * 	   and store those contents in a matrix data structure linearly
 *         Parameters:  dayData* - All data read from the burgerplace file
 *     that will be used to simulate a day at the restauraunt
 *         Returns: void
 */
void startDay(burgerPlaceData *dayData);

/* deleteDayData()
 *     This method will delete the linked list data and free the malloc'd 
 *     structs to ensure no memory leaks.
 *         Parameters:  burgerPlaceData* - data to be deleted
 *         Returns: void
 */
void deleteDayData(burgerPlaceData *burgerPlaceData);

#endif