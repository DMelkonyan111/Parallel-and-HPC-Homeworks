#include <stdio.h>
#include <stdlib.h>

void assignment1();
void assignment2();
void assignment3();
void swap();
void assignment4();
void assignment5();
void assignment6();
int str_length();
void assignment7();


int main()
{
    assignment1();
    assignment2();
    assignment3();
    assignment4();
    assignment5();
    assignment6();
    assignment7();
}

void assignment1()
{
    printf("---------- ASSIGNMENT 1 ---------- \n");

    int num = 1;
    int *pointer = &num;
    printf("The address of the integer using variable %p\n", &num);
    printf("The address of the intgere using pointer %p\n", pointer);
    *pointer = 2;
    printf("The value of the freshly changed variable %d\n", *pointer);
}

void assignment2()
{
    printf("---------- ASSIGNMENT 2 ---------- \n");

    int numbers[5] = {1, 2, 3, 4, 5};
    int *pointer = numbers;

    // Traverse the array using pointer
    for (int i = 0; i < 5; i++)
    {
        printf("Element #%d is %d \n", (i + 1), *(pointer + i));
    }

    // Change the value of each element
    for (int i = 0; i < 5; i++)
    {
        *(pointer + i) += 10;
    }

    // Traverse the array using pointer
    for (int i = 0; i < 5; i++)
    {
        printf("Element #%d is %d \n", (i + 1), *(pointer + i));
    }

    // Traverse the array using array name
    for (int i = 0; i < 5; i++)
    {
        printf("Element #%d is %d \n", (i + 1), numbers[i]);
    }
}

void assignment3()
{
    printf("---------- ASSIGNMENT 3 ---------- \n");

    int num1 = 5;
    int num2 = 10;

    printf("num1 is %d and num2 is %d \n", num1, num2);
    swap(&num1, &num2);
    printf("num1 is %d and num2 is %d \n", num1, num2);
}

void swap(int *a, int *b)
{
    int c = *a;
    *a = *b;
    *b = c;
}

void assignment4()
{
    printf("---------- ASSIGNMENT 4 ---------- \n");

    int num = 5; 
    int *ptr = &num;
    int **ptrToPtr = &ptr;

    printf("The value of the integer using the pointer %d \n", *ptr);
    printf("The value of the integer using the pointer of the pointer %d \n", **ptrToPtr);
}

void assignment5()
{
    printf("---------- ASSIGNMENT 5 ---------- \n");

    int *ptr1 = (int*) malloc(sizeof(int));
    *ptr1 = 5;
    printf("The newly allocated memory holds the integer %d \n", *ptr1);

    int *ptr2 = (int*) malloc(5 * sizeof(int));

    for (int i = 0; i < 5; i++) {
        *(ptr2 + i) = i + 1;
        printf("Integer at index %d is %d \n", i, *(ptr2 + i));
    }

    free(ptr1);
    free(ptr2);
}

void assignment6()
{
    printf("---------- ASSIGNMENT 6 ---------- \n");

    char *ptr = "Word";

    for (int i = 0; i < 4; i++) {
        printf("The letter at index %d is %c \n", i, ptr[i]);
    }

    int size = str_length(ptr);

    printf("The size of the string is %d\n", size);
}

int str_length(char *str) {
    int size = 0;

    while (*str != '\0') {
        size++;
        *str++;
    }

    return size;
}

void assignment7()
{
    printf("---------- ASSIGNMENT 7 ---------- \n");

    char *ptr[] = {"Word1", "Word2", "Word3"};

    for (int i = 0; i < 3; i++) {
        printf("The string at index %d is %s \n", i, *(ptr + i));
    }

    *(ptr + 2) = "The last word";
    printf("After the change \n");

    for (int i = 0; i < 3; i++) {
        printf("The string at index %d is %s \n", i, *(ptr + i));
    }
}