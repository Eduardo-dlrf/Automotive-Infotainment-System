# Reproduce audio con pygame
import pygame, serial, os, time, ctypes
path = "/home/piBBC/Music/Canciones/"
sound_files = ["Cure.wav","Jose.wav","Smiths.wav"]
cornPops = serial.Serial("/dev/ttyS0",baudrate=9600,timeout=1)

#Iniciacion Pygame
pygame.mixer.init()
speaker_volume = 1 #100% volumen
pygame.mixer.music.set_volume(speaker_volume)

class Pause(object):
    def __init__(self):
        self.paused=pygame.mixer.music.get_busy()

    def toggle(self):    
        if self.paused:
            pygame.mixer.music.unpause()
        if not self.paused:
            pygame.mixer.music.pause()
        self.paused = not self.paused

#class player
class Player:

    songs = [
        ("Cure.ogg","Just like heaven -The Cure"), 
        #("cherry.wav","Cherry Blossoms -Marselo Storm")
        ("Jose-converted.ogg","La Dama y El Moribundo -José Madero"),
        #("sat.wav","Saturn's first ring -Marselo Storm"),
        ("Smiths-converted.ogg","This Charming Man -The Smiths"),
        ("Funeral-converted.ogg","All Those Friendly People -Funeral Suits"),
        ("TimeTo-converted.ogg","Time To Pretend -MGMT"),
        ("Everlong-converted.ogg","Everlong -Foo Fighters"),
        ("EyeOf-converted.ogg","Eye Of The Tiger -Survivor"),
        ("Dust-converted.ogg","Another One Bites The Dust -Queen"),
        ("Woman-converted.ogg", "American Woman -The Guess Who"),
        ("RocketMan-converted.ogg","RocketMan -Elton John")
    ]

    def __init__(self):
        self.current:int = None
        self.previous:int = None

    def play(self, track:int) -> None:
        #self.current = track
        pygame.mixer.music.load("/home/piBBC/Music/Canciones/"+self.songs[track][0])
        pygame.mixer.music.play()
        print(self.current)
        print(self.previous)

    def stop(self) -> None:
        PAUSE = Pause()
        PAUSE.toggle()
        ...
    def increaseVol(self) -> None:
        global speaker_volume
        if speaker_volume < 1:
            speaker_volume += 0.1
            pygame.mixer.music.set_volume(speaker_volume)
    
    def reduceVol(self) -> None:
        global speaker_volume
        if speaker_volume > 0:
            speaker_volume -= 0.1
            pygame.mixer.music.set_volume(speaker_volume)
        

    def update(self, processedInput) -> None:
        
        #print(f"antes {self.current}")
        if processedInput in ['0','1','2','3','4','5','6','7','8','9']:
            self.stop()
            self.previous = self.current
            self.current = int(processedInput)
            self.play(int(processedInput))
        elif processedInput == '#':
            self.increaseVol()
        elif processedInput == '*':
            self.reduceVol()
        elif processedInput == 'A':
            pygame.mixer.music.pause()
        elif processedInput == '-4':
            pygame.mixer.music.unpause()
        elif processedInput == 'B':
            pygame.mixer.music.unpause()
        elif processedInput == 'C':
            # next track
            #print("ajhgfjghj")
            if self.current == len(self.songs)-1:
                self.play(0)
                self.previous = self.current
                self.current = 0
            else:
                self.play(self.current+1)
                self.previous = self.current
                self.current += 1
        elif processedInput == 'D':
            # previous track
            if self.current == 0:
                self.play(len(self.songs)-1)
                self.previous = self.current
                self.current = len(self.songs)-1
            else:
                self.play(self.current-1)
                self.previous = self.current
                self.current -= 1
        #print(f"despues {self.current}")

def processInput(serialInput):
    ...

serialInput:... = ...

#end class player

player=Player()
z=1
while z == 1:
    upd = cornPops.read(1).decode("ascii") 
    player.update(upd)
    print(upd)
    cornPops.write(str(player.current).encode())

        #print("Dato Basura")
    
