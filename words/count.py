import os

for lang in os.listdir('.'):
    if os.path.isdir(lang):
        for length in os.listdir(lang):
            p = lang + "/" + length
            os.system(f'cat {p} | wc -l | cat - {p} > tmp && mv tmp {p}')