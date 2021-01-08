import os
import random


if __name__ == "__main__":
    with open("./test.bat", "w") as bat_file:
        for root, dirs, files in os.walk("../data/"):
            for fl in files:
                bat_file.write(
                        "./VWTS"+" \t "
                        +root+fl+" \t "
                        +"./solutions/"+fl.replace(".txt", ".sol")+" \t "
                        +"400 \t "
                        +str(random.randint(0,1000000))
                        +"\n"
                    )