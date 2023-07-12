# benchmarkfileio

Simple program for benchmarking the various Windows FileIO APIs using simulated image data.  

# example output

Sector size:  512
Testing write to D:\
CPP: 1.647720 (1262.350553 MB/s)
C I/O: 1.609159 (1292.600830 MB/s)
Win32: 1.605563 (1295.495643 MB/s)
Win32 Unbuffered: 0.762789 (2726.836767 MB/s)
