logfile: play referee player
	touch logfile

all: play player referee

#readstr: readstr.c 
#	gcc -c readstr.c 
	
play: play.c 
	gcc -g -o play play.c 

player: player.c 
#	gcc player.c readstr.c -o player
	gcc -g player.c -o player

referee: referee.c
#	gcc referee.c readstr.c -o referee
	gcc -g referee.c -o referee

clean: 
	rm -f *.o play readstr