### Legit-Security home assignment - given to Max (aka me)

### Task
Write a command line application which monitors events of a github organization and detects suspicious behavior, notifying the user of such events.

Suspicious behaviors include:
- Pushing between 14:00 and 16:00
- creating a team with prefix "hacker"
- creating a repo and deleting it in less than 10 minutes

The design should be readable, efficient, coded in a way which can be expanded later on, and use good Object Oriented Design principles.

### My development process

1. When I was given the assignment I immediately thought of the subject/observer pattern, where the application (observer) is getting notified by github (subject). I looked into it and found out about webhooks, even before I had read the full assignment, so it was nice to know I was on the mark.

2. I took time to research about the REST api, and the various possible commands.

3. Next, I decided on the language. I chose C++, as it has great libraries and I'm most comfortable with it.

4. I looked into libraries which help me interact with webhooks, and a header only library called ![Crow](https://crowcpp.org/master/), which provided server creation functionalities as well as json parsing capabilities, so it suited my needs.

5. Then I started reading about the webhook documentation on github, and specifically on what kind of content I will be parsing (which turns out to just be json content).

6. I decided to sketch out a UML diagram with various classes I thought would be fitting for this exercise. Namely, I needed a way to check conditions and a way to parse an event and check it against each of those conditions. As such, I created an interface called IEventTypeChecker, and various classes which implement the interface. These implementations, in turn, will provide an IsValidEvent() method that checks various conditions. Additionally, I created an EventHandler class, which will contain a map between event type and its corresponding handler.

7. I wrote out the initial stub implementation of the classes, and the next thing I wanted to focus on is actually creating the github organization, repository, and webhook. 

8. On the next day I sketched out a fuller UML diagram, and used that as reference a few times. Here it is:

![GH Organization Monitor UML drawio](https://github.com/SystemGlitch101/gh_organization_activity_monitor/assets/59316055/0306e85d-9818-4e94-ae1a-1e64c3d1751d)

9. I spent time creating and getting the webhook to work and get sent to a local port, which with the docs was very straightforward.

10. Finally, I implemented the logic checks, tidied up the code, and updated the relevant documents.

### DEPENDENCIES INSTALLATION (Ubuntu/Debian)

- `$ sudo apt install build-essential make libboost-all-dev`


### COMPILING

- `$ make`

### SETUP
- open the
- change the PORT, PROXY_ROUTE, and WEBHOOK_PROXY_URL as needed
- close the makefile
- make sure you have smee.io installed (see ![this doc](https://docs.github.com/en/enterprise-cloud@latest/apps/creating-github-apps/writing-code-for-a-github-app/quickstart)). Alternatively, you could use your own proxy and configure it accordingly. In that case you can skip the following step.
- open a separate terminal in the same directory and run the following:
- `$ make runproxy`

### RUNNING:
- `$ make run`

## TODO:
1. patch up docs
2. receive review from mentor
3. send the assignment
