**AWALE**

To make the programm work, you need to use the commands : 
- make server
- make client
- ./server 127.0.0.1

Then you have to create a new terminal for each client you want to create and use the commands :

./client 127.0.0.1 [username]

Username corresponds to the username you will want to use while playing. All usernames have to be different, if it is already used, you will have to change it.

When the client is connected, a list of commands appear. 
The client can see the other usernames, chat with someone, create a bio and read someone else's bio, challenge someone or observe a game and if he is observing, he can stop observing. For the game, the client can accept or reject the challenge and move the seeds.
The client can use all the commands while playing except for challenging, and observing someone.

The player 1 (the one that begins) is chosen randomly and the game is over if a player can't play anymore (the other player can't feed him and he doesn't have any seeds) or one player has more than 25 points.

