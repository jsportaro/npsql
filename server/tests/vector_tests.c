#include <vector.h>

#include <stdio.h>
#include <string.h>

typedef struct VectorTest
{
    int a;
    int b;
    int c;
} VectorTest;

VectorOf(VectorTest)

int main(void)
{
    VectorTestVector* vec = VectorTestVectorCreate();
  
    for (int i = 0; i < 10; i++)
    {
        VectorTest t = { .a = 1 + i, .b = 2 * i, .c = 3 * i};
        VectorTestVectorPush(vec, t);
    }
    
    {
        VectorTest* v = NULL;
        int i = 0;
        for (i = 0, v = VectorTestVectorGet(vec, i); i < 10; i++)
        {   
            printf("%d %d %d  \n",  v[i].a, v[i].b, v[i].c);
        }
    }
    return 0;
}