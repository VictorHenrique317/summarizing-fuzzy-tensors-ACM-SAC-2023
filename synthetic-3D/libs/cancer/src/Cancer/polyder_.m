function q = polyder_(p)
[~, idx] = max(p>0);
if isempty(idx)
    p = 0;
else
    p(1 : idx(1)-1) = [];
end
n = numel(p) - 1;
t = n - (0:n);
q = p .* t;
q(end) = [];


end

