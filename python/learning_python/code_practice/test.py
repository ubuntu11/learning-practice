"""
從csv檔讀入資料，然後繪圖的一個例子.
@author: ken
"""
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from datetime import datetime

dataset = pd.read_csv('youtube.pcap-by-minute.csv')

#dataset = dataset[1:]
dataset['time'] = dataset['time'].map(lambda x: datetime.strptime(str(x), '%Y-%m-%d %H:%M'))
dataset['txBytes'] = dataset['txBytes'].map(lambda x: int(str(x)))
dataset['rxBytes'] = dataset['rxBytes'].map(lambda x: int(str(x)))
  
x = dataset['time']
y1 = dataset['txBytes']
y2 = dataset['rxBytes']

# plot
plt.plot(x,y1)
plt.plot(x,y2)
# beautify the x-labels
plt.gcf().autofmt_xdate()

plt.show()
