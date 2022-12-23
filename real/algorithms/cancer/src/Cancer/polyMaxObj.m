function val = polyMaxObj(alpha, p)
n = numel(p);
val = 0;
temp = 1;
for i = 1 : n
    val = val + p(i) * temp;
    temp = temp * alpha;
end
end