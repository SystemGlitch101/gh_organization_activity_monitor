The home assignment I was given for Legit-Security:

Write a command line application which monitors events of a github organization and detects suspicious behavior.

Suspicious behaviors include:
- Pushing between 14:00 and 16:00
- creating a team with prefix "hacker"
- creating a repo and deleting it in less than 10 minutes

My development process:

1. When I was given the assignment I immediately thought of the subject/observer pattern, where the application (observer) is getting notified by github (subject). I looked into it and found out about webhooks, even before I had read the full assignment, so it was nice to know I was on the mark.

2. I took time to research about the REST api, and the various possible commands.

3. Next, I decided on the language. I chose c++, as it as great libraries and I'm most comfortable with it.

4. I looked into libraries which help me interact with webhooks, and I discovered the Crow library. (and it's header only!).

5. TBD


TODO:
1. create a github organization 
2. create a webhook for it

