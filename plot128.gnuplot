#!/usr/bin/gnuplot

!echo donothing128
!taskset -c 1 ./SMHasher donothing128 -s -n > donothing128.txt
!tail -1 donothing128.txt | awk '{ printf "%s %s donothing128\n", $1, $2 }' > donothing128label.txt
!echo City128
!taskset -c 1 ./SMHasher City128 -s -n > City128.txt
!tail -1 City128.txt | awk '{ printf "%s %s City128\n", $1, $2 }' > City128label.txt
!echo Spooky128
!taskset -c 1 ./SMHasher Spooky128 -s -n > Spooky128.txt
!tail -1 Spooky128.txt | awk '{ printf "%s %s Spooky128\n", $1, $2 }' > Spooky128label.txt
!echo AES128
!taskset -c 1 ./SMHasher AES128 -s -n > AES128.txt
!tail -1 AES128.txt | awk '{ printf "%s %s AES128\n", $1, $2 }' > AES128label.txt
!echo Risky128
!taskset -c 1 ./SMHasher Risky128 -s -n > Risky128.txt
!tail -1 Risky128.txt | awk '{ printf "%s %s Risky128\n", $1, $2 }' > Risky128label.txt
!echo Murmur3C
!taskset -c 1 ./SMHasher Murmur3C -s -n > Murmur3C.txt
!tail -1 Murmur3C.txt | awk '{ printf "%s %s Murmur3C\n", $1, $2 }' > Murmur3Clabel.txt
!echo Murmur3F
!taskset -c 1 ./SMHasher Murmur3F -s -n > Murmur3F.txt
!tail -1 Murmur3F.txt | awk '{ printf "%s %s Murmur3F\n", $1, $2 }' > Murmur3Flabel.txt

set title "SMHasher 128-bit hashes"
set ylabel "CPU cycles / hash"
set xlabel "Key length bytes"
unset key
set grid

plot "donothing128.txt" with linespoints, \
     "donothing128label.txt" with labels offset +7,0, \
     "City128.txt" with linespoints, \
     "City128label.txt" with labels offset +5,0, \
     "Spooky128.txt" with linespoints, \
     "Spooky128label.txt" with labels offset +6,0, \
     "AES128.txt" with linespoints, \
     "AES128label.txt" with labels offset +5,0, \
     "Risky128.txt" with linespoints, \
     "Risky128label.txt" with labels offset +5,0, \
     "Murmur3C.txt" with linespoints, \
     "Murmur3Clabel.txt" with labels offset +6,0, \
     "Murmur3F.txt" with linespoints, \
     "Murmur3Flabel.txt" with labels offset +6,0
