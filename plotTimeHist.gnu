n=8 #number of intervals
max=2400. #max value
min=0. #min value
width=(max-min)/n #interval width
#function used to map a value to the intervals
hist(x,width)=width*floor(x/width)+width/2.0
set boxwidth width*0.9

set xlabel 'Time'
set ylabel 'Count'

#count and plot
plot [0:3000]  "peter.txt" u (hist($1,width)):(1.0) smooth freq w boxes lc rgb"red" title 'Customers'

set term png
set output 'advanced.png'
plot [0:4500] [0:300]   "peter2.txt" u (hist($1,width)):(1.0) smooth freq w boxes lc rgb"red" title 'Customers'

