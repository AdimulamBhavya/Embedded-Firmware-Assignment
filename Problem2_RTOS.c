#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdio.h>

// creating global variables
volatile uint8_t G_DataID;
volatile int32_t G_DataValue;

// create task handles
TaskHandle_t TaskHandle_1;
TaskHandle_t TaskHandle_2;

// creating queue handle
QueueHandle_t Queue1;

// given data structure definition
typedef struct {
    uint8_t dataID;
    int32_t DataValue;
} Data_t;

// given task prototypes
void ExampleTask1(void *pV);
void ExampleTask2(void *pV);

int main(void) {
    // create the queue
    Queue1 = xQueueCreate(5, sizeof(Data_t));
    if (Queue1 == NULL) {
        printf("Queue creation failed\n");
        while (1);
    }

    // creating the tasks
    xTaskCreate(ExampleTask1, "Task1", 1000, NULL, 1, &TaskHandle_1);
    xTaskCreate(ExampleTask2, "Task2", 1000, NULL, 1, &TaskHandle_2);

    // to start the scheduler
    vTaskStartScheduler();

    // Should never reach here
    for (;;) {}
}

//given task prototype
void ExampleTask1(void *pV) {
    Data_t data;

    for (;;) {
        data.dataID = G_DataID;
        data.DataValue = G_DataValue;

        // sending data to the queue
        if (xQueueSend(Queue1, &data, portMAX_DELAY) != pdPASS) {
            printf("Failed to send to queue\n");
        }

        // with given delay for 500 ms
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void ExampleTask2(void *pV) {
    Data_t receivedData;
    UBaseType_t priority = uxTaskPriorityGet(NULL);

    for (;;) {
        // receive data from the queue
        if (xQueueReceive(Queue1, &receivedData, portMAX_DELAY) == pdPASS) {
            // print the received data
            printf("Received dataID: %d, DataValue: %d\n", receivedData.dataID, receivedData.DataValue);

            // apply logic based on received data
            if (receivedData.dataID == 0 || receivedData.dataID == 2) {
                // delete exampletask2 task
                vTaskDelete(NULL);
            } else if (receivedData.dataID == 1) {
                // process DataValue member
                if (receivedData.DataValue == 0) {
                    // increase priority by 2
                    vTaskPrioritySet(NULL, priority + 2);
                } else if (receivedData.DataValue == 1) {
                    // decrease priority if previously increased
                    vTaskPrioritySet(NULL, priority);
                } else if (receivedData.DataValue == 2) {
                    // delete exampletask2 task
                    vTaskDelete(NULL);
                }
            }
        }
    }
}
