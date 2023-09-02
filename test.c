#define abs(x) (((x) - 4000) > 0 ? ((x) - (4000)) : ((4000)-(x)))
#include <stdio.h>
int main(){
    int x =4001;
    printf("%d",abs(x));
}