# sset2c #

* 'sset' file usualy used provides for register setting code from manufacturer of digital camera module.
* So I made it to convert C or H file to use in embedded linux drivers.

## Supported OS ##

* Most of all,
	- Windows 32/64 bit
	- Linux 32/64 bit
	- Mac OS X

## Example ##

* sset here :
```
s010000
s300004
s300203
s300304
s300405
s300500
s300610
s30070A
```
* may changed to C file with some options ...
```
static struct astructure sset2c_convert_data[] = 
{
	{ 0x0100, 0x00 }, 
	{ 0x3000, 0x04 }, 
	{ 0x3002, 0x03 }, 
	{ 0x3003, 0x04 }, 
	{ 0x3004, 0x05 }, 
	{ 0x3005, 0x00 }, 
	{ 0x3006, 0x10 }, 
	{ 0x3007, 0x0A }
};
```

* there's more many options like here:
```
sset2c.exe, sset to c/h converter, rageworx@gmail.com, version 0.1.0.0

 usages :
         sset2c.exe [source file] (options) (output file)

	 options :
         -h(-H) : Convert sset to C language header type
                  Default is C language (.c)
         -w(-W) : Convert in 2 bytes (word) type instead single byte.
                  Default is using single byte.
         -s(-S)=[symbol name] : Using symbol name
                                If it ignored, randomly generates.
         -t(-T)=[structure name] : Using structure name
                                   Default is a comment : /*struct name here*/
```
