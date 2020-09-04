import matplotlib.pyplot as plt
import matplotlib.ticker
import numpy as np
import os
import subprocess

output_dir = os.path.expanduser("~") + "/Sync/research/devel/2019_ispass/tmp/taco_19/figures"
fig_name = "fig_mot_intel"
filename = "%s/%s.pdf" % (output_dir, fig_name)
workloads = [
'Clang',
'PostgreSQL-p',
'PostgreSQL-r',
'Javac',
'Derby',
'Node.js',
'MySQL',
]
list_no_code_superpage = [
    5.78,
    13.44,
    9.28,
    7.37,
    1.60,
    10.50,
    9.22,
]
list_stock_freebsd = [
    5.17,
    13.43,
    9.26,
    6.33,
    0.26,
    8.93,
    5.50,
]
# (x,y,z), where x is the total number of superpage-sized regions, y is the number of touched regions, and z is the number of promoted superpages at stock FreeBSD.
# TODO the JVM numbers don't include JIT code superpages yet
annotations = [
    '(27,27,1)',
    '(2,2,0)',
    '(2,2,0)',
    '(5,5,0)',
    '(5,5,0)',
    '(11,8,1)',
    '(20,15,5)',
]
assert len(list_no_code_superpage) == len(list_stock_freebsd), "list lengths differ"

opacity = 0.7
bar_width = 0.35


#plt.xlabel('')
plt.ylabel('% of execution cycles')

plt.xticks(np.arange(len(workloads)), workloads, rotation=30)
plt.gca().yaxis.set_major_formatter(matplotlib.ticker.FuncFormatter(lambda y, pos: "%d%%" % (y)))

bar1 = plt.bar(np.arange(len(list_no_code_superpage)), list_no_code_superpage, bar_width, align='center', alpha=opacity, color='b', label='w/o code superpages')
bar2 = plt.bar(np.arange(len(list_stock_freebsd)) + bar_width, list_stock_freebsd, bar_width, align='center', alpha=opacity, color='r', label='w/ stock reservation-based code superpages')

# Annotate the bars
for i in range(len(bar1)):
    rect = bar1[i]
    height = rect.get_height()
    plt.text(rect.get_x(), height, annotations[i], ha='left', va='bottom', fontsize='small')

plt.legend(prop={'size': 'medium'})
plt.savefig(filename, bbox_inches='tight')
#plt.show()
plt.close()
