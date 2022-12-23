function score = blockSelectionScore(x, A, rows, cols)
[n, m] = size(A);
B = zeros(n, 0);
C = zeros(0, m);
idx = find(x);
for i = 1 : numel(idx)
    B = [B, rows{idx(i)}];
    C = [C; cols{idx(i)}];
end

score = L1(A, B, C, 0);
end

