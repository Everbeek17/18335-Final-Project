% This is to plot the timings of a few runs

% number of time steps was kept to a constant 100
% (as scaling this purely multiplies the total timing)

% number of bodies = n

n_values = [3, 8, 16, 32, 64, 128, 250];

% FMM_timings = [0.001708, 0.001853, 0.005668, 0.038911];
% Direct_timings = [0.000044, 0.000405, 0.056310, 4.320246];

% Each test run three times, with the slowest result excluded (to account
% for runtime variation) each of the three tests used random initial values
% (except for the tests with three bodies, they used the orbits seen in the
% plot)

% 3
FMM(1) = (0.001677 + 0.001636)/2;
Direct(1) = (0.000053 + 0.000044)/2;

% 8
FMM(2) = (0.001794 + 0.001807)/2;
Direct(2) = (0.000268 + 0.000267)/2;

% 16
FMM(3) = (0.002207 + 0.002044)/2;
Direct(3) = (0.000981 + 0.000991)/2;

% 32
FMM(4) = (0.002506 + 0.002513)/2;
Direct(4) = (0.003760 + 0.003844)/2;

% 64
FMM(5) = (0.003566 + 0.003323)/2;
Direct(5) = (0.017362 + 0.016532)/2;

% 128
FMM(6) = (0.005106 + 0.005400)/2;
Direct(6) = (0.089844 + 0.072732)/2;

% 250
FMM(7) = (0.008152 + 0.008291)/2;
Direct(7) = (0.261498 + 0.275600)/2;

slope = (FMM(7) - FMM(6))/ (n_values(7) - n_values(6) + 77);

linear_line = n_values*slope;
loglog(n_values, Direct, "LineWidth", 1.0)
hold on
loglog(n_values, FMM, "LineWidth", 1.0)
loglog(n_values, linear_line, "--", "LineWidth", 0.5)
hold off

xlabel("n")
ylabel("seconds")
xlim([min(n_values), max(n_values)])
lgnd = legend("Direct Method", "Fast Multipole Method", "O(n)", "location", "northwest");
fontsize(12, 'points')
print -depsc scaling_figure.eps