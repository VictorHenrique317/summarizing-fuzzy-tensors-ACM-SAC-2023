function p = getPolyMaxObj(a, a_, b, n, ALGEBRA)
if ALGEBRA == 0
    mn = 0;
    mx = 5;
else
    mn = 0;
    mx = n;
end
x = mn + rand(n+1, 1) * (mx - mn);
y = nan(size(x));
for i = 1 : n+1
    y(i) = maxObj(x(i), a, a_, b, ALGEBRA);
end
q = polyfit_(x, y, n);
p = fliplr(q);
end