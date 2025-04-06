#include <stdio.h>

void QuickSort(int* p, int size)
{

}
void Sort(int* p, int size )
{
    QuickSort(p, size);
}
int main(void)
{
    int* p;
    int array[] = {5,3,8,223,75,35,32,9,4,1};
    p = array;
    int index = 9;
    Sort(p, 10);
    return 0;
}