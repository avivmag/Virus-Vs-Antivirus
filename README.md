# Virus-Vs-Antivirus

This repository is divided to two inseparable projects.
The first one is a virus that is searching for a file named "ELFexec" (on the same directory it runs in) and infect it with its own code.
The second program is an antivirus that searches for the same "ELFexec" file, scans it and checks for a predefined virus signatures. if it finds that this file is infected, it disinfect it immediatly.</br>
The antivirus program is, in fact, an evolution of the [Virus Detector](https://github.com/avivmag/Virus-Detector) (reading this repository is highly recommended for understanding how does it preload the signatures from the "signatures" file).

The development of this simulation was done as part of an assignment in "Computer Architecture" course at Ben-Gurion University in the second semester of 2016.

A detailed description of the virus and the antivirus can be found in the assignment description attached.

## Virus

From Wikipedia (link attached below): "A computer virus is a type of malicious software program ("malware") that, when executed, replicates itself by modifying other computer programs and inserting its own code.

In this assignment the virus searches for a file named "ELFexec" which is a linux Executable and Linkable Format (ELF) file and modify it so the file will have the virus's code in it.</br>
Once the virus is attached to the victimed file, it replaces the file's entry point so it will point to the virus code, and adds at the end of the attached virus code a command to jump to the original file entry point.</br>
The final result is an infected file which runs the virus code prior to the code itself and only after it finishes, it runs the original program code as well.

## Position Independent Code (PIC)

When an assembly file compilation is made, the compiler replaces some of the text labels to the labels' addresses in the compiled program.</br>
The problem with this compilation process is that after the virus infected other files, the infected files will have exactly the same already-compiled addresses of the addresses in the virus's labels, meaning the infected file will receive some unknown addresses for use instead of the ones that should be where the virus code dwells.</br>
Note: using `call` is not included, because it is known in fact to be replaced with the delta distance of the current position of the code to the destinate labels, so this case solves itself and actualy will be in use in solving our issue.</br>
For that, PIC trick has been created.</br>
What this trick is all about is trying to figure from where this command which is mentioning this label is being used, get the distance from the current position to the destinate label and replace the label with the truely calculated position of the label.</br>
Confused? well, I believe an example would best be used here for demonstration:</br>
Let's assume we have this code:
```
jmp Label_1
... lines of code ...
Label_1:
... some more lines of code ...
```
Label_1 will be replaced with an address of the program's actual address after compiling this program. This is bad for us because this place will be changed once the code will run on another files that are not the original virus.</br>
Let's add (partially) the PIC trick:
```
jmp Label_1
... lines of code ...
Label_1:
... some more lines of code ...
IndependedText_1:
	call IndependedText_2
IndependedText_2:
	pop edx
	ret
```
Let's assume that we added this rows in a place that the program will not reach unless we explicitly called it.</br>
For the full PIC trick, let's change the `jmp Label_1`:
```
call 	IndependedText_1
sub 	edx, IndependedText_2-Label_1
jmp 	[edx]
... lines of code ...
Label_1:
... some more lines of code ...
IndependedText_1:
	call IndependedText_2
IndependedText_2:
	pop edx
	ret
```
Do not panic!</br>
Let's try to follow what happend - one step at a time.</br>
`call IndependedText_1` - as mentioned above, calling is different than jumping. In compilation process this call will calculates the distance of the current position and the destinate one (IndependedText_1 label) and replace its label with this calculated value. 
At program run-time, it will push the current position of the code (The value that is in the EIP register) on the stack and jump the calculated delta distance to the destination.</br>
Currently, on top of the stack we have the position of where we left.</br>
In independent_code_1 we immediatly do `call IndependedText_2`, which means we now push on top of the stack the position of IndependedText_1, meaning the stack should have this two mentioned addresses.</br>
`pop edx` - We poped IndependedText_1 address to edx, which means we now have on top of the stack the position from where we initially left off and edx register should store the position of IndependedText_1.</br>
`ret` - we pop the position we initially jump from to the EIP, meaning it currently position us on this line of code - `sub 	edx, IndependedText_2-Label_1`.</br>
After we do the next code edx should have the value: `IndependedText_2 - (IndependedText_2-Label_1)`, which means it has the PIC value of `Label_1` in hand.</br>
Next, we just jump to the value in the edx variable (this time the compiler won't replace the label in this jump). </br>
Now the code is position independent.

## Antivirus

The antivirus program receives a "signatures" file with the list of viruses we know of and want to remove. What it does is searching for a virus on the "ELFexec" file. once it found one it replaces the entry point of the code to the original place it should be (the .TEXT section). 

## Getting Started
### Prerequisites

1. Kubuntu - this program was tested only on kubuntu, but it probably can be ran on any other known nasm and gcc compatible operating systems.
	https://kubuntu.org/getkubuntu/</br>
(The followings are for those who want to compile the files themselves)
2. GNU make
	https://www.gnu.org/software/make/
3. gcc compiler
	via ```sudo apt-get install gcc-4.8``` on ubuntu based os (kubuntu included).
4. nasm compiler 
	via ```sudo apt-get install nasm``` on ubuntu based os (kubuntu included).
	
Note: this is how I used to build and run the program. There are many other well-known compilers to compile this assembly file for other types of operating systems.

### Simulating the process

1. open terminal and navigate to the virus program directory
2. do this step only if virus rebuilt is needed: type `make` and press enter.
3. type `./ELFexec` and press enter - you can see the program output.
4. type `./Virus` and press enter - congratulation, you've infected the "ELFexec" file (if "perhaps not message" is shown, something went wrong and the file is not infected).
5. type `./ELFexec` and press enter - you can see the program is now infected and trying to infect itself again.
5. copy the infected file to the AntiVirus folder.
6. open the AntiVirus folder.
7. do this step only if AntiVirus rebuilt is needed: type `make` and press enter.
8. type `./AntiVirus` and press enter - congratulation, you've disinfected the "ELFexec" file.
9. type `./ELFexec` and press enter - you can see the program is now disinfected.

## Built With

* [GNU make](https://www.gnu.org/software/make/) - A framework used for simple code compilation.
* [gcc](https://gcc.gnu.org/)
* [nasm](http://www.nasm.us/)

## Useful links

* The original source of the assignment: https://www.cs.bgu.ac.il/~caspl162/Lab92/Tasks?format=standalone.
* https://en.wikipedia.org/wiki/Computer_virus.
* https://en.wikipedia.org/wiki/Antivirus_software.
* https://en.wikipedia.org/wiki/Position-independent_code.
* https://en.wikipedia.org/wiki/Executable_and_Linkable_Format.
