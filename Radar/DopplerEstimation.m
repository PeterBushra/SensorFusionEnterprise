Freq = 77e9;
C    = 3e8;
Fd = [3e3 -4.5e3 11e3 -3e3] ; %

% calculate Wavelength
lambda = C / Freq;

% calculate velocity
% TODO : Calculate the velocity of the targets  fd = 2*vr/lambda
Vr = Fd * lambda / 2;

disp(Vr); %% m/s  ,  when it come tooward number will be positive