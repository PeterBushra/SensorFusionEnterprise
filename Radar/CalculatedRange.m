Rmax = 300;
Rres = 1;
C = 3e8;
Tchrip = 5.5*2*Rmax /C;
Bsweep = C / (2*Rres);
Fbeat = [0 1.1e6 13e6 24e6];

Calculated_Range = C * Tchrip  * Fbeat  / (2 * Bsweep);

disp(Calculated_Range)