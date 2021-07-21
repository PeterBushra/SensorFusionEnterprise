Fs = 1000;            % Sampling frequency                    
T = 1/Fs;             % Sampling period       
L = 1500; %length
t = (0:L-1)*T;        % Time vector

A = .7 ;
fa = 77;
B =2 ;
fb = 43;

signal = A*sin(2*pi*fa*t) + B*sin(2*pi*fb*t);
% Corrupt the signal with noise 
X = signal + 2*randn(size(t));

plot(1000*t(1:50) ,X(1:50))
title('Signal Corrupted with Zero-Mean Random Noise')
xlabel('t (milliseconds)')
ylabel('X(t)')


%Compute FFT OF Signal
signal_fft = fft(X);
P2 = abs(signal_fft / L);
P1  = P2(1:L/2+1);

f = Fs*(0:(L/2))/L;
plot(f,P1) 
title('Single-Sided Amplitude Spectrum of X(t)')
xlabel('f (Hz)')
ylabel('|P1(f)|')
