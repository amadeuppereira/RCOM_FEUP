#!/bin/bash

gcc -o sender logic.c sender.c -Wall
gcc -o receiver logic.c receiver.c -Wall
