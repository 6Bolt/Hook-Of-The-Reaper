

# Hook-Of-The-Reaper
![HookOfTheReaperLogo330](https://github.com/user-attachments/assets/ff4479bc-adf3-4b0f-8dfb-0a8755b2ad6b)

# Quick Links

You might have to click the link a couple of times for it to work correctly. Sorry, but a problem with GitHub. 

[Tutorial: How to Setup Hook of the Reaper](https://hotr.6bolt.com/pmwiki.php/Tutorial/Tutorial) 

[FAQ](https://github.com/6Bolt/Hook-Of-The-Reaper#faq)

[Game Files](https://github.com/6Bolt/Hook-Of-The-Reaper#game-files)

[Commands](https://github.com/6Bolt/Hook-Of-The-Reaper#commands)

[Recoil_R2S Command](https://github.com/6Bolt/Hook-Of-The-Reaper#recoil_r2s-command)

[Location of Default Light Gun Game Files](https://github.com/6Bolt/Hook-Of-The-Reaper#location-of-default-light-gun-game-files)

[Player Ordering in the Default Light Gun Game Format](https://github.com/6Bolt/Hook-Of-The-Reaper#player-ordering-in-the-default-light-gun-game-format)

[Recoil & Reload Header](https://github.com/6Bolt/Hook-Of-The-Reaper#recoil--reload-header)

[Reload](https://github.com/6Bolt/Hook-Of-The-Reaper#reload)

[Recoil and Reload Commands](https://github.com/6Bolt/Hook-Of-The-Reaper#recoil-and-reload-commands)


# Contact 

### DO NOT EMAIL ME FOR TECH SUPPORT. First, read the FAQ (link above). If you have a real bug, go to 'Issues' and read the rules first (IE the README there). Then you can file a Bug, Enhancment or Issue. I cannot email back from this email address. All unwanted email, will be ignored and deleted. The email is for people to submit DefaultLG game files. Also, for companies to reach me. If you don't meet those two requirments, then don't email. 

Email: gamefiles@hotr.6bolt.com

Best way to contact me, is emailing me at gamefiles@hotr.6bolt.com. Another way is to open a new issue, here on GitHub. It can be found at the top left of the webpage, just to the right of 'Code'. Click on the 'Issues' tab, which will bring up a new webpage, with all the current issues. Just above the issues, and to the right, is a green button called 'New Issuse'. Please click that, and create a new issue, which can be a quetion, bug, enhancment, and so on. 

Also, I can be found on Discord from time to time. I totally suck at Discord, and have not used it much. Actually, I only have it, to play Fortnight with my daughter. Yeah, I am old. I was first of my friends to get DSL intenet at a blazing fast 256KBaud, and was always hosting games like GTA II.  
>Discord Username: 6bolt_77860

>Display Name: 6Bolt

## Hook of the Reaper DefaultLG Game File Site

https://hotr.6bolt.com

This is were the latest DefaultLG game files are stored. Also, lists what games are supported by Hook of the Reaper. If a game is not working, check the site first, to see if it is supported. As you don't want to look like a dumbass, when complaining about an unsupported game.   

## Out from Time to Time

My health is not great, as I am disabled. Sometimes I will be out for multiple days, and rare time for a week or more. Just wanted to let everyone know, as I am not ignoring anyone. As during these times, people might try to contact me, and it will take days to get a response back. I will do my best to warn people when I will be out. 

# YouTube Videos

## Hook Of The Reaper - Project Introduction

Talk about the main feature of the program and how to use it. This is old now, but still good info.

https://youtu.be/9Fx3xqHKH-o

## Hook Of The Reaper - Advance Feature & How to file a Bug, Enhancement, or Quetion

Talk about the advance feature of the program. Also how to file a bug, enhancement, question. 

https://youtu.be/LRuflVbOHfQ

# The Goals for Hook of the Reaper


#### 1) To implement the new Default Light Gun format. This will make any default light gun work with the associated game file, like the INI files.
#### 2) To be open source with the GPLv3 license, so it will remain always open source.
#### 3) Have the best performance, and be multi-threaded.
#### 4) To be platform independent. Reason why I chose Qt, as it can compile on many platforms. (Now Low Priority)


#### X) To do everything that MAMEHooker can do with Serial Port Devices. Starting with light guns, then IO Controllers, and finally force feedback. Able to load INI Files. Now that people have switched over to the new DefaultLG game file, the is the lowest priority now. I still plan on doing everything that MAMEHooker does, but with the DefaultLG files, and whatever I call the LED/Lights/Display Controller stuff. Right now, Hook Of The Reaper can do everything MAMEHooker can do, with respects to light guns, and more. It now uses one game file for everything, and no more custom game files. Also, displays work more with Hook Of The Reaper. As both digits work on the Alien Light Gun, and Life gylphs and bars work on OpenFire. 


I started this project after getting two Retro Shooter RS3 Reaper light guns as a Christmas present from my daughter in 2024. Never had light guns before, so was a learning experience with the light guns, Tekno-Parrot, MAME, Demulshooter, and MAMEHooker. The only problem I had was with MAMEHooker. It would work for one game, and then freeze up. I tried multiple Windows comparability modes, but it never worked right. I tried some things from the internet, but I never got it to work consistently. I am not blaming MAMEHooker and/or its coder. It is a great program, and many people through out the years have use this program. It was written a while ago and a lot of older programs have problems working with newer Windows OSes. Hence why they have  comparability modes. Seeing a project, I started coding and decided to name the new program, Hook Of The Reaper. Which takes Hook from MAMEHooker, and Reaper from the RS3 Reaper light guns, which are awesome. The picture of the cross hair being ‘hooked’ by a reaper scythe, came into my head for the program icon, and truly represents the program.


# Getting the Program to Work

Currently, only Windows (10 & Higher) is supported now. I plan on doing Linux and older Windows after the program is complete. I don't plan on doing MacOS support.

This has moved to the main site, to the Tutorial. Link is below.
https://hotr.6bolt.com/pmwiki.php/Tutorial/Tutorial



# Game Files

## INI Game Files

Now you just need to have game files to tell Hook Of The Reaper what signals to watch out for. You can use the MAMEHooker INI gamefiles. Hook Of The Reaper can load these files and process the data for light guns only. Currently, only serial port and USB HID commands are supported. I have not set up any I/O controllers, LED controllers, or force feedback. The INI game files can be put in the directory below. Also the characters ‘%s%’ and ‘|’ are supported in Hook Of the Reaper INI game files.

Now that majority of people are using the new DefaultLG patterns, the MAMEHooker INI support is becoming lower and lower priority for me. As I only put the INI game file support in, to get people to switch over to HOTR. Now that many have, and mainly using the DefaultLG game files, I see little gain from continuing support for INI game file.

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
| Recoil_R2S | Converts Rumble Recoil to Solenoid Recoil |
| Recoil_Value | Output signal, like PX_CtmRecoil, Controls the Solenoid |
| Reload | Doesn't Happen on 0, no need for '\|' |
| Reload_Value | Uses Ammo Value to Know When Reload Happens | 
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


A signal can have no Player/All and no Command.

- :UnusedSignal1
- :UnusedSignal2
- :UnuesdSignal3

A signal can have Player/All and Command. It cannot have multiple players and after a Signal and Command. For example, below is an error in the DefaultLG format. 

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

The '|' cannot be used with the Recoil_R2S command. As it is only used on binary signals, and it uses both the '0' and '1' values. So there is no need for it. And Hook Of The Reaper will error out if you try to use it.

The '|' is not supported for the Recoil and Reload commands with the '#' infront of them. As these commands now how to use the signals already. So there is no need for them. And Hook Of The Reaper will error out if you try to use it. 


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


# Recoil & Reload Header

All new DefaultLG game files will need this header. It will start with 'Recoil & Reload' then follow by four recoil methods. After each recoil method, will be a '0' or '1'. The '0' means that recoil method is not supported in the game file. Where a '1' means that recoil method is supported in the game file. Then, if reload is suported, the reload method is after the '1', as it is not needed after the '0'. The Ammo_Value, can be Ammo_Value or blank, as Ammo_Value does both recoil and reload. The four recoil methods must be in the order below, in the example header.

| Header |
|--------|
|Recoil & Reload |
|Ammo_value 1 Ammo_Value |
|Recoil 1 Reload_Value |
|Recoil_R2S 1 Reload |
|Recoil_Value 1 Reload_Value |

## Ammo_Value

The Ammo_Value readds the ammo count for the player. It saves the last ammo count. If the new ammo count is less than the older ammo count, then a recoil happens. If the new ammo count is greater than the old ammo count, then it is a reload. 

## Recoil

The recoil output signal is watched. When a 0 to 1 transitition happens, the recoil command is sent to the light gun. The light gun takes care of the recoil. Also, when the recoil is happening, the light gun will ignore any new recoil commands. 

## Recoil_R2S

Since a section details how this works already. Please read the Recoil_R2S section above.

https://github.com/6Bolt/Hook-Of-The-Reaper#recoil_r2s-command

## Recoil_Value

This is were the output signal directly control the light gun's solenoid. When the signal is high, the solnoid is active, and current going through it. When it is low, then it is at rest. Hook Of The Reaper, has a safety feature, as if the solnoid is active for over 200ms, it will close it, and block any new Recoil_Value commands until the game ends. Currently, only 3 light gun support this. Which are OpenFire, Fusion, and Gun4IR.


# Reload

There are only three reload commands that can be used in the DefaultLG game files. If the recoil method supports reload, it will be after the '1` in the header. 

## Ammo_Value

The Ammo_Value readds the ammo count for the player. It saves the last ammo count. If the new ammo count is less than the older ammo count, then a recoil happens. If the new ammo count is greater than the old ammo count, then it is a reload. 

## Reload

The recoil output signal is watched. When a 0 to 1 transitition happens, the reload command is sent to the light gun. 

## Reload_Value

The Ammo_Value readds the ammo count for the player. It saves the last ammo count. If the new ammo count is greater than the old ammo count, then it is a reload. 

# Recoil and Reload Commands

Now the recoil and reload commands need a '#' in front of them, instead of the '\>'. This is a safety feature. Also, for Hook Of The Reaper to know what to use, based on the light gun's recoil priority. For the selected recoil, it will replace the '#' with the '\>'. Please see the included game files for examples. 



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


### I Started Hook of the Reaper, but don't See it Anywhere

Hook of the Reaper starts in the tray icon. As it normally not needed to be seen. Also, when playing video games, it shouldn't be seen. To bring up Hook of the Reaper, open the tray icon, and left click onto the Hook of the Reaper icon. This will open up Hook of the Reaper onto the desktop. When minimized, it will go back to the tray icon. Also, when it is in the tray icon, a right click on the icon, will bring up a small menu, to easily access the most used features. 

### The Recoil on the OpenFire Light Gun is Not Good

I strongly advise using Vidal's OpenFire setting, when using it with Hook of the Reaper. I and other agree that recoil is a lot better with the new settings. Link to settings is below. 

https://github.com/6Bolt/Hook-Of-The-Reaper/issues/32


### Is Hook Of The Reaper complete?

No, currently Hook Of The Reaper is a work in progress. The main code for the light guns is done, but still adding to it when new light guns come out. I am working on the game data base and game files. Then I will do some short videos on how to set-up Hook of the Reaper. After that, I will work on getting the LED & I/O controller into Hook Of The Reaper. After that, I plan on getting the wiki done. Then it will be complete.


### Other Peoples Light Guns work, but Mine Doesn't, Can you Help Me Out?

Sorry, I cannot help you. I suggest going to a forum or Facebook group to get help. Also, there are free and paid game drives that come with and with out support. Also, you can use the debug setting that was put in v1.1.1. The debug setting is safe for sort term use. If your light gun starts working in the debug mode, then something is most likely wrong with your USB or USB cabling. 


### Can you Help Me Set-Up my Light Gun and Light Gun Games? Or Help Make a Game File?

No.

### Can you Help Me Set-Up Hook Of The Reaper?

No. Sorry, there will be no tech support now or ever for this program. It is a free open source project, that comes with no warrenty, no guarantees, and no support. I am disabled and don't have that much time in a day. I used to help people, but then all I did for 2 weeks was helping people set-up their light gun(s), and teach them about their light gun(s). As they say Hook of the Reaper is not working. But Hook of the Reaper was working in 100% of those situations. The problem was the people didn't set things up correctly, because, I am guessing, couldn't be bothered by reading a wiki or learning. I had to stop this, as no work was being done on Hook of the Reaper, and I did not enjoy this at all. It came down to scrapping the project, or not doing tech support. I choose to not do tech support. Also, hundreds of people have gotten Hook of the Reaper working, with no problems and no tech support. I did not want to punish the vast majority, because a small minority couldn't set up their light gun(s) correctly. So if you read nothing, and blast all my ways of communications, I will ignore you, and maybe block you too.  

### No Tech Support, but What if I Need Help?

Sorry your are on your own. Again, I suggest you join a forum or Facebook group. I have made Hook of the Reaper easy as I can. Now needed fields in 'Add Light Gun' window are highlighted red. I do my best to test everything out before a release. The number of bugs have been really low. Most Issues are not Hook Of The Reaper bugs. I think I have only gotten 2-3 bugs so far from Issues. Most bugs have been caught by myself. Also, hundreds have gotten Hook Of The Reaper working with no problems. I know people have gotten all the supported light gun working. I have tested all the game files on the game file site. I have tested almost all the supported like guns, except for the Fusion light gun, Gun4IR, and XGunner. However, I have had multiple people test out Fusion, Gun4IR, and XGunner light guns.  

### Do you Have the Game File XXXXX? It is Not Included with Hook Of The Reaper?

All the game files that I have, are include with Hook Of The Reaper. Also, all game files are stored at the Hook of the Reaper game file site. Please check there, for the latest game files, as new game files will be posted there. It is located at: https://hotr.6bolt.com


### Hook of the Reaper has No TCP Network Connection when the Emulator is Running?

Most likly, only 2 things could cause this. The first, is that the emulator or DemulShooter wasn't set up correctly, to output to the network. Or the emulator or DemulShooter is not being open properly. For example, the DemulShooter GUI is being open, and not the command line with the needed target and rom. The second thing, is that Hook of the Reaper and/or the emulator/DemulShooter is being blocked by the firewall and/or anti-virus. Make sure that all programs are approved in the computer's firewall and anti-virus. This could be the Window's firewall and Window's Defender, which is standard. Or a thirp party software, like Norton.


### What is the Best Way to Debug?

First, do not use any scripts or LaunchBox. Get it working with command line commands first. Once working, then can compared against the scipt or what is in LaunchBox. A small error could be in the script, and people will run it over and over again, never seeing the error. Then blame something else. Reason why I don't like when people including a script for a bug or problem. As I will always tell them, don't use scripts for debug. Also, my other program could be useful, which is LGTCPTester. It will display output signal, and can log them too. The timestamp gets reset when a game starts. Located at: https://github.com/6Bolt/LGTCPTester . Also, always check the basics, like the DIP switches on the light gun, and confirm their serial port. Also, make sure the 'Player Assignments' are set correctly. I have seen some very smart engineer's ignore the basic, and lost many hours of debugging. 


### I am using DemulShooter and Hook of the Reaper with the New Game that Just Came Out, But it is Not Working?

It takes time for me and Argon (coder for DemulShooter) to get everything set up. If the game has recoil and more outputs, and can be outputted without DemulShooter, then Hook of the Reaper can be used. A game file needs to be created for the game. This tells Hook of the Reaper, to know what output signals control what light gun actions, like recoil. When I create the game file, I will then post it at the Hook of the Reaper game file site, which is linked below. If the game has no output signals, then Argon will have to code it into DemulShooter, and then release it. After he release it, is when I would then make the game file. I usually can get the game file done, if I am not working on anything else. I am disabled and can take a few days. But nothing stops you from creating the new game file before I do. If you do, please email it to gamefiles@hotr.6bolt.com. 

[Hook of the Reaper's Game File Site](https://hotr.6bolt.com/pmwiki.php/Main/HomePage)


### The Games Input Controls are not Working Correctly? Or Game Fails After a Certain Level or Point?

Hook of the Reaper has nothing to do with input controls. So, if the crosshair is not moving with the light gun, it is not Hook of the Reapers doing. You will need to contact DemulShooter or the people that coded the emulator. Also, if a game fails after a certain level or point, it too has nothing to do with Hook of the Reaper. You will need to contact the people that coded the emulator. Yes, people have sent me these bugs. 





