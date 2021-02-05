## What is this project?
The aim of this project is to bring two programmes together. In the end we will have a program that listens continuously and a second one with which it will communicate. This second one will be called in another terminal. Both programs will communicate through the PID of the first stored in a file.<br><br>
The second will send a date, a frequency and a command to the first. This command will then be executed from the date entered and then repeated at a regular interval defined by the frequency. All arguments are in seconds.<br>
<i>For more details the file Details.txt answers the questions of the project and therefore explains many technical choices</i>

## Demo

![Preview](https://github.com/ThomasCorcoral/Periodic_C/blob/master/periodic.png)

## How was it done?

The project was entirely carried out in C.

## Installation Guide

#### Step 1
Clone the Github project. Go to the folder.

##### Step 2
Create executables by typing make

##### Step 3
Open <b>2</b> terminals

#### Step 4
In the first terminal run period (./period)

#### Step 5
In the second terminal run the desired commands:s<br>
for example . /periodic 0 5 ls<br>

##### Step 6
To display the viewer again, you will need to restart it in the first terminal (./race-viewer)

##### Step 7
To see the orders returned ./periodic in the second termial<br>
To stop period ctrl+c in the first.
