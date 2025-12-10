import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from cycler import cycler

PART=2

if PART==1:
	df = pd.read_csv('day08-1.csv', index_col=0).fillna(0)
	#dff = df.stack().reset_index(name="values")
else:
	df = pd.read_csv('day08.csv', header=None).fillna(0).T

#colours = ['orangered', 'limegreen', 'royalblue'] * ((df.shape[0]+2) // 3)
colours = ['#1AAFBC', '#6A7BA2'] * ((df.shape[0]+1) // 2)

fig, ax = plt.subplots(figsize=(12,7))
if PART==1:
	bars = ax.barh(df.index, df['1'], height=1.0, color=colours)
	ax.set_xlim(0, 80)
else:
	bars = ax.barh(df.index, df[0], height=1.0, color=colours)
	ax.set_xlim(0, 1000)
plt.subplots_adjust(left=0.05, right=0.97, bottom=0.05, top=0.95, wspace=None, hspace=None)

def update(frame):
	for i, bar in enumerate(bars):
		if PART==1:
			bar.set_width(df[str(frame+1)][i+1])
		else:
			bar.set_width(df[frame][i])
	ax.set_title(f"Frame: {frame}")
	return bars

if PART==1:
	ani = animation.FuncAnimation(fig, update, frames=df.shape[1], interval=20)
else:
	ani = animation.FuncAnimation(fig, update, frames=range(0, df.shape[1], 5), interval=20)

FFwriter = animation.FFMpegWriter(fps=50)
ani.save(f"part{PART}.mp4", writer = FFwriter)

plt.show()
