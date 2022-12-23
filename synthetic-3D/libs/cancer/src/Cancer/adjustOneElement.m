function [idx, newVal] = adjustOneElement(A, b, c, A_, degree, ALGEBRA)

% if isempty(varargin)
%% Use polynomial approximation.
sm = getBaseErrors(A, A_, b, c, ALGEBRA);
[~, m] = size(A);
nVal = nan(1, m);
vals = nan(1, m);
for j = 1 : m
    p = getPolyMaxObj(A(:, j), A_(:, j), b, degree, ALGEBRA);
    [vals(j), nVal(j)] = polyMin(p, A(:, j), A_(:, j), b, ALGEBRA);
end
[~, idx] = min(vals - sm);
newVal = nVal(idx);
end


function sm = getBaseErrors(A, A_, b, c, ALGEBRA)
if ALGEBRA == 0
    E = A - max(A_, bsxfun(@times, b, c));
else
    E = A - max(A_, bsxfun(@plus, b, c));
end
sm = sum(E .* E, 1);
end