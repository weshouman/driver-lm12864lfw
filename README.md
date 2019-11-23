### Usage and API
Please refer the header of the [library](myGLCD.c)

### Contents
This project has:
- Wiring Diagram will be added, viewable with [Fritzing](). Currently found in: https://drive.google.com/file/d/0BzytVYRRva62NUtJbW5iTG5LWTQ/view 
- Library itself

### Design Features
This library was designed with the following points in mind
- Compatiblity with both PICC and MikroC (I recommend using PICC*)
- Small ROM size
- Easy of use and understand
- Full documentation
- Read function, *a troublesome function* , is totally embedded.
- The whole file can be easily included into any project

\* Until MikroC (v:5.61), the compiler didn't compile the ```EN()``` , ```set_tris_d()``` & ```EN_Off()``` correctly. *Those calls are located in the READ function*.  
  Even though, one can delve into the assembly file and fix it manually. I'm against doing that.  

### System Info
To know more about the system info, consider reading [this post](https://walidshouman.blogspot.com/2012/07/glcd-lm12864lfw-driven-using-18f4550.html) and [this post](https://walidshouman.blogspot.com/2012/07/easy-glcd-controller-for-ccs-picc-and.html)

### License
This is a Waqf project.  
Licensed under CC0-1.0 as the most permissive license I know so far.  
