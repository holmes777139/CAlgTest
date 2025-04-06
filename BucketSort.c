#include <stdio.h>

void BucketSort(int* p, int size)
{
    int bucket[10] = {0};
    for (int i = 1; i < size; i++)
    {
        bucket[p[i]]++;
    }

    for(int i = 0; i < 10; i++)
    {
        if(bucket[i] != 0){
            for(int j = 0; j < bucket[i]; j++)
            {
                printf("%d ", i);
            }           
        }
    }
    return;
}
int main(void)
{
    int* p;
    int array[] = {5,3,8,9,2,3,4,5,9,4,1};
    p = array;
    BucketSort(p, sizeof(array)/sizeof(array[0]));
    return 0;
}