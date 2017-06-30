from sys import stdin, maxsize
from matplotlib import pyplot as plt
import numpy as np

counter = 0

time_t = []
delay_t = []

for line in stdin:
    try:
        info = line.split()
        delay = int(info[1])

        time_t += [counter]
        delay_t += [delay]
        counter += .125
    except:
        continue

data = np.array(delay_t)
x = np.array(time_t)
plt.plot(x, data)

# set intervals     
plt.axis([0, time_t[-1], 0, np.amax(delay_t)*1.1+1])

# plot labels
plt.ylabel("Atraso (ms)")
plt.xlabel("Tempo (s)")
plt.title("Relacao com dez carros no sistema")

fig = plt.gcf()
fig.canvas.set_window_title(":D")

# GO!
plt.show()

print time_t
print delay_t