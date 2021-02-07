y = audioread('1690.wav');
sr = 44100; %sampling rate
w = 2048; %window size
T = w/sr; %period
% t is an array of times at which the window function is evaluated
t = linspace(0, 1, sr);
twindow = t(1:w); %first 2048 elements of t


%hann window function
%win = 0.5 - 0.5 * cos((2 * pi * twindow)/T);

%hamming window function
%win = 0.54 - 0.46 * cos((2 * pi * twindow)/T);

%blackmann-harris window function
win = 0.35875 - 0.48829 * cos((2 * pi * twindow)/T)  + 0.14128 * cos((4 * pi * twindow)/T) - 0.01168 * cos((6 * pi * twindow)/T);

plot(win);
title('Blackmann-Harris Window');


yshort = y(1:w); %first 2048 samples from sound file
%Multiply the audio values in the window by the window function values,
% using element by element multiplication with .*.
% first convert hamming from a column vector to a row vector
win2 = win.';
ywindowed = win2 .* yshort;
ydetrend=detrend(yshort);


figure;
plot(yshort);
title('First 2048 samples of audio data');
figure;
plot(ywindowed);
title('First 2048 samples of audio data, tapered by windowing function');
figure;
plot(ydetrend);
title('Detrend');


figure
plot(abs(fft(yshort)), 'Color', 'red' );
title('FFT of original audio');
%hold on
figure
plot(abs(fft(ywindowed)),'Color', 'blue');
title('FFT after window function');
