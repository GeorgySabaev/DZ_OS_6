target: gen read
gen: gen.c
	gcc gen.c -o gen
read: read.c
	gcc read.c -o read