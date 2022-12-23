function p = rowRepresentingBlock(A, bIdx)
[n, ~] = size(A);
results = nan(n, 1);
for i = 1 : numel(bIdx)
    idx = bIdx(i);
    alpha = getAlpha(A, bIdx, idx);
    block = alpha * A(idx, :);
    diff = A - block;
    isNan = isnan(diff);
    diff(isNan) = 0;
    results(idx) = norm(diff, 'fro');
end

% [~, idx] = sort(results);
% best = idx(1);
% c = A(best, :);
% b = getAlpha(A, bIdx, best);

notNan = find(~isnan(results));
temp = results(notNan);
nn = numel(temp);
if rem(nn, 2) == 0
    temp(nn+1) = 1e10;
end
med = median(temp);
t = find(abs(temp - med) < 1e-7);
try
    idx = t(1);
catch e
    wtf=1;
end
p = notNan(idx);

end

