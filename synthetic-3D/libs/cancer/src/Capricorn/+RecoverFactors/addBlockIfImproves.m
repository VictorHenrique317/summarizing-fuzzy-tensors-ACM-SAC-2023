function [B, C] = addBlockIfImproves(A, rows, cols)

% Find the average change to the objective that each block makes when it is
% added. Since block order plays a significant role, we average the results
% across different block orderings.

nBlocks = numel(rows);
nReps = 10;
samples = nan(nReps, nBlocks);
for iter = 1 : nReps
    fprintf('Trying permutation N%d\n', iter);
    p = randperm(nBlocks);
    [~, pInverse] = sort(p);
    samples(iter, :) = RecoverFactors.computeScores(A, rows(p), cols(p))';
    samples(iter, :) = samples(iter, pInverse);
end
scores = mean(samples);
[~, blockOrder] = sort(scores);
rows = rows(blockOrder);
cols = cols(blockOrder);
[n, m] = size(A);
B = zeros(n, 0);
C = zeros(0, m);
currentError = L1(A, B, C, 0);
% Add new blocks if they improve the score
for i = 1 : min(1000, numel(blockOrder))   
    Bnew = [B, rows{blockOrder(i)}];
    Cnew = [C; cols{blockOrder(i)}];
    newError = L1(A, Bnew, Cnew, 0);
    if newError < currentError
        currentError = newError;
        B = Bnew;
        C = Cnew;
        [B, C] = squeezeFactor(B, C, 2);
        [C, B] = squeezeFactor(C', B', 2);
        B = B';
        C = C';
    end
end

end