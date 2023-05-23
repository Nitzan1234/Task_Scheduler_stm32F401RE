/* This is the first attempt to write a task scheduler for stm32F401RE */
#include <stdint.h>
#include "stm32f401xc.h"
#define MAX 6

int numOfTasks = 0;
typedef struct
{
    int PID;
    char state; //process state: 'w' for waiting, 'b' for busy 't' for terminated
    uint32_t PC; // the Address of the next instruction to be executed
    uint32_t Mem_Start; //Address of start of address space of the process
    uint32_t Mem_End;
    int PPID; //Parent of the process ID
}Task;

Task TaskArray[MAX];

void Task_Array_init()
{
    for(int i=0;i<MAX;i++)
    {
        if(i==0)
        {
            TaskArray[i].state = 'b';
        }
        else
        {
            TaskArray[i].state = 'w';
        }
    }
    numOfTasks = 6;
}

// Timer1 initialization function
void Timer1_Init(void)
{
    // Enable the TIM1 peripheral clock
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

    // Configure Timer1
    // Set the prescaler value to achieve desired timer frequency
    TIM1->PSC = SystemCoreClock / 1000 - 1;  // Timer frequency = 1kHz

    // Set the auto-reload value to achieve desired timer period
    TIM1->ARR = 1000 - 1;  // Timer period = 1s

    // Enable Timer1 update interrupt
    TIM1->DIER |= TIM_DIER_UIE;

    // Enable Timer1
    TIM1->CR1 |= TIM_CR1_CEN;

    // Enable the TIM1 global interrupt
    NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
}

// Timer1 interrupt handler
void TIM1_UP_TIM10_IRQHandler(void)
{
    // Clear the timer interrupt flag
    TIM1->SR &= ~TIM_SR_UIF;
    for(int i=0;i<MAX;++i)
    {
        if (TaskArray[i].state == 'b')
        {
            TaskArray[i].state = 'w'; //changing task state to waiting
            TaskArray[i+1].state = 'b'; // changing task state to busy
            break;
        }
        if(TaskArray[i].state == 't')
        {
            for (i+1;i<MAX-1;++i)
            {
                TaskArray[i] =  TaskArray[i+1]; //removing Task from the TaskArray
            }
        }
    }
}
void main(void)
{
    Task_Array_init();
    Timer1_Init();

}

