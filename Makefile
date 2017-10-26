CC=gcc
CFLAGS=-O -Wall
VARIANTS=repmovsb memmove memcpy ssecpy ssemove $(EXTRA)
SIZES=1 8 32 64 128 256 512 1024 2048 4096 8192 16384
SOURCES=Makefile main.c memcpy.c memmove.c repmovsb.c avxcpy.c avxmemcpy.c ssecpy.c ssememcpy.c random.c testmemcpy.c testmemmove.c avxmove.c ssemove.c
RND="0 1 3 25 79 100" "0 1 3 7 13 25 39 55"
TEST=avx

all: $(VARIANTS) $(addprefix rnd,$(VARIANTS))

clean:
	-rm *.o memcpy memmove repmovsb avxcpy ssecpy avxmove ssemove rndmemcpy rndmemmove rndrepmovsb rndavxcpy rndssecpy perf.data* *~

dist: $(SOURCES)
	-rm -rf move
	mkdir move
	cp -p $(SOURCES) move
	zip -9r move.zip move
	-rm -rf move

bench: memcpy memmove repmovsb
	make clean
	make all
	ldd --version #show glibc version
	@for j in $(VARIANTS); do for i in $(SIZES); do perf stat -e cycles $$j $$i 0 2>&1|awk '/cycles/ {printf("%4d ",$$1/(4096*4096))}'; done; echo $$j; done
	@for j in $(VARIANTS); do for i in $(SIZES); do perf stat -e cycles $$j $$i 1 2>&1|awk '/cycles/ {printf("%4d ",$$1/(4096*4096))}'; done; echo $$j aligned; done
	@for j in $(VARIANTS); do for i in $(SIZES); do perf stat -e cycles $$j `expr $$i - 1` 0 2>&1|awk '/cycles/ {printf("%4d ",$$1/(4096*4096))}'; done; echo $$j blksz-1; done
	@echo " anti-avx  anti-sse"
	@for j in $(VARIANTS); do for i in $(RND); do perf stat -e cycles -e branch-misses rnd$$j $$i 2>&1|awk '/cycles/ {printf("%4d ",$$1/(4096*4096))} /branch-misses/ {printf("%.2f ",$$1/(4096*4096))}'; done; echo $$j random; done

#for some reason perf-4.11 wants "record" for repmovsb
perf-4.11: memcpy memmove repmovsb
	make clean
	make all
	ldd --version #show glibc version
	@for j in $(VARIANTS); do for i in $(SIZES); do perf stat -x " " record -e cycles $$j $$i 0 2>/dev/null && perf stat -x " " report 2>&1|awk '{printf("%4d ",$$1/(4096*4096))}'; done; echo $$j; done
	@for j in $(VARIANTS); do for i in $(SIZES); do perf stat -x " " record -e cycles $$j $$i 1 2>/dev/null && perf stat -x " " report 2>&1|awk '{printf("%4d ",$$1/(4096*4096))}'; done; echo $$j aligned; done
	@for j in $(VARIANTS); do for i in $(SIZES); do perf stat -x " " record -e cycles $$j `expr $$i - 1` 0 2>/dev/null && perf stat -x " " report 2>&1|awk '{printf("%4d ",$$1/(4096*4096))}'; done; echo $$j blksz-1; done
	@echo " anti-avx  anti-sse"
	@for j in $(VARIANTS); do for i in $(RND); do perf stat -e cycles -e branch-misses rnd$$j $$i 2>&1|awk '/cycles/ {printf("%4d ",$$1/(4096*4096))} /branch-misses/ {printf("%.2f ",$$1/(4096*4096))}'; done; echo $$j random; done

perfex: memcpy memmove repmovsb
	make clean
	make all
	ldd --version #show glibc version
	@for j in $(VARIANTS); do for i in $(SIZES); do perfex $$j $$i 0 2>&1|awk '{printf("%4d ",$$2/(4096*4096))}'; done; echo $$j; done
	@for j in $(VARIANTS); do for i in $(SIZES); do perfex $$j $$i 1 2>&1|awk '{printf("%4d ",$$2/(4096*4096))}'; done; echo $$j aligned; done
	@for j in $(VARIANTS); do for i in $(SIZES); do perfex $$j `expr $$i - 1` 0 2>&1|awk '{printf("%4d ",$$2/(4096*4096))}'; done; echo $$j blksz-1; done
	@echo " anti-avx  anti-sse"
	@for j in $(VARIANTS); do for i in $(RND); do perfex rnd$$j $$i 2>&1|awk '{printf("%4d       ",$$2/(4096*4096))}'; done; echo $$j random; done

avxmemcpy.o: avxmemcpy.c
	$(CC) $(CFLAGS) -mavx -c avxmemcpy.c

avxmemmove.o: avxmemmove.c
	$(CC) $(CFLAGS) -mavx -c avxmemmove.c

memcpy: memcpy.o main.o

memmove: memmove.o main.o

repmovsb: repmovsb.o main.o

avxcpy: avxmemcpy.o avxcpy.o main.o

ssecpy: ssememcpy.o ssecpy.o main.o

avxmove: avxmemmove.o avxmove.o main.o

ssemove: ssememmove.o ssemove.o main.o

rndmemcpy: memcpy.o random.o
	$(CC) $^ -o $@

rndmemmove: memmove.o random.o
	$(CC) $^ -o $@

rndrepmovsb: repmovsb.o random.o
	$(CC) $^ -o $@

rndavxcpy: avxmemcpy.o avxcpy.o random.o
	$(CC) $^ -o $@

rndssecpy: ssememcpy.o ssecpy.o random.o
	$(CC) $^ -o $@

rndavxmove: avxmemmove.o avxmove.o random.o
	$(CC) $^ -o $@

rndssemove: ssememmove.o ssemove.o random.o
	$(CC) $^ -o $@

testcpy: testmemcpy.c $(TEST)memcpy.o
	gcc $(CFLAGS) -Dtestmemcpy=$(TEST)memcpy -c testmemcpy.c
	gcc testmemcpy.o $(TEST)memcpy.o -o testmemcpy
	./testmemcpy

testmove: testmemmove.c $(TEST)memmove.o
	gcc $(CFLAGS) -Dtestmemmove=$(TEST)memmove -c testmemmove.c
	gcc testmemmove.o $(TEST)memmove.o -o testmemmove
	./testmemmove
