#!/usr/bin/env python3

import pandas as pd
import os
import glob
import matplotlib.pyplot as plt
import numpy as np

import matplotlib
matplotlib.style.use('ggplot')

fig = plt.figure(1)
plt.clf()
ax = fig.add_subplot(111)
ax.set_ymargin(0.1)

current_dir = os.path.abspath(os.path.dirname(__file__))
trials = next(os.walk(current_dir))[1]

for trial in trials:
    benchmark_files = glob.glob(os.path.join(current_dir, trial, '*.tsv'))
    12
    df = pd.DataFrame()
    for benchmark_file in benchmark_files:
        benchmark_df = pd.read_table(benchmark_file)
        df = df.append(benchmark_df)
    df = df[~((df['Iterations'] == 3) & (df['Name Of Probe (Time)'] ==
    'GradientMagnitude'))]
    df.index = pd.Index(np.arange(len(df)))

    ax.errorbar(np.arange(len(df)), df['Mean (s)'], yerr=2*df['StdErr (s)'],
            fmt='o', barsabove=True, capsize=4, uplims=True, lolims=True,
            label=trial)

ax.set_xlim(-1, len(df))
ax.set_xticks(np.arange(len(df)))
ax.set_xticklabels(df['Name Of Probe (Time)'], rotation=40)
ax.set_ylabel('Time (s)')
ax.set_ylim(bottom=0)
ax.legend(loc='best')
plt.subplots_adjust(bottom=0.2)

fig.savefig('ProcessorPriority.eps', dpi=300)
# plt.show()
