import matplotlib.pyplot as plt
import matplotlib.lines as lines
from mpl_toolkits.axes_grid1 import make_axes_locatable
from matplotlib.colors import LogNorm
from matplotlib.ticker import MultipleLocator
import numpy as np
import pandas as pd
import math 

plt.rcParams['font.family'] = 'serif'
plt.rcParams['font.serif'] = ['Times New Roman'] + plt.rcParams['font.serif']

fig, axes = plt.subplots(nrows=1, ncols=5, sharex=True, sharey=True,  figsize=(15,3), constrained_layout=True)

folder_w = "data/GROUPED_SystemTemperature.tsv"
folder_p = "data/PATTERN_SystemTemperature.tsv"
folder_pid = "data/PIDTM_SystemTemperature.tsv"
folder_flea35 = "data/FLEA35_SystemTemperature.tsv"
folder_flea535 = "data/FLEA535_SystemTemperature.tsv"

folders = [folder_w, folder_p, folder_pid, folder_flea35, folder_flea535] 
labels = ["Grouped", "Pattern", "PIDTM", "FLEA35", "FLEA535"]
#colors = ["#f9c80e", "#f86624", "#ea3546", "#43bccd" ]
colors = ["#390099", "#9e0059", "#ff5400", "#ffbd00", "#00d5ff"]

boxplot = []
picplot = []

k = 0
for folder in folders:
        print("Reading file " + folder + "...")
        tsv_data = pd.read_csv(folder, sep='\t', nrows=60000)
        raw_data = tsv_data.to_numpy()

        
        generalMax = 0
        sysSize = len(raw_data[0])-1
        side = int(math.sqrt(sysSize))
        picMax = np.zeros((side,side))

        picAvg = np.zeros((side,side))
        nsamples = 0

        time = []
        samples = np.zeros((len(raw_data[0])-1, len(raw_data)))

        n_pes = len(raw_data[0])-1
        n_samples = len(raw_data)

        max_temp = np.zeros(n_samples)
        sample = np.zeros(n_pes)

        for i in range(len(raw_data)):
                time.append(raw_data[i][0])
                nsamples += 1
                for j in range(len(raw_data[i])-1):
                        sample[j] = raw_data[i][j+1]
                        picAvg[int(j%side)][int(j/side)] += sample[j]
                        if generalMax < sample[j]:
                                generalMax = sample[j]
                                uj = 1
                                for ux in range(side):
                                        for uy in range(side):
                                                picMax[ux][uy] = raw_data[i][uj]
                                                uj+=1

                sample.sort()
                max_temp[i] = sample[n_pes-1]
        
        # print(picAvg)
        # print(nsamples)
        for x in range(side):
                for y in range(side):
                        
                        picAvg[x][y] = picAvg[x][y] / nsamples


        # https://matplotlib.org/stable/gallery/images_contours_and_fields/interpolation_methods.html -> more interpolation methods
        # https://matplotlib.org/stable/tutorials/colors/colormaps.html                               -> more colors
        pcm = axes[k].imshow(picAvg, interpolation='gaussian', aspect='auto', vmin=50, vmax=100, cmap='jet') # YlOrRd, jet, turbo

        # Major ticks
        axes[k].set_xticks(np.arange(0, side, 1))
        axes[k].set_yticks(np.arange(0, side, 1))

        # Labels for major ticks
        axes[k].set_xticklabels(np.arange(0, side, 1))
        axes[k].set_yticklabels(np.arange(0, side, 1))

        # Minor ticks
        axes[k].set_xticks(np.arange(-.5, side-1, 1), minor=True)
        axes[k].set_yticks(np.arange(-.5, side-1, 1), minor=True)

        #Title
        axes[k].set_title(labels[k])

        # Gridlines based on minor ticks
        axes[k].grid(which='minor', color='black', linestyle='-', linewidth=1)
        axes[k].tick_params(axis='both', which='major', labelsize=8)

        k+=1

fig.colorbar(pcm, ax=axes[:], location='right', shrink=1.0, label="Temperature (°C)")
fig.suptitle('Average PE Temperature', fontsize=16)

# for i in range(len(axes)):
#         pass
        # axes[i].yaxis.grid(True)
        # axes[i].xaxis.grid(True)
        #axes[i].legend()
        # axes[i].plot([0,1],[80,80], linestyle='dotted', color="black")
        # if i == 0:
        #         axes[i].set_title('Peak Temperature Comparison')

# lines_labels = [ax.get_legend_handles_labels() for ax in fig.axes]
# lines, labels = [sum(lol, []) for lol in zip(*lines_labels)]
#fig.legend(lines, labels, ncol=4, loc="lower center",  bbox_to_anchor=(0.673, -0.12))


# fig.text(0.5, -0.02, 'Time (s)', ha='center')
# fig.text(-0.02, 0.5, 'Temperature (°C)', va='center', rotation='vertical')

fig.savefig('data/thermal_shot.png', format='png', dpi=300, bbox_inches='tight')
fig.savefig('data/thermal_shot.pdf', format='pdf', bbox_inches='tight')

