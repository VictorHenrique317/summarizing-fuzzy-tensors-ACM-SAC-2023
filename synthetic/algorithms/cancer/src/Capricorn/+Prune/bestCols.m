function [rowSetsPruned, colSetsPruned] = bestCols(A, rowSets, colSets)
display('For each row set find the best column set.');
% For each row set find the best column set.
blockIdx = 1;
for i = 1 : numel(rowSets)
    fprintf('processing %d out of %d\n', i, numel(rowSets));
    bestCols = 1;
    [bBest, cBest] = recoverBlock(A, rowSets{i}, colSets{bestCols});
    bestScore = blockFitness(A, bBest, cBest);
    for j = 2 : numel(colSets)
        [b, c] = recoverBlock(A, rowSets{i}, colSets{j});
        newScore = blockFitness(A, b, c);
        if newScore < bestScore
            bestScore = newScore;
            bestCols = j;
        end
    end
    rowSetsPruned{blockIdx} = rowSets{i};
    colSetsPruned{blockIdx} = colSets{bestCols};  
    blockIdx = blockIdx + 1;
end

end