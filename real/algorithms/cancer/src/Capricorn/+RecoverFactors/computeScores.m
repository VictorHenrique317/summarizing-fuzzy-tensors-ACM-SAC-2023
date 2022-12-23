function scores = computeScores(A, rows, cols)
algebra = 1;
nBlocks = numel(rows);
scores = nan(nBlocks, 1);
[n, m] = size(A);
B = zeros(n, 0);
C = zeros(0, m);
currentError = L1(A, B, C, algebra);
for i = 1 : nBlocks   
    B = [B, rows{i}];
    C = [C; cols{i}];
    newError = L1(A, B, C, algebra);
    scores(i) = newError - currentError;
    currentError = newError;
end

end