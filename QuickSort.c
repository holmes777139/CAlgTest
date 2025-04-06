#include <stdio.h>

void QuickSort(int *p, int size)
{
    int pivot, i, j, temp;
    pivot = *p;
    i = 0;
    j = size - 1;

    if (size <= 1)
    {
        return;
    }

    while (i != j)
    {
        while (*(p + j) >= pivot && i < j)
            j--;
        while (*(p + i) <= pivot && i < j)
            i++;

        if (i < j)
        {
            temp = *(p + i);
            *(p + i) = *(p + j);
            *(p + j) = temp;
        }
    }
    *p = *(p + i);
    *(p + i) = pivot;
    QuickSort(p, i);
    QuickSort(p + i + 1, size - i - 1);
}
void Sort(int *p, int size)
{
    QuickSort(p, size);
}
int main(void)
{
    int *p;
    int array[] = {5, 3, 8, 223, 75, 35, 32, 9, 4, 1};
    p = array;
    Sort(p, 10);
    for (int i = 0; i < 10; i++)
    {
        printf("%d ", *(p + i));
    }
    return 0;
}