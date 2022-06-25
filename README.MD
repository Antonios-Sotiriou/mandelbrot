# Mandelbrot set with xlib on c programming. Multithreading Version.
### The compiler used to run this software is "gcc version 11.3.0 (Debian 11.3.0-3)"
Only Xlib and standard C Libraries have been used to develop this software.<br>

This software displays a Main window upon which is drawed a Mandelbrot set.<br>
When the user left clicks on a particular area, the set is getting centered on the click point and zoomed in.<br>
In opposition to the above, when the user right clicks, the set is centered on the click point and get zoomed out.<br>
Same goes for Enter button upon which the set is zoomed in.<br>
The keyboard arrows are used for left, right, up, or down moving.<br>

In the board.c file after the include headers, you can choose the number of threads<br>
or the starting size of the application window.For better output, because of mandelbrot's nature,<br>
it is advised to use same values for width and height.Don't forget that you need to recompile<br>
if you change those values.

If you are interested in obtaining this software, clone it and from inside the project's main folder,
run first the following command to download and install the xlib library, if you do not already have it installed.<br>
```
make install
```
After library is ready, compile the project with the following command...<br>
```
make all
```
If compiling was success run the executable
```
./plot
```
Or you can use the Makefile to run the program.Just use
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

