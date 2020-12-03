import random
x = 8
y = 8
z = 8
res = [[]]
for i in range(x):
    for j in range(y):
        for k in range(z):
            string = []
            string.append(i)
            string.append(j)
            string.append(k)
            string.append(0)
            res.append(string)
random.shuffle(res)
f = open('map.map', 'w')
for i in range(x * y * z):
    f.write(" ".join(map(str, res[i])) + '\n')

f.close()
