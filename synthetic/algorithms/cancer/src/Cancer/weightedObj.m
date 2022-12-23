function val = weightedObj(a, a_, w)
t = a - a_;
v = w*(t<0) + 1;   % t<0 -> overcover
val = norm(t .* v, 'fro')^2;

end

