# Mandelbrot set with xlib on c programming.
### The compiler used to run this software is "gcc (Debian 10.3.0-9) 10.3.0"
Only Xlib and standard C Libraries have been used to develop this software.<br><br>
This software displays a Main window upon which is drawed a Mandelbrot set.<br>
When the user left clicks on a particular area, the set is getting centered on the click point and zoomed in.<br>
In opposition to the above, when the user right clicks, the set is centered on the click point and get zoomed out.<br>

If you are interested in obtaining this software, clone it and compile it with the following command.<br>
This command must be run from inside the project main directory.<br>
```
make all
```
I include the gcc debugger flag "-g" in Makefile.If you don't need to use debugger you can remove it.<br>
If compiling will be success run the executable
```
./plot
```
If you don't see any errors software is ready for use.<br><br>
Now press any key and a mandelbrot set should be ploted on the opened window.<br>
You can change the parameters in plot.c file to experiment with the representation of the set.

# Enjoy...