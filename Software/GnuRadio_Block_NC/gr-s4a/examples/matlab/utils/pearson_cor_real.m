%% Channel Correlation Test
% taken from "Numerical Recipes in C", 2002 

TINY =1.0e-20;

x =  10*rand(128,1);
x_sz = numel(x);

y = +10*rand(128,1);
y_sz = numel(y);
 
yt = 0;
xt = 0;
syy = 0;
sxy = 0;
sxx = 0;
ax = mean(abs(x));
ay = mean(abs(y));

for idx=1:x_sz
    xt = abs(x(idx)) - ax;
    yt = abs(y(idx)) - ay;
    sxx = sxx + xt*xt;
    syy = syy + yt*yt;
    sxy = sxy + xt*yt;
end

r = sxy / (sqrt(sxx*syy) + TINY)
