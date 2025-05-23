

# Hook-Of-The-Reaper
![HookOfTheReaperLogo330](https://github.com/user-attachments/assets/ff4479bc-adf3-4b0f-8dfb-0a8755b2ad6b)

Best way to contact me, is to open a new issue, here on GitHub. It can be found at the top left of the webpage, just to the right of 'Code'. Click on the 'Issues' tab, which will bring up a new webpage, with all the current issues. Just above the issues, and to the right, is a green button called 'New Issuse'. Please click that, and create a new issue, which can be a quetion, bug, enhancment, and so on. 

Also, I can be found on Discord from time to time. I totally suck at Discord, and have not used it much. Actually, I only have it, to play Fortnight with my daughter. Yeah, I am old. I was first of my friends to get DSL intenet at a blazing fast 256KBaud, and was always hosting games like GTA II.  
>Discord Username: 6bolt_77860

>Display Name: 6Bolt

Also, when I get the Default Light Gun game file site up and running, I will have a way to contact me there too. Still working on it, and will be some time. 

## Out from Time to Time

My health is not great, as I am disabled. Sometimes I will be out for multiple days, and rare time for a week or more. Just wanted to let everyone know, as I am not ignoring anyone. As during these times, people might try to contact me, and it will take days to get a response back. I will do my best to warn people when I will be out. 

# YouTube Videos

## Hook Of The Reaper - Project Introduction

Talk about the main feature of the program and how to use it.

https://youtu.be/9Fx3xqHKH-o

## Hook Of The Reaper - Advance Feature & How to file a Bug, Enhancement, or Quetion

Talk about the advance feature of the program. Also how to file a bug, enhancement, question. 

https://youtu.be/LRuflVbOHfQ

# The Goals for Hook of the Reaper

#### 1) To do everything that MAMEHooker can do with Serial Port Devices. Starting with light guns, then IO Controllers, and finally force feedback. Able to load INI Files
#### 2) To implement the new Default Light Gun format. This will make any default light gun work with the associated game file, like the INI files.
#### 3) To be open source with the GPLv3 license, so it will remain always open source.
#### 4) To be platform independent. Reason why I chose Qt, as it can compile on many platforms. (Now Low Priority)
#### 5) Have the best performance, and be multi-threaded.



I started this project after getting two Retro Shooter RS3 Reaper light guns as a Christmas present from my daughter in 2024. Never had light guns before, so was a learning experience with the light guns, Tekno-Parrot, MAME, Demulshooter, and MAMEHooker. The only problem I had was with MAMEHooker. It would work for one game, and then freeze up. I tried multiple Windows comparability modes, but it never worked right. I tried some things from the internet, but I never got it to work consistently. I am not blaming MAMEHooker and/or its coder. It is a great program, and many people through out the years have use this program. It was written a while ago and a lot of older programs have problems working with newer Windows OSes. Hence why they have  comparability modes. Seeing a project, I started coding and decided to name the new program, Hook Of The Reaper. Which takes Hook from MAMEHooker, and Reaper from the RS3 Reaper light guns, which are awesome. The picture of the cross hair being ‘hooked’ by a reaper scythe, came into my head for the program icon, and truly represents the program.


# Getting the Program to Work

Since the project is still in early release, only Windows is supported right now. I plan on getting Linux and Mac OSes later on. I do have a Linux box, which doubles as my NFS/Clould server. I plan to use that for getting Linux up in running next. A person could take the code, and compile in the needed OS. I detail compiling below.

There is one major change that needs to be done to get Hook Of The Reaper working with MAME and DemulShooter. The ‘network’ has to be enabled on MAME and DemulShooter. MAMEHooker used the window messaging to receive data. This is a Windows type thing, as Linux and MacOS (think it is based on Debian) don’t have this window message type. They might have something that is close to it. But to meet goal #4, I needed a solution that would easily work on all platforms. The ‘network’ was the best options. Instead of using window messaging, it uses TCP Socket data on the localhost (IP 127.0.0.1) on port 8000. TCP Socket has major support on all platforms.

### Enable Network on MAME

Open the mame.ini file, in the main MAME directory. Should be right under or above the mame(64).exe. If it is not there, you might have to start MAME for the first time and run a game. Once you have mame.ini open, go down or search for “OSD OUTPUT OPTIONS.” Below that, you will see “ouput” on the left, and a setting on the right, which might be “windows” if you used MAMEHooker. The “output” needs to be set to “network”. So erase any existing option, like “windows” and put in “network”. Then save the file and you are done.


### Enable Network on DemulShooter

This is a lot easier then MAME. Open up the DemulShooter GUI (DemulShooter_GUI.exe). Pull down the “Page selection:” combo box at the top, and then select “Output” at the bottom. Then check the top box, which is labeled “Enable Outputs”. Then check the third box, which is labeled, “Network Outputs”. Then click the button at the bottom, labeled “Save Config”. That is it, Hook Of The Reaper will now be able to connect to MAME and DemulShooter. 



# Game Files

## INI Game Files

Now you just need to have game files to tell Hook Of The Reaper what signals to watch out for. You can use the MAMEHooker INI gamefiles. Hook Of The Reaper can load these files and process the data for light guns only. I have not set up any I/O controllers, LED controllers, or force feedback. The INI game files can be put in the directory below. Also the characters ‘%s%’ and ‘|’ are supported in Hook Of the Reaper INI game files.

### Location of INI Game Files

- path/to/HookOfTheReaper/ini/MAME


## Default Light Gun Game Files

I am working on a new game file format for light guns, I call it Default Light Gun games file. It will take away a lot of the complexity of writing a game file. Lets look how MAMEHooker does damage for 3 Light Guns, one RS3 Reaper and 2 made up. Also how Hook Of The Reaper could do it with the new format.

| Light Gun | MAMEHooker INI | HOTR Default LG |
|-----------|----------------|-----------------|
| RS3 Reaper | P1_Damage=nll\|cmr 4 ZZ |:P1_Damage *P1 >Damage |
| Default Gun #2 | P1_Damage=nll\|cmr 7 M0(Z)%s% | :P1_Damage *P1 >Damage |
| Default Gun #3 | P1_Damage=nll\|cmr 9 TTXER#7 | :P1_Damage *P1 >Damage |


 As you can see, the Hook Of The Reaper, didn’t change. The program will load up the command, based on the Default Light Gun being used. Then the same game files can be used for every light gun. So Default Light Gun game files can be slowly built, and used by everyone. Instead of everyone making custom INI game files. 

## Commands

Light guns only have so many commands, as they can recoil, shake, and few other things. I do have some overlap in commands, but that is to support HD Recoil features in the future. Here is the List of commands I have so far:

### Current Default Light Gun Commands

| Command | Notes |
|---------|-------|
| Open_COM | Connects to the Serial Port or USB HID. Also Enters External Control for Light Gun  |
| Close_COM | Disconnects from Serial Port or USB HID. Also Exit out of External Control for Light Gun |  
| Open_COM_NoInit | Connects to the Serial Port or USB HID only  |
| Close_COM_NoInit | Disconnects to the Serial Port or USB HID only |
| Close_COM_InitOnly | Only Exit out of External Control for Light Gun |
| Damage | Doesn't Happen on 0, no need for '\|' |
| Recoil | Doesn't Happen on 0, no need for '\|' |
| Recoil_Value | Output signal, like PX_CtmRecoil, Controls the Solenoid |
| Recoil_R2S | Converts Rumble Recoil to Solenoid Recoil |
| Reload | Doesn't Happen on 0, no need for '\|' |
| Reload_Value | Uses Ammo Value to Know When Reload Happens | 
| Ammo   | Doesn't Happen on 0, no need for '\|'    |
| Ammo_Value|  Doesn't Happen on 0, no need for '\|', except for Reapers for Z0    |
| Shake | Doesn't Happen on 0, no need for '\|' |
| Auto_LED |     |
| AspectRatio_16:9 | Can be used at mame_start/stop   |
| AspectRatio_4:3 | Can be used at mame_start/stop     |
| Joystick_Mode | Can be used at mame_start/stop     |
| Keyboard_Mouse_Mode | Can be used at mame_start/stop    |
| Display_Ammo | Displays Ammo Value  |
| Display_Ammo_Init | Init Command to Display Ammo Value, If Needed |
| Display_Life | Displays Life Value  |
| Display_Life_Init | Init Command to Display Life Value, If Needed |
| Display_Other | Displays Other Value  |
| Display_Other_Init | Init Command to Display Other Value, If Needed |
| Display_Refresh | Display Refresh in Milliseconds |
| Offscreen_Button | Reload for Offscreen Shot (Default Setting for Open_COM)|
| Offscreen_Normal_Shot | Normal Shot for Offscreen Shot
| Null |      |


But, what if you are playing a new game, and don’t have the signals. Hook Of The Reaper works like MAMEHooker does. If you choose Default Light Guns files first in Settings, and there is no game file of that game name for defaultLG or INI, Hook Of The Reaper will make a new game file in the defaultLG directory, with the name of the game dot txt. The top will have a standard top, and then list out all the signals observed during the time the game was open. So there must be no game file for both INI and Default Light Gun. 

If the setting is not set for use Default Light Gun files first, it will make a new game file in the ini/MAME directory, with the name of the game dot ini. The top will be a standard INI file top, and then list out all the signals observed during the time the game was open. Also, if any new signals pop up later, that are not in the file, it will be added to the file, when the game closes, like MAMEHooker, does. 


### Recoil_R2S Command

The command is used when the arcade gun used rumble for recoil. So when the trigger is held down, the motor is turned on, and when trigger is released, motor is off. So the signal is just 0 (motor off) and 1 (motor on). This doesn't work for at home light gun's which uses a solenoid. A solenoid, needs pulese (0 -> 1 -> 0) to work. Then the pulses are correctly delayed. Now Hook Of The Reaper can convert the rumble motor signal to work with the light gun's soleniod. When the signal goes high, it does a recoil and delay. Then it loops the recoil and delay, until the rumble signal goes back to 0. The delay is in milliseconds, and is located in the light gun's .hor file in the data directory. I pasted the RS3 Reaper below.

Recoil_R2S=100

The first and only variable after the '=' is the delay in milliseconds. The recoil data is taken from the Recoil command. Currently, every gun is set to 100ms. I don't know if this is the best setting for every light gun. As I only have the Reapers right now. So you might have to increase or decrease the delay for your light gun. An easy game to try it out on, is Let's Go Island 3D. Instead of using PX_CtmRecoil signal with the Recoil command, use the PX_GunMotor signal with the Recoil_R2S command. Then you can increase/decrease the delay to what works best for the light gun. Then you can report back to me the delay and light gun. Then I will use the average I get from everyone.

I made this command because of Alien 3 game, which has the acceleration. Which means the signal goes 0 -> 1 for awhile, then starts going up and down, when the damage for the gun gets close to zero. This happens in Terminator 2 too, but with deceleration. So the gun works good, until damage of the gun drops to ~1/4, and it bounces around, which is to tell the player, you cannot hold the trigger all the time. In the 'alien3.txt' Default Light Gun game file, I put a number after the \>Recoil_R2S command, please see below.

\>Recoil_R2S 125

The 125 is a percentage, so it is 125% of the delay. Since the delay is at 100ms, then it would be 100ms * 1.25 (125%) = 125ms. I did a percentage, because in theory, if the light gun delay is set correctly, then every light gun would work, because it is based on percentage of said delay. I found that the Reaper worked good at 125% delay, and enough of a change to know the gun damage is has dropped. You can remove the 125, or change it around to see what you like for Alien 3 and Terminator 2.



## Location of Default Light Gun Game Files

- path/to/HookOfTheReaper/defaultLG

Majority of games are in the defaultLG directory, so a person can see how it is set up. Unlike above, every thing has its own line. The Damage example would be like this in the file.


| File Line | What it Represents |
|-----------|--------------------|
| :P1_Damage | A Signal with a beginning : |
| *P1 | A Player or ALL, with a beginning * |
| \>Damage | A Command, with a beginning \>


A signal can have no Player/All and no Command, or a signal can have Player/All and Command. Also Multiple Players and Commands can be after a Signal.

- :P1_Damage
- *P1
- \>Damage
- *P3
- \>Shake

Also, multiple commands can be used, and be selected based on the Signal’s current value. It is implemented the same way MAMEHooker does, with the ‘|’. The first command needs a '\>' infront of it. The other commands, it is optional. So If you had:

- :P1_Damage
- *P1
- \>Null|Shake|Recoil|Damage	&emsp;	So When P1_Damage=0 → Null, 1 -> Shake, 2 → Recoil, 3 → Damage

The 0 is the left most Command, then increase by one, going to the right. The same way MAMEHooker has it. Also, the INI files can use the %s% variable. The %s% is replaced by the Signal’s value. It is not used in the Default Light gun files, but want to mention that it is supported.  

# Player Ordering in the Default Light Gun Game Format

At the top of the file, it will start with 'Players' and then the next line is how many players needed for the game, which can be 1 to 4 currently. The next lines will tell Hook Of The Reaper the player ordering. If there are 2 playeres, then there will be P1 and P2. If 4 players, then P1, P2, P3, and P4. But you can put in different players. For example, there is 2 players, and you put in P3 and P4. What will happen is P3 will map onto P1, and P4 will map onto P2. I did it this way, so you could you different light guns in Player's Assignment. So for a certain game, you would like to use a different light guns that are assigned to P3 and P4. then you could change the game file to use players P3 and P4, instead of P1 and P2. Then you didn't have to change the Player's Assignment all the time, if you wanted to use a different light guns for certain games. Below is an example of this.

| Default Light Gun Game File | Notes |
|-----------------------------|-------|
| Players |  |
| 2 | Number of Players |
| P3 | P3 -> P1 in this file  |
| P4   | P4 -> P2 in this File  |
| [States] |      |
| :mame_start |  |
| *All | P3 & P4  |
| >Open_COM | Open COM Ports for P3 & P4 Light Guns |
| [Signals] |      |
| :P1_Damaged |      |
| *P1 | Which is mapped to P3  |
| >Damage | Send Damage CMDs to P3's Light Gun |
| :P2_Ammo |        |
| *P2 | Which is mapped to P4   |
| >Ammo_Value | Send Ammo_Value to P4's Light Gun |

## ChangePlayerNum Program

I created a small program to modify the top section of the Default Light Gun game file. With the program, it can easily change player number, for all the *.txt files in a directory, or just one file. There are many options. Link to the repository is below, which has a release.

https://github.com/6Bolt/ChangePlayerNum

Can run the program with no inputs to get the help menu. 


# Current Default Light Gun Files Included

Most all games in the Default Light Gun game format, are included with Hook Of The Reaper program now. There is another directory, that uses Ammo_Value instead of Recoil. These will work for all light guns, but the RS3 Reaper will get the most use, as it will now use the Z0 & Z6 commands. Also, I put in multiple checks in, to make sure it cannot do multiple Z0 commands in a row, which will overheat the light gun. Also, try not to do the Z0 command when starting and stopping a game, which could reboot connection if using 2 Reapers. This is because of the power supply is only 3A, where 5-6A is needed. It seems the control side is not fully isolated from the 12-24V side. 


# How to Use Hook Of The Reaper 

### Add a Light Gun:

Select ‘File’ in the menu at the top, and to the left. Select “Add Light Gun” and a new small child window will pop up. The first combo box has the supported default light guns, which currently are the Retro Shooter RS3 Reaper, Retro Shooter MX24, JB Gun4IR, Fusion, Blamcon, and OpenFire. If you have a light gun that is not in the list, then choose the blank option, and fill out the name and COM port options. Then you will have to add the COM Port commands to the 'nonDefaultLG.hor' file, located in the data directory. When you select a default light gun, it will lock in certain combo boxes, which is the correct settings for that light gun. Then a name is needed in the name box. Also the correct COM port needs to be selected. If the light guns are connected and powered on, the COM Port info will show up on the right side. You can toggle through COM port number, until the correct data shows up on the right. Then you can click the “Add” button or the “OK” button. The “Add" button will add and save the light gun. The “OK button will do the same as the “Add” button, but will close the window after saving. If any changes need to be made after the light gun has been saved, then select “Edit” in the top menu, and then select “Edit Light Gun.” This will bring up a similar child window, but called “Edit Light Gun.” You will be able to edit or delete the light gun. There is also windows for adding a COM Device, but nothing can be done with it yet.


### Player’s Assignment:

After the light guns have been added to Hook Of The Reaper, they need to be assigned to a player number. Click the “File” in the top menu, and then select the “Player’s Assignment.” Which will bring up the Player’s Assignment child window. Currently, there are eight player's assignments. Next to each Player, is a combo box loaded with all the saved light guns. Then the light guns can be assigned to a player. Once they have been assigned, the “Assign” or “OK” button can be pressed to save the assignment. 



### Settings:

##### Currently there are only four settings. The first is to use the Default Light Gun game files before the INI game files. There is a check box to the left, that can be checked or un-checked. But if it is set, and you load a game that doesn’t have a Default Light Gun game file, but there is an INI game file, it will load the INI game file. If both were missing it will make new Default Light Gun game file with all the signals. If it is not set, and you load a game that doesn’t have a INI game file, but there is a Default Light Gun game file, it will load the Default Light Gun game file. If both were missing it will make new INI game file with all the signals.  

##### The second option is to disable the multi-threading. Hook Of The Reaper uses multi-threading to boost performance. Currently, it uses 3 threads. One main thread, a 2nd thread for the TCP Socket, and a 3rd for the Serial COM Port. If Multi-threading is disabled, then the GUI will work horribly, if it is waiting to connect to to a TCP Socket. It is highly recommended not to disable multi-threading, unless it is needed. This is why I wanted to use multi-threading, so waiting for the TCP Connection, doesn’t affect the GUI performance. A re-start of the program is needed.

##### The third option is the refresh time for the output signals in msecs. So that performance will not suffer from updating the signal values on the main window, I put the updating on a timer. So smaller the time setting, the faster it will refresh the data, and vice versa. So if you don’t see any signal values changing, then the refresh display time needs to be a smaller amount. But smaller the number, will make the performance less and less. So it is a trade off.  

##### The fourth and last option is to close or leave the COM port connections open. If if the option is checked, then it closes the port connection. If un-checked, it leaves the connection open. The one bad thing is if you leave the COM port open, then you can not use the test COM port window option.

### Hooker Engine

The Hooker Engine, which does all the magic, will start right away. Also, when any child windows is open, the Hooker Engine will stop working. Once the child window is closed, the engine will re-start. Just make sure that you don’t open a child window when playing a game.    



### State of the Program

The program is currently in early release (v1.0). Right now I plan on adding in more default light guns. Also add in more Default Light Gun game files. Once that is done, I will move onto I/O Devices and LED controllers. 

If you find any bugs, please report them here. 



## Compile the Code

Almost everything is in Qt, so you need Qt and the MSVC 2022 tools. Then you need the libusb/hidapi library. This is for USB HID communication. Below is what I used in Qt to compile the program. 

- Qt 6.8.1 with a CMake File with MSVC 2022
* My CMake Version: 3.29.3
- libusb's hidapi library on GitHub - https://github.com/libusb/hidapi


# FAQ

### Is Hook Of The Reaper complete?

No, currently Hook Of The Reaper is a work in progress. I am almost done with the light gun work. When the light gun work is done, I will then work on the game data base. After that, I will work on getting the LED & I/O controller into Hook Of The Reaper. After that, I plan on getting the wiki done. Then it will be complete.


### Other Peoples Light Guns work, but Mine Doesn't, Can you Help Me Out?

Sorry, I cannot help you. But you can use the debug setting that was put in v1.1.1. The debug setting is safe for sort term use. If your light gun starts working in the debug mode, then something is most likely wrong with your USB or USB cabling. 


### Can you Help Me Set-Up my Light Gun and Light Gun Games? Or Help Make a Game File?

No.

### Can you Help Me Set-UP my Hook Of The Reaper?

No. Sorry, there will be no tech support now or ever for this program. It is a free open source project, that comes with no warrenty or guarantees. 

### No Tech Support, but What if I Need Help?

Sorry your are on your own. I do my best to test eveerything out before a release. The number of bugs have been really low. Most Issues are not Hook Of The Reaper bugs. I think I have only gotten 2-3 bugs so far from Issues. Most bugs have been caught by myself. Also, many others have gotten Hook Of The Reaper working with no problems. I know people have gotten all the supported light gun working.  

### Do you Have the Game File XXXXX? It is Not Included with Hook Of The Reaper?

No. All the game files that I have, are include with Hook Of The Reaper. 



