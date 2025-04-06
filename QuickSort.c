#include <stdio.h>

void BubbleSortLittle(int* p, int size)
{
    int temp;

    if(size == 1){
        return;
    }
    if(*p > *(p+1)){
        temp = *p;
        *p = *(p+1);
        *(p+1) = temp;
    }
    BubbleSortLittle(++p, --size);
}

void BubbleSortBig(int* p, int size)
{
    int temp;

    if(size == 1){
        return;
    }
    if(*p < *(p+1)){
        temp = *p;
        *p = *(p+1);
        *(p+1) = temp;
    }
    BubbleSortBig(++p, --size);
}

void Sort(int* p, int size )
{
    if(size == 1){
        printf("%d ", *p);
        return;
    }
    BubbleSortBig(p, size);
    Sort(p, --size);
    printf("%d ", *(p + size));
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