%% Channel Correlation Test

TINY =1.0e-20;

x =  10*rand(128,1)+1i*10*rand(128,1);
x_sz = numel(x);

y = 10*rand(128,1)+1i*10*rand(128,1);
y_sz = numel(y);

% Calculate coefficient for real part
syy = 0;
sxy = 0;
sxx = 0;
ax = mean(real(x));
ay = mean(real(y));

for idx=1:x_sz
    xt = real(x(idx)) - ax;
    yt = real(y(idx)) - ay;
    sxx = sxx + xt*xt;
    syy = syy + yt*yt;
    sxy = sxy + xt*yt;
end

r_real = sxy / (sqrt(sxx*syy) + TINY);

% Calculate coefficient for complex part
yt = 0;
xt = 0;
syy = 0;
sxy = 0;
sxx = 0;
ax = mean(imag(x));
ay = mean(imag(y));

for idx=1:x_sz
    xt = imag(x(idx)) - ax;
    yt = imag(y(idx)) - ay;
    sxx = sxx + xt*xt;
    syy = syy + yt*yt;
    sxy = sxy + xt*yt;
end

r_comp = sxy / (sqrt(sxx*syy) + TINY);

% Calculate final coefficient
r = (r_real + r_comp) / 2
