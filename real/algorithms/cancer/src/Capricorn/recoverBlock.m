function [b, c] = recoverBlock(A, bIdx, cIdx, Corrs)
idx = Corrs==0;
A(idx) = nan;

mask = zeros(size(A));
mask(bIdx, cIdx) = 1;
A = A .* mask;

% Choose the row that represent the block best.
p = rowRepresentingBlock(A, bIdx);
c = A(p, :);
b = getAlpha(A, bIdx, p);

if sum(isnan(c)) > 0
    idxx = find(isnan(c));
    c(idxx) = getRow(A, b, idxx);
end

idx = isnan(c);
c(idx) = 0;
end


function c_ = getRow(A, b, idxx)
A = A(:, idxx);
idx = isnan(A);
A(idx) = 0;
W = ones(size(A));
W(idx) = 0;
c_ = zeros(1, numel(idxx));
for j = 1 : numel(idxx)
    c_(j) = b'*(A(:,j).*W(:,j)) / norm(b.*W(:,j), 'fro')^2;
end 
end