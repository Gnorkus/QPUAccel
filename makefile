# include the mailbox driver interface.  This will
# open the vcio driver.
MBOX_C = /opt/vc/src/hello_pi/hello_fft/mailbox.c
MBOX_INC = -I/opt/vc/src/hello_pi/hello_fft/

F = -lrt -lm -ldl

MainShort: MainShort.o PixelBuffer_linux.o CString_linux.o Geometry_linux.o
	g++ -o MainShort MainShort.o PixelBuffer_linux.o CString_linux.o Geometry_linux.o mailbox.o $(F) 

MaxDialateThread.c: MaxDialateThread.asm
	./vc4asm -V -C MaxDialateThread.c MaxDialateThread.asm

MaxDialateMain.c: MaxDialateMain.asm
	./vc4asm -V -C MaxDialateMain.c MaxDialateMain.asm

SubFloorGpuThread.c: SubFloorGpuThread.asm
	./vc4asm -V -C SubFloorGpuThread.c SubFloorGpuThread.asm

SubFloorGpuMain.c: SubFloorGpuMain.asm
	./vc4asm -V -C SubFloorGpuMain.c SubFloorGpuMain.asm

NewDepthGpuThread.c: NewDepthGpuThread.asm
	./vc4asm -V -C NewDepthGpuThread.c NewDepthGpuThread.asm

NewDepthGpuMain.c: NewDepthGpuMain.asm
	./vc4asm -V -C NewDepthGpuMain.c NewDepthGpuMain.asm

MainShort.o: MainShort.cpp PixelBuffer_linux.h
	g++ -O3 -mfpu=neon -c MainShort.cpp $(MBOX_C) $(MBOX_INC)

PixelBuffer_linux.o: PixelBuffer_linux.cpp MaxDialateMain.c MaxDialateThread.c SubFloorGpuMain.c SubFloorGpuThread.c NewDepthGpuMain.c NewDepthGpuThread.c PixelBuffer_linux.h
	g++ -c PixelBuffer_linux.cpp $(MBOX_C) $(MBOX_INC)

CString_linux.o: CString_linux.cpp CString_linux.h
	g++ -c CString_linux.cpp $(MBOX_C) $(MBOX_INC)

Geometry_linux.o: Geometry_linux.cpp Geometry_linux.h
	g++ -O3 -c Geometry_linux.cpp $(MBOX_C) $(MBOX_INC)

.PHONY: clean

clean:
	rm -f *.o
	rm -f MaxDialateMain.c
	rm -f MaxDialateThread.c
	rm -f SubFloorGpuMain.c
	rm -f SubFloorGpuThread.c
	rm -f NewDepthGpuMain.c
	rm -f NewDepthGpuThread.c
	rm MainShort
 
