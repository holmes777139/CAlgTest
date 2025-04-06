#include <stdio.h>
void BucketSort(int* bucket, int bucketSize, int* p, int size)
{
    for (int i = 0; i < size; i++)
    {
        bucket[*(p + i)]++;
    }

    for(int i = 0; i < bucketSize; i++)
    {
        if(*(bucket + i) != 0){
            //for(int j = 0; j < *(bucket + i); j++)
            {
                printf("%d ", i);
            }           
        }
    }
    return;
}
int main(void)
{
    int bucket[10] = {0};
    int array[] = {5,3,8,9,2,3,0,0,4,5,9,4,1,4,5,6};
    BucketSort(bucket, 10,array,sizeof(array)/sizeof(array[0]));
    return 0;
}