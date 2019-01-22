#!/usr/bin/gnuplot

!echo donothing64
!taskset -c 1 ./SMHasher donothing64 -s -n > donothing64.txt
!tail -1 donothing64.txt | awk '{ printf "%s %s donothing64\n", $1, $2 }' > donothing64label.txt
!echo City64
!taskset -c 1 ./SMHasher City64 -s -n > City64.txt
!tail -1 City64.txt | awk '{ printf "%s %s City64\n", $1, $2 }' > City64label.txt
!echo Spooky64
!taskset -c 1 ./SMHasher Spooky64 -s -n > Spooky64.txt
!tail -1 Spooky64.txt | awk '{ printf "%s %s Spooky64\n", $1, $2 }' > Spooky64label.txt
!echo AES64
!taskset -c 1 ./SMHasher AES64 -s -n > AES64.txt
!tail -1 AES64.txt | awk '{ printf "%s %s AES64\n", $1, $2 }' > AES64label.txt
!echo Risky64
!taskset -c 1 ./SMHasher Risky64 -s -n > Risky64.txt
!tail -1 Risky64.txt | awk '{ printf "%s %s Risky64\n", $1, $2 }' > Risky64label.txt
!echo Murmur2B
!taskset -c 1 ./SMHasher Murmur2B -s -n > Murmur2B.txt
!tail -1 Murmur2B.txt | awk '{ printf "%s %s Murmur2B\n", $1, $2 }' > Murmur2Blabel.txt
!echo Murmur2C
!taskset -c 1 ./SMHasher Murmur2C -s -n > Murmur2C.txt
!tail -1 Murmur2C.txt | awk '{ printf "%s %s Murmur2C\n", $1, $2 }' > Murmur2Clabel.txt

set title "SMHasher 64-bit hashes"
set ylabel "CPU cycles / hash"
set xlabel "Key length bytes"
unset key
set grid

plot "donothing64.txt" with linespoints, \
     "donothing64label.txt" with labels offset +8,0, \
     "City64.txt" with linespoints, \
     "City64label.txt" with labels offset +5,0, \
     "Spooky64.txt" with linespoints, \
     "Spooky64label.txt" with labels offset +6,0, \
     "AES64.txt" with linespoints, \
     "AES64label.txt" with labels offset +4,0, \
     "Risky64.txt" with linespoints, \
     "Risky64label.txt" with labels offset +5,0, \
     "Murmur2B.txt" with linespoints, \
     "Murmur2Blabel.txt" with labels offset +6,+1, \
     "Murmur2C.txt" with linespoints, \
     "Murmur2Clabel.txt" with labels offset +6,0
