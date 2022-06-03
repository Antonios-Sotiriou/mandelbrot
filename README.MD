# Mandelbrot set with xlib on c programming.
### The compiler used to run this software is "gcc (Debian 10.3.0-9) 10.3.0"
Only Xlib and standard C Libraries have been used to develop this software.<br>
This software displays a Main window upon which is drawed a Mandelbrot set.<br>
When the user left clicks on a particular area, the set is getting centered on the click point and zoomed in.<br>
In opposition to the above, when the user right clicks, the set is centered on the click point and get zoomed out.<br>
Same goes for Enter button upon which the set is zoomed in.<br>
The keyboard arrows are used for left, right, up, or down moving.<br>
All the importand variables are keeped in the **header_files/global_vars.h** file.<br>
There can someone change the window size, the number of iterations as long as the number of processes, but...<br>
even number of processes cause some display problems due to float dividation of the program's variables.<br>
So it is advised, at least for this version, to use **1, 2, 4, 5, 8, _or_ 10** number of processes.<br>

If you are interested in obtaining this software, clone it and from inside the projects main folder,
run first the following command to download and install the xlib library if you not already have it installed.<br>
```
make install
```
After library is ready, compile the project with the following command...<br>
```
make all
```
I include the gcc debugger flag *-g* in Makefile.If you don't need to use debugger you can remove it.<br>
If compiling will be success run the executable
```
./plot
```
Or you can use the Makefile to run the program also.Just use
```
make exec
```
If you don't see any errors software is ready for use.<br>
### Enjoy...

# Uninstall and clear all software rellevant files and libraries.<br>
If you want to uninstall the libraries run the following.
```
make uninstall
```
After command finishes execution, remove all program relevant files and folders
```
make clean
```
At long last delete the project folder.

### Now your machine is back to before program aquisition state

