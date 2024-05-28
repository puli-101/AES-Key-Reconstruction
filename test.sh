#!/bin/bash

make ;

./bin/main 545d5cf2662556cd2fb60f2273ff4c8d > output ;

cat output > comparison ;
cat control > comparison ;