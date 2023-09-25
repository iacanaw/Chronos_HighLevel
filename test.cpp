#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int main(){
    for(float delta = -15.0; delta < 15.0; delta += 1.0){
        float reward = 50.0+(delta*delta);
        //reward = Q_rsqrt(reward);
        reward = 1/sqrt(reward);
        reward = ((-200.0 * reward * delta)+100.0);
        printf("%f;%f\n", delta, reward);
    }

    return 0;
}
