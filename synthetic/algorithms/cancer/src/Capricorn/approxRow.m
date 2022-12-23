function alpha = approxRow(x, y)
% Find alpha such that norm(x - alpha*y, 'fro') is minimized.
idx = ~isnan(x) & ~isnan(y);
sumX = sum(x(idx));
sumY = sum(y(idx));
if sumY > 1e-4
    alpha = sumX / sumY;
else
    alpha = 1;
end
end