function alpha = getAlpha(A, bIdx, idx)
[n, ~] = size(A);
alpha = zeros(n, 1);
% Choose A(idx, :) to cover the rest of A.
for j = 1 : numel(bIdx)
    alpha(bIdx(j)) = approxRow(A(bIdx(j), :), A(idx, :));
end
end