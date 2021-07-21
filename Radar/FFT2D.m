% 2-D Transform
% The 2-D Fourier transform is useful for processing 2-D signals and other 2-D data such as images.
% Create and plot 2-D data with repeated blocks.



P = peaks(20);
X = repmat(P,[5 10]);

Y = fft2(X);
signal_shifted = fftshift(Y);
signal_magnitude = abs(signal_shifted);

imagesc(signal_magnitude)

