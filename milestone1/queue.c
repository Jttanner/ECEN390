#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"
#include <assert.h>


// Allocates the memory to you queue (the data* pointer) and initializes all parts of the data structure.
// Prints out an error message if malloc() fails and calls assert(false) to
// print-out line-number information and die.
//  queue_t * newQueue;
//  queue_init(newQueue, 500, "a name")

void queue_init(queue_t* q, queue_size_t size, const char* name){

    q->underflowFlag = false;  // True if queue_pop() is called on an empty queue.
  q-> overflowFlag = false;  // True if queue_push() is called on a full queue.
  q->indexIn = 0;
  q->indexOut = 0;
  q->elementCount = 0;       // Not required but may ease implementation.
  q->size = size+1; // Add one additional location for the empty location; size is data-array size, exactly.
  q->data = (queue_data_t *) malloc(q->size * sizeof(queue_data_t));
  if (q->data == 0) {
    printf("Error!!!: queue_init() failed to allocate the required memory in queue_init()!!! (%ld).\n\r", size);
    printf("Check your heap and stack size and set them both to 2,000,000 bytes (2,000 kB).\n\r");
    printf("View Video V11 on the ECEn 330 Youtube channel to see how to set heap and stack size.\n\r");
    printf("Copy this link to your browser: https://www.youtube.com/watch?v=onO-XojGFMM&feature=youtu.be\n\r");
    assert(false);
  }
  strncpy(q->name, name, QUEUE_MAX_NAME_SIZE);
#ifdef QUEUE_PRINT_INFO_MESSAGES
  printf("initialized %s.\n\r", q->name);
#endif
}

// Get the user-assigned name for the queue.
const char* queue_name(queue_t* q){
    return q->name;                        //return name of queue
}

// Returns the number of elements that the queue can hold when completely full. This number will be 1 less than the size of the array that holds the elements.
queue_size_t queue_size(queue_t* q){
    return q->size - 1;                    //returns actual size of queue which is -1 because of the one element needed for indexIn
}

// Returns true if the queue is full.
bool queue_full(queue_t* q){
    if (q->indexOut == 0 && q->indexIn == queue_size(q) ||      //when full without indexIn wrapping around
        q->indexOut == q->indexIn + 1){                         //when "full" meaning indexOut is one above indexIn
        return true;
    } else{
        return false;
    }
}

// Returns true if the queue is empty.
bool queue_empty(queue_t* q){
    if (q->indexIn == q->indexOut){                 //if both indexes are equal then empty
        return true;
    } else{
        return false;
    }
}

// If the queue is not full, pushes a new element into the queue and clears the underflowFlag.
// IF the queue is full, set the overflowFlag, print an error message and DO NOT change the queue.
void queue_push(queue_t* q, queue_data_t value){
    if (queue_full(q)){
        printf("OVERFLOW: CAN'T PUSH INTO FULL QUEUE\n");            //first check to see if queue full. If is then can't push and print error message
        q->overflowFlag = true;                                      //sets an overflowFlag for error checking
    } else{
        //printf("VALUE: %lf \n\r", value);
        //printf("TEST: %lf \n\r", q->data[q->indexIn]);
        q->data[q->indexIn] = value;                                 //pushes value into the next indexIn
        q->underflowFlag = false;                                    //Always resets the underflowFlag if pushes without error
        //printf("TEST: %lf \n\r", q->data[q->indexIn]);
        //printf("INDEX IN: %ld DATA: %lf VALUE %lf\n\r", q->indexIn, q->data[q->indexIn], value);
        if (q->indexIn < q->size - 1){                               //If indexIn isn't at the top point to the next highest
            q->indexIn++;
        } else{
            q->indexIn = 0;                                          //else wrap around to 0
        }
    }
}

// If the queue is not empty, remove and return the oldest element in the queue.
// If the queue is empty, set the underflowFlag, print an error message, and DO NOT change the queue.
queue_data_t queue_pop(queue_t* q){
    if (queue_empty(q)){
        printf("UNDERFLOW: CAN'T POP FROM EMPTY QUEUE\n");            //first check to see if queue empty. If is then can't pop and print error message
        q->underflowFlag = true;                                      //sets an underflowFlag for error checking
    } else{
        q->overflowFlag = false;                                      //Always resets the overflowFlag if pushes without error
        queue_index_t index = q->indexOut;                            //create a variable to hold current index so we can return the correct data
        if (q->indexOut < q->size - 1){                               //if indexOut less than size of array minus one (because index start at 0) then indexOut points to the next element
            q->indexOut++;
        } else{
            q->indexOut = 0;                                          //else wraps around to 0
        }
        return (q->data[index]);                                      //returns value popped off
    }
}

// If the queue is full, call queue_pop() and then call queue_push().
// If the queue is not full, just call queue_push().
void queue_overwritePush(queue_t* q, queue_data_t value){
    if (queue_full(q)){                                               //if queue is full it needs to first pop then push
        queue_pop(q);                                                 //pop
    }
    queue_push(q, value);                                             //push
}

bool queue_indexOutOfBounds(queue_t* q, queue_index_t index){         //helper function used in queue_readElementAt() function
    if (queue_empty(q)){                                              //if empty queue can't index stuff
        return true;
    }else if (q->indexIn > q->indexOut){                              //if indexIn above indexOut
        if (index > q->indexIn - q->indexOut){                        //if index greater than difference of In and Out then out of bounds
            return true;
        } else{
            return false;
        }
    } else{
        if (index > q->size - q->indexOut + q->indexIn){              //if index greater than size - Out + In then out of bounds
            return true;
        } else{
            return false;
        }
    }
}

// Provides random-access read capability to the queue.
// Low-valued indexes access older queue elements while higher-value indexes access newer elements
// (according to the order that they were added).
// Print a meaningful error message if an error condition is detected.
queue_data_t queue_readElementAt(queue_t* q, queue_index_t index){
    //TODO: CATCH ERROR
    if(queue_indexOutOfBounds(q, index)){                                      //first check to see if index is out of bounds if so print error and return 0.0
        printf("ERROR: INDEX OUT OF BOUNDS\n");
        return QUEUE_RETURN_ERROR_VALUE;
    }
    if (q->indexIn > q->indexOut){                                             //normal case when indexIn above indexOut
        //printf("READ: %lf AT %ld\n", q->data[index + q->indexOut], index);
        //printf("REAL INDEX: %ld\n", index + q->indexOut);
        return q->data[index + q->indexOut];                                   //return data at index plus indexOut in case it's been popped a couple times
    } else{
        if (index < q->size - q->indexOut){                                    //if index less than size - indexOut
            //printf("INDEX LESS THAN SIZE\n");
            return q->data[index + q->indexOut];                               //return data at index + indexOut
        } else{
            //printf("ANOTHER INDEX TEST");
            return q->data[index - (q->size - q->indexOut)];                       //return data at index - queue size - indexOut
            //return q->data[q->indexIn - (index - (queue_size(q) - q->indexOut))];
        }

    }
}

// Returns a count of the elements currently contained in the queue.
queue_size_t queue_elementCount(queue_t* q){
    if(queue_empty(q)){                                                            //if empty queue return 0
        return 0;
    } else if (q->indexIn > q->indexOut){                                          //if indexIn greater than indexOut
        //printf("ELEMENT COUNT IN > OUT:%ld\n", q->indexIn - q->indexOut);
        return q->indexIn - q->indexOut;                                           //size equal to in - out
    } else{
        //printf("ELEMENT COUNT IN <= OUT:%ld\n", q->indexIn + (queue_size(q) - q->indexOut));
        return q->indexIn + (q->size - q->indexOut);                               //size equal to in + size - out
    }
}

// Returns true if an underflow has occurred (queue_pop() called on an empty queue).
bool queue_underflow(queue_t* q){
    return q->underflowFlag;                                                       //returns underflowFlag for error checking
}

// Returns true if an overflow has occurred (queue_push() called on a full queue).
bool queue_overflow(queue_t* q){
    return q->overflowFlag;                                                        //returns overflowFlag for error checking
}

// Frees the storage that you malloc'd before.
void queue_garbageCollect(queue_t* q){
    free(q->data);                                                                 //frees storage that we allocated
}

// Prints the current contents of the queue. Handy for debugging.
// This must print out the contents of the queue in the order of oldest element first to newest element last.
// HINT: Just use queue_readElementAt() in a for-loop. Trivial to implement this way.
void queue_print(queue_t* q){
    uint32_t i;
    for(i = 0; i < queue_elementCount(q); ++i){                                            //steps through the queue and prints everything off
        printf("ELEMENT AT INDEX %ld: %lf\n\r", i, queue_readElementAt(q, i));
    }
}

/********************************************************
************* Test Code starts here. ********************
****** invoke queue_runTest() to run test code. *********
********************************************************/

// Used to check the status of the queue flags.
// Returns true if the all of the queue status matches values of the arguments.
// Returns false otherwise.
// Prints informational messages if the queue status does not match the provided argument values.
static bool queue_printQueueStatus(queue_t* q, bool overflowArg, bool underflowArg, bool fullArg, bool emptyArg) {
  bool result = true;
  bool flag;
  if ((flag=queue_overflow(q)) != overflowArg) {  // Check the queue status against the flag.
    result = flag ? result: false;                // Note failure.
    if (flag)                                     // Print helpful informational messages.
      printf("* queue_overFlow(%s) returned true. Should have returned false.\n\r", q->name);
    else
      printf("* queue_overFlow(%s) returned false. Should have returned true.\n\r", q->name);
  }
  if ((flag=queue_underflow(q)) != underflowArg) {  // Check the queue status against the flag.
    result = flag ? result: false;                  // Note failure.
    if (flag)                                       // Print helpful informational messages.
      printf("* queue_underFlow(%s) returned true. Should have returned false.\n\r", q->name);
    else
      printf("* queue_underFlow(%s) returned false. Should have returned true.\n\r", q->name);
  }
  if ((flag=queue_full(q)) != fullArg) {  // Check the queue status against the flag.
    result = flag ? result: false;        // Note failure.
    if (flag) {                           // Print helpful informational messages.
      printf("* queue_full(%s) returned true. Should have returned false.\n\r", q->name);
      printf("* queue: %s contains %ld elements.\n\r", queue_name(q), queue_elementCount(q));
    }
    else {
      printf("* queue_full(%s) returned false. Should have returned true.\n\r", q->name);
      printf("* queue: %s contains %ld elements.\n\r", queue_name(q), queue_elementCount(q));
    }
  }
  if ((flag=queue_empty(q)) != emptyArg) {  // Check the queue status against the flag.
    result = flag ? result: false;          // Note failure.
    if (flag) {                              // Print helpful informational messages.
      printf("* queue_empty(%s) returned true. Should have returned false.\n\r", q->name);
    }
    else {
      printf("* queue_empty(%s) returned false. Should have returned true.\n\r", q->name);
      printf("* queue: %s contains %ld elements.\n\r", queue_name(q), queue_elementCount(q));
    }
  }
  return result;
}

// Assumes testQ is filled with the contents from dataArray.
// Repeatedly calls queue_pop until all of the queue contents have been removed.
// Uses queue_pop() and queue_readElementAt().
// Reads the entire contents of the queue after each pop.
static bool queue_emptyTest(queue_t* testQ, double* dataArray, queue_size_t arraySize) {
  bool tempResult = true;
  bool testResult = true;
  for (int32_t testDataIndex=arraySize-1; testDataIndex>=0; testDataIndex--) {
    // First, pop an element of the queue and check to see if the pop'd value is correct.
    double poppedValue = queue_pop(testQ);
    tempResult = true;
    if (poppedValue != dataArray[arraySize-(testDataIndex+1)]) {
      printf("* Error: queue_pop() returned %lf, should have returned %lf.\n\r", poppedValue, dataArray[arraySize-(testDataIndex+1)]);
      printf("* queue_pop invoked %ld times, testDataIndex: %ld\n\r", arraySize-(testDataIndex+1), testDataIndex);
      tempResult = false;
      printf("* queue_pop() failed. Recommend that you fix this problem before proceeding further.\n\r");
      return false;
    }
    testResult = tempResult ? testResult : false; // Just a logical AND of testResult and tempResult.
    // Next, read all of the remaining elements in the queue to see if they are correct.
    // Because you are using queue_pop(), you must offset the indicies for the array,
    // moving forward after each pop. The indices used with queue_readElementAt() are not offset.
    tempResult = true;
    for (int32_t queueIndex=0; queueIndex<testDataIndex; queueIndex++) {
      double temp;
      if ((temp=queue_readElementAt(testQ, queueIndex)) != dataArray[arraySize-(testDataIndex)+queueIndex]) {
        printf("* Error: queue_readElementAt(%ld) read %lf, queue should contain %lf.\n\r", queueIndex, temp, dataArray[arraySize-(testDataIndex)+queueIndex]);
        printf("* Either queue_pop() or queue_readElementAt() contains a bug.\n\r");
        printf("* Repair this bug before proceeding further.\n\r");
        tempResult = false;
      }
    }
    testResult = tempResult ? testResult : false; // Just a logical AND of testResult and tempResult.
    // Pop a value and check to see if correct.
  }
  tempResult=queue_printQueueStatus(testQ, false, false, false, true);
  testResult = tempResult ? testResult : false; // Just a logical AND of testResult and tempResult.
  return testResult;
}

// Assumes an initialized queue. Fills the queue with the contents of dataArray using queue_push().
// Each time data is pushed, queue_readElementAt() is invoked to ensure the queue contains the
// correct data. This test will pass if queue_push() and queue_readElementAt() work correctly.
static bool queue_fillTest(queue_t* testQ, double* dataArray, queue_size_t arraySize) {
  bool testResult = true;  // Keep track of the results of the test.
  uint32_t testDataIndex = 0;  // Just an index.
  for (testDataIndex=0; testDataIndex<arraySize; testDataIndex++) { // Iterate across the dataArray.
    queue_push(testQ, dataArray[testDataIndex]);  // Push a value onto the queue.
    double temp;  // Keep track of the value ready by queue_readElementAt().
    // Read all elements currently in queue and check to see that they match against the data contained in dataArray.
    if ((temp=queue_readElementAt(testQ, testDataIndex) != dataArray[testDataIndex])) {
      printf("* Error: queue_readElementAt() failed. push value %lf does not match read value %lf.\n\r",
          temp, dataArray[testDataIndex]);
      printf("* Bug is likely in queue_push() or queue_readElementAt().\n\r");
      testResult = false;  // Failed the test.
      break;               // Stop iterating at this point.
    }
  }
  // Queue should be full at this point with no overflow or underflow, argument order:(overflow, underflow, full, empty).
  bool tempResult = true;  // Keep track of the queue_printQueueStatus() test.
  if (testResult) {
    tempResult=queue_printQueueStatus(testQ, false, false, true, false);  // See comment above for argument order.
  }
  testResult = tempResult ? testResult : false;  // Logical AND of testResult and tempResult.
  return testResult;
}

// Builds a test queue and applies a series of pushes and pops.
// The value returned by queue_pop() is checked for correctness.
// queue_elementCount() is also checked frequently to ensure that
// it returns the correct value.
// To perform the test, a non-circular queue is implemented as
// a simple array with a ncqPushIndexPtr and a ncqPopIndexPtr. The
// queue code under test should exhibit the same behavior as the non-circular queue.
// The non-circular queue is initialized to contain random values.
// Values from the non-circular queue are from the ncqPushIndexPtr
// and values to be popped from the queue under test are compared
// to the ncqPopIndexPtr location. The test terminates upon detecting an
// error or if no error has been detected and the entire contents of the
// non-circular queue have been pushed into the test Q and then pop'd
// from the test queue.
#define PUSH_POP_Q_SIZE 100        // The size of the queue.
#define NON_CIRC_Q_SIZE 1000       // Size of noncircularQ used to test the testQ.
#define MAX_PUSH_POP_COUNT 20      // Maximum number of pushes or pops in one pass.
#define PUSH_POP_Q_NAME "pushPopQ" // Name the queue.
static bool queue_pushPopTest() {
  bool testResult = true;
  bool tempResult = true;
  // ncq: non-circular queue
  double* ncq = (double *) malloc(NON_CIRC_Q_SIZE * sizeof(double));
  for (uint16_t i=0; i<NON_CIRC_Q_SIZE; i++) {  // Fill up the non-circular queue with data.
    ncq[i] = (double) rand();
  }
  // Emulate a simple non-circular queue for testing purposes.
  uint16_t ncqPopIndexPtr = 0;  // The pop-pointer for the non-circular queue.
  uint16_t ncqPushIndexPtr = 0; // The push-pointer for the non-circular queue.
  queue_t testQ;             // This is the test Q.
  queue_init(&testQ, PUSH_POP_Q_SIZE,  PUSH_POP_Q_NAME);  // Init the test Q.
  // Test queue_empty().
  tempResult = queue_empty(&testQ);
  if (!tempResult) {
    printf("* Error: queue_empty(%s) should return true but returned false.\n\r", queue_name(&testQ));
  }
  testResult = tempResult ? testResult : false;  // Logical AND of testResult and tempResult.
  // Run the loop below until you have used up all of the values in the nonCircularQ.
  // Each run of the loop will cause some number of pushes and pops to occur.
  // The loop runs until all values of the nonCircularQ have been pushed, and
  // all values in the push-pop Q are pop'd.
  do {
    // Make sure not to push too much into testQ, or to go beyond the bounds of the nonCircularQ.
    uint16_t spaceInTestQ = queue_size(&testQ) - queue_elementCount(&testQ);    // Size left in testQ.
    uint16_t ncqUnusedValues = NON_CIRC_Q_SIZE - 1 - ncqPushIndexPtr;  // Unused values in noncircularQ.
    uint16_t pushCount = rand() % MAX_PUSH_POP_COUNT;        // Compute a base number of pops.
    // Potentially reduce push-count to available space in testQ
    pushCount = pushCount <= spaceInTestQ ? pushCount : spaceInTestQ;
    // Potentially reduce pushCount to remaining values in noncircularQ.
    pushCount = pushCount <= ncqUnusedValues ? pushCount : ncqUnusedValues;
#ifdef QUEUE_PRINT_INFO_MESSAGES
    printf("push-count:%d\n\r", pushCount);
#endif
    for (uint16_t i=0; i<pushCount; i++) {
      queue_push(&testQ, ncq[ncqPushIndexPtr++]);
    }
    // Number of elements in the simulated non-circular queue that
    // are currently stored in the testQ.
    uint16_t simQElementCount = ncqPushIndexPtr - ncqPopIndexPtr;
    if (queue_elementCount(&testQ) != simQElementCount) {
      printf("* Error: queue_elementCount(%s) returned %ld should be %d\n\r.", queue_name(&testQ),
          queue_elementCount(&testQ), simQElementCount);
      tempResult = false;
      break;  // This needs to be fixed before proceeding further.
    }
    // Don't pop more than you have pushed.
    uint16_t popCount = rand() % MAX_PUSH_POP_COUNT;
    // set popCount to be no more that the current simQElementCount (don't overrun the non-circular queue).
    popCount = simQElementCount >= popCount ? popCount : simQElementCount;
#ifdef QUEUE_PRINT_INFO_MESSAGES
    printf("pop-count:%d\n\r", popCount);
#endif
    // Check each pop'd value for correctness.
    double temp;
    for (uint16_t i=0; i<popCount; i++) {  // Iterate over the pop'd values.
      if ((temp=queue_pop(&testQ)) != ncq[ncqPopIndexPtr]) {  // Should match the non-circular queue.
        printf("* Error: queue_pop(%s) returned %lf, should be %lf", queue_name(&testQ), temp, ncq[ncqPopIndexPtr]);
        tempResult = false;
      }
      ncqPopIndexPtr++;
    }
    // Keep going until all values contained in the non-circular queue are exhausted.
  } while((ncqPushIndexPtr != ncqPopIndexPtr) || (ncqPushIndexPtr != NON_CIRC_Q_SIZE-1));
  testResult = tempResult ? testResult : false;
  return testResult;
}

// Checks to see that underflow and overflow work, and that error messages are printed.
#define ERROR_CONDITION_Q_SIZE 10
#define ERROR_CONDITION_Q_NAME "errorQ"
bool queue_testErrorConditions() {
  bool tempResult = true;  // Local test results.
  bool testResult = true;  // Overall test results.
  // A queue for testing.
  queue_t testQ;
  queue_init(&testQ, ERROR_CONDITION_Q_SIZE, ERROR_CONDITION_Q_NAME);
  // See that the empty function works correctly.
  tempResult = queue_empty(&testQ);
  if (!tempResult) {
    printf("* Error: queue_empty(%s) returned false, should be true.\n\r", queue_name(&testQ));
  }
  testResult = tempResult ? testResult : false;
  // pop an element from the empty queue and check for underflow.
  printf("=== + User code should print a queue empty error message-> ");
  // Check for underflow by popping an empty queue.
  queue_pop(&testQ);
  tempResult = queue_underflow(&testQ);
  if (!tempResult) {
    printf("* Error: queue_underflow(%s) returned false, should be true.\n\r", queue_name(&testQ));
  }
  testResult = tempResult ? testResult : false;
  // Fill up the queue with one too many elements.
  printf("=== + User code should print a queue full error message-> ");
  for (uint16_t i=0; i<ERROR_CONDITION_Q_SIZE+1; i++) {
    queue_push(&testQ, 0.0);
  }
  // Check for overflow should be true.
  tempResult = queue_overflow(&testQ);
  if (!tempResult) {
    printf("* Error: queue_overflow(%s) returned false, should be true.\n\r", queue_name(&testQ));
  }
  testResult = tempResult ? testResult : false;
  // underflow flag should have been cleared with the first push, should return false.
  tempResult = queue_underflow(&testQ);
  if (tempResult) {
    printf("* Error: queue_underflow(%s) returned true, should be false.\n\r", queue_name(&testQ));
  }
  testResult = !tempResult ? testResult : false;
  // Check to see that the queue is full.
  tempResult = queue_full(&testQ);
  if (!tempResult) {
    printf("* Error: queue_full(%s) returned false, should be true.\n\r", queue_name(&testQ));
  }
  testResult = tempResult ? testResult : false;
  // Calling queue_pop() should clear the overflow flag.
  queue_pop(&testQ);
  tempResult = queue_overflow(&testQ);
  if (tempResult) {
    printf("** Error: queue_overflow(%s) returned true, should be false.\n\r", queue_name(&testQ));
  }
  testResult = !tempResult ? testResult : false;
  // Check to see that the queue is no longer full after one pop.
  tempResult = queue_full(&testQ);
  if (tempResult) {
    printf("* Error: queue_full(%s) returned true, should be false.\n\r", queue_name(&testQ));
  }
  testResult = !tempResult ? testResult : false;
  // underflow flag should also be false at this point.
  tempResult = queue_underflow(&testQ);
  if (tempResult) {
    printf("* Error: queue_underflow(%s) returned true, should be false.\n\r", queue_name(&testQ));
  }
  testResult = !tempResult ? testResult : false;
  queue_garbageCollect(&testQ);
  return testResult;
}

// Checks to see that queue_overwritePush() works correctly.
// Simple test: just overwritePushes one set of values, and then another.
// Checks to see that contents are correct afterwards.
#define OVERWRITE_PUSH_TEST_QUEUE_SIZE 100  // tested queue will be this big.
#define OVERWRITE_PUSH_TEST_QUEUE_NAME "overwriteQ"  // tested queue will be this big.
bool queue_overwritePushTest() {
  bool testResult = true;  // Keep track of overall test results.
  // Build a queue for testing.
  queue_t testQ;
  queue_init(&testQ, OVERWRITE_PUSH_TEST_QUEUE_SIZE, OVERWRITE_PUSH_TEST_QUEUE_NAME);
  // Allocate two arrays of test data.
  double* dataArray1 = (double *) malloc((OVERWRITE_PUSH_TEST_QUEUE_SIZE) * sizeof(double));
  for (uint16_t i=0; i<OVERWRITE_PUSH_TEST_QUEUE_SIZE; i++)
    dataArray1[i] = (double) rand();
  double* dataArray2 = (double *) malloc((OVERWRITE_PUSH_TEST_QUEUE_SIZE) * sizeof(double));
  for (uint16_t i=0; i<OVERWRITE_PUSH_TEST_QUEUE_SIZE; i++)
    dataArray2[i] = (double) rand();
  // Fill the queue with all data values.
  for (uint16_t i=0; i<OVERWRITE_PUSH_TEST_QUEUE_SIZE; i++) {
    queue_overwritePush(&testQ, dataArray1[i]);
  }
  for (uint16_t i=0; i<OVERWRITE_PUSH_TEST_QUEUE_SIZE; i++) {
    queue_overwritePush(&testQ, dataArray2[i]);
  }
  // All that should remain are values from dataArray2.
  for (uint16_t i=0; i<OVERWRITE_PUSH_TEST_QUEUE_SIZE; i++) {
    if (queue_readElementAt(&testQ, i) != dataArray2[i]) {  // Are the correct values returned from the queue?
      printf("* Error: the values read from the queue: %s are incorrect after queue_overwritePush().\n\r", queue_name(&testQ));
      testResult = false;
      break;
    }
  }
  // Garbage collect all of the allocated memory.
  queue_garbageCollect(&testQ);
  free(dataArray1);
  free(dataArray2);
  return testResult;
}

// Returns true if test passed, false otherwise.
// This test will build a queue of random size between 10,000 and 20,000 elements, and:
// 1. Create a same-sized array to contain random values to store in the queue.
// 2. Fill the queue with the random values and from the array and then check for full.
// 3. Test to see that queue_readElementAt() works correctly.
// 3. Pop each of the values (checking to see that the correct values are pop'd and then check for empty.
// 4. Test the queue by interspersing pushes and pops in the same queue.
// 5. Refill the array with the previous random values.
// 6. Use queue_overwritePush() to write over all of the elements of the array, checking the contents.
#define QUEUE_TEST_MAX_QUEUE_SIZE 100  // Used for the fill/empty tests.
#define QUEUE_TEST_MAX_LOOP_COUNT 10   // All tests will be invoked this many times.
#define QUEUE_TEST_QUEUE_NAME "test_queue"
bool queue_runTest() {
  bool testResult = true;  // Be optimistic.
  // Overall test will be executed QUEUE_TEST_MAX_LOOP_COUNT times.
  for (uint32_t loopCount = 0; loopCount<QUEUE_TEST_MAX_LOOP_COUNT; loopCount++) {
    printf("=== Queue Test Iteration %ld ===\n\r", loopCount);
    // Compute the size of the data set randomly within given bounds.
    uint32_t arraySize = (rand() % (QUEUE_TEST_MAX_QUEUE_SIZE/2)) + (QUEUE_TEST_MAX_QUEUE_SIZE/2);
    printf("=== Commencing basic fill test (calling queue_push() until full) of queue of size: %ld. === \n\r", arraySize);
    // Allocate the array.
    double* dataArray = (queue_data_t *) malloc(sizeof(queue_data_t) * arraySize);
    for (uint32_t i=0; i<arraySize; i++) {
      dataArray[i] = (double) rand();
    }
    queue_t testQ;  // queue instance used for testing.
    queue_init(&testQ, arraySize, QUEUE_TEST_QUEUE_NAME);  // Init the queue.
    testResult = queue_fillTest(&testQ, dataArray, arraySize) ? testResult : false;
    if (testResult) {
      printf("=== Queue: %s passed a basic fill test.\n\r", queue_name(&testQ));
    } else {
      printf("=== Queue: %s failed a basic fill test.\n\r", queue_name(&testQ));
    }
    if (queue_elementCount(&testQ) != arraySize) {
      printf("* Error: queue_elementCount(%s) is %ld, should be %ld\n\r", queue_name(&testQ), queue_elementCount(&testQ), arraySize);
      printf("* Error: queue_elementCount() likely has a bug.\n\r");
      testResult = false;
    }
    // Run the queue-empty test.
    printf("=== Commencing basic empty test (calling queue_pop() until empty) === \n\r");
    bool tempResult = queue_emptyTest(&testQ, dataArray, arraySize);
    if (tempResult) {
      printf("=== Queue: %s passed a basic empty test.\n\r", queue_name(&testQ));
    } else {
      printf("=== Queue: %s failed a basic empty test.\n\r", queue_name(&testQ));
    }
    testResult = tempResult ? testResult : false;  // Logical AND of testResult and tempResult.
    tempResult = true;
    if (queue_elementCount(&testQ) != 0) {
      printf("* Error: queue_elementCount(%s) is %ld, should be %d\n\r", queue_name(&testQ), queue_elementCount(&testQ), 0);
      printf("* Error: queue_elementCount() likely has a bug.\n\r");
      tempResult = false;
    }
    // Run the push/pop test.
    printf("=== Commencing push/pop test.) === \n\r");
    tempResult = queue_pushPopTest();
    if (tempResult) {
      printf("=== Queue: %s passed a push/pop test.\n\r", queue_name(&testQ));
    } else {
      printf("=== Queue: %s failed a push/pop test.\n\r", queue_name(&testQ));
    }
    testResult = tempResult ? testResult : false;  // Logical AND of testResult and tempResult.
    printf("=== Commencing error-condition test (calling queue_pop() until empty) === \n\r");
    tempResult = queue_testErrorConditions();
    if (tempResult) {
      printf("=== Queue: %s passed error-condition test.\n\r", queue_name(&testQ));
    } else {
      printf("=== Queue: %s failed error-condition test.\n\r", queue_name(&testQ));
    }
    testResult = tempResult ? testResult : false;  // Logical AND of testResult and tempResult.
    printf("=== Commencing overwritePush test (calling queue_pop() until empty) === \n\r");
    tempResult = queue_overwritePushTest();
    if (tempResult) {
      printf("=== Queue: %s passed overwritePush test.\n\r", queue_name(&testQ));
    } else {
      printf("=== Queue: %s failed overwritePush test.\n\r", queue_name(&testQ));
    }
    testResult = tempResult ? testResult : false;  // Logical AND of testResult and tempResult.
    if (testResult) {
      printf("=== All queue tests passed. ===\n\r\n\r");
    } else {
      printf("=== Some queue tests failed. Look at informational messages.\n\r\n\r");
    }
    // All done. Free up all allocated memory.
    queue_garbageCollect(&testQ);
    free(dataArray);
  }
  return testResult;
}
