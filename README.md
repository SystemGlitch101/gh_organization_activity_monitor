The home assignment I was given for Legit-Security:

Write a command line application which monitors events of a github organization and detects suspicious behavior, notifying the user of such events.

Suspicious behaviors include:
- Pushing between 14:00 and 16:00
- creating a team with prefix "hacker"
- creating a repo and deleting it in less than 10 minutes

The design should be readable, efficient, coded in a way which can be expanded later on, and use good Object Oriented Design principles.



My development process:

1. When I was given the assignment I immediately thought of the subject/observer pattern, where the application (observer) is getting notified by github (subject). I looked into it and found out about webhooks, even before I had read the full assignment, so it was nice to know I was on the mark.

2. I took time to research about the REST api, and the various possible commands.

3. Next, I decided on the language. I chose c++, as it as great libraries and I'm most comfortable with it.

4. I looked into libraries which help me interact with webhooks, and a header only library called Crow, which provided server creation functionalities as well as json parsing capabilities, so it suited my needs.

5. Then I started reading about the webhook documentation on github, and specifically on what kind of content I will be parsing (which turns out to just be json content).

6. I decided to sketch out a uml diagram with various classes I thought would be fitting for this exercise. Namely, I needed a way to check conditions and a way to parse an event and check it against each of those conditions. As such, I created an interface called IEventType, and various classes which implement the interface. These implementations, in turn, will provide an IsValidEvent() method that checks various conditions. Additionally, I created an EventHandler class, which will contain a map between event type and its corresponding handler.

7. I wrote out the initial stub implementation of the classes, and the next thing I wanted to focus on is actually creating the github organization, repository, and webhook. 


DEPENDENCIES INSTALLATION (Ubuntu/Debian):
`$ sudo apt install build-essential make libboost-all-dev`



COMPILING:
`$ make`

SETUP
    - open the Makefile
    - change the PORT, PROXY_ROUTE, and WEBHOOK_PROXY_URL as needed
    - close the makefile
    - open a separate terminal in the same directory and run the following:
    - `$ make runproxy`

RUNNING: 
- `$ make run`

TODO:
1. write parsing logic for team event
2. write parsing logic for repo event 
3. seperate code into separate sources