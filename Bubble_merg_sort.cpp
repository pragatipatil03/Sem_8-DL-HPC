#include <iostream>
#include <ctime>
#include <cstdlib>
#include <omp.h>
#include <chrono>

using namespace std;
using namespace std::chrono;

void bubbleSort(int arr[], int n)
{
    for (int i = 0; i < n - 1; ++i)
    {
        for (int j = 0; j < n - i - 1; ++j)
        {
            if (arr[j] > arr[j + 1])
            {
                swap(arr[j], arr[j + 1]);
            }
        }
    }
}

void merge(int arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    int *L = new int[n1];
    int *R = new int[n2];

    for (i = 0; i < n1; ++i)
    {
        L[i] = arr[l + i];
    }
    for (j = 0; j < n2; ++j)
    {
        R[j] = arr[m + 1 + j];
    }

    i = 0;
    j = 0;
    k = l;

    while (i < n1 && j < n2)
    {
        if (L[i] <= R[j])
        {
            arr[k] = L[i];
            ++i;
        }
        else
        {
            arr[k] = R[j];
            ++j;
        }
        ++k;
    }

    while (i < n1)
    {
        arr[k] = L[i];
        ++i;
        ++k;
    }

    while (j < n2)
    {
        arr[k] = R[j];
        ++j;
        ++k;
    }

    delete[] L;
    delete[] R;
}

void mergeSort(int arr[], int l, int r)
{
    if (l < r)
    {
        int m = l + (r - l) / 2;
        #pragma omp parallel sections
        {
            #pragma omp section
            {
                mergeSort(arr, l, m);
            }
            #pragma omp section
            {
                mergeSort(arr, m + 1, r);
            }
        }

        merge(arr, l, m, r);
    }
}

void printArray(int arr[], int size)
{
    for (int i = 0; i < size; ++i)
    {
        cout << arr[i] << " ";
    }
    cout << endl;
}

int main()
{
    int n;
    cout << "Enter the size of the array: ";
    cin >> n;

    int *arr = new int[n];
    for (int i = 0; i < n; ++i)
    {
        arr[i]=rand() % 1000;
    }

    cout << "Original array: ";
    printArray(arr, n);

    // Time measurement variables
    auto start = high_resolution_clock::now();
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);    

    // Sequential Bubble Sort
    start = high_resolution_clock::now();
    bubbleSort(arr, n);
    end = high_resolution_clock::now();
    duration = duration_cast<microseconds>(end - start);
    cout << "\nSequential Bubble Sorted array: ";
    printArray(arr, n);
    cout << "Time taken: " << duration.count() << " microseconds\n";

    // Parallel Bubble Sort
    start = high_resolution_clock::now();
        #pragma omp parallel
    {
        #pragma omp single
        {
            bubbleSort(arr, n);
        }
    }
    end = high_resolution_clock::now();
    duration = duration_cast<microseconds>(end - start);
    cout << "Parallel Bubble Sorted array: ";
    printArray(arr, n);
    cout << "Time taken: " << duration.count() << " microseconds\n";

    // Sequential Merge Sort
    start = high_resolution_clock::now();
    mergeSort(arr, 0, n - 1);
    end = high_resolution_clock::now();
    duration = duration_cast<microseconds>(end - start);
    cout << "Sequential Merge Sorted array: ";
    printArray(arr, n);
    cout << "Time taken: " << duration.count() << " microseconds\n";
    
    // Parallel Merge Sort
    start = high_resolution_clock::now();
    #pragma omp parallel
    {
        #pragma omp single
        {
            mergeSort(arr, 0, n - 1);
        }
    }
    end = high_resolution_clock::now();
    duration = duration_cast<microseconds>(end - start);
    cout << "Parallel Merge Sorted array: ";
    printArray(arr, n);
    cout << "Time taken: " << duration.count() << " microseconds\n";

    delete[] arr;

    return 0;
}
