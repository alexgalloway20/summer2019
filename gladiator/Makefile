ALL:  sem_helper colosseum gladiator

sem_helper: sem_helper.c
	gcc -c sem_helper.c

colosseum: sem_helper colosseum.c
	gcc -c colosseum.c
	gcc colosseum.o sem_helper.o -o colosseum
	rm colosseum.o
	rm sem_helper.o

gladiator: gladiator.c
	gcc gladiator.c -o gladiator

clean:
	rm colosseum gladiator
