function [B, C] = svdWrapper(A, k)
[U, S, V] = svd(A);
S(:, k+1:end) = [];
B = U * S;
V(:, k+1:end) = [];
C = V';

end

