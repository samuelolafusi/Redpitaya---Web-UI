these codes are bare c codes.<br>
they acquire signals through channel 1.<br>
first code does not have trigger, it prints samples to screen and writes aquired samples to sample.txt.<br>
seconcd code uses the input signal as trigger, asks user for output file name, prints samples to screen and also saves the file.<br>
to compile, edit makefile, add code filename without the .c extension under PRGS, save and exit.<br>
then type make, run compiled binary.<br>
to remove compiled binary, type make clean.<br>
run python file to plot samples.txt, ensure that sample.txt is in the same dir as plot.py.<br>
