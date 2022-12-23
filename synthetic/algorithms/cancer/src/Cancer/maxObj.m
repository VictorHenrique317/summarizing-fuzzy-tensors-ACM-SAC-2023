function val = maxObj(alpha, a, a_, b, ALGEBRA)
if ALGEBRA == 0
    val = norm(a - max(alpha*b, a_), 'fro')^2;
else
    val = norm(a - max(alpha + b, a_), 'fro')^2;
end
end