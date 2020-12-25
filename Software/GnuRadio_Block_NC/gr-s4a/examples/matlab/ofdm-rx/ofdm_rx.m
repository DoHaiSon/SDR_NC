%% Simulation of OFDM Receiver
clc
close all
%% Parameters
N   = 128;        % Number of subcarriers
Noc = 48;
L   = 16;         % Number of pilots
Nleft  = floor((N-Noc)/2); 
SNR = 25;

%% Init
Ls = N/2;        % Half of subcarriers

load ofdm_data;

r = reshape(usrp_rx,[],1);
r = r(20*1024:40*1024);

r = awgn(r,SNR,'measured');

%r = r ./ max(abs(r));
%% Coarse Timing Synhronization (using Schimdl-Cox Method)
P = zeros(numel(r),1);
for d=1:numel(P)-2*Ls
    for m=0:Ls-1
        P(d) = P(d) + conj(r(d+m))*r(d+m+Ls);
    end
end

R = zeros(numel(r),1);
for d=1:numel(R)-2*Ls
    for m=0:Ls-1
        R(d) = R(d) + abs(r(d+m+Ls))^2;
    end
end

% Improved equation of Schmidl-Cox (Otherwise, noise has a big impact on peaks)
M = abs(P).^2 ./ (R.^2); 

idx_sync_coarse = find(M > 0.80,1,'first')

%% Fine Syhcronization (using Correlation)
% Now, we need to find exact timing point
% for implementation of Pearson correlation, see "Numerical Recipes in C",
% page 636-639

TINY = 1.0e-20;
x = ofdm_tx_t(:,1);
x_sz = numel(x);
y = r(idx_sync_coarse:idx_sync_coarse+1*(N));
y_sz = numel(y);

RC = [];
for idx=1:Ls
    yt = 0;
    xt = 0;

    syy = 0;
    sxy = 0;
    sxx = 0;
    ax = mean(abs(x));
    ay = mean(abs(y));
    for jdx=1:Ls
        xt = abs(x(jdx)) - ax;
        yt = abs(y(idx+jdx)) - ay;
        sxx = sxx + xt*xt;
        syy = syy + yt*yt;
        sxy = sxy + xt*yt;
    end
    rc = sxy / (sqrt(sxx*syy) + TINY); 
    RC = [RC rc];
end

[idx_val,idx_sync_fine] = max(abs(RC));
idx_sync = idx_sync_coarse + idx_sync_fine - L; 
%idx_sync = 156; %idx_sync - 1


rxt_frame = r(idx_sync + 1:idx_sync+4*(N+L));
rxt_frame2 = reshape(rxt_frame,N+L,[]);

rxt_preamble = rxt_frame2(:,1); 
plot(abs(rxt_preamble))
rxt_pilot = rxt_frame2(:,2);
rxt_data = rxt_frame2(:,3);
rxt_silence = rxt_frame2(:,4);

est_SNR = 10*log10(mean(abs(rxt_preamble).^2)/mean(abs(rxt_silence).^2))

%% Carrier Offset Recovery
est_angle = angle(P(idx_sync + L));
%fco_est = 0;

r = r.*exp(-1i*2*est_angle*(0:1:length(r)-1)'./N);         % Suppress CFO

if 1%abs(est_angle) >= pi
    p1 = fftshift(fft(r(idx_sync+1:idx_sync+N/2),N));
    p2 = fftshift(fft(r(idx_sync+N/2+1:idx_sync+N),N));
    
    % Iterate on even subcarriers
    % g_span1 = -4;
    % g_span2 = 4;
    % g = 1;
    % B = 0;
    % for idx=g_span1:g_span2
    %     B1 = 0;
    %     B2 = 0;
    %     for jdx=Nleft+2:2:Nleft+Noc
    %         B1 = B1 + conj(p1(jdx+idx))*conj(p2(jdx+idx));
    %         B2 = B2 + abs(p2(jdx+idx)^2);
    %     end
    %     B(g) = abs(B1)^2 / (2*B2^2);
    %     g = g + 1;
    % end
    g_span1 = -Nleft;
    g_span2 = Nleft;
    B = 0;
    g=1;
    for idx_k = g_span1:g_span2
        B1 = 0;
        B2 = 0;
        for jdx=Nleft+1:2:Nleft+Noc
            B1 = B1 + conj(p1(jdx+idx_k))*p2(jdx+idx_k);
            B2 = B2 + abs(p2(jdx)^2);
        end
        B(g) = abs(B1)^2 / (2*B2^2);
        g = g + 1;
    end
    
    [gval gidx] = max(B);
    g_est = gidx + g_span1 - 1;
    
    fco_remaining = (2*g_est/N);
    fco_est = (est_angle/(pi*N)) + fco_remaining;
    
    r = r.*exp(-1i*2*pi*(fco_remaining)*(0:1:length(r)-1)');
else
    fco_est = (est_angle/(pi*N));
end
%% OFDM Rx Chain
tx_bits = [...
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1, ...
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1, ...
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1, ...
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1, ...
     1,     1,     1,     1,     1,     1,     1,     1].';

txf_pilot = ofdm_tx_f(:,2);                         % Reference pilots
r1     = r(idx_sync+1:idx_sync+4*(N+L));            % Select the frame
r_f    = reshape(r1,(N+L),[]);                      % Serial to Parallel
r_f    = r_f(17:end,:);                             % CP Removal
r_f    = fft(r_f);                                  % FFT

r_preamble = fftshift(r_f(:,1));        % Received preamble
r_pilot = fftshift(r_f(:,2));           % Received pilot
r_data = fftshift(r_f(:,3));            % Received data
r_silence = fftshift(r_f(:,4));         % Received silence

occupied = (Nleft+1:Nleft+Noc);
h = txf_pilot(occupied) ./ r_pilot(occupied);
data = r_data(occupied) .* h;
%data = data ./ max(abs(data));

rx_bits = real(data) > 0;
[~, data_ber] = biterr(tx_bits,rx_bits);

data_ber
%plot(abs(data))


